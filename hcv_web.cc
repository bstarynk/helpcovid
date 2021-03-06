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


extern "C" std::string
hcv_get_web_root(void)
{
  return hcv_webroot;
} // end  hcv_get_web_root

const Json::StreamWriterBuilder&
hcv_get_json_builder(void)
{
  return  hcv_json_builder;
} // end  hcv_get_json_builder


/// this function should *NOT* be public (for readability reasons)
static inline long
hcv_incremented_request_counter(void)
{
#if __GNUC__ >= 9
  return 1+std::atomic_fetch_add(&hcv_web_request_counter, 1);
#else
  //return __sync_fetch_and_add(&hcv_web_request_counter, 1);
  return 1+__sync_fetch_and_add(reinterpret_cast<long*>(&hcv_web_request_counter),
                                1L);
#endif /* __GNUC__ >= 9 */
} // end hcv_incremented_request_counter


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
  if (webroot[webroot.size()-1] != '/')
    HCV_FATALOUT("hcv_initialize_web: webroot directory " << webroot
                 << " does not end with a terminating slash" <<std::endl
                 << "... but it should." << std::endl
                 << " For example something like /var/helpcovid/webroot/");
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

  hcv_webserver->set_mount_point("/", webroot.c_str());
} // end hcv_initialize_web


void
hcv_stop_web()
{
  HCV_DEBUGOUT("start of hcv_stop_web");
  HCV_ASSERT(hcv_webserver);
  hcv_webserver->stop();
  delete hcv_webserver;
  hcv_webserver = nullptr;
  HCV_SYSLOGOUT(LOG_NOTICE, "hcv_stop_web stopped the web service");
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

#define HCV_WEBCOOKIE_RANDOMSTR_WIDTH 24 /* also width of wcookie_random in hcv_database.cc */
static std::string
hcv_web_make_cookie_string(long id, const char*randomstr, int webhash)
{
  char buf[64];
  memset (buf, 0, sizeof(buf));
  HCV_ASSERT(id>0);
  snprintf(buf, sizeof(buf), "HCV%06lx-%24s-A%08x",
	   id, randomstr, webhash);
  return std::string(buf);
} // end hcv_web_make_cookie_string

bool
hcv_web_extract_cookie_string(const std::string&str, long *id, char randomstr[HCV_WEBCOOKIE_RANDOMSTR_WIDTH+4], int*phash)
{
  long xid=0;
  int xhash=0;
  if (str.size() < HCV_WEBCOOKIE_RANDOMSTR_WIDTH+8)
    return false;
  char xrandombuf[HCV_WEBCOOKIE_RANDOMSTR_WIDTH+4];
  memset (xrandombuf, 0, sizeof(xrandombuf));
  if (sscanf(str.c_str(), "HCV%lx-%24[A-Za-z0-9]-A%x", &xid,xrandombuf, &xhash) < 3)
    return false;
  if (id)
    *id = xid;
  if (randomstr)
    strncpy(randomstr, xrandombuf, HCV_WEBCOOKIE_RANDOMSTR_WIDTH);
  if (phash)
    *phash = xhash;
  return true;
} // end hcv_web_extract_cookie_string


static constexpr unsigned hcv_web_cookie_duration = 5400; // in seconds, so one hour and a half
/// register a fresh cookie in the database and return it.
/// see also https://tools.ietf.org/html/rfc6265
#warning we may want to implement secure or httponly web cookies, see RFC6265
std::string
hcv_web_register_fresh_cookie(Hcv_http_template_data*htpl)
{
  static constexpr const char alphanumchars[]=
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static constexpr unsigned nbalphanum=62;
  static_assert(sizeof(alphanumchars)==nbalphanum+1);
  if (!htpl) {
    HCV_SYSLOGOUT(LOG_WARNING, "hcv_web_register_fresh_cookie: missing htpl");
    return "";
  }
  auto hreq = htpl->request();;
  if (!hreq) {
    HCV_SYSLOGOUT(LOG_WARNING,
		  "hcv_web_register_fresh_cookie: missing web request, reqnum#"
		  << htpl->serial());
    return "";
  };
  auto hresp = htpl->response();
  if (!hresp) {
    HCV_SYSLOGOUT(LOG_WARNING,
		  "hcv_web_register_fresh_cookie: missing web response, reqnum#"
		  << htpl->serial());
    return "";
  };
  std::string webagentstr;
  {
    auto webagendit = hreq->headers.find("User-Agent");
    if (webagendit != hreq->headers.end())
      webagentstr = webagendit->second;
  }
  auto reqnum = htpl->request_number();
  char randombuf[HCV_WEBCOOKIE_RANDOMSTR_WIDTH+4];
  memset (randombuf, 0, sizeof(randombuf));
  for (unsigned ix=0; ix<HCV_WEBCOOKIE_RANDOMSTR_WIDTH; ix++) {
    char uc=0;
    uc = alphanumchars[Hcv_Random::random_quickly_8bits() % nbalphanum];
    // we don't want too much O digits, so ....
    if (ix%2) {
      while (uc=='0')
      uc = alphanumchars[Hcv_Random::random_quickly_8bits() % nbalphanum];
    }
    randombuf[ix] = uc;
  };
  time_t expiret = 0;
  if (time(&expiret)<0)
    HCV_FATALOUT("hcv_web_register_fresh_cookie time(2) failed");
  expiret += hcv_web_cookie_duration;
  int webagenthash = 0;
  if (!webagentstr.empty()) {
    webagenthash = std::hash<std::string>{}(webagentstr);
    if (webagenthash == 0)
      webagenthash = webagentstr.size();
  }
  HCV_DEBUGOUT("hcv_web_register_fresh_cookie reqnum#" << reqnum << " randombuf=" << randombuf
	       << " expiret=" << expiret << " webagentstr='" << webagentstr
	       << "', webagenthash=" << webagenthash);
  long id = hcv_database_get_id_of_added_web_cookie(std::string(randombuf), expiret, webagenthash);
  HCV_DEBUGOUT("hcv_web_register_fresh_cookie reqnum#" << reqnum << " randombuf=" << randombuf
	       << " webagenthash=" << webagenthash
	       << " id=" << id);
  std::string res = hcv_web_make_cookie_string(id, randombuf, webagenthash);
  HCV_DEBUGOUT("hcv_web_register_fresh_cookie reqnum#" << reqnum << " gives " << res);
  char agebuf[32];
  memset(agebuf, 0, sizeof(agebuf));
  snprintf(agebuf, sizeof(agebuf), "; Max-Age=%u",  hcv_web_cookie_duration);
  std::string cookiestr=  std::string(HCV_COOKIE_NAME "=") + res+ agebuf;
  hresp->set_header("Set-Cookie", cookiestr);
  HCV_DEBUGOUT ("hcv_web_register_fresh_cookie reqnum#" << reqnum
		<< " SETCOOKIE cookiestr=" << cookiestr);
  return res;
} // end hcv_web_register_fresh_cookie


void
hcv_web_forget_cookie(Hcv_http_template_data*htpl)
{
  
  if (!htpl) {
    HCV_SYSLOGOUT(LOG_WARNING, "hcv_web_forget_cookie: missing htpl");
    return;
  }
  auto hreq = htpl->request();
  if (!hreq) {
    HCV_SYSLOGOUT(LOG_WARNING,
		  "hcv_web_forget_cookie: missing web request, reqnum#"
		  << htpl->serial());
    return;
  };
  auto hresp = htpl->response();
  if (!hresp) {
    HCV_SYSLOGOUT(LOG_WARNING,
		  "hcv_web_forget_cookie: missing web response, reqnum#"
		  << htpl->serial());
    return;
  };
  
  constexpr const char* forgetcookie
    = HCV_COOKIE_NAME "=; Expires=" HCV_HTTP_DATE_RFC822_LONG_TIME_AGO;
  HCV_DEBUGOUT("hcv_web_forget_cookie reqnum#" << htpl->serial()
	       << " method " << hreq->method
	       << " path=" << hreq->path <<  " CLEARSETCOOKIE" << std::endl
	       << forgetcookie << std::endl);
  hresp->set_header("Set-Cookie", forgetcookie);
} // end of hcv_web_forget_cookie
  
////////////////////////////////////////////////////////////////

void
hcv_web_get_json_status(const httplib::Request&req, httplib::Response& resp, long reqcnt, double startcputime, double startmonotonictime)
{
  HCV_DEBUGOUT("hcv_web_get_json_status start path=" << req.path << " req#" << reqcnt);
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
    jsob["process_size"] = (Json::Value::Int)procsize;
  if (procrss>0)
    jsob["process_rss"] = (Json::Value::Int)procrss;
  if (procshared>0)
    jsob["process_shared"] = (Json::Value::Int)procshared;
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
  jsob["nowtime"] = (Json::Value::Int64) nowt;
  jsob["pid"] = (Json::Value::Int)getpid();
  jsob["web_request_count"] =  (Json::Value::Int64)reqcnt;
  jsob["cxx"] = hcv_cxx_compiler;
  jsob["build_time"] = hcv_timestamp;
  jsob["build_timestamp"] =  (Json::Value::Int64)hcv_timelong;
  {
    auto pluginvect = hcv_get_loaded_plugins_vector();
    if (!pluginvect.empty()) {
      Json::Value jsarr(Json::arrayValue);
      for (auto curplugname : pluginvect)
	jsarr.append(curplugname);
      jsob["plugins"] = jsarr;
    }
  }
  auto str = Json::writeString(hcv_json_builder, jsob);
  resp.set_content(str.c_str(), "application/json");
} // end hcv_web_get_json_status


void
hcv_web_get_html_status(const httplib::Request&req, httplib::Response& resp, long reqcnt, double startcputime, double startmonotonictime)
{
  HCV_DEBUGOUT("hcv_web_get_html_status start path=" << req.path << " req#" << reqcnt);
  Hcv_http_template_data statusdata(req, resp, reqcnt);
  /// sleep a tiny bit against abusive usage...
  usleep (500+Hcv_Random::random_quickly_8bits());
  std::ostringstream outstatus;
  outstatus << 
    R"statusprefix(<!DOCTYPE html>
<html>
<head>
 <meta charset="utf-8">
 <title>HelpCovid builtin status</title>
</head>
<body>
<h1><i>HelpCovid</i> status</h1>
)statusprefix" <<std::endl;

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

  outstatus
    << "<p>"
    "<a href='https://github.com/bstarynk/helpcovid/'>HelpCovid</a>"
    " (a GPLv3+ <a href='https://www.gnu.org/philosophy/free-sw.en.html>'>free software</a>"
    " <b>without warranty</b>)" << std::endl
    << "running on <a href='" << hcv_weburl << "'>"
    << hcv_weburl << "</a>.</p>" << std::endl;
  outstatus << "<ul>" <<std::endl;
  outstatus << "<li>lastgitcommit: <tt>"
	    <<  hcv_lastgitcommit << "</tt></li>" << std::endl;
  outstatus << "<li>md5sum: <tt>"
	    << hcv_md5sum << "</tt></li>" << std::endl;
  outstatus << "<li>gitid: <tt>"
	    << hcv_gitid << "</tt></li>" << std::endl;
  outstatus << "<li>built: <tt>"
	    << hcv_timestamp << "</tt></li>" << std::endl;
  outstatus << "<li>total CPU time: <tt>"
	    <<  (hcv_process_cpu_time() - startcputime)
	    << "</tt> (seconds)</li>" << std::endl;
  outstatus << "<li>total elapsed time: <tt>"
	    <<  (hcv_monotonic_real_time() - startmonotonictime) 
	    << "</tt> (seconds)</li>" << std::endl;
  outstatus << "<li>CPU time per request: <tt>"
	    <<   (1.0e6
		  *(hcv_process_cpu_time() - startcputime) / reqcnt)
	    << "</tt> (µs)</li>" << std::endl;
  outstatus << "<li>elapsed time per request: <tt>"
	    <<   (1.0e3
		  *(hcv_process_cpu_time() - startcputime) / reqcnt)
	    << "</tt> (ms)</li>" << std::endl;
  if (procsize>0)
    outstatus << "<li>process size: <tt>"
	      << (procsize*4) << "</tt> kilobytes.</li>" << std::endl;
  if (procrss>0)
    outstatus << "<li>process RSS: <tt>"
	      << (procrss*4) << "</tt> kilobytes.</li>" << std::endl;
  if (procshared>0)
    outstatus << "<li>process shared: <tt>"
	      << (procshared*4) << "</tt> kilobytes.</li>" << std::endl;
  outstatus << "<li><a href='https://postgresql.org/'>PostGreSQL</a> version: <tt>"
	    << hcv_postgresql_version() << "</tt>.</li>"  << std::endl;
  {
    time_t nowt = 0;
    time(&nowt);
    struct tm nowtm;
    memset (&nowtm, 0, sizeof(nowtm));
    char nowbuf[80];
    memset (nowbuf, 0, sizeof(nowbuf));
    localtime_r (&nowt, &nowtm);
    strftime(nowbuf, sizeof(nowbuf), "%c %Z", &nowtm);
    outstatus << "<li>current time: <tt>" << nowbuf << "</tt></li>" << std::endl;
  }
  {
    char hostbuf[64];
    memset(hostbuf, 0, sizeof(hostbuf));
    gethostname(hostbuf, sizeof(hostbuf));
    outstatus << "<li>host: <tt>" << hostbuf << "</tt></li>" << std::endl;
  }
  outstatus << "<li>pid: <tt>" << ((long)getpid()) << "</tt></li>" << std::endl;
  outstatus << "<li>web request count: <tt>" << reqcnt  << "</tt></li>" << std::endl;
  outstatus << "<li>compiled with: <tt>" << hcv_cxx_compiler << "</tt></li>" << std::endl;
  {
    auto pluginvect = hcv_get_loaded_plugins_vector();
    if (!pluginvect.empty()) {
      outstatus << "<li>plugins: ";
      int plcnt = 0;
      Json::Value jsarr(Json::arrayValue);
      for (auto curplugname : pluginvect) {
	if (plcnt++ > 0) outstatus << ",";
	outstatus << " <tt>" << curplugname << "</tt>";	
      }
      outstatus << " ;</li>" << std::endl;
    }      
  }
  outstatus << "</ul>" << std::endl;
  outstatus << "<hr/>" << std::endl;
  outstatus << "<p><small>Use <tt>" << "<a href='" << hcv_weburl
	    << "/status.json'>status.json</a></tt> to get "
    " programmatically the same information.</small></p>"
	    << std::endl;
  outstatus << std::endl;
  outstatus << "</body>\n</html>" << std::endl;
  outstatus << std::flush;
  usleep (1000+Hcv_Random::random_quickly_8bits());
  resp.set_content(outstatus.str().c_str(), "text/html");
} // end hcv_web_get_html_status



void
hcv_webserver_run(void)
{
  static double startcputime = hcv_process_cpu_time();
  static double startmonotonictime = hcv_monotonic_real_time();
  unsigned webport=1000000;
  unsigned defaultwebport=2000000;
  if (getuid() == 0)
    defaultwebport = webport = 80;
  else
    defaultwebport= webport = 8080;
  if (!hcv_webserver)
    HCV_FATALOUT("no hcv_webserver");
  //
  if (hcv_weburl.empty())
    HCV_FATALOUT("no hcv_weburl");
  HCV_SYSLOGOUT(LOG_INFO, "Starting HelpCovid web server hcv_webserver@"
		<< (void*)hcv_webserver
                << " with hcv_weburl=" << hcv_weburl
		<< " and " <<  hcv_http_max_threads << " threads and "
		<< hcv_http_payload_max << " maximal payload");
  // parse hcv_weburl
  char webhost[64];
  {
    int endpos= -1;
    memset(webhost,0,sizeof(webhost));
    if (sscanf(hcv_weburl.c_str(), "http://%60[a-zA-Z0-9_.-]:%u%n", webhost, &webport, &endpos)>=2
	&& endpos>0 && webhost[0] != (char)0 && webport>0)
      {
	HCV_DEBUGOUT("helpcovid HTTP webhost='" << webhost << "' webport=" << webport);
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" //
		      << webhost << " webport=" << webport);
      }
    else if ((memset(webhost,0,sizeof(webhost))), (endpos= -1),
	     sscanf(hcv_weburl.c_str(), "https://%60[a-zA-Z0-9_.-]:%u%n", webhost, &webport, &endpos)>=2
	     && endpos>0 && webhost[0] != (char)0 && webport>0)
      {
	HCV_DEBUGOUT("helpcovid HTTPS webhost='" << webhost << "' webport=" << webport);
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" //
		      << webhost << " webport=" << webport);
      }
    else if ((memset(webhost,0,sizeof(webhost))), (endpos= -1),
	     sscanf(hcv_weburl.c_str(), "http://%60[a-zA-Z0-9_.-]%n", webhost, &endpos)>=1
	     && webhost[0] != (char)0 && endpos>0)
      {
	webport = defaultwebport;
	HCV_DEBUGOUT("helpcovid HTTP webhost='" << webhost << "' default webport=" << webport);
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" //
		      << webhost << " default webport=" << webport);
      }
    else if ((memset(webhost,0,sizeof(webhost))), (endpos= -1),
	     sscanf(hcv_weburl.c_str(), "https://%60[a-zA-Z0-9_.-]%n", webhost, &endpos)>=1
	     && webhost[0] != (char)0 && endpos>0)
      {
	webport = defaultwebport;
	HCV_DEBUGOUT("helpcovid HTTPS webhost='" << webhost << "' default webport=" << webport);
        HCV_SYSLOGOUT(LOG_INFO, "weburl=" << hcv_weburl << " listening on webhost=" //
		      << webhost << " default webport=" << webport);
      }
    else
      HCV_FATALOUT("bad hcv_weburl " << hcv_weburl);
  }
  HCV_DEBUGOUT("hcv_webserver_run with webport=" << webport
	       << " weburl= '" << hcv_weburl<< "'..");
  hcv_webserver->set_error_handler
  ([](const httplib::Request& req,
      httplib::Response& resp)
  {
    auto n = std::atomic_load(&hcv_web_request_counter);
    HCV_DEBUGOUT("error web handling '" << req.path
		 << "' req#" << n);
    hcv_web_error_handler(req, resp, n);
  });
  //////////////// /status.json serving
  hcv_webserver->Get("/status.json",
                     [](const httplib::Request&req, httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    if (reqcnt<=0)
      reqcnt=1;
       HCV_DEBUGOUT("status.json URL handling GET path '" << req.path
		    << "' req#" << reqcnt);
       hcv_web_get_json_status(req, resp, reqcnt, startcputime, startmonotonictime);
  });
  ////////////////////////////////////////////////////////////////
  //////////////// /status.html serving
  hcv_webserver->Get("/status.html",
                     [](const httplib::Request&req, httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    if (reqcnt<=0)
      reqcnt=1;
       HCV_DEBUGOUT("status.html URL handling GET path '" << req.path
		    << "' req#" << reqcnt);
    hcv_web_get_html_status(req, resp, reqcnt, startcputime, startmonotonictime);
  });

  ////////////////////////////////////////////////////////////////
  //////////////// /ajax/ serving
  hcv_webserver->Get
    ("/ajax/",
     [](const httplib::Request&req, httplib::Response&)
     {
       errno = 0;
       long reqcnt = hcv_incremented_request_counter();
       HCV_DEBUGOUT("ajax URL handling POST path '" << req.path
		    << "' req#" << reqcnt);
       HCV_SYSLOGOUT(LOG_WARNING,
		     "hcv_webserver_run AJAX GET request unimplemented path="
		     << req.path);
#warning hcv_webserver_run unimplemented AJAX GET
		     });

  hcv_webserver->Post
    ("/ajax/",
     [](const httplib::Request&req, httplib::Response&)
     {
       errno = 0;
       long reqcnt = hcv_incremented_request_counter();
       HCV_DEBUGOUT("ajax URL handling POST path '" << req.path
		    << "' req#" << reqcnt);
       HCV_SYSLOGOUT(LOG_WARNING,
		     "hcv_webserver_run AJAX POST request unimplemented path="
		     << req.path);
#warning hcv_webserver_run unimplemented AJAX POST
		     });
		     
  ////////////////////////////////////////////////////////////////
  
  hcv_webserver->Get("", [](const httplib::Request& req,
                             httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("root URL handling GET path '" << req.path
		 << "' req#" << reqcnt);
    std::string htmlcont;
    htmlcont = hcv_home_view_get(req, resp, reqcnt);
    if (htmlcont.size() > HCV_HTML_RESPONSE_MAX_LEN)
      HCV_FATALOUT("root URL handling GET sending too many bytes " << htmlcont.size());
    resp.set_content(htmlcont, "text/html");
  });
  hcv_webserver->Get("/", [](const httplib::Request& req,
                             httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("root URL handling GET path '" << req.path
		 << "' req#" << reqcnt);
    resp.set_content(hcv_home_view_get(req, resp, reqcnt), "text/html");
  });
  hcv_webserver->Get("^/?$", [](const httplib::Request& req,
                             httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("root URL handling GET path '" << req.path
		 << "' req#" << reqcnt);
    std::string htmlcont;
    htmlcont = hcv_home_view_get(req, resp, reqcnt);
    if (htmlcont.size() > HCV_HTML_RESPONSE_MAX_LEN)
      HCV_FATALOUT("root URL handling GET sending too many bytes " << htmlcont.size());
    resp.set_content(htmlcont, "text/html");
  });

  //////////////// /login/ serving
  hcv_webserver->Get("/login", [](const httplib::Request& req,
                                  httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("login URL handling GET path '" << req.path
		 << "' req#" << reqcnt);
    std::string htmlcont;
    htmlcont = hcv_login_view_get(req, resp, reqcnt);
    if (htmlcont.size() > HCV_HTML_RESPONSE_MAX_LEN)
      HCV_FATALOUT("login URL handling POST sending too many bytes " << htmlcont.size());
    HCV_DEBUGOUT("login URL handling GET sending " << htmlcont.size() << " bytes in response");;
    resp.set_content(htmlcont, "text/html");
  });
  ///////
  hcv_webserver->Post("/ajax/login", [](const httplib::Request& req, 
                                   httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("login URL handling POST path '" << req.path
		 << "' req#" << reqcnt);
    std::string jsoncont;
    jsoncont = hcv_login_view_post(req, resp, reqcnt);
    if (jsoncont.size() > HCV_JSON_RESPONSE_MAX_LEN)
      HCV_FATALOUT("login URL handling POST sending too many bytes " << jsoncont.size());
    resp.set_content(jsoncont, "application/json");
  });
  //////////////// /register/ serving
  hcv_webserver->Get("/register", [](const httplib::Request& req,
                                  httplib::Response& resp)
  {
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("register URL handling GET path '" << req.path
		 << "' req#" << reqcnt);
    std::string htmlcont;
     htmlcont = hcv_register_view_get(req, resp, reqcnt);;
    if (htmlcont.size() > HCV_HTML_RESPONSE_MAX_LEN)
      HCV_FATALOUT("register URL handling POST sending too many bytes " << htmlcont.size());
    HCV_DEBUGOUT("register URL handling GET sending " << htmlcont.size() << " bytes in response");
    resp.set_content(htmlcont, "text/html");
  });
  ///////
  hcv_webserver->Post("/register", [](const httplib::Request& req, 
                                   httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("register URL handling POST path '" << req.path
		 << "' req#" << reqcnt);
    std::string jsoncont;
    jsoncont = hcv_register_view_post(req, resp, reqcnt);
    if (jsoncont.size() > HCV_JSON_RESPONSE_MAX_LEN)
      HCV_FATALOUT("register URL handling POST sending too many bytes " << jsoncont.size());
    HCV_DEBUGOUT("register URL handling POST sending " << jsoncont.size() << " bytes in response");
    resp.set_content(jsoncont, "application/json");
  });
  ////////////////////////////////////////////////////////////////
  
  hcv_webserver->Get("/profile", [](const httplib::Request& req,
                                    httplib::Response& resp)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("profile GET URL: '" << req.path << "' req # " << reqcnt);

    std::string html = hcv_profile_view_get(req, resp, reqcnt);
    if (html.size() > HCV_HTML_RESPONSE_MAX_LEN)
      HCV_FATALOUT("profile GET view sent too many bytes: " << html.size());

    HCV_DEBUGOUT("profile GET view sent " << html.size() << " bytes");
    resp.set_content(html, "text/html");
  });

  //////////////// /images/ serving
  hcv_webserver->Get("/images/", [](const httplib::Request& req,
                                  httplib::Response&)
  {
    errno = 0;
    long reqcnt = hcv_incremented_request_counter();
    HCV_DEBUGOUT("images URL handling GET path '" << req.path
		 << "' req#" << reqcnt);
#warning hcv_webserver->Get("/images/"...) dont work
    /* we probably need a hcv_webserver->GetPrefixed function */
    HCV_SYSLOGOUT(LOG_WARNING,
		  "hcv_webserver_run GET /images request unimplemented path="
		  << req.path);
  }); // end /images/ serving
  ////////
  //////// initialize plugins, if any
  hcv_initialize_plugins_for_web(hcv_webserver);
  ////////////////////////////////////////////////////////////////
  hcv_webserver->listen(webhost, webport);
  HCV_SYSLOGOUT(LOG_INFO, "end hcv_webserver_run webhost=" << webhost << " webport=" << webport);
} // end hcv_webserver_run


//////////////////// end of file hcv_web.cc of github.com/bstarynk/helpcovid
