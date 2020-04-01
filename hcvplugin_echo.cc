/****************************************************************
 * file hcvplugin_echo.cc
 *
 * Description:
 *      Example plugin of https://github.com/bstarynk/helpcovid just echoing the argument
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


/// mandatory in every HelpCovid plugin
const char hcvplugin_name[]="echo";
const char hcvplugin_version[]=__DATE__ "@" __TIME__;
const char hcvplugin_gpl_compatible_license[]="GPLv3+";
const char hcvplugin_gitapi[]=HELPCOVID_GITID;

/// mandatory initialization routine
void
hcvplugin_initialize_web(httplib::Server*,const char*arg)
{
  if (!arg)
    HCV_SYSLOGOUT(LOG_WARNING, "echo plugin " << hcvplugin_version
                  << " hcvplugin_initialize_web without arguments");
  else
    HCV_SYSLOGOUT(LOG_NOTICE, "echo plugin  " << hcvplugin_version
                  << " hcvplugin_initialize_web got argument: " << arg);
} // end of hcvplugin_initialize_web


/***************** end of example plugin hcvplugin_echo.cc in github.com/bstarynk/helpcovid */
