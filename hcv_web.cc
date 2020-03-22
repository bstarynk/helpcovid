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
httplib::Server* hcv_webserver;
std::string hcv_weburl;
std::string hcv_webroot;

/// this could be run with root privilege if we need to serve the :80
/// HTTP TCP port. So be specially careful here!
void hcv_initialize_web(const std::string&weburl, const std::string&webroot, const std::string&opensslcert, const std::string&opensslkey)
{
  HCV_SYSLOGOUT(LOG_INFO, "hcv_initialize_web: weburl='" << weburl
                << "', webroot='" << webroot
                << "', opensslcert='" << opensslcert
                << "', opensslkey='" << opensslkey
                << "'");
  if (weburl.empty())
    HCV_FATALOUT("hcv_initialize_web: missing weburl");
  if (webroot.empty())
    HCV_FATALOUT("hcv_initialize_web: missing webroot");
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
      hcv_webserver = new httplib::SSLServer(opensslcert.c_str(), opensslkey.c_str());
      HCV_SYSLOGOUT(LOG_NOTICE, "starting HTTPS server with OpenSSL certificate " << opensslcert
                    << " and key " << opensslkey << std::endl
                    << "... using weburl " << weburl << " and webroot "<< webroot
                    << " hcv_webserver@" << (void*)hcv_webserver);
    }
  else
    {
      hcv_webserver = new httplib::Server();
      HCV_SYSLOGOUT(LOG_NOTICE, "starting plain HTTP server using weburl " << weburl << " and webroot "<< webroot
                    << " hcv_webserver@" << (void*)hcv_webserver);
    }
  hcv_weburl = weburl;
  hcv_webroot = webroot;
} // end hcv_initialize_web


void
hcv_stop_web()
{
  HCV_SYSLOGOUT(LOG_WARNING, "unimplemented hcv_stop_web with hcv_webserver@" << (void*)hcv_webserver);
  delete hcv_webserver;
  hcv_webserver = nullptr;
#warning TODO: implement hcv_stop_web
} // end hcv_stop_web




void
hcv_webserver_run(void)
{
  static std::atomic<long> nbreq;
  static double startcputime = hcv_process_cpu_time();
  static double startmonotonictime = hcv_monotonic_real_time();
  unsigned webport;
  if (getuid() == 0) webport = 80;
  else webport = 8080;
  HCV_SYSLOGOUT(LOG_INFO, "Starting HelpCovid web server hcv_webserver@" << (void*)hcv_webserver
                << " with hcv_weburl=" << hcv_weburl);
  if (!hcv_webserver)
    HCV_FATALOUT("no hcv_webserver");
  //
  if (hcv_weburl.empty())
    HCV_FATALOUT("no hcv_weburl");
  // parse hcv_weburl
  char webhost[64];
  {
    int endpos= -1;
    memset(webhost,0,sizeof(webhost));
    if (sscanf(hcv_weburl.c_str(), "http://%60[a-zA-Z0-9_.]:%u%n", webhost, &webport, &endpos)>=2 && endpos>0 && webport>0)
      {
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" << webhost << " webport=" << webport);
      }
    else if (sscanf(hcv_weburl.c_str(), "https://%60[a-zA-Z0-9_.]:%u%n", webhost, &webport, &endpos)>=2 && endpos>0 && webport>0)
      {
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" << webhost << " webport=" << webport);
      }
    else if (sscanf(hcv_weburl.c_str(), "http://%60[a-zA-Z0-9_.]%n", webhost, &endpos)>=1 && endpos>0)
      {
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" << webhost << " default webport=" << webport);
      }
    else if (sscanf(hcv_weburl.c_str(), "https://%60[a-zA-Z0-9_.]%n", webhost, &endpos)>=1 && endpos>0)
      {
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" << webhost << " default webport=" << webport);
      }
    else
      HCV_FATALOUT("bad hcv_weburl " << hcv_weburl);
  }
  hcv_webserver->Get("/hello",
                     [](const httplib::Request&req, httplib::Response& resp)
  {
    std::atomic_fetch_add(&nbreq, 1);
    std::ostringstream outs;
    outs << "Hello, World, from HelpCovid - github.com/bstarynk/helpcovid - a GPLv3+ software." << std::endl
         << " hcv_lastgitcommit: " << hcv_lastgitcommit << std::endl
         << " hcv_md5sum: " << hcv_md5sum << std::endl
         << " hcv_timestamp: " << hcv_timestamp << std::endl;
    time_t nowt = 0;
    time(&nowt);
    struct tm nowtm;
    memset (&nowtm, 0, sizeof(nowtm));
    char nowbuf[80];
    memset (nowbuf, 0, sizeof(nowbuf));
    localtime_r (&nowt, &nowtm);
    strftime(nowbuf, sizeof(nowbuf), "%c %Z", &nowtm);
    char hostbuf[64];
    memset(hostbuf, 0, sizeof(hostbuf));
    gethostname(hostbuf, sizeof(hostbuf));
    outs << " on " << nowbuf
         << " host " <<  hostbuf
         << " pid " << (int)(getpid())
         << std::endl;
    resp.set_content(outs.str().c_str(), "text/plain");
  });
  hcv_webserver->Get("/status.js",
                     [](const httplib::Request&req, httplib::Response& resp)
  {
    long reqcnt = std::atomic_fetch_add(&nbreq, 1);
    Json::Value jsob(Json::objectValue);
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = " ";
    jsob["helpcovid"] = "github.com/bstarynk/helpcovid";
    jsob["license"] = "GPLv3+";
    jsob["lastgitcommit"] = hcv_lastgitcommit;
    jsob["md5sum"] = hcv_md5sum;
    jsob["gitid"] = hcv_gitid;
    jsob["cputime"] = hcv_process_cpu_time() - startcputime;
    jsob["elapsedtime"] = hcv_monotonic_real_time() - startmonotonictime;
    time_t nowt = 0;
    time(&nowt);
    struct tm nowtm;
    memset (&nowtm, 0, sizeof(nowtm));
    char nowbuf[80];
    memset (nowbuf, 0, sizeof(nowbuf));
    localtime_r (&nowt, &nowtm);
    strftime(nowbuf, sizeof(nowbuf), "%c %Z", &nowtm);
    char hostbuf[64];
    memset(hostbuf, 0, sizeof(hostbuf));
    gethostname(hostbuf, sizeof(hostbuf));
    jsob["ctime"] = nowbuf;
    jsob["hostname"] = hostbuf;
    jsob["nowtime"] = (long) nowt;
    jsob["pid"] = (int)getpid();
    jsob["reqcnt"] = reqcnt;
    auto str = Json::writeString(builder, jsob);
    resp.set_content(str.c_str(), "application/json");
  });
  hcv_webserver->listen(webhost, webport);
  HCV_SYSLOGOUT(LOG_INFO, "end hcv_webserver_run webhost=" << webhost << " webport=" << webport);
} // end hcv_webserver_run




//////////////////// end of file hcv_web.cc of github.com/bstarynk/helpcovid
