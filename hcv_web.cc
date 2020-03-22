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
std::string hcv_weburl;
std::string hcv_webroot;

/// this could be run with root privilege if we need to serve the :80
/// HTTP TCP port. So be specially careful here!
void hcv_initialize_web(const std::string&weburl, const std::string&webroot, const std::string&opensslcert, const std::string&opensslkey)
{
  if (!opensslcert.empty() && !opensslkey.empty())
    {
      struct stat certstat, keystat;
      memset (&certstat, 0, sizeof(certstat));
      memset (&keystat, 0, sizeof(keystat));
      if (stat(opensslcert.c_str(), &certstat))
        HCV_FATALOUT("stat of OpenSSL certificate " << opensslcert << " failed.");
      if (stat(opensslkey.c_str(), &keystat))
        HCV_FATALOUT("stat of OpenSSL key " << opensslkey << " failed.");
      if (!S_ISREG(certstat.st_mode))
        HCV_FATALOUT("OpenSSL certificate " << opensslcert << " is not a regular file.");
      if (!S_ISREG(keystat.st_mode))
        HCV_FATALOUT("OpenSSL key " << opensslkey << " is not a regular file.");
      if (keystat.st_mode & S_IRWXO)
        HCV_FATALOUT("OpenSSL key " << opensslkey << " is world readable or writable but should not be.");
      hcv_webserver.reset(new httplib::SSLServer(opensslcert.c_str(), opensslkey.c_str()));
      HCV_SYSLOGOUT(LOG_NOTICE, "starting HTTPS server with OpenSSL certificate " << opensslcert
                    << " and key " << opensslkey << std::endl
                    << "... using weburl " << weburl << " and webroot "<< webroot
                    << " hcv_webserver@" << (void*)hcv_webserver.get());
    }
  else
    {
      hcv_webserver.reset(new httplib::Server);
      HCV_SYSLOGOUT(LOG_NOTICE, "starting plain HTTP server using weburl " << weburl << " and webroot "<< webroot
                    << " hcv_webserver@" << (void*)hcv_webserver.get());
    }
  hcv_weburl = weburl;
  hcv_webroot = webroot;
} // end hcv_initialize_web


void hcv_webserver_run(void)
{
  HCV_SYSLOGOUT(LOG_INFO, "Starting HelpCovid web server...");
  std::cout << "**Starting HelpCovid web server..." << std::endl;

  httplib::Server srv;
  srv.Get("/", [](const httplib::Request&, httplib::Response& resp)
  {
    std::ifstream html("webroot/html/signin.html");
    std::string bfr, line;
    while (std::getline(html, line))
      {
        bfr += line;
        bfr.push_back('\n');
      }
    resp.set_content(bfr, "text/html");
  });

  srv.listen("localhost", 8000);

    // This check seems to indicate that hcv_webserver is out of scope here
    if (!hcv_webserver) {
        HCV_FATALOUT("Webserver is null!");
    }

    hcv_webserver->Get("/", [](const httplib::Request&, 
        httplib::Response& resp) {
      std::ifstream html("webroot/html/signin.html");
      std::string bfr, line;
      while (std::getline(html, line))
          bfr += line;

      resp.set_content(bfr, "text/html");
    });

    hcv_webserver->listen(hcv_weburl.c_str(), 8000);
} // end hcv_webserver_run




//////////////////// end of file hcv_web.cc of github.com/bstarynk/helpcovid
