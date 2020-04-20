/****************************************************************
 * file hcv_views.cc
 *
 * Description:
 *      Implementation of views processing HTTP requests.
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
#include <json/writer.h>



extern "C" const char hcv_views_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_views_date[] = __DATE__;


std::string
hcv_login_view_get(const httplib::Request& req, httplib::Response& resp, long reqnum)
{
  if (req.method != "GET")
    HCV_FATALOUT("hcv_login_view_get() called with non GET request");

  Hcv_http_template_data data(req, resp, reqnum);
  std::string thtml = hcv_get_web_root() + "html/login.html";

  return hcv_expand_template_file(thtml, &data);
} // end hcv_login_view_get


std::string
hcv_login_view_post(const httplib::Request& req,  httplib::Response& resp, long reqnum)
{
  if (req.method != "POST")
    HCV_FATALOUT("hcv_login_view_post() called with not POST request");
  Hcv_http_template_data data(req, resp, reqnum);

  auto email = req.get_param_value("email");
  auto passwd = req.get_param_value("password");
  bool status = hcv_user_model_authenticate(email, passwd);
  HCV_DEBUGOUT("hcv_login_view_post reqpath:" << req.path
               << " req#" << reqnum
               << " email=" << email
               << " passwd=" << passwd
               << " status=" << status);

#warning hcv_login_view_post should not wire-in French or English
  std::string msg_en = status ? "OK" : "Your e-mail address and password do not"
                       " match. Please try again.";
  std::string msg_fr = status ? "OK" : "Votre adresse e-mail et votre mot de"
                       "  passe ne correspondent pas. Veuillez réessayer.";

  Json::StreamWriterBuilder jstr;
  Json::Value jsob(Json::objectValue);
  jsob["status"] = status;
  jsob["msg_en"] = msg_en;
  jsob["msg_fr"] = msg_fr;

#warning cookie setting needs to be implemented.
  return Json::writeString(jstr, jsob);

#if 0
  std::string thtml;

#warning we should return a JSON response
  if (hcv_user_model_authenticate(email, passwd))
    thtml = hcv_get_web_root() + "html/index.html";
  else
    thtml = hcv_get_web_root() + "html/error.html";

  return hcv_expand_template_file(thtml, &data);
#warning cookie setting needs to be implemented.
#endif
} // end hcv_login_view_post


std::string
hcv_home_view_get(const httplib::Request& req, httplib::Response& resp, long reqcnt)
{
  if (req.method != "GET")
    HCV_FATALOUT("hcv_home_view_get() called with non GET request");
  //
  HCV_DEBUGOUT("hcv_home_view_get start '" << req.path << "' req#" << reqcnt);
  //
  // return login .html for now
  Hcv_http_template_data webdata(req, resp, reqcnt);
  std::string reqlang = webdata.request_language();
  HCV_DEBUGOUT("hcv_home_view_get req#" << reqcnt << " reqlang='" << reqlang << "'");
  std::string thtml = hcv_get_web_root() + "html/login.html";
  auto res =  hcv_expand_template_file(thtml, &webdata);
  HCV_ASSERT(res.size() < HCV_HTML_RESPONSE_MAX_LEN);
  hcv_web_forget_cookie(&webdata);
  HCV_DEBUGOUT("hcv_home_view_get '" << req.path << "' req#" << reqcnt
               << " response size=" << res.size());
  return res;
} // end of hcv_home_view_get





//////////////////////////////////////////////// registering a new user

std::string
hcv_view_register_form_token(Hcv_http_template_data*httpdata)
{
  if (!httpdata)
    HCV_FATALOUT("no httpdata in hcv_view_register_form_token");
  long reqnum = httpdata->request_number();
  HCV_DEBUGOUT("hcv_view_register_form_token req#" << reqnum);
  char buf[48];
  memset (buf, 0, sizeof(buf));
  unsigned ur= Hcv_Random::random_32u();
  snprintf(buf, sizeof(buf), "r%ld/%u", reqnum, ur);
  HCV_SYSLOGOUT(LOG_WARNING, "incomplete hcv_view_register_form_token buf=" << buf);
#warning hcv_view_register_form_token incomplete
  /* TODO: we need to keep that buf in some data structure and clean
     it up later using hcv_do_postpone_background */
  return std::string(buf);
} // end hcv_view_register_form_token



std::string
hcv_register_view_get(const httplib::Request& req, httplib::Response& resp, long reqnum)
{
  if (req.method != "GET")
    HCV_FATALOUT("hcv_register_view_get() called with non GET request");

  Hcv_http_template_data data(req, resp, reqnum);
  std::string thtml = hcv_get_web_root() + "html/register.html";
  std::string cookiestr= hcv_web_register_fresh_cookie(&data);
  HCV_DEBUGOUT("hcv_register_view_get reqpath:" << req.path
               << " req#" << reqnum
               << " cookiestr=" << cookiestr);
#warning hcv_register_view_get incomplete
  HCV_SYSLOGOUT(LOG_WARNING,
                "hcv_register_view_get incomplete "
                << req.path << " req#" << reqnum);
  /// notice that  <?hcv register_form_token?> is likely to be expanded below
  return hcv_expand_template_file(thtml, &data);
} // end hcv_register_view_get




// related to webroot/html/register.html, its <form> of class
// 'form-signin' near line 76
std::string
hcv_register_view_post(const httplib::Request& req,  httplib::Response& resp, long reqnum)
{
  std::string jsonres;
  if (req.method != "POST")
    HCV_FATALOUT("hcv_register_view_post() not called with POST request");
  Hcv_http_template_data data(req, resp, reqnum);
  auto regtokenstr = req.get_param_value("registerToken");
  auto firstnamestr = req.get_param_value("inputFirstName");
  auto lastnamestr = req.get_param_value("inputLastName");
  auto longitudestr = req.get_param_value("longitude");
  auto latitudestr = req.get_param_value("latitude");
  auto genderstr = req.get_param_value("gender");
  auto phonestr = req.get_param_value("inputPhone");
  auto emailstr = req.get_param_value("inputEmail");
  auto agreestr = req.get_param_value("registerAgree");
  auto cookiestr = req.get_header_value("Set-Cookie");
  HCV_DEBUGOUT("hcv_register_view_post reqpath:" << req.path
               << " req#" << reqnum << std::endl
               << " .. regtoken=" << regtokenstr << std::endl
               << " .. firstname=" << firstnamestr << std::endl
               << " .. lastname=" << lastnamestr << std::endl
               << " .. longitude=" << longitudestr << std::endl
               << " .. latitude=" << latitudestr << std::endl
               << " .. gender=" << genderstr << std::endl
               << " .. phonestr=" << phonestr << std::endl
               << " .. emailstr=" << emailstr << std::endl
               << " .. cookiestr=" << cookiestr << std::endl
              );
#warning hcv_register_view_post incomplete
  HCV_SYSLOGOUT(LOG_WARNING,
                "hcv_register_view_post incomplete "
                << req.path << " req#" << reqnum);
  Json::StreamWriterBuilder jstr;
  Json::Value jsob(Json::objectValue);
  jsob["unimplemented_cxx_function"]
    = Json::Value(Json::StaticString("hcv_expand_template_file"));
  jsob["unimplemented_request_number"] = (Json::Value::Int64)reqnum;
  jsob["unimplemented_request_path"]
    = Json::Value((const char*)(req.path.c_str()));
  jsob["unimplemented_cxx_file"]
    = Json::Value(Json::StaticString(__FILE__));
  jsob["unimplemented_cxx_line"] = (Json::Value::Int)__LINE__;
  jsob["gitid"]
    = Json::Value(Json::StaticString(hcv_gitid));
  jsonres = Json::writeString(hcv_get_json_builder(), jsob);
  HCV_DEBUGOUT("hcv_register_view_post reqpath:" << req.path << " unimplemented for gitid "
               << std::string (hcv_gitid, 16) << "...; jsonres=" << std::endl
               << jsonres);
#warning hcv_register_view_post unimplemented
  return jsonres;
  ///  return hcv_expand_template_file(thtml, &data);
} // end hcv_register_view_post


std::string
hcv_profile_view_get(const httplib::Request& req, httplib::Response& resp,
                     long reqnum)
{
  if (req.method != "GET")
    HCV_FATALOUT("hcv_profile_view_get() called with non GET request");

  Hcv_http_template_data data(req, resp, reqnum);
  std::string thtml = hcv_get_web_root() + "html/profile.html";
  HCV_DEBUGOUT("hcv_profile_view_get reqpath:" << req.path
               << " req#" << reqnum);

  std::string str = hcv_expand_template_file(thtml, &data);
  HCV_DEBUGOUT("hcv_profile_view_get reqpath:" << req.path
               << " req#" << reqnum << " gives " << str.size() << " bytes");
  return str;
} // end hcv_profile_view_get


///////////////////////////
// message views - to emit some message (usually request specific, e.g. localized, or customized)
///////////////////////////////////////////////////////////////////////////////
extern "C" std::string  /// for <?hcv msg ...?>
hcv_view_expand_msg(Hcv_http_template_data*tdata, const std::string &procinstr,
                    const char*filename, int lineno, long offset)
{
  HCV_ASSERT(tdata != nullptr && tdata->request() != nullptr && tdata->response() != nullptr);
  HCV_DEBUGOUT("hcv_view_expand_msg " << procinstr << " @STARTMSG@ at "  << filename << ":" << lineno);
  char msgidbuf[40];
  memset (msgidbuf, 0, sizeof(msgidbuf));
  int endp = -1;
  if (sscanf(procinstr.c_str(), "<?hcv msg %38[A-Za-z0-9_] %n", msgidbuf, &endp) >= 1
      && endp > 0)
    {
      if (!isalpha(msgidbuf[0]))
        HCV_FATALOUT("hcv_view_expand_msg " << procinstr << " at "  << filename << ":" << lineno
                     << "invalid msgidbuf:" << msgidbuf);
      HCV_DEBUGOUT("hcv_view_expand_msg " << procinstr << " at "  << filename << ":" << lineno
                   << "msgidbuf:" << msgidbuf << ".");
      const char*begmsg = procinstr.c_str() + endp;
      const char*endmsg = strstr(begmsg, "?>");
      HCV_ASSERT(endmsg != nullptr);
      std::string chunkent = hcv_get_chunkmap_entry(std::string(msgidbuf));
      if (!chunkent.empty())
        {
          HCV_DEBUGOUT("hcv_view_expand_msg chunked msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                       << " => " << chunkent);
          return chunkent;
        }
      /// see http://man7.org/linux/man-pages/man7/locale.7.html
      /// see http://man7.org/linux/man-pages/man5/locale.5.html
      /// see http://man7.org/linux/man-pages/man3/dgettext.3.html
      char* localizedmsg = dgettext(HCV_DGETTEXT_DOMAIN, msgidbuf);
      std::string reqlang = tdata->request_language();
      std::string chunknam{msgidbuf};
      std::string langchunknam = (reqlang.empty()?chunknam:(chunknam+"_"+reqlang));
      std::string entry;
      HCV_DEBUGOUT("hcv_view_expand_msg chunked langchunknam='" << langchunknam << "'");
      if (localizedmsg && strcmp(localizedmsg, msgidbuf))
        {
          HCV_DEBUGOUT("hcv_view_expand_msg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                       << " => " << localizedmsg);
          return std::string(localizedmsg);
        }
      else if (!(entry=hcv_get_chunkmap_entry(langchunknam)).empty())
        {
          HCV_DEBUGOUT("hcv_view_expand_msg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                       << " => chunkentry " << entry);
          return entry;
        }
      else
        {
          HCV_SYSLOGOUT(LOG_NOTICE, "hcv_view_expand_msg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                        << " not found");
          std::string rawmsg(begmsg, endmsg-begmsg);
          HCV_DEBUGOUT("hcv_view_expand_msg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                       << ":::" << rawmsg);
          return rawmsg;
        }
    }
  else
    {
      HCV_SYSLOGOUT(LOG_WARNING,
                    "hcv_view_expand_msg bad PI " << procinstr
                    << " at " << filename << ":" << lineno << "@" << offset);
      return "";
    }
} // end hcv_view_expand_msg




extern "C" std::string  /// for <?hcv confmsg ...?>
hcv_view_expand_confmsg(Hcv_http_template_data*tdata, const std::string &procinstr,
                        const char*filename, int lineno, long offset)
{
  HCV_ASSERT(tdata != nullptr && tdata->request() != nullptr && tdata->response() != nullptr);
  HCV_DEBUGOUT("hcv_view_expand_msg " << procinstr << " @STARTMSG@ at "  << filename << ":" << lineno);
  char msgidbuf[40];
  memset (msgidbuf, 0, sizeof(msgidbuf));
  int endp = -1;
  if (sscanf(procinstr.c_str(), "<?hcv confmsg %38[A-Za-z0-9_] %n", msgidbuf, &endp) >= 1
      && endp > 0)
    {
      if (!isalpha(msgidbuf[0]))
        HCV_FATALOUT("hcv_view_expand_confmsg " << procinstr << " at "  << filename << ":" << lineno
                     << "invalid msgidbuf:" << msgidbuf);
      HCV_DEBUGOUT("hcv_view_expand_confmsg " << procinstr << " at "  << filename << ":" << lineno
                   << "msgidbuf:" << msgidbuf << ".");
      const char*begmsg = procinstr.c_str() + endp;
      const char*endmsg = strstr(begmsg, "?>");
      HCV_ASSERT(endmsg != nullptr);
#if 0 && not_implemented_hcv_view_expand_confmsg
      char* localizedmsg = dgettext(HCV_DGETTEXT_DOMAIN, msgidbuf);
      if (localizedmsg && strcmp(localizedmsg, msgidbuf))
        {
          HCV_DEBUGOUT("hcv_view_expand_confmsg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                       << " => " << localizedmsg);
          return std::string(localizedmsg);
        }
      else
        {
          HCV_SYSLOGOUT(LOG_NOTICE, "hcv_view_expand_confmsg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                        << " not found");
          std::string rawmsg(begmsg, endmsg-begmsg);
          HCV_DEBUGOUT("hcv_view_expand_confmsg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                       << ":::" << rawmsg);
          return rawmsg;
        };
#endif  0 && not_implemented_hcv_view_expand_confmsg
      //////////@@@@@@@@@@@@@@TODO
#warning TODO: unimplemented hcv_view_expand_confmsg
      HCV_FATALOUT("hcv_view_expand_confmsg msgidbuf=" << msgidbuf << " at "  << filename << ":" << lineno
                   << " not implemented");
    }
  else
    {
      HCV_SYSLOGOUT(LOG_WARNING,
                    "hcv_view_expand_confmsg bad PI " << procinstr
                    << " at " << filename << ":" << lineno << "@" << offset);
      return "";
    }
} // end hcv_view_expand_confmsg



//////////////////// end of file hcv_views.cc of github.com/bstarynk/helpcovid

