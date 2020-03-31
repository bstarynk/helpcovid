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

  HCV_DEBUGOUT("hcv_home_view_get '" << req.path << "' req#" << reqcnt);

  // return login .html for now
  Hcv_http_template_data webdata(req, resp, reqcnt);
  std::string thtml = hcv_get_web_root() + "html/login.html";
  return hcv_expand_template_file(thtml, &webdata);

#if 0 && old_code
  // for now, redirect to the login view by default
  hcv_login_view_get(req, resp);
#warning implement conditional redirection based on session checks
#endif /*0 && old_code*/

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
  std::string cookiestr= hcv_web_register_fresh_cookie(data);
  HCV_DEBUGOUT("hcv_register_view_get reqpath:" << req.path
               << " req#" << reqnum
               << " cookiestr=" << cookie);
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
  if (req.method != "POST")
    HCV_FATALOUT("hcv_register_view_post() called with not POST request");
  Hcv_http_template_data data(req, resp, reqnum);
  auto regtokenstr = req.get_param_value("registerToken");
  auto firstnamestr = req.get_param_value("inputFirstName");
  auto lastnamestr = req.get_param_value("inputLastName");
  auto genderstr = req.get_param_value("gender");
  auto phonestr = req.get_param_value("inputPhone");
  auto emailstr = req.get_param_value("inputEmail");
  auto agreestr = req.get_param_value("registerAgree");
  HCV_DEBUGOUT("hcv_register_view_post reqpath:" << req.path
               << " req#" << reqnum << std::endl
               << " .. regtoken=" << regtokenstr << std::endl
               << " .. firstname=" << firstnamestr << std::endl
               << " .. lastname=" << lastnamestr << std::endl
               << " .. phonestr=" << phonestr << std::endl
               << " .. emailstr=" << emailstr << std::endl
              );
#warning hcv_register_view_post incomplete
  HCV_SYSLOGOUT(LOG_WARNING,
                "hcv_register_view_post incomplete "
                << req.path << " req#" << reqnum);
  ///  return hcv_expand_template_file(thtml, &data);

} // end hcv_register_view_post


//////////////////// end of file hcv_views.cc of github.com/bstarynk/helpcovid

