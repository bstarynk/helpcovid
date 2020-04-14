/****************************************************************
 * file hcv_database.cc
 *
 * Description:
 *      PostGreSQL database of https://github.com/bstarynk/helpcovid
 *
 * Author(s):
 *      © Copyright 2020
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *
 *
 * License:
 *    This HELPCOVID program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "hcv_header.hh"
#include <pqxx/prepared_statement.hxx>
//NOT NEEDED: #include <pqxx/prepared_statement.hxx>

extern "C" const char hcv_database_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_database_date[] = __DATE__;

extern "C" std::unique_ptr<pqxx::connection> hcv_dbconn;
extern "C" std::recursive_mutex hcv_dbmtx;

/// the database connection
std::unique_ptr<pqxx::connection> hcv_dbconn;

/// the short PostGreSQL server version
std::string hcv_our_postgresql_server_version;

/// the recursive mutex to serialize access to that database
std::recursive_mutex hcv_dbmtx;

extern "C" void hcv_prepare_statements_in_database(void);


Hcv_PreparedStatement::Hcv_PreparedStatement(const std::string& name)
  : m_name(name)
{
  std::lock_guard<std::recursive_mutex> guard(hcv_dbmtx);
  m_txn = new pqxx::work(*hcv_dbconn);
  m_inv = new pqxx::prepare::invocation(m_txn->prepared(m_name));
#warning TODO: pqxx::transaction_base::prepared is deprecated and should not be used.
} // end Hcv_PreparedStatement::Hcv_PreparedStatement


Hcv_PreparedStatement::~Hcv_PreparedStatement()
{
  delete m_inv;
  delete m_txn;
}


void
Hcv_PreparedStatement::bind(const std::string& arg)
{
  if (m_inv)
    (*m_inv)(arg);
}


void
Hcv_PreparedStatement::bind(std::int64_t arg)
{
  if (m_inv)
    (*m_inv)(arg);
}


pqxx::result
Hcv_PreparedStatement::query()
{
  if (!m_inv)
    HCV_FATALOUT("Invcation not initialised!");

  pqxx::result res = m_inv->exec();
  m_txn->commit();

  return res;
}


const std::string
hcv_postgresql_version(void)
{
  return hcv_our_postgresql_server_version;
} // end  hcv_postgresql_version



void
hcv_initialize_database(const std::string&uri, bool cleardata)
{
  std::string connstr = uri;
  if (connstr.empty())
    {
      if (hcv_config_has_group("postgresql"))
        {
          hcv_config_do([&connstr](const Glib::KeyFile*kf)
          {
            if (!kf->has_key("postgresql","connection"))
              HCV_FATALOUT("missing 'connection' key in [postgresql] section of configuration");
            connstr = kf->get_string("postgresql","connection");
            HCV_SYSLOGOUT(LOG_INFO, "using '" << connstr
                          << "' as PostGreSQL connection string.");
          });
        }
      else
        {
          HCV_SYSLOGOUT(LOG_WARNING, "cannot initialize database. Configuration file should have a [postgresql] group.");
          return;
        }
    };
  if (connstr.empty() || connstr=="-")
    {
      HCV_SYSLOGOUT(LOG_WARNING, "no PostGreSQL database initialization: " << connstr);
      return;
    }
  HCV_SYSLOGOUT(LOG_INFO, "hcv_initialize_database connstr=" << connstr);
  ///
  hcv_dbconn.reset(new pqxx::connection(connstr));
  {
    HCV_SYSLOGOUT(LOG_INFO, "hcv_initialize_database for connstr=" << connstr << " hcv_dbconn is " << hcv_dbconn.get());
    {
      /// https://stackoverflow.com/a/27019316/841108
      if (cleardata)
        {
          pqxx::nontransaction clearwork(*hcv_dbconn);
          // bad idea https://dba.stackexchange.com/a/154075/204015
          // better idea https://www.postgresql.org/docs/current/sql-dropdatabase.html
          /*clearwork.exec0(std::string("DROP DATABASE IF EXISTS ")
            + hcv_dbconn->dbname());*/
          clearwork.exec0(std::string("DROP OWNED BY CURRENT_USER CASCADE;"));
          HCV_SYSLOGOUT(LOG_NOTICE, "hcv_initialize_database cleared database");
        }
      ///================ add something into PostGreSQL log
      pqxx::work firsttransact(*hcv_dbconn);
      {
        /// see https://stackoverflow.com/a/60954480/841108
        char logreqbuf[256];
        memset (logreqbuf, 0, sizeof(logreqbuf));
        char gitbuf[24];
        memset (gitbuf, 0, sizeof(gitbuf));
        strncpy(gitbuf, hcv_gitid, sizeof(gitbuf)-5);
        if (strchr(hcv_gitid, '+'))
          strcat(gitbuf, "+");
        snprintf (logreqbuf, sizeof(logreqbuf),
                  "starting HelpCovid git %.22s (built %.80s, md5 %.20s...) %s on %.64s pid %d",
                  gitbuf, hcv_timestamp, hcv_md5sum,
                  (cleardata?"cleared":"initialized"),
                  hcv_get_hostname(), (int)getpid());
        if (strchr(logreqbuf, '\'') || strchr(logreqbuf, ';') || strchr(logreqbuf, '\\'))
          HCV_FATALOUT("hcv_initialize_database invalid logreqbuf:" << logreqbuf);
        static char fullreqbuf[300];
        snprintf(fullreqbuf, sizeof (fullreqbuf),
                 "DO $$BEGIN RAISE LOG '%s'; END;$$;", logreqbuf);
        HCV_DEBUGOUT("hcv_initialize_database: fullreqbuf=" << fullreqbuf);
        firsttransact.exec0(fullreqbuf);
      }
      ////================ query the PostGreSQL version
      pqxx::row rversion = firsttransact.exec1("SELECT VERSION();");
      std::string pqversion = rversion[0].as<std::string>();
      pqxx::row r2version = firsttransact.exec1("SHOW SERVER_VERSION;");
      hcv_our_postgresql_server_version = r2version[0].as<std::string>();
      HCV_SYSLOGOUT(LOG_INFO, "hcv_initialize_database got PostGreSQL version " << pqversion
                    << "(server version " << hcv_our_postgresql_server_version << ")");
      firsttransact.commit();
    }
    ////================ create tables if they are missing
    pqxx::work transact(*hcv_dbconn);
    ////================ user table and indexes, with mandatory data
    transact.exec0(R"crusertab(
---- TABLE tb_user
CREATE TABLE IF NOT EXISTS tb_user (
  user_id SERIAL PRIMARY KEY NOT NULL,  -- unique user_id
  user_firstname VARCHAR(31) NOT NULL,  -- first name, in capitals, UTF8
  user_familyname VARCHAR(62) NOT NULL, -- family name, in capitals, UTF8
  user_email VARCHAR(71) NOT NULL,      -- email, in lowercase, UTF8
  user_telephone VARCHAR(23) NOT NULL,  -- telephone number (digits, +, - or space)
  user_gender CHAR(1) NOT NULL,         -- 'F' | 'M' | '?'
  user_crtime TIMESTAMP DEFAULT current_timestamp -- user entry creation time
); --- end TABLE tb_user
)crusertab");
    transact.exec0(R"cruserfamix(
---- INDEX ix_user_familyname
  CREATE INDEX IF NOT EXISTS ix_user_familyname 
    ON tb_user(user_familyname);
--- end INDEX ix_user_familyname
)cruserfamix");
    transact.exec0(R"cruseremailix(
---- INDEX ix_user_email
  CREATE INDEX IF NOT EXISTS ix_user_email 
    ON tb_user(user_email);
--- end INDEX ix_user_email
)cruseremailix");
    transact.exec0(R"crusertimeix(
---- INDEX ix_user_crtime 
  CREATE INDEX IF NOT EXISTS ix_user_crtime 
    ON tb_user(user_crtime);
--- end INDEX ix_user_crtime
)crusertimeix");
    ////================ password table
    transact.exec0(R"crpasswdtab(
---- TABLE tb_password
CREATE TABLE IF NOT EXISTS tb_password (
  passw_id SERIAL PRIMARY KEY NOT NULL, -- unique key in this table
  passw_userid INT NOT NULL,            -- the user id whose password we store
  passw_encr text NOT NULL,             -- the encrypted password
  passw_mtime  TIMESTAMP DEFAULT current_timestamp  -- the last time that password was modified
); --- end TABLE tb_password
)crpasswdtab");
    ////================ web cookie table, related to web cookies
    // see https://www.postgresql.org/docs/current/datatype-net-types.html
    // read https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
    // see http://man7.org/linux/man-pages/man2/getsockname.2.html
    // see http://man7.org/linux/man-pages/man7/ip.7.html
    // and http://man7.org/linux/man-pages/man7/ipv6.7.html
    /// we are aware that the browser IP is unreliable information
    /// see https://stackoverflow.com/q/527638/841108
    transact.exec0(R"crwebcookietab(
---- TABLE tb_web_cookie
CREATE TABLE IF NOT EXISTS tb_web_cookie (
  wcookie_id SERIAL PRIMARY  KEY NOT NULL, -- unique key in this table
  wcookie_random CHAR(24) NOT NULL,        -- a random key, hopefully usually unique
  wcookie_exptime TIMESTAMP NOT NULL,      -- the cookie expiration time
  wcookie_webagenthash INT NOT NULL        -- a quick hashcode of the browser's User-Agent:
); --- end TABLE tb_web_cookie
)crwebcookietab");
    transact.exec0(R"crcookierandomix(
---- INDEX ix_cookie_random
  CREATE INDEX IF NOT EXISTS ix_cookie_random
    ON tb_web_cookie(wcookie_random);
--- end INDEX ix_cookie_random
)crcookierandomix");
    transact.exec0(R"crcookietimeix(
---- INDEX ix_cookie_exptime
  CREATE INDEX IF NOT EXISTS ix_cookie_exptime
    ON tb_web_cookie(wcookie_exptime);
--- end INDEX ix_cookie_exptime
)crcookietimeix");
    transact.commit();
  }
  HCV_DEBUGOUT("hcv_initialize_database before preparing statements in " << connstr);
  hcv_prepare_statements_in_database();
  hcv_initialize_plugins_for_database(hcv_dbconn.get());
  HCV_SYSLOGOUT(LOG_NOTICE, "PostGreSQL database " << connstr << " successfully initialized");
} // end hcv_initialize_database


static void
prepare_user_model_statements(void)
{
  // user_crtime is updated by default
  hcv_database_register_prepared_statement
    ("user_create_pstm",
     "INSERT INTO tb_user (user_firstname, user_familyname, user_email, user_gender "
     " VALUES ($1, $2, $3, $4);");

  hcv_database_register_prepared_statement("user_get_password_by_email_pstm",
      "SELECT passwd_encr FROM tb_password WHERE passw_userid = "
      "(SELECT user_id WHERE user_email = $1) ORDER BY passw_mtime DESC"
      " LIMIT 1;");
} // end prepare_user_model_statements


/// https://libpqxx.readthedocs.io/en/stable/a01331.html
/// https://www.tutorialspoint.com/postgresql/postgresql_c_cpp.htm
void
hcv_prepare_statements_in_database(void)
{
  std::lock_guard<std::recursive_mutex> gu(hcv_dbmtx);
  ////// find a user by his/her email
  HCV_DEBUGOUT("preparing find_user_by_email_pstm");
  hcv_dbconn->prepare
    ("find_user_by_email_pstm",
     R"finduseremail(
SELECT user_id FROM tb_user WHERE user_email=$1
)finduseremail");
  ///// insert a fresh web cookie; see also for LASTVAL
  ///// https://www.postgresql.org/docs/current/functions-sequence.html
  hcv_dbconn->prepare
    ("add_web_cookie_pstm",
     R"addwebcookie(
INSERT INTO tb_web_cookie
     (wcookie_random, wcookie_exptime, wcookie_webagenthash)
VALUES ($1, to_timestamp($2), $3)
)addwebcookie");
  hcv_dbconn->prepare
    ("lastval_pstm", "SELECT LASTVAL()");
  prepare_user_model_statements();
} // end hcv_prepare_statements_in_database




void
hcv_database_register_prepared_statement(const std::string& name, 
                                         const std::string& sql)
{
    HCV_DEBUGOUT("Registering prepared SQL statement " << name);

    std::lock_guard<std::recursive_mutex> guard(hcv_dbmtx);
    hcv_dbconn->prepare(name, sql);
} // end hcv_database_register_prepared_statement






// https://www.postgresqltutorial.com/postgresql-where/
// https://www.postgresql.org/docs/current/sql-prepare.html
bool
hcv_database_with_known_email (const std::string& emailstr)
{
  HCV_DEBUGOUT("hcv_database_with_known_email: emailstr='" << emailstr << "'");
  if (emailstr.empty() || emailstr.find('@') == std::string::npos)
    {
      HCV_DEBUGOUT("hcv_database_with_known_email bad email" << emailstr);
      return false;
    }
  std::lock_guard<std::recursive_mutex> gu(hcv_dbmtx);
  pqxx::work transact(*hcv_dbconn);
  pqxx::result res = transact.exec_prepared("find_user_by_email_pstm", emailstr);
  long id = -1;
  for (auto rowit : res) {
    id = rowit[0].as<long>();
  }
  transact.commit();
  return id>0;
} // end hcv_database_with_known_email

long
hcv_database_get_id_of_added_web_cookie(const std::string& randomstr, time_t exptime, int webagenthash)
{
  long id = -1;
  HCV_ASSERT(!randomstr.empty());
  HCV_DEBUGOUT("hcv_database_get_id_of_added_web_cookie start randomstr='"
	       << randomstr << " exptime=" << exptime
	       << " webagenthash=" << webagenthash);
  std::lock_guard<std::recursive_mutex> gu(hcv_dbmtx);
  try {
  pqxx::work transact(*hcv_dbconn);
  HCV_DEBUGOUT("hcv_database_get_id_of_added_web_cookie before add_web_cookie_pstm randomstr="
	       << randomstr);
  pqxx::result res;
  res = transact.exec_prepared("add_web_cookie_pstm",
			       randomstr, (double)exptime, webagenthash);
  HCV_DEBUGOUT("hcv_database_get_id_of_added_web_cookie add_web_cookie_pstm res size:"
	       << res.size() << " affected rows:" << res.affected_rows());
  res = transact.exec_prepared("lastval_pstm");
  HCV_DEBUGOUT("hcv_database_get_id_of_added_web_cookie lastval_pstm res size:"
	       << res.size());
  for (auto rowit : res) {
    id = rowit[0].as<long>();
  }
  transact.commit();
  HCV_DEBUGOUT("hcv_database_get_id_of_added_web_cookie randomstr='"
	       << randomstr << "', exptime=" << exptime
	       << ", webagenthash=" << webagenthash
	       << " => id=" << id);
  } catch (std::exception& exc) {
    HCV_SYSLOGOUT(LOG_WARNING,
		  "hcv_database_get_id_of_added_web_cookie got exception:"
		  << exc.what());
    id = -2;
  }
  return id;
} // end hcv_database_get_id_of_added_web_cookie



void
hcv_close_database(void)
{
  HCV_DEBUGOUT("hcv_close_database start");
  std::lock_guard<std::recursive_mutex> gu(hcv_dbmtx);
  HCV_ASSERT(hcv_dbconn);
  std::string dbnamestr(hcv_dbconn->dbname());
  hcv_dbconn.reset(nullptr);
  HCV_SYSLOGOUT(LOG_NOTICE, "closed database " << dbnamestr);
} // end hcv_close_database

/////////// end of file hcv_database.cc in github.com/bstarynk/helpcovid
