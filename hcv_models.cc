/****************************************************************
 * file hcv_models.cc
 *
 * Description:
 *      Implementation of database models.
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


extern "C" const char hcv_models_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_models_date[] = __DATE__;


extern "C" bool
hcv_user_model_verify(const hcv_user_model& model, std::string& msg)
{
    if (model.user_first_name.empty()) {
        msg = "The first name is required";
        return false;
    }

    if (model.user_family_name.empty()) {
        msg = "The family name is required";
        return false;
    }

    if (model.user_email.empty()) {
        msg = "The e-mail address is required";
        return false;
    }

    if (model.user_gender.empty()) {
        msg = "The gender is required.";
        return false;
    }

    const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    if (!std::regex_match(model.user_email, pattern)) {
        msg = "The e-mail address is invalid";
        return false;
    }

    msg = "OK";
    return true;
}


extern "C" void
hcv_user_model_create(const hcv_user_model& model)
{
}

