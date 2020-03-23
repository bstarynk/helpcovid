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
 *      Nimesh Neema <nimeshneema@gmail.com>
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

  HCVPROGOPT_WEBSSLCERT=1000,
  HCVPROGOPT_WEBSSLKEY=1001,
};

struct argp_option hcv_progoptions[] =
{
  /* ======= set the served WEB url ======= */
  {/*name:*/ "web-url", ///
    /*key:*/ HCVPROGOPT_WEBURL, ///
    /*arg:*/ "WEBURL", ///
    /*flags:*/ 0, ///
    /*doc:*/ "sets the served WEB url (defaults to $HELPCOVID_URL), e.g. --web-url=https://myexample.com/helpcovid/", ///
    /*group:*/0 ///
  },
  /* ======= set the PostgreSQL database URI ======= */
  {/*name:*/ "postgresql-database", ///
    /*key:*/ HCVPROGOPT_POSTGRESURI, ///
    /*arg:*/ "POSTGRESQLURI", ///
    /*flags:*/0, ///
    /*doc:*/ "sets the PostgreSQL database URI (defaults to $HELPCOVID_POSTGRESQL), e.g. --postgresql-database=postgresql://www-data@localhost/helpcovid", ///
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
  /* ======= set the web document root ======= */
  {/*name:*/ "webroot", ///
    /*key:*/ HCVPROGOPT_WEBROOT, ///
    /*arg:*/ "WEBROOT", ///
    /*flags:*/0, ///
    /*doc:*/ "default webroot local file directory to serve static URLs without .. (defaults to $HELPCOVID_WEBROOT)", ///
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
    /*doc:*/ "web OpenSSL certificate file, e.g. --websslcert=/etc/helpcovid/somecert.pem (defaults to $HELPCOVID_SSLCERT)", ///
    /*group:*/0 ///
  },
  /* ======= set the web OpenSSL private key ======= */
  {/*name:*/ "websslkey", ///
    /*key:*/ HCVPROGOPT_WEBSSLKEY, ///
    /*arg:*/ "WEBSSLKEY", ///
    /*flags:*/0, ///
    /*doc:*/ "web OpenSSL private key, e.g. --websslkey=/etc/helpcovid/somekey.pem (defaults to $HELPCOVID_SSLKEY)", ///
    /*group:*/0 ///
  },
  /* ======= set the effective user id ======= */
  {/*name:*/ "seteuid", ///
    /*key:*/ HCVPROGOPT_SETEUID, ///
    /*arg:*/ "EFFECTIVEUSER", ///
    /*flags:*/0, ///
    /*doc:*/ "call seteuid(2) on given user name or numerical id (defaults to $HELPCOVID_SETEUID), cybersecurity risk if badly used...", ///
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
    case HCVPROGOPT_WEBURL:
      progargs->hcvprog_weburl = std::string(arg);
      return 0;

    case HCVPROGOPT_POSTGRESURI:
      progargs->hcvprog_postgresuri = std::string(arg);
      return 0;

    case HCVPROGOPT_CONFIG:
      progargs->hcvprog_config = std::string(arg);
      return 0;

    case HCVPROGOPT_SYSLOG:
    {
      int lev = -1;
      if (isdigit(arg[0]))
        lev = atoi(arg);
      if (lev >= 0 && lev <= 7)
        {
          openlog(progargs->hcvprog_progname.c_str(), LOG_PID, LOG_LOCAL0+lev);
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

    default:
      return ARGP_ERR_UNKNOWN;
    }
} // end hcv_parse1opt



void
hcv_early_initialize(const char*progname)
{
  errno = 0;
  if (gethostname(hcv_hostname, sizeof(hcv_hostname)))
    HCV_FATALOUT("gethostname failure");
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
void
hcv_load_config_file(const char*configfile)
{
  std::lock_guard<std::recursive_mutex> gu(hcv_config_mtx);
  std::string configpath;
  std::string defaultconfigpath= std::string(getenv("HOME")?:"/etc") + "/helpcovid.conf";
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
        HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid loaded configuration file " << configpath);
      else
        HCV_FATALOUT("helpcovid configuration file " << configpath << " failed to load");
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

////////////////////////////////////////////////////////////////
int
main(int argc, char**argv)
{
  std::string seteuid;
  hcv_early_initialize(argv[0]);
  hcv_parse_program_arguments(argc, argv);
  HCV_SYSLOGOUT(LOG_NOTICE, "start of " << argv[0] << std::endl
                <<  " version:" << hcv_versionmsg << std::endl
                << " at " << hcv_startimbuf
                << " on " << hcv_hostname);
  hcv_load_config_file();
  /// the web interface should be initialized early, before seteuid(2)
  if (!hcv_progargs.hcvprog_weburl.empty())
    hcv_initialize_web(hcv_progargs.hcvprog_weburl, hcv_progargs.hcvprog_webroot,
                       hcv_progargs.hcvprog_opensslcert, hcv_progargs.hcvprog_opensslkey);
  else if (hcv_config_has_group("web"))
    {
      hcv_config_do([=](const Glib::KeyFile*kf)
      {
        hcv_initialize_web(std::string(kf->get_string("web","url")),
                           std::string(kf->get_string("web","root")),
                           std::string(kf->get_string("web","sslcert")),
                           std::string(kf->get_string("web","sslkey")));
      });
    };
  if (hcv_config_has_group("helpcovid"))
    {
      hcv_config_do([&](const Glib::KeyFile*kf)
      {
        std::string logmsg = kf->get_string("helpcovid","log_message");
        if (!logmsg.empty())
          HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid log_message" << logmsg);
        seteuid = kf->get_string("helpcovid","seteuid");
        if (!seteuid.empty())
          HCV_SYSLOGOUT(LOG_NOTICE, "helpcovid will seteuid " << seteuid << " from configuration file");
      });
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
    {
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
          }
      });
    }
  errno = 0;
  hcv_initialize_database(hcv_progargs.hcvprog_postgresuri);
  errno = 0;
  hcv_webserver_run();

  HCV_SYSLOGOUT(LOG_INFO, "normal end of " << argv[0]);
  return 0;
} // end of main



////////////////////////// end of file hcv_main.cc
