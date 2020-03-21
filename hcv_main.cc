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



////////////////////////////////////////////////////////////////
/// https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html
struct argp_option hcv_progoptions[] =
{
};

#define HCV_PROGARG_MAGIC 722486817 /*0x2b104621*/
struct hcv_progarguments
{
  unsigned hcv_progmagic; // always HCV_PROGARG_MAGIC
  std::string hcv_weburl;
  std::string hcv_postgresuri;
};

static struct hcv_progarguments hcv_progargs;

void
hcv_parse_program_arguments(int &argc, char**argv)
{
  struct argp_state argstate;
  memset (&argstate, 0, sizeof(argstate));
  hcv_progargs.hcv_progmagic = HCV_PROGARG_MAGIC;
#warning TODO: complete hcv_parse_program_arguments
} // end hcv_parse_program_arguments


////////////////////////////////////////////////////////////////
int
main(int argc, char**argv)
{
  hcv_parse_program_arguments(argc, argv);
} // end of main
