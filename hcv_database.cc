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

extern "C" std::atomic<long> hcv_database_serial;
std::atomic<long> hcv_database_serial;
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


// Enumerator for user status types
static void
sql_en_status(pqxx::work& transact)
{
    transact.exec0(R"sqlenstatus(
        DO $$
        BEGIN
            CREATE TYPE en_status as enum(
                    'STATUS_REJECTED',
                    'STATUS_EXPIRED',
                    'STATUS_PENDING',
                    'STATUS_VERIFIED');
        EXCEPTION
            WHEN duplicate_object THEN NULL;
        END
        $$ language plpgsql;
    )sqlenstatus");
}


// Enumerator for gender types. We need to be flexible about gender because
// different legal jurisdictions may recognise genders other than the
// standard ones
static void
sql_en_gender(pqxx::work& transact)
{
    transact.exec0(R"sqlengender(
        DO $$
        BEGIN
            CREATE TYPE en_gender as enum(
                    'GENDER_MALE',
                    'GENDER_FEMALE',
                    'GENDER_OTHER',
                    'GENDER_UNDISCLOSED');
        EXCEPTION
            WHEN duplicate_object THEN NULL;
        END
        $$ language plpgsql;
    )sqlengender");
} // end sql_en_gender


static void
sql_tb_user(pqxx::work& transact)
{
  transact.exec0(R"crusertab(
        CREATE TABLE IF NOT EXISTS tb_user (
            user_id SERIAL PRIMARY KEY NOT NULL,
            user_firstname VARCHAR(31) NOT NULL,
            user_familyname VARCHAR(62) NOT NULL,
            user_email VARCHAR(71) NOT NULL,
            user_telephone VARCHAR(23) NOT NULL,
            user_gender en_gender NOT NULL,
            user_status en_status NOT NULL DEFAULT 'STATUS_PENDING',
            user_crtime TIMESTAMP DEFAULT current_timestamp
        );
        CREATE INDEX IF NOT EXISTS ix_user_familyname 
            ON tb_user(user_familyname);
        CREATE INDEX IF NOT EXISTS ix_user_email 
            ON tb_user(user_email);
        CREATE INDEX IF NOT EXISTS ix_user_crtime 
            ON tb_user(user_crtime);
    )crusertab");
} // end sql_tb_user


static void
sql_tb_email_confirmation(pqxx::work& transact)
{
  transact.exec0(R"confsql(
        CREATE TABLE IF NOT EXISTS tb_email_confirmation(
            confirm_id serial PRIMARY KEY NOT NULL,
            confirm_userid INTEGER NOT NULL REFERENCES tb_user(user_id) 
                ON DELETE CASCADE,
            confirm_token UUID NOT NULL,
            confirm_expiry TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP 
                + interval '1 hour'
        );
    )confsql");
} // end sql_tb_email_confirmation


static void
sql_tb_password(pqxx::work& transact)
{
  transact.exec0(R"sqlpass(
        CREATE TABLE IF NOT EXISTS tb_password(
            passw_id SERIAL PRIMARY KEY NOT NULL,
            passw_userid INT NOT NULL,
            passw_encr TEXT NOT NULL,
            passw_mtime TIMESTAMP DEFAULT current_timestamp 
        );
    )sqlpass");
} // end sql_tb_password


////================ web cookie table, related to web cookies
// see https://www.postgresql.org/docs/current/datatype-net-types.html
// read https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
// see http://man7.org/linux/man-pages/man2/getsockname.2.html
// see http://man7.org/linux/man-pages/man7/ip.7.html
// and http://man7.org/linux/man-pages/man7/ipv6.7.html
/// we are aware that the browser IP is unreliable information
/// see https://stackoverflow.com/q/527638/841108
static void
sql_tb_web_cookie(pqxx::work& transact)
{
  transact.exec0(R"sqlweb(
        CREATE TABLE IF NOT EXISTS tb_web_cookie(
            wcookie_id SERIAL PRIMARY  KEY NOT NULL,
            wcookie_random CHAR(24) NOT NULL,
            wcookie_exptime TIMESTAMP NOT NULL,
            wcookie_webagenthash INT NOT NULL
        );
        CREATE INDEX IF NOT EXISTS ix_cookie_random
            ON tb_web_cookie(wcookie_random);
        CREATE INDEX IF NOT EXISTS ix_cookie_exptime
            on tb_web_cookie(wcookie_exptime);
    )sqlweb");
} // end sql_tb_web_cookie


// Session table
static void
sql_tb_session(pqxx::work& transact)
{
    transact.exec0(R"sqltbsession(
        CREATE TABLE IF NOT EXISTS tb_session(
                id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
                user_id INT NOT NULL REFERENCES tb_user(user_id) ON DELETE CASCADE,
                http_ua TEXT NOT NULL,
                ip INET NOT NULL,
                start TIMESTAMP NOT NULL DEFAULT now(),
                expiry TIMESTAMP NOT NULL DEFAULT now() + interval '1 hour');
    )sqltbsession");
}


// the create_new_user() SQL function creates a new user, taking care to
// ensure that the password is encrypted with an MD5 hashed salt.
static void
sql_create_new_user(pqxx::work& transact)
{
  transact.exec0(R"sqlcruser(
        CREATE OR REPLACE FUNCTION create_new_user(
                p_email varchar, 
                p_password text, 
                p_firstname varchar, 
                p_familyname varchar, 
                p_telephone varchar, 
                p_gender en_gender) RETURNS integer 
        as $func$
        BEGIN
            INSERT INTO tb_user(
                    user_firstname, 
                    user_familyname, 
                    user_email,
                    user_telephone, 
                    user_gender) VALUES (
                    p_firstname, 
                    p_familyname,
                    p_email,
                    p_telephone, 
                    p_gender);

            INSERT INTO tb_password(
                    passw_userid, 
                    passw_encr) values(
                    (SELECT user_id from tb_user where user_email = p_email),
                    crypt(p_password, gen_salt('md5')));

            RETURN (SELECT user_id FROM tb_user WHERE user_email = _email);
        end; 
        $func$ language plpgsql;
    )sqlcruser");
} // end sql_create_new_user


// the verify_email_token() stored function determines if a UUID e-mail token
// generated at the time of registration is valid. This function is used to
// verify a user's e-mail address.
static void
sql_verify_email_token(pqxx::work& transact)
{
  transact.exec0(R"sqlverifemail(
        CREATE OR REPLACE FUNCTION verify_email_token(
            _user_id integer,
            _token uuid) RETURNS integer
        as $func$
        DECLARE
            expires timestamp;
            check_id integer;
        BEGIN
            SELECT expiry FROM tb_email_confirmation where user_id = _user_id
                into expires;

            IF NOT FOUND THEN
                RAISE EXCEPTION 'e-mail token not generated';
            END IF;

            IF expires >= current_timestamp THEN
                UPDATE tb_user 
                    set user_status = get_status_id('STATUS_EXPIRED');
                RETURN get_status_id('STATUS_EXPIRED');
            END IF;

            SELECT user_id FROM tb_email_confirmation WHERE token = _token
                INTO check_id;

            IF check_id = _userid THEN
                UPDATE tb_user 
                    SET user_status = get_status_id('STATUS_VERIFIED');
                DELETE FROM tb_email_confirmation WHERE user_id = _user_id;
                RETURN get_status_id('STATUS_VERIFIED');
            ELSE
                UPDATE tb_user 
                    SET user_status = get_status_id('STATUS_REJECTED');
                RETURN get_status_id('STATUS_REJECTED');
            END IF;
        END;		    
        $func$ language plpgsql;
    )sqlverifemail");
} // end sql_verify_email_token


static void
sql_get_email_verification_token(pqxx::work& transact)
{
  transact.exec0(R"sqlgetemtok(
        CREATE OR REPLACE FUNCTION get_email_verification_token(
            _user_id integer) RETURNS uuid
        as $func$
        DECLARE
            check_id integer;
        BEGIN
            SELECT user_id FROM tb_user where user_id = _user_id INTO check_id;
            IF NOT check_id = _user_id THEN
                RAISE EXCEPTION 'user ID does not exist: %', _user_id;
            END IF;

            RETURN(SELECT token FROM tb_email_confirmation 
                where user_id = _user_id);

            IF NOT FOUND THEN
                INSERT INTO tb_email_confirmation(user_id, token) VALUES(
                    _user_id, gen_random_uuid());
                RETURN(SELECT token FROM tb_email_confirmation
                    WHERE user_id = _user_id);
            END IF;
        END;
        $func$ language plpgsql;
    )sqlgetemtok");
} // end sql_get_email_verification_token


// procedure to identify and delete dormant users who have not signed in
// for more than 3 months
static void
sql_delete_dormant_users(pqxx::work& transact)
{
    transact.exec0(R"sqldeldormusers(
        CREATE OR REPLACE PROCEDURE delete_dormant_users()
        AS $$
        BEGIN
            DELETE FROM tb_user WHERE user_id IN (SELECT user_id FROM (
                    SELECT DISTINCT ON (user_id) user_id, expiry FROM tb_session
                    ORDER BY user_id, expiry DESC) AS latest_session
                    WHERE expiry < now() - interval '3 months');
        END;
        $$ LANGUAGE plpgsql;
    )sqldeldormusers");
}


/// We create and fill a tiny SQL table tb_helpcovidinstance to
/// register this particular instance of helpcovid process in the
/// database.  This might be useful in large scale HelpCovid
/// deployments, where a single database is shared between several
/// helpcovid processes running on different web servers declared with
/// the same hostname at the DNS.
static void
sql_register_helpcovid_instance(pqxx::work& transact)
{
  HCV_DEBUGOUT("sql_register_helpcovid_instance starting");
  hcv_database_serial.store(-1);
  /// first, create the table of running helpcovid instances in the database
  /// and a few indexes inside it
  transact.exec0(R"sqlcrhelpcovid(
CREATE TABLE IF NOT EXISTS tb_helpcovidinstance (
hcvinst_id SERIAL PRIMARY KEY          -- unique serial
           NOT NULL,
hcvinst_host VARCHAR(80) NOT NULL,     -- the hostname running helpcovid
hcvinst_pid  INTEGER NOT NULL,         -- the Linux process id
hcvinst_execelf VARCHAR(80) NOT NULL,  -- the path of the Linux ELF executable
hcvinst_startime TIMESTAMP             -- the start time of the process
    DEFAULT current_timestamp,
hcvinst_buildtime TIMESTAMP NOT NULL,  -- the executable build time from hcv_timelong
hcvinst_cwd  VARCHAR(255) NOT NULL,    -- the current working directory from getcwd(3)
hcvinst_topdir VARCHAR(255) NOT NULL,  -- the top directory from hcv_topdirectory
hcvinst_gitid VARCHAR(63) NOT NULL,    -- the git id from hcv_gitid
hcvinst_lastgitcommit VARCHAR(383)     -- from hcv_lastgitcommit
    NOT NULL,
hcvinst_md5sum VARCHAR(47) NOT NULL,   -- the md5sum from hcv_md5sum
hcvinst_linuxuid INTEGER NOT NULL,     -- from getuid(2)
hcvinst_linuxeuid INTEGER NOT NULL,    -- from geteuid(2)
hcvinst_linuxuser VARCHAR(15)          -- from getuid(2) then getpwuid(3)
    NOT NULL,
hcvinst_linuxeffuser VARCHAR(15)       -- from geteuid(2) then getpwuid(3)
    NOT NULL,
hcvinst_linuxgid INTEGER NOT NULL,     -- from getgid(2)
hcvinst_linuxegid INTEGER NOT NULL     -- from getegid(2)
    NOT NULL,
hcvinst_compiler_version VARCHAR(80)   -- from hcv_cxx_compiler
    NOT NULL 
); --------- end of table tb_helpcovidinstance

CREATE INDEX IF NOT EXISTS ix_helpcovinst_hostpid 
    ON tb_helpcovidinstance(hcvinst_host, hcvinst_pid);
CREATE INDEX IF NOT EXISTS ix_helpcovinst_linuxuser 
    ON tb_helpcovidinstance(hcvinst_linuxuid, hcvinst_linuxuser);
CREATE INDEX IF NOT EXISTS ix_helpcovinst_build
    ON tb_helpcovidinstance(hcvinst_buildtime, hcvinst_gitid);
CREATE INDEX IF NOT EXISTS ix_helpcovinst_meta
    ON tb_helpcovidinstance(hcvinst_gitid, hcvinst_lastgitcommit);
CREATE INDEX IF NOT EXISTS ix_helpcovinst_md5
    ON tb_helpcovidinstance(hcvinst_md5sum);
------------------------ end of indexes related to tb_helpcovidinstance
)sqlcrhelpcovid");
  long helpcovidinstserial = -1;
  ////////////////
  ///// insert into tb_helpcovidinstance a single row describing the current helpcovid process.
  char curexepath[200];
  memset (curexepath, 0, sizeof(curexepath));
  auto lsz = readlink("/proc/self/exe", curexepath, sizeof(curexepath));
  if (lsz < 0 || (int)lsz >= (int)sizeof(curexepath)-2)
    HCV_FATALOUT("sql_register_helpcovid_instance failed to readlink(2) /proc/self/exe");
  char cwdpath[128];
  memset (cwdpath, 0, sizeof(cwdpath));
  if (!getcwd(cwdpath, sizeof(cwdpath)-1))
    HCV_FATALOUT("sql_register_helpcovid_instance failed to getcwd");
  time_t nowt = 0;
  time(&nowt);
  uid_t myuid = getuid();
  uid_t myefuid= geteuid();
  gid_t mygid= getgid();
  gid_t myefgid= getegid();  
  HCV_DEBUGOUT("sql_register_helpcovid_instance before insertion nowt=" << (long)nowt
	       << ", timelong=" << (long)hcv_timelong
	       << ", myuid=" << (int)myuid
	       << ", myefuid=" << (int)myefuid
	       << ", mygid=" << (int)mygid
	       << ", myefgid=" << (int)myefgid
	       << ", curexepath=" << curexepath
	       << ", cwdpath=" << cwdpath);
  std::string uidstr;
  std::string efuidstr;
  {
  std::ostringstream osql;
  osql << R"sqlinsthelpcovid(
INSERT INTO tb_helpcovidinstance
    (hcvinst_host, hcvinst_pid, hcvinst_execelf, 
            hcvinst_startime, hcvinst_buildtime,
            hcvinst_cwd, hcvinst_topdir,
	    hcvinst_gitid, hcvinst_lastgitcommit,
	    hcvinst_md5sum,
            hcvinst_linuxuid, hcvinst_linuxeuid,
            hcvinst_linuxuser, hcvinst_linuxeffuser,
            hcvinst_linuxgid, hcvinst_linuxegid,
	    hcvinst_compiler_version)
)sqlinsthelpcovid";
  osql << std::endl << " VALUES ("
       << transact.quote(hcv_get_hostname()) << ", "  //= hcvinst_host
       << (int)getpid() << ", " //= hcvinst_pid
       << transact.quote(std::string{curexepath}) << "," //= hcvinst_execelf
       << std::endl << " ---  @" << __FILE__ << ":" << __LINE__ << std::endl
       << " to_timestamp(" << (long)nowt <<"), " //=hcvinst_startime
       << " to_timestamp(" << (long)hcv_timelong << "), "
       << transact.quote(std::string{cwdpath}) << ", "
       << transact.quote(std::string{hcv_topdirectory}) << ", -- @! " << __FILE__ << ":" << __LINE__ << std::endl
       << " " << transact.quote(std::string{hcv_gitid}) << "," << std::endl
       << " " << transact.quote(std::string{hcv_lastgitcommit}) << "," << std::endl
       << " " << transact.quote(std::string{hcv_md5sum}) << ","
       << std::endl << " ---  @" << __FILE__ << ":" << __LINE__ << std::endl
       << (int)myuid << ", " //=hcvinst_linuxuid,
       << (int)myefuid << ", "  //=hcvinst_linuxeuid
       << transact.quote(uidstr) << ", " //= hcvinst_linuxuser, hcvinst_linuxeffuser
       << transact.quote(efuidstr) << ","
       << std::endl << " ---  @" << __FILE__ << ":" << __LINE__ << std::endl
       << (int)mygid << ", "  //=hcvinst_linuxgid, hcvinst_linuxegid
       << (int)myefgid << ", "
       << transact.quote(std::string{hcv_cxx_compiler}) //=hcvinst_compiler_version
       << " )"
       << std::endl;
  std::string osqlstr = osql.str();
  HCV_DEBUGOUT("sql_register_helpcovid_instance osqlstr::" << std::endl
	       << osqlstr << std::endl << "!!!end osqlstr" << std::endl);
  transact.exec0(osqlstr);
  }
  pqxx::row r2serial = transact.exec1("SELECT LASTVAL();");
  helpcovidinstserial = r2serial[0].as<long>();
  ///
  HCV_SYSLOGOUT(LOG_INFO, "sql_register_helpcovid_instance completed, serial#" << helpcovidinstserial);
  hcv_database_serial.store(helpcovidinstserial);
} // end sql_register_helpcovid_instance


static void
sql_unregister_helpcovid_instance(pqxx::work& transact)
{
  auto serial = hcv_database_serial.load();
  HCV_DEBUGOUT("sql_unregister_helpcovid_instance starting serial#" << serial);
  if (serial > 0) {
    std::ostringstream osql;
    osql << "DELETE FROM tb_helpcovidinstance WHERE hcvinst_id = " << serial << std::endl;
    std::string sqlstr = osql.str();
    HCV_DEBUGOUT("sql_unregister_helpcovid_instance sqlstr:" << std::endl << sqlstr  << std::endl);
    hcv_database_serial.store(0);
    transact.exec(sqlstr);
    HCV_DEBUGOUT("sql_unregister_helpcovid_instance deleted serial#" << serial);
  }
} // end sql_unregister_helpcovid_instance


////////////////////////////////////////////////////////////////
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

    sql_en_status(transact);
    sql_en_gender(transact);
    sql_tb_user(transact);
    sql_tb_email_confirmation(transact);
    sql_tb_password(transact);
    sql_tb_web_cookie(transact);
    sql_tb_session(transact);
    sql_create_new_user(transact);
    sql_verify_email_token(transact);
    sql_get_email_verification_token(transact);
    sql_delete_dormant_users(transact);
    sql_register_helpcovid_instance(transact);

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
  HCV_DEBUGOUT("hcv_close_database dbnamestr=" << dbnamestr);
  {
    pqxx::work finaltransact(*hcv_dbconn);
    sql_unregister_helpcovid_instance(finaltransact);
    finaltransact.commit();
  }
  HCV_DEBUGOUT("hcv_close_database before resetting database connection");
  hcv_dbconn.reset(nullptr);
  HCV_SYSLOGOUT(LOG_NOTICE, "closed database " << dbnamestr);
} // end hcv_close_database

/////////// end of file hcv_database.cc in github.com/bstarynk/helpcovid
