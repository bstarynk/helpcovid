/****************************************************************
 * file hcv_web.cc
 *
 * Description:
 *      Web interface of https://github.com/bstarynk/helpcovid
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

extern "C" const char hcv_web_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_web_date[] = __DATE__;

/// the web server
std::unique_ptr<httplib::Server> hcv_webserver;

/// this could be run with root privilege if we need to serve the :80
/// HTTP TCP port. So be specially careful here!
void hcv_initialize_web(const std::string&weburl, const std::string&webroot, const std::string&opensslcert, const std::string&opensslkey)
{
  HCV_SYSLOGOUT(LOG_WARNING, "hcv_initialize_web incomplete weburl="
                << weburl << " webroot=" << webroot
                << " opensslcert='" << opensslcert
                << "', opensslkey='" << opensslkey
                << "';");
#warning hcv_initialize_web incomplate
} // end hcv_initialize_web







//////////////////// end of file hcv_web.cc of github.com/bstarynk/helpcovid
