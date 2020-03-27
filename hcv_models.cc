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
hcv_model_validator_required(const std::string& field, const std::string& tag,
                             std::string& msg)
{
    if (field.empty()) {
        msg = "The " + tag + " is required";
        return false;
    }

    msg = "OK";
    return true;
}


extern "C" bool
hcv_model_validator_email(const std::string& field, const std::string& tag,
                          std::string& msg)
{
    const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");

    if (!std::regex_match(field, pattern)) {
        msg = "The " + tag + " is invalid";
        return false;
    }

    msg = "OK";
    return true;
}


extern "C" bool
hcv_user_model_validate(const hcv_user_model& model, hcv_user_model& status)
{
    bool check = true;

    check &= hcv_model_validator_required(model.user_first_name, "first name",
                                          status.user_first_name);

    check &= hcv_model_validator_required(model.user_family_name, "family name",
                                          status.user_family_name);

    check &= hcv_model_validator_required(model.user_email, "e-mail address",
                                         status.user_email);

    check &= hcv_model_validator_required(model.user_gender, "gender",
                                          status.user_gender);

    if (check) {
        return hcv_model_validator_email(model.user_email, "e-mail address",
                                         status.user_email);
    }

    return true;
}


extern "C" void
hcv_user_model_create(const hcv_user_model& model)
{
}

