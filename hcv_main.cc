/****************************************************************
 * file hcv_main.cc
 *
 * Description:
 *      Main of https://github.com/bstarynk/helpcovid
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

extern "C" const char hcv_main_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_main_date[] = __DATE__;

std::recursive_mutex hcv_fatalmtx;
std::recursive_mutex hcv_syslogmtx;

char hcv_startimbuf[80];
std::atomic<bool> hcv_debugging;
double hcv_monotonic_start_time;
unsigned hcv_http_max_threads = 8;
unsigned hcv_http_payload_max = 16*1024*1024;
bool hcv_should_clear_database;

/// the email command to send HTML5 emails  is popen-ed as <command> <subject> <to_addr> ....
/// see also https://unix.stackexchange.com/a/15463/50557
std::string hcv_email_command;


thread_local Hcv_Random Hcv_Random::_rand_thr_;

static int hcv_main_argc;
static char** hcv_main_argv;

static void hcv_syslog_program_arguments(void);

extern "C" void hcv_load_config_file(const char*);
////////////////////////////////////////////////////////////////
/// https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html

enum hcv_progoption_en
{
  HCVPROGOPT__NONE=0,
  HCVPROGOPT_WEBURL='W',
  HCVPROGOPT_POSTGRESURI='P',
  HCVPROGOPT_SYSLOG='S',
  HCVPROGOPT_WEBROOT='R',
  HCVPROGOPT_SETEUID='U',
  HCVPROGOPT_CONFIG='C',
  HCVPROGOPT_THREADS='T',
  HCVPROGOPT_WRITEPID='p',
  HCVPROGOPT_DEBUG = 'D',

  HCVPROGOPT_WEBSSLCERT=1000,
  HCVPROGOPT_WEBSSLKEY=1001,
  HCVPROGOPT_PLUGIN=1002,
  HCVPROGOPT_CLEARDATABASE=1003,
};

struct argp_option hcv_progoptions[] =
{
  /* ======= set the served WEB url ======= */
  {/*name:*/ "web-url", ///
    /*key:*/ HCVPROGOPT_WEBURL, ///
    /*arg:*/ "WEBURL", ///
    /*flags:*/ 0, ///
    /*doc:*/ "sets the served WEB url (defaults to $HELPCOVID_URL),"
    "  e.g. --web-url=https://myexample.com/helpcovid/, config: web/url", ///
    /*group:*/0 ///
  },
  /* ======= set the PostgreSQL database URI ======= */
  {/*name:*/ "postgresql-database", ///
    /*key:*/ HCVPROGOPT_POSTGRESURI, ///
    /*arg:*/ "POSTGRESQLURI", ///
    /*flags:*/0, ///
    /*doc:*/ "sets the PostgreSQL database URI (defaults to $HELPCOVID_POSTGRESQL),"
    " e.g. --postgresql-database=postgresql://www-data@localhost/helpcovid, config: postgresql/connection; use - to disable PostGreSQL (useless except for debugging purposes)", ///
    /*group:*/0 ///
  },
  /* ======= set the syslog level ======= */
  {/*name:*/ "syslog-level", ///
    /*key:*/ HCVPROGOPT_SYSLOG, ///
    /*arg:*/ "SYSLOGLEVEL", ///
    /*flags:*/0, ///
    /*doc:*/ "sets the syslog level to SYSLOGLEVEL for openlog(3) (defaults to 0 for LOG_LOCAL0), between 0 and 7, e.g. --syslog-level=1", ///
    /*group:*/0 ///
  },
  /* ======= set the number of worker threads ======= */
  {/*name:*/ "threads", ///
    /*key:*/ HCVPROGOPT_THREADS, ///
    /*arg:*/ "NBWORKERTHREADS", ///
    /*flags:*/0, ///
    /*doc:*/ "sets the number of worker threads, (defaults to $HELPCOVID_NBWORKERTHREADS), config: helpcovid/threads", ///
    /*group:*/0 ///
  },
  /* ======= set the web document root ======= */
  {/*name:*/ "webroot", ///
    /*key:*/ HCVPROGOPT_WEBROOT, ///
    /*arg:*/ "WEBROOT", ///
    /*flags:*/0, ///
    /*doc:*/ "default webroot local file directory to serve static URLs without ..\n"
    " (defaults to $HELPCOVID_WEBROOT), config: web/root", ///
    /*group:*/0 ///
  },
  /* ======= set the configuration file ======= */
  {/*name:*/ "config", ///
    /*key:*/ HCVPROGOPT_CONFIG, ///
    /*arg:*/ "CONFIGFILE", ///
    /*flags:*/0, ///
    /*doc:*/ "default configuration key file (defaults to $HELPCOVID_CONFIG or else " HCV_DEFAULT_CONFIG_PATH ")", ///
    /*group:*/0 ///
  },
  /* ======= set the web OpenSSL certificate ======= */
  {/*name:*/ "websslcert", ///
    /*key:*/ HCVPROGOPT_WEBSSLCERT, ///
    /*arg:*/ "WEBSSLCERT", ///
    /*flags:*/0, ///
    /*doc:*/ "web OpenSSL certificate file, e.g. --websslcert=/etc/helpcovid/somecert.pem"
    " (defaults to $HELPCOVID_SSLCERT), config: web/sslcert", ///
    /*group:*/0 ///
  },
  /* ======= set the web OpenSSL private key ======= */
  {/*name:*/ "websslkey", ///
    /*key:*/ HCVPROGOPT_WEBSSLKEY, ///
    /*arg:*/ "WEBSSLKEY", ///
    /*flags:*/0, ///
    /*doc:*/ "web OpenSSL private key, e.g. --websslkey=/etc/helpcovid/somekey.pem"
    " (defaults to $HELPCOVID_SSLKEY), config: web/sslkey", ///
    /*group:*/0 ///
  },
  /* ======= set the effective user id ======= */
  {/*name:*/ "seteuid", ///
    /*key:*/ HCVPROGOPT_SETEUID, ///
    /*arg:*/ "EFFECTIVEUSER", ///
    /*flags:*/0, ///
    /*doc:*/ "call seteuid(2) on given user name or numerical id"
    " (defaults to $HELPCOVID_SETEUID), config: helpcovid/seteuid."
    " Huge cybersecurity risk if badly used...", ///
    /*group:*/0 ///
  },
  /* ======= set the pid file ======= */
  {/*name:*/ "write-pid", ///
    /*key:*/ HCVPROGOPT_WRITEPID, ///
    /*arg:*/ "PIDFILE", ///
    /*flags:*/0, ///
    /*doc:*/ "write the current pid to the given file"
    " (defaults to $HELPCOVID_PIDFILE or else /var/run/helpcovid.pid), config: helpcovid/pid_file", ///
    /*group:*/0 ///
  },
  /* ======= enable debug messages ======= */
  {/*name:*/ "debug", ///
    /*key:*/ HCVPROGOPT_DEBUG, ///
    /*arg:*/ nullptr, ///
    /*flags:*/0, ///
    /*doc:*/ "write debug messages to syslog(LOG_DEBUG, ...)", ///
    /*group:*/0 ///
  },
  /* ======= enable clearing of every database table  ======= */
  {/*name:*/ "clear-database", ///
    /*key:*/ HCVPROGOPT_CLEARDATABASE, ///
    /*arg:*/ nullptr, ///
    /*flags:*/0, ///
    /*doc:*/ "clear database entirely", ///
    /*group:*/0 ///
  },
  /* ======= load a plugin ======= */
  {/*name:*/ "plugin", ///
    /*key:*/ HCVPROGOPT_PLUGIN, ///
    /*arg:*/ "PLUGIN_NAME", ///
    /*flags:*/0, ///
    /*doc:*/ "dlopen a given plugin named PLUGIN_NAME and optional string argument\n"
    " ... for example --plugin=foo would dlopen(3) hcvplugin_foo.so\n"
    " ... and --plugin=bar_2:argbar would dlopen(3) hcvplugin_bar_2.so"
    " and pass argument 'argbar' to it.\n",
    /*group:*/0 ///
  },
  /* ======= terminating empty option ======= */
  {/*name:*/(const char*)0, ///
    /*key:*/0, ///
    /*arg:*/(const char*)0, ///
    /*flags:*/0, ///
    /*doc:*/(const char*)0, ///
    /*group:*/0 ///
  }
};

#define HCV_PROGARG_MAGIC 722486817 /*0x2b104621*/
struct hcv_progarguments
{
  unsigned hcvprog_magic; // always HCV_PROGARG_MAGIC
  std::string hcvprog_progname;
  std::string hcvprog_weburl;
  std::string hcvprog_config;
  std::string hcvprog_postgresuri;
  std::string hcvprog_webroot;
  std::string hcvprog_seteuid;
  std::string hcvprog_opensslcert;
  std::string hcvprog_opensslkey;
  std::string hcvprog_pidfile;
};

static struct hcv_progarguments hcv_progargs =
{
  .hcvprog_magic = HCV_PROGARG_MAGIC,
  .hcvprog_progname = "",
  .hcvprog_weburl = "",
  .hcvprog_config = "",
  .hcvprog_postgresuri = "",
  .hcvprog_webroot = "",
  .hcvprog_seteuid = "",
  .hcvprog_opensslcert = "",
  .hcvprog_opensslkey = "",
  .hcvprog_pidfile = "",
};

static char hcv_hostname[64];
static char hcv_versionmsg[384];

static void* hcv_proghandle;
void hcv_fatal_stop_at (const char *fil, int lin, int err)
{
  std::clog << "**** FATAL ERROR " << fil << ":" << lin << std::endl;
  if (err>0)
    std::clog << " errno: " << strerror(err) << std::endl;
  syslog(LOG_EMERG, "FATAL STOP %s:%d (%s)\n"
         "* version %s",
         fil, lin, strerror(err), hcv_versionmsg);
  closelog();
  abort();
} // end hcv_fatal_stop_at

void
hcv_debug_at (const char*fil, int lin, std::ostringstream&outs)
{
  static std::recursive_mutex dbgmtx;
  static long dbgcnt;
  outs.flush();
  if (!fil) fil="??";
  if (fil)
    {
      auto ls = strrchr(fil, '/');
      if (ls && ls[1]) fil = ls+1;
    };
  std::lock_guard<std::recursive_mutex> gu(dbgmtx);
  dbgcnt++;
  auto elapsed = hcv_monotonic_real_time() - hcv_monotonic_start_time;
  if (dbgcnt % 100 == 0)
    {
      time_t nowt = 0;
      time(&nowt);
      struct tm nowtm = {};
      localtime_r(&nowt, &nowtm);
      char timbuf[64];
      strftime(timbuf, sizeof(timbuf), "%c %Z", &nowtm);
      syslog(LOG_DEBUG, "========== DEBUG timestamp %s #%ld ==========", timbuf, dbgcnt);
    }
  // we use the Δ U+0394 GREEK CAPITAL LETTER DELTA and ▪ U+25AA BLACK SMALL SQUARE and ‣ U+2023 TRIANGULAR BULLET
  syslog(LOG_DEBUG, "ΔBG!%s:%d▪ %05.2f s‣ %s", fil, lin, elapsed, outs.str().c_str());
} // end hcv_debug_at

// parse a single program option
static error_t
hcv_parse1opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  if (!state->input)
    HCV_FATALOUT("state->input is NULL");
  struct hcv_progarguments *progargs
    = reinterpret_cast<hcv_progarguments *>(state->input);
  if (!progargs || progargs->hcvprog_magic != HCV_PROGARG_MAGIC)
    // this should never happen
    HCV_FATALOUT("corrupted program arguments");

  switch (key)
    {
    case HCVPROGOPT_DEBUG:
      hcv_debugging.store(true);
      return 0;

    case HCVPROGOPT_WEBURL:
      progargs->hcvprog_weburl = std::string(arg);
      return 0;

    case HCVPROGOPT_POSTGRESURI:
      progargs->hcvprog_postgresuri = std::string(arg);
      return 0;

    case HCVPROGOPT_CONFIG:
      progargs->hcvprog_config = std::string(arg);
      return 0;

    case HCVPROGOPT_WRITEPID:
      progargs->hcvprog_pidfile = std::string(arg);
      return 0;

    case HCVPROGOPT_THREADS:
      hcv_http_max_threads = (unsigned)atoi(arg);
      if (hcv_http_max_threads < 2)
        hcv_http_max_threads = 2;
      else if (hcv_http_max_threads > 4 && hcv_http_max_threads > std::thread::hardware_concurrency() - 1)
        hcv_http_max_threads =
          (std::thread::hardware_concurrency() > 8)
          ? ((std::thread::hardware_concurrency() -1))
          : 4;
      return 0;

    case HCVPROGOPT_SYSLOG:
    {
      int lev = -1;
      if (isdigit(arg[0]))
        lev = atoi(arg);
      if (lev >= 0 && lev <= 7)
        {
          openlog(progargs->hcvprog_progname.c_str(), LOG_PID, LOG_LOCAL0+lev);
          hcv_syslog_program_arguments ();
          HCV_SYSLOGOUT(LOG_NOTICE, " logging " << progargs->hcvprog_progname << std::endl
                        <<  " version:" << hcv_versionmsg<< std::endl
                        << " at " << hcv_startimbuf
                        << " on " << hcv_hostname);
        }
      else
        HCV_FATALOUT("bad --syslog option " << arg);
    }
    return 0;

    case HCVPROGOPT_WEBROOT:
      progargs->hcvprog_webroot = std::string(arg);
      return 0;

    case HCVPROGOPT_WEBSSLCERT:
      progargs->hcvprog_opensslcert = std::string(arg);
      return 0;

    case HCVPROGOPT_WEBSSLKEY:
      progargs->hcvprog_opensslkey = std::string(arg);
      return 0;

    case HCVPROGOPT_PLUGIN:
    {
      char plugnambuf[64];
      memset (plugnambuf, 0, sizeof(plugnambuf));
      int endp = -1;
      const char*plugarg = nullptr;
      if (sscanf(arg, "%60[A-Za-z0-9_]%n", plugnambuf, &endp) < 1 || endp <= 0)
        HCV_FATALOUT("bad --plugin option " << arg <<std::endl
                     << "... (expected plugin name with letters, digits, underscore)");
      if (sscanf(arg, "%60[A-Za-z0-9_]:%n", plugnambuf, &endp) >= 1 && endp > 2)
        plugarg = arg+endp;
      hcv_load_plugin(plugnambuf, plugarg);
      return 0;
    }

    case HCVPROGOPT_CLEARDATABASE:
      hcv_should_clear_database = true;
      return 0;

    default:
      return ARGP_ERR_UNKNOWN;
    }
} // end hcv_parse1opt


void
hcv_syslog_program_arguments(void)
{
  if (hcv_main_argc == 0 || !hcv_main_argv)
    return;
  std::ostringstream os;
  for (int ix=0; ix<hcv_main_argc; ix++)
    {
      if (ix>0) os << ' ';
      os << hcv_main_argv[ix];
    };
  os << std::endl;
  syslog (LOG_NOTICE, "HelpCovid %30s program arguments:\n... %s",
          hcv_lastgitcommit, os.str().c_str());
} // end hcv_syslog_program_arguments



void
hcv_early_initialize(const char*progname)
{
  errno = 0;
  hcv_monotonic_start_time = hcv_monotonic_real_time();
  if (gethostname(hcv_hostname, sizeof(hcv_hostname)))
    HCV_FATALOUT("gethostname failure");
  if (getenv("$HELPCOVID_NBWORKERTHREADS"))
    hcv_http_max_threads = atoi(getenv("$HELPCOVID_NBWORKERTHREADS"));
  if (hcv_http_max_threads >= std::thread::hardware_concurrency())
    hcv_http_max_threads = std::thread::hardware_concurrency()-1;
  if (hcv_http_max_threads < 2)
    hcv_http_max_threads = 2;
  snprintf(hcv_versionmsg, sizeof(hcv_versionmsg),
           "github.com/bstarynk/helpcovid built %s\n... gitcommit %s\n... md5sum %s on %s",
           hcv_timestamp, hcv_lastgitcommit,
           hcv_md5sum,
           hcv_hostname);
  hcv_versionmsg[sizeof(hcv_versionmsg)-1] = (char)0;
  argp_program_version = hcv_versionmsg;
  argp_program_bug_address = "https://github.com/bstarynk/helpcovid/";
  hcv_proghandle = dlopen(nullptr, RTLD_NOW);
  if (!hcv_proghandle)
    HCV_FATALOUT("program dlopen failed: " << dlerror());
  {
    time_t nowt = 0;
    time(&nowt);
    struct tm nowtm = {};
    localtime_r(&nowt, &nowtm);
    strftime(hcv_startimbuf, sizeof(hcv_startimbuf), "%c %Z", &nowtm);
  }
  openlog(progname, LOG_PID|LOG_PERROR, LOG_LOCAL0);
  hcv_syslog_program_arguments ();
} // end hcv_early_initialize


/*************
 * Seteuid techniques. Beware, if used properly, this can increase the
 * security of the system. But if it is badly used, this can open
 * huge cybersecurity holes....
 *
 * This is a very complex topic, be sure to read at least
 * https://en.wikipedia.org/wiki/Setuid then
 * http://man7.org/linux/man-pages/man2/execve.2.html then
 * http://man7.org/linux/man-pages/man2/seteuid.2.html then
 * http://man7.org/linux/man-pages/man7/credentials.7.html
 *
 ************/
void
hcv_set_euid(const std::string& euidstr)
{
  uid_t neweuid= -1;
  if (euidstr.empty() || euidstr=="-")
    {
      HCV_SYSLOGOUT(LOG_WARNING, "hcv_set_euid empty '" << euidstr << "'");
      return;
    };
  if (std::isdigit(euidstr[0]))
    {
      neweuid = (uid_t)atol(euidstr.c_str());
    }
  else if (std::isalpha(euidstr[0]))
    {
      struct passwd* pw = getpwnam(euidstr.c_str());
      if (!pw)
        HCV_FATALOUT("hcv_set_euid with invalid user nanme '" << euidstr << "'");
      neweuid = pw->pw_uid;
    }
  else
    HCV_FATALOUT("hcv_set_euid invalid euidstr '" << euidstr << "'");
  if (neweuid== (uid_t)-1)
    HCV_FATALOUT("hcv_set_euid no user");
  if (seteuid(neweuid))
    HCV_FATALOUT("hcv_set_euid failed to seteuid to " << (int)neweuid);
  HCV_SYSLOGOUT(LOG_NOTICE, "hcv_set_euid did seteuid to " << neweuid);
} // end hcv_set_euid



void
hcv_parse_program_arguments(int &argc, char**argv)
{
  errno = 0;
  struct argp_state argstate;
  memset (&argstate, 0, sizeof(argstate));

  argstate.input = &hcv_progargs;
  if (!argstate.input)
    HCV_FATALOUT("ARGSTATE INPUT IS NULL");

  hcv_progargs.hcvprog_magic = HCV_PROGARG_MAGIC;
  hcv_progargs.hcvprog_progname = basename(argv[0]);

  static struct argp argparser;
  argparser.options = hcv_progoptions;
  argparser.parser = hcv_parse1opt;
  argparser.args_doc = "*no-positional-arguments*";
  argparser.doc = "github.com/bstarynk/helpcovid\n"
                  " - a GPLv3+ free software C++ Web application for Linux/x86-64\n"
                  "... to help organizing against Covid19 - NO WARRANTY";
  argparser.children = nullptr;
  argparser.help_filter = nullptr;
  argparser.argp_domain = nullptr;
  if (argp_parse(&argparser, argc, argv, 0, nullptr, &hcv_progargs))
    HCV_FATALOUT("failed to parse program arguments to " << argv[0]);
  ///////////////////////////////////////////
  /// set suitable defaults
  if (hcv_progargs.hcvprog_weburl.empty() && getenv("HELPCOVID_URL"))
    {
      hcv_progargs.hcvprog_weburl = getenv("HELPCOVID_URL");
      HCV_SYSLOGOUT(LOG_INFO, "using $HELPCOVID_URL=" << hcv_progargs.hcvprog_weburl);
    };
  if (hcv_progargs.hcvprog_postgresuri.empty() && getenv("HELPCOVID_POSTGRESQL"))
    {
      hcv_progargs.hcvprog_postgresuri = getenv("HELPCOVID_POSTGRESQL");
      HCV_SYSLOGOUT(LOG_INFO, "using $HELPCOVID_POSTGRESQL=" << hcv_progargs.hcvprog_postgresuri);
    }
  if (hcv_progargs.hcvprog_webroot.empty() && getenv("HELPCOVID_WEBROOT"))
    {
      hcv_progargs.hcvprog_webroot = getenv("HELPCOVID_WEBROOT");
      HCV_SYSLOGOUT(LOG_INFO, "using $HELPCOVID_WEBROOT=" << hcv_progargs.hcvprog_webroot);
    }
  if (hcv_progargs.hcvprog_seteuid.empty() && getenv("HELPCOVID_SETEUID"))
    {
      hcv_progargs.hcvprog_seteuid = getenv("HELPCOVID_SETEUID");
      HCV_SYSLOGOUT(LOG_INFO, "using $HELPCOVID_SETEUID=" << hcv_progargs.hcvprog_seteuid);
    }
  if (hcv_progargs.hcvprog_opensslcert.empty() && getenv("HELPCOVID_SSLCERT"))
    {
      hcv_progargs.hcvprog_opensslcert = getenv("HELPCOVID_SSLCERT");
      HCV_SYSLOGOUT(LOG_INFO, "using $HELPCOVID_SSLCERT=" << hcv_progargs.hcvprog_opensslcert);
    }
  if (hcv_progargs.hcvprog_opensslkey.empty() && getenv("HELPCOVID_SSLKEY"))
    {
      hcv_progargs.hcvprog_opensslkey = getenv("HELPCOVID_SSLKEY");
      HCV_SYSLOGOUT(LOG_INFO, "using $HELPCOVID_SSLKEY=" << hcv_progargs.hcvprog_opensslkey);
    }
  if (hcv_progargs.hcvprog_pidfile.empty() && getenv("HELPCOVID_PIDFILE"))
    {
      hcv_progargs.hcvprog_pidfile = getenv("HELPCOVID_PIDFILE");
      HCV_SYSLOGOUT(LOG_INFO, "using $HELPCOVID_PIDFILE=" << hcv_progargs.hcvprog_pidfile);
    }
  HCV_SYSLOGOUT(LOG_NOTICE, "parsed " << argc << " program arguments");
} // end hcv_parse_program_arguments



/// this hcv_syslog_at function is used by the HCV_SYSLOGOUT macro
void
hcv_syslog_at (const char *fil, int lin, int prio, const std::string&str)
{
  syslog(prio, "%s:%d - %s", fil, lin, str.c_str());
} // end hcv_syslog_at....

////////////////////////////////////////////////// configuration
extern "C" Glib::KeyFile hcv_config_key_file;
Glib::KeyFile hcv_config_key_file;
extern "C" std::recursive_mutex hcv_config_mtx;
std::recursive_mutex hcv_config_mtx;
std::string hcv_config_file_path;

std::string
hcv_get_config_file_path(void)
{
  return   hcv_config_file_path;
} // end of hcv_get_config_file_path

void
hcv_load_config_file(const char*configfile)
{
  std::lock_guard<std::recursive_mutex> gu(hcv_config_mtx);
  std::string configpath;
  std::string defaultconfigpath= std::string(getenv("HOME")?:"/etc") + "/.helpcovidrc";
  errno=0;
  if (configfile && configfile[0])
    configpath = std::string(configfile);
  else if (!hcv_progargs.hcvprog_config.empty())
    configpath=hcv_progargs.hcvprog_config;
  else if (getenv("HELPCOVID_CONFIG"))
    configpath=std::string(getenv("HELPCOVID_CONFIG"));
  else if (!access(defaultconfigpath.c_str(), R_OK))
    configpath = defaultconfigpath;
  else
    configpath=HCV_DEFAULT_CONFIG_PATH;
  errno=0;
  struct stat configstat;
  memset (&configstat, 0, sizeof(configstat));
  if (stat(configpath.c_str(), &configstat))
    HCV_FATALOUT("helpcovid failed to stat configuration file " << configpath);
  if (!S_ISREG(configstat.st_mode))
    HCV_FATALOUT("helpcovid configuration file " << configpath
                 << " is not a regular file.");
  if (configstat.st_mode & S_IRWXO)
    HCV_FATALOUT("helpcovid configuration file " << configpath
                 << " is world readable or writable but should not be. Run chmod o-rwx " << configpath);
  HCV_SYSLOGOUT(LOG_NOTICE, "loading configuration file " << configpath);
  try
    {
      bool ok = hcv_config_key_file.load_from_file(configpath);
      if (ok)
        {
          HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid loaded configuration file " << configpath);
          hcv_config_file_path = configpath;
        }
      else
        HCV_FATALOUT("helpcovid configuration file " << configpath << " failed to load");
    }
  catch (Glib::KeyFileError &kferr)
    {
      HCV_FATALOUT("helpcovid configuration load of " << configpath << " got Glib::KeyFileError " << kferr.what() << " of code#" << kferr.code());
    }
  catch (std::exception &sex)
    {
      HCV_FATALOUT("helpcovid configuration load of " << configpath << " got standard exception " << sex.what());
    }
  catch (Glib::Exception &gex)
    {
      HCV_FATALOUT("helpcovid configuration load of " << configpath << " got Glib exception " << gex.what());
    }
  errno = 0;
} // end hcv_load_config_file

bool
hcv_config_has_group(const char*grpname)
{
  if (!grpname || !std::isalpha(grpname[0]))
    HCV_FATALOUT("helpcovid bad configuration group name " << (grpname?:"**null**"));
  std::lock_guard<std::recursive_mutex> gu(hcv_config_mtx);
  return hcv_config_key_file.has_group(grpname);
} // end of hcv_config_has_group

bool
hcv_config_has_key(const char*grpname, const char*keyname)
{
  if (!grpname || !std::isalpha(grpname[0]))
    HCV_FATALOUT("helpcovid bad configuration group name " << (grpname?:"**null**"));
  if (!keyname || !std::isalpha(keyname[0]))
    HCV_FATALOUT("helpcovid bad configuration key name " << (keyname?:"**null**"));
  std::lock_guard<std::recursive_mutex> gu(hcv_config_mtx);
  return hcv_config_key_file.has_key(grpname,keyname);
} // end of hcv_config_has_key


void
hcv_config_do(const std::function<void(const Glib::KeyFile*)>&dofun)
{
  if (!dofun)
    HCV_FATALOUT("helpcovid missing function to hcv_config_do");
  std::lock_guard<std::recursive_mutex> gu(hcv_config_mtx);
  dofun(&hcv_config_key_file);
} // end hcv_config_do


std::string
hcv_get_config_html(const std::string &name)
{
  std::string restr;
  if (name.empty() || !isalpha(name[0]))
    return "";
  char nambuf[HCV_CONFIG_HTML_NAME_MAXLEN+4];
  memset(nambuf, 0, sizeof(nambuf));
  int nbc = 0;
  for (auto c: name)
    {
      if (nbc>=HCV_CONFIG_HTML_NAME_MAXLEN)
        return "";
      if (!isalnum(c) && c!='_')
        return "";
      nambuf[nbc++] = c;
    };
  hcv_config_do([&](const Glib::KeyFile*kf)
  {
    HCV_ASSERT(kf != nullptr);
    if (kf->has_key("html", nambuf))
      {
        //restr = std::string(kf->get_string("html", nambuf));
        const char*ps = kf->get_string("html", nambuf).data();
        restr = std::string(ps);
      }
  });
  HCV_DEBUGOUT("hcv_get_config_html name='" << name << "' -> '"
               << restr << "'");
  return restr;
} // end hcv_get_config_html

void
hcv_config_handle_helpcovid_config_group(void)
{
  HCV_SYSLOGOUT(LOG_INFO, "helpcovid is handling 'helpcovid' config group");
  hcv_config_do([&](const Glib::KeyFile*kf)
  {
    /// we have to be paranoid here.... This could be a cybersecurity risk.
    std::string startupcmd = kf->get_string("helpcovid","startup_popen_command");
    static constexpr double maxelapsedtime = 5.0;
    double startim = hcv_monotonic_real_time();
    if (!startupcmd.empty())
      {
        static constexpr int  maxlinecnt = 1000;
        std::string cmd = "ulimit -t 4 -u 128 -v 2048 ; ";
        cmd += startupcmd;
        HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid running perhaps dangerous startup popen command " << cmd);
        fflush(nullptr);
        FILE* popcmd= popen(cmd.c_str(), "r");
        if (!popcmd)
          HCV_FATALOUT("popen " << cmd.c_str() << " failed");
        char linbuf[1024];
        int lincnt = 0;
        while (lincnt<maxlinecnt+2)
          {
            struct pollfd mypollarr[3];
            memset (mypollarr, 0, sizeof(mypollarr));
            mypollarr[0].fd = fileno(popcmd);
            mypollarr[0].events = POLLIN;
            int p = poll(mypollarr, 1, 100);
            if (hcv_monotonic_real_time() - startim >= maxelapsedtime)
              HCV_FATALOUT("helpcovid startup popen command " << cmd
                           << " timed-out " << (hcv_monotonic_real_time() - startim) << " seconds");
            if (p==0)
              continue;
            if (p<0)
              HCV_FATALOUT("helpcovid startup startup popen command " << cmd << " poll failure");
            memset(linbuf, 0, sizeof(linbuf));
            if (!fgets(linbuf, sizeof(linbuf), popcmd))
              break;
            lincnt ++;
            if (linbuf[sizeof(linbuf)-2])
              HCV_FATALOUT("popen line#" << lincnt << "is too wide: " << linbuf);
            char lincntbuf[8];
            snprintf(lincntbuf, sizeof(lincntbuf), "%04d", lincnt);
            HCV_SYSLOGOUT(LOG_NOTICE, "startup.L." <<  lincntbuf << ":" << linbuf);
          }; // end while lincnt<maxlinecnt+2
        if (lincnt>=maxlinecnt)
          HCV_FATALOUT("helpcovid startup popen command " << cmd
                       << " too verbose: " << lincnt << " lines. Use shell redirections");
        int pc = pclose(popcmd);
        if (pc != 0)
          HCV_FATALOUT("helpcovid startup popen command " << cmd << " failed with pclose code " << pc);
        HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid did startup popen command " << cmd
                      << " in " << (hcv_monotonic_real_time() - startim) << " elapsed seconds");
      };
    /// we have to be paranoid here.... This could be a cybersecurity risk.
    std::string emailcmd = kf->get_string("helpcovid","html_email_popen_command");
    if (emailcmd.empty())
      HCV_SYSLOGOUT(LOG_WARNING,
                    "helpcovid misses an html_email_popen_command in configuration file");
    else
      {
        hcv_email_command = emailcmd;
        HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid will use " << hcv_email_command << " to send emails");
      }
  });

  HCV_SYSLOGOUT(LOG_INFO, "helpcovid did handle 'helpcovid' config group");
} // end hcv_config_handle_helpcovid_config_group



////////////////////////////////////////////////////////////////

const char*
hcv_get_hostname(void)
{
  return hcv_hostname;
} // end hcv_get_hostname


const std::string&
hcv_get_html_email_command(void)
{
  if (hcv_email_command.empty())
    HCV_FATALOUT("hcv_get_html_email_command: unconfigured empty html_email_popen_command in [helpcovid]");
  return hcv_email_command;
} // end hcv_get_html_email_command




////////////////////////////////////////////////////////////////
std::atomic<unsigned> Hcv_Random::_rand_threadcount;
bool Hcv_Random::_rand_is_deterministic_;
std::ranlux48 Hcv_Random::_rand_gen_deterministic_;
std::mutex Hcv_Random::_rand_mtx_deterministic_;


// static method called once by main
void
Hcv_Random::start_deterministic(long seed)
{
  std::lock_guard<std::mutex> guard(_rand_mtx_deterministic_);
  _rand_gen_deterministic_.seed (seed);
  _rand_is_deterministic_ = true;
} // end of Hcv_Random::start_deterministic


// private initializer, thread specific
void
Hcv_Random::init_deterministic(void)
{
  std::lock_guard<std::mutex> guard(_rand_mtx_deterministic_);
  HCV_ASSERT(_rand_is_deterministic_);
  _rand_generator.seed(_rand_gen_deterministic_());
} // end of  Hcv_Random::init_deterministic

void
Hcv_Random::deterministic_reseed(void)
{
  std::lock_guard<std::mutex> guard(_rand_mtx_deterministic_);
  HCV_ASSERT(_rand_is_deterministic_);
  _rand_generator.seed(_rand_gen_deterministic_());
} // end of Hcv_Random::deterministic_reseed

////////////////////////////////////////////////////////////////
int
main(int argc, char**argv)
{
  hcv_main_argc = argc;
  hcv_main_argv = argv;
  std::string seteuid;
  hcv_early_initialize(argv[0]);
  hcv_parse_program_arguments(argc, argv);
  HCV_SYSLOGOUT(LOG_NOTICE, "start of " << argv[0] << std::endl
                <<  " version:" << hcv_versionmsg << std::endl
#ifdef HELPCOVID_SANITIZE
                << " **sanitizing for " << HELPCOVID_SANITIZE "**" << std::endl
#endif /*HELPCOVID_SANITIZE*/
                << " at " << hcv_startimbuf
                << " on " << hcv_hostname);
  hcv_load_config_file();
  if (hcv_config_has_group("helpcovid"))
    {
      hcv_config_do([&](const Glib::KeyFile*kf)
      {
        if (kf->has_key("helpcovid","threads"))
          {
            hcv_http_max_threads = (unsigned)kf->get_int64("helpcovid","threads");
            if (hcv_http_max_threads < 2)
              hcv_http_max_threads = 2;
            else if (hcv_http_max_threads > 4 && hcv_http_max_threads > std::thread::hardware_concurrency() - 1)
              hcv_http_max_threads =
                (std::thread::hardware_concurrency() > 8)
                ? ((std::thread::hardware_concurrency() -1))
                : 4;
            HCV_SYSLOGOUT(LOG_INFO,"helpcovid startup threads#" << hcv_http_max_threads);
          }
      });
    };
  /// the web interface should be initialized early, before seteuid(2)
  if (!hcv_progargs.hcvprog_weburl.empty())
    hcv_initialize_web(hcv_progargs.hcvprog_weburl, hcv_progargs.hcvprog_webroot,
                       hcv_progargs.hcvprog_opensslcert, hcv_progargs.hcvprog_opensslkey);
  else if (hcv_config_has_group("web"))
    {
      hcv_config_do([=](const Glib::KeyFile*kf)
      {
        if (!kf->has_key("web","url"))
          HCV_FATALOUT("helpcovid missing 'url' key in group 'web' of config file " << hcv_config_file_path);
        if (!kf->has_key("web","root"))
          HCV_FATALOUT("helpcovid missing 'root' key in group 'web' of config file " << hcv_config_file_path);
        std::string sslcertstr, sslkeystr;
        if (kf->has_key("web","sslcert") && kf->has_key("web","sslkey"))
          {
            sslcertstr = std::string(kf->get_string("web","sslcert"));
            sslkeystr = std::string(kf->get_string("web","sslkey"));
          };
        hcv_initialize_web(std::string(kf->get_string("web","url")),
                           std::string(kf->get_string("web","root")),
                           sslcertstr, sslkeystr);
      });
    };
  if (hcv_config_has_group("helpcovid"))
    {
      hcv_config_do([&](const Glib::KeyFile*kf)
      {
        if (kf->has_key("helpcovid","log_message"))
          {
            std::string logmsg = kf->get_string("helpcovid","log_message");
            if (!logmsg.empty())
              HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid log_message:" << logmsg);
          };
        if (kf->has_key("helpcovid","seteuid"))
          {
            seteuid = kf->get_string("helpcovid","seteuid");
            if (!seteuid.empty())
              HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid will seteuid " << seteuid << " from configuration file");
          };
        if (kf->has_key("helpcovid","pid_file"))
          {
            std::string pidpath = kf->get_string("helpcovid","pid_file");
            if (!pidpath.empty() && pidpath[0] == '/')
              {
                hcv_progargs.hcvprog_pidfile = pidpath;
                HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid will use pid_file " << pidpath << " from configuration file");
              }
          };
      });
    };
  //////////////// debugging
  errno = 0;
  if (hcv_debugging.load())
    {
      HCV_SYSLOGOUT(LOG_INFO, "helpcovid debugging enabled");
      HCV_DEBUGOUT("helpcovid is debugging");
    }
  else
    {
      HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid debugging disabled");
    };
  errno = 0;
  /////==================================================
  //// CYBERSECURITY RISK: use seteuid(2).....
  //// complex topic, be sure to read at least https://en.wikipedia.org/wiki/Setuid
  //// then http://man7.org/linux/man-pages/man2/execve.2.html
  //// then http://man7.org/linux/man-pages/man2/seteuid.2.html
  //// then http://man7.org/linux/man-pages/man7/credentials.7.html
  if (!hcv_progargs.hcvprog_seteuid.empty())
    hcv_set_euid(hcv_progargs.hcvprog_seteuid);
  else if (!seteuid.empty())
    hcv_set_euid(seteuid);
  ////===================================================
  if (hcv_config_has_group("helpcovid"))
    hcv_config_handle_helpcovid_config_group();
  /// write the pidfile
  if (!hcv_progargs.hcvprog_pidfile.empty() && hcv_progargs.hcvprog_pidfile != "-")
    {
      FILE*pidf = fopen(hcv_progargs.hcvprog_pidfile.c_str(), "w");
      if (!pidf)
        HCV_FATALOUT("helpcovid failed to fopen pidfile " << hcv_progargs.hcvprog_pidfile);
      fprintf(pidf, "%ld\n", (long)getpid());
      if (fchmod(fileno(pidf), 0644))
        HCV_FATALOUT("helpcovid failed to fchmod pidfile " << hcv_progargs.hcvprog_pidfile);
      if (fclose(pidf))
        HCV_FATALOUT("helpcovid failed to fclose pidfile " << hcv_progargs.hcvprog_pidfile);
      HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid wrote its pid into " << hcv_progargs.hcvprog_pidfile);
    }
  else
    {
#define HCV_BUILTIN_PIDFILE "/var/run/helpcovid.pid"
      FILE*pidf = fopen(HCV_BUILTIN_PIDFILE, "w");
      if (pidf)
        {
          fprintf(pidf, "%ld\n", (long)getpid());
          if (fchmod(fileno(pidf), 0644))
            HCV_FATALOUT("helpcovid failed to fchmod " << HCV_BUILTIN_PIDFILE);
          if (fclose(pidf))
            HCV_FATALOUT("helpcovid failed to fclose  " << HCV_BUILTIN_PIDFILE);
        }
      else
        HCV_SYSLOGOUT(LOG_WARNING, "helpcovid unable to write builtin pidfile " << HCV_BUILTIN_PIDFILE);
    }
  errno = 0;
  hcv_initialize_database(hcv_progargs.hcvprog_postgresuri, hcv_should_clear_database);
  errno = 0;
  hcv_initialize_templates();
  errno = 0;
  hcv_start_background_thread();
  errno = 0;
  hcv_webserver_run();

  HCV_SYSLOGOUT(LOG_INFO, "normal end of " << argv[0]);
  hcv_main_argc = 0;
  hcv_main_argv = nullptr;
  return 0;
} // end of main



////////////////////////// end of file hcv_main.cc
