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


std::recursive_mutex hcv_fatalmtx;
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

static struct hcv_progarguments hcv_progargs;

static char hcv_hostname[64];
static char hcv_versionmsg[384];

static void* hcv_proghandle;
void hcv_fatal_stop_at (const char *fil, int lin, int err)
{
  std::clog << "**** FATAL ERROR " << fil << ":" << lin << std::endl;
  if (err>0)
    std::clog << " errno: " << strerror(err) << std::endl;
  abort();
} // end hcv_fatal_stop_at


// parse a single program option
static error_t
hcv_parse1opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
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
hcv_early_initialize(void)
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
} // end hcv_early_initialize

void
hcv_parse_program_arguments(int &argc, char**argv)
{
  static struct argp argparser
    = {hcv_progoptions, hcv_parse1opt, "*no-positional-arguments*",
    "github.com/bstarynk/helpcovid - a GPLv3+ free software to help organizing against Covid19 - NO WARRANTY"
  };
  struct argp_state argstate;
  memset (&argstate, 0, sizeof(argstate));
  hcv_progargs.hcv_progmagic = HCV_PROGARG_MAGIC;
  if (argp_parse(&argparser, argc, argv, 0, nullptr, nullptr))
    HCV_FATALOUT("failed to parse program arguments to " << argv[0]);
#warning TODO: complete hcv_parse_program_arguments
} // end hcv_parse_program_arguments


////////////////////////////////////////////////////////////////
int
main(int argc, char**argv)
{
  hcv_early_initialize();
  hcv_parse_program_arguments(argc, argv);
} // end of main
