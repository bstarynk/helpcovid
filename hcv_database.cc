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

extern "C" const char hcv_database_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_database_date[] = __DATE__;

/// the database connection
std::unique_ptr<pqxx::connection> hcv_dbconn;

/// the short PostGreSQL server version
std::string hcv_our_postgresql_server_version;

/// the recursive mutex to serialize access to that database
std::recursive_mutex hcv_dbmtx;

extern "C" void hcv_prepare_statements_in_database(void);

const std::string
hcv_postgresql_version(void)
{
  return hcv_our_postgresql_server_version;
} // end  hcv_postgresql_version

void
hcv_initialize_database(const std::string&uri)
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
    ////================ query the PostGreSQL version
    {
      pqxx::work firsttransact(*hcv_dbconn);
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
  user_gender CHAR(1) NOT NULL,         -- 'F' | 'M' | '?'
  user_crtime DATE NOT NULL             -- user entry creation time
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
  passw_encr VARCHAR(62) NOT NULL,      -- the encrypted password
  passw_mtime  TIMESTAMP NOT NULL       -- the last time that password was modified
); --- end TABLE tb_password
)crpasswdtab");
    ////================ web cookie table, related to web cookies
    // see https://www.postgresql.org/docs/current/datatype-net-types.html
    // read https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
    // see http://man7.org/linux/man-pages/man2/getsockname.2.html
    // see http://man7.org/linux/man-pages/man7/ip.7.html
    // and http://man7.org/linux/man-pages/man7/ipv6.7.html
    transact.exec0(R"crwebcookietab(
---- TABLE tb_web_cookie
CREATE TABLE IF NOT EXISTS tb_web_cookie (
  wcookie_id SERIAL PRIMARY  KEY NOT NULL, -- unique key in this table
  wcookie_random CHAR(24) NOT NULL,        -- a random key, hopefully usually unique
  wcookie_exptime TIMESTAMP NOT NULL,      -- the cookie expiration time
  wcookie_webagenthash INT NOT NULL,       -- a quick hashcode of the browser's User-Agent:
  wcookie_ipaddr INET NOT NULL,            -- the browser IP address (beware of NAT)
); --- end TABLE tb_web_cookie
)crwebcookietab");
    transact.commit();
  }
  HCV_DEBUGOUT("hcv_initialize_database before preparing statements in " << connstr);
  hcv_prepare_statements_in_database();
  HCV_SYSLOGOUT(LOG_NOTICE, "PostGreSQL database " << connstr << " successfully initialized");
} // end hcv_initialize_database


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
} // end hcv_prepare_statements_in_database


void
hcv_database_register_prepared_statement(const std::string& name, 
                                         const std::string& sql)
{
    HCV_DEBUGOUT("Registering prepared SQL statement " << name);

    std::lock_guard<std::recursive_mutex> guard(hcv_dbmtx);
    hcv_dbconn->prepare(name, sql);
}


// https://www.postgresqltutorial.com/postgresql-where/
// https://www.postgresql.org/docs/current/sql-prepare.html
#warning we need to use PostGreSQL prepared statements 
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



/////////// end of file hcv_database.cc in github.com/bstarynk/helpcovid
