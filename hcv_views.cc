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



extern "C" const char hcv_views_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_views_date[] = __DATE__;


class Hcv_LoginView
{
public:
  Hcv_LoginView(const httplib::Request& req, const httplib::Response& resp)
    : m_req(req), m_resp(resp)
  { }

  ~Hcv_LoginView()
  { }

  std::string get()
  {
    Hcv_http_template_data data(m_req, m_resp, hcv_get_web_request_counter());
    std::string thtml = hcv_get_web_root() + "html/signin.html";

    return hcv_expand_template_file(thtml, &data);
  }

  std::string post()
  {
    // TODO: user authentication needs to be implemented.

    Hcv_http_template_data data(m_req, m_resp, hcv_get_web_request_counter());
    std::string thtml = hcv_get_web_root() + "html/index.html";

#warning we need to set a cookie. See https://en.wikipedia.org/wiki/HTTP_cookie
    return hcv_expand_template_file(thtml, &data);
  }

private:
  httplib::Request m_req;
  httplib::Response m_resp;
};				// end of class Hcv_LoginView




std::string
hcv_login_view_get(const httplib::Request& req, httplib::Response& resp)
{
  Hcv_LoginView vw(req, resp);
  return vw.get();
} // end hcv_login_view_get


std::string
hcv_login_view_post(const httplib::Request& req,  httplib::Response& resp)
{
  Hcv_LoginView vw(req, resp);
  return vw.post();
} // end hcv_login_view_post


//////////////////// end of file hcv_views.cc of github.com/bstarynk/helpcovid

