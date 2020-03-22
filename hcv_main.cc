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
 *    This program is free software: you can redistribute it and/or modify
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
////////////////////////////////////////////////////////////////
/// https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html

enum hcv_progoption_en
{
  HCVPROGOPT__NONE=0,
  HCVPROGOPT_WEBURL='W',
  HCVPROGOPT_POSTGRESURI='P',
  HCVPROGOPT_SYSLOG='S',
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
  std::string hcvprog_postgresuri;
};

static struct hcv_progarguments hcv_progargs =
{
  .hcvprog_magic = HCV_PROGARG_MAGIC,
  .hcvprog_progname = "",
  .hcvprog_weburl = "",
  .hcvprog_postgresuri = ""
};

static char hcv_hostname[64];
static char hcv_versionmsg[384];

static void* hcv_proghandle;
void hcv_fatal_stop_at (const char *fil, int lin, int err)
{
  std::clog << "**** FATAL ERROR " << fil << ":" << lin << std::endl;
  if (err>0)
    std::clog << " errno: " << strerror(err) << std::endl;
  syslog(LOG_EMERG, "FATAL STOP %s:%d (%s)",
         fil, lin, strerror(err));
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
    default:
      return ARGP_ERR_UNKNOWN;
    }
} // end hcv_parse1opt



void
hcv_early_initialize(const char*progname)
{
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

void
hcv_parse_program_arguments(int &argc, char**argv)
{
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
#warning TODO: complete hcv_parse_program_arguments
} // end hcv_parse_program_arguments

void
hcv_syslog_at (const char *fil, int lin, int prio, const std::string&str)
{
  syslog(prio, "%s:%d - %s", fil, lin, str.c_str());
}
////////////////////////////////////////////////////////////////
int
main(int argc, char**argv)
{
  hcv_early_initialize(argv[0]);
  hcv_parse_program_arguments(argc, argv);
  HCV_SYSLOGOUT(LOG_NOTICE, "start of " << argv[0] << std::endl
                <<  " version:" << hcv_versionmsg << std::endl
                << " at " << hcv_startimbuf
                << " on " << hcv_hostname);
  HCV_SYSLOGOUT(LOG_INFO, "normal end of " << argv[0]);
  return 0;
} // end of main
