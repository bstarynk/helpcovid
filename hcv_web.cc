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
extern "C" httplib::Server* hcv_webserver;
extern "C" std::string hcv_weburl;
extern "C" std::string hcv_webroot;
extern "C" std::atomic<long> hcv_web_request_counter;
httplib::Server* hcv_webserver;
std::string hcv_weburl;
std::string hcv_webroot;
std::atomic<long> hcv_web_request_counter;
Json::StreamWriterBuilder hcv_json_builder;

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
        HCV_FATALOUT("OpenSSL key " << opensslkey << " is world readable or writable but should not be. Run chmod o-rwx " << opensslkey);
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
  hcv_json_builder["commentStyle"] = "None";
  hcv_json_builder["indentation"] = " ";
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
hcv_initialize_webserver(void)
{
  HCV_SYSLOGOUT(LOG_WARNING, "unimplemented hcv_initialize_webserver");
#warning hcv_initialize_webserver unimplemented
} // end of hcv_initialize_webserver

void
hcv_output_encoded_html(std::ostream&out, const std::string&str)
{
  for (char c: str)
    {
      switch(c)
        {
        case '<':
          out << "&lt;";
          break;
        case '>':
          out << "&gt;";
          break;
        case '\'':
          out << "&apos;";
          break;
        case '&':
          out << "&amp;";
          break;
        case '\"':
          out << "&quot;";
          break;
        default:
          out << c;
          break;
        }
    }
} // end hcv_output_encoded_html


void
hcv_output_cstr_encoded_html(std::ostream&out, const char*cstr)
{
  if (!cstr)
    return;
  for (const char*pc=cstr; *pc; pc++)
    {
      switch(*pc)
        {
        case '<':
          out << "&lt;";
          break;
        case '>':
          out << "&gt;";
          break;
        case '\'':
          out << "&apos;";
          break;
        case '&':
          out << "&amp;";
          break;
        case '\"':
          out << "&quot;";
          break;
        default:
          out << *pc;
          break;
        }
    }
} // end hcv_output_cstr_encoded_html


///////////////////////////// HTTP error handler, uses html/error.html when available
void
hcv_web_error_handler(const httplib::Request& req,
                      httplib::Response& resp, long reqnum)
{
  Hcv_http_template_data webdata(req,resp,reqnum);
  HCV_SYSLOGOUT(LOG_WARNING, "hcv_web_error_handler reqnum=" << reqnum << " req." << req.method << " path=" << req.path);
  std::string outhtmlstr;
  bool goodhtml = false;
  std::string errfilpath;
  if (!hcv_webroot.empty() && hcv_webroot[0] == '/')
    {
      errfilpath = hcv_webroot + "/html/error.html";
      if (auto ferr= fopen(errfilpath.c_str(), "r"))
        {
          char firstlinbuf[80];
          memset (firstlinbuf, 0, sizeof(firstlinbuf));
          fgets(firstlinbuf, sizeof(firstlinbuf), ferr);
          if (!strncmp(firstlinbuf, HCV_HTML5_START, strlen(HCV_HTML5_START)))
            goodhtml = true;
          fclose(ferr);
        };
    };
  if (goodhtml)
    outhtmlstr = hcv_expand_template_file(errfilpath, &webdata);
  else
    {
      constexpr const char builtin_error_html[] =
        R"builtinerror(<!DOCTYPE html>
<html>
<head>
 <meta charset="utf-8">
 <title>HelpCovid builtin error</title>
</head>
<body>
<h1>HelpCovid builtin error</h1>
  <p>Please ask the webmaster to add a proper <tt>html/error.html</tt> file.<br/></p>
  <p>Error on <?hcv now?> for request #<?hcv request_number?> to <?hcv request_method?> of <?hcv request_path?>.<br/>
    HelpCovid git <tt><?hcv gitid?></tt> pid <?hcv pid?> on <tt><i><?hcv hostname?></i></tt>.<br/> 
    Please go back to <a href='/'>root webpage</a>.
  </p>
</body>
</html>
)builtinerror";
      outhtmlstr = hcv_expand_template_string(std::string(builtin_error_html),
					      "*builtin-error*", &webdata);
    }
  resp.set_content(outhtmlstr.c_str(), "text/html");
} // end hcv_web_error_handler



void
hcv_webserver_run(void)
{
  static double startcputime = hcv_process_cpu_time();
  static double startmonotonictime = hcv_monotonic_real_time();
  unsigned webport;
  if (getuid() == 0)
    webport = 80;
  else
    webport = 8080;
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
  hcv_webserver->set_error_handler
  ([](const httplib::Request& req,
      httplib::Response& resp)
  {
    auto n = std::atomic_load(&hcv_web_request_counter);
    hcv_web_error_handler(req, resp, n);
  });
#if 0 && oldhellocode
  hcv_webserver->Get("/hello",
                     [](const httplib::Request&req, httplib::Response& resp)
  {
    std::atomic_fetch_add(&hcv_web_request_counter, 1);
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
#endif /*old hello code*/
  hcv_webserver->Get("/status.json",
                     [](const httplib::Request&req, httplib::Response& resp)
  {
    long reqcnt = std::atomic_fetch_add(&hcv_web_request_counter, 1);
    if (reqcnt<=0)
      reqcnt=1;
    long procsize=0, procrss=0, procshared=0;
    {
      FILE* pself = fopen("/proc/self/statm", "r");
      if (pself)
        {
          fscanf(pself, " %ld %ld %ld", &procsize, &procrss, &procshared);
          fclose(pself);
          /// we sleep slightly against DoS attacks; this is not very effective, but might be helpful
          usleep(1000*(1+(reqcnt%8)) + (reqcnt&0xff));
        }
    }
    Json::Value jsob(Json::objectValue);
    jsob["helpcovid"] = "github.com/bstarynk/helpcovid";
    jsob["license"] = "GPLv3+";
    jsob["lastgitcommit"] = hcv_lastgitcommit;
    jsob["md5sum"] = hcv_md5sum;
    jsob["gitid"] = hcv_gitid;
    jsob["total_cpu_time"] = hcv_process_cpu_time() - startcputime;
    jsob["total_elapsed_time"] = hcv_monotonic_real_time() - startmonotonictime;
    jsob["cpu_time_per_request"] = (hcv_process_cpu_time() - startcputime) / reqcnt;
    jsob["elapsed_time_per_request"] = (hcv_monotonic_real_time() - startmonotonictime) / reqcnt;
    if (procsize>0)
      jsob["process_size"] = procsize;
    if (procrss>0)
      jsob["process_rss"] = procrss;
    if (procshared>0)
      jsob["process_shared"] = procshared;
    jsob["postgresql_version"] = hcv_postgresql_version();
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
    jsob["web_request_count"] = reqcnt;
    auto str = Json::writeString(hcv_json_builder, jsob);
    resp.set_content(str.c_str(), "application/json");
  });

  hcv_webserver->Get("/login", [](const httplib::Request& req,
                                  httplib::Response& resp)
  {

    auto view = Hcv_LoginView(req, resp, "html/login.html");
    resp.set_content(view.get(), "text/html");
  });

  hcv_webserver->listen(webhost, webport);
  HCV_SYSLOGOUT(LOG_INFO, "end hcv_webserver_run webhost=" << webhost << " webport=" << webport);
} // end hcv_webserver_run




//////////////////// end of file hcv_web.cc of github.com/bstarynk/helpcovid
