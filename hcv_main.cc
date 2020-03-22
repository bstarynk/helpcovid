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

////////////////////////////////////////////////////////////////
/// https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html
struct argp_option hcv_progoptions[] =
{
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
  unsigned hcv_progmagic; // always HCV_PROGARG_MAGIC
  std::string hcv_weburl;
  std::string hcv_postgresuri;
};

static struct hcv_progarguments hcv_progargs =
{
  .hcv_progmagic = HCV_PROGARG_MAGIC,
  .hcv_weburl = "",
  .hcv_postgresuri = ""
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
  if (!progargs || progargs->hcv_progmagic != HCV_PROGARG_MAGIC)
    // this should never happen
    HCV_FATALOUT("corrupted program arguments");
  switch (key)
    {
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

  hcv_progargs.hcv_progmagic = HCV_PROGARG_MAGIC;

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
                <<  " version:" << hcv_versionmsg);
  HCV_SYSLOGOUT(LOG_INFO, "normal end of " << argv[0]);
  return 0;
} // end of main
