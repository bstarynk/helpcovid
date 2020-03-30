/****************************************************************
 * file hcv_plugins.cc
 *
 * Description:
 *      Plugins machinery of https://github.com/bstarynk/helpcovid
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

extern "C" const char hcv_plugins_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_plugins_date[] = __DATE__;

static std::recursive_mutex hcv_plugin_mtx;
struct Hcv_plugin
{
  std::string hcvpl_name;
  void* hcvpl_handle; 		// result of dlopen
  std::string hcvpl_gitid;
  std::string hcvpl_license;
};

std::vector<Hcv_plugin> hcv_plugin_vect;

void hcv_load_plugin(const char*plugin)
{
  if (!plugin || !plugin[0])
    HCV_FATALOUT("missing plugin name");
  for (const char*pc = plugin; *pc; pc++)
    if (!isalnum(*pc) && *pc != '_')
      HCV_FATALOUT("invalid plugin name " << plugin);
  HCV_DEBUGOUT("hcv_load_plugin " << plugin << " starting");
  ////
  HCV_SYSLOGOUT(LOG_WARNING, "hcv_load_plugin " << plugin << " not implemented");
#warning hcv_load_plugin not implemented
} // end hcv_load_plugin

/****************** end of file hcv_plugins.cc of github.com/bstarynk/helpcovid **/
