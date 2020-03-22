/****************************************************************
 * file hcv_database.cc
 *
 * Description:
 *      PostGreSQL database of https://github.com/bstarynk/helpcovid
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

extern "C" const char hcv_database_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_database_date[] = __DATE__;

std::unique_ptr<pqxx::connection> hcv_dbconn;

void
hcv_initialize_database(const std::string&uri)
{
  if (uri.empty() || uri=="-")
    {
      HCV_SYSLOGOUT(LOG_WARNING, "no PostGreSQL database initialization: " << uri);
      return;
    }
  hcv_dbconn.reset(new pqxx::connection(uri));
  {
    pqxx::work transact(*hcv_dbconn);
    ////================ user table, with mandatory data
    transact.exec0(R"crusertab(
---- TABLE tb_user
CREATE TABLE IF NOT EXISTS tb_user (
  user_id SERIAL PRIMARY KEY NOT NULL,
  user_firstname VARCHAR(31) NOT NULL,
  user_familyname VARCHAR(62) NOT NULL,
  user_email VARCHAR(71) NOY NULL,
  user_gender CHAR(1) NOT NULL,
  user_crtime DATE NOT NULL,
); --- end TABLE tb_user
)crusertab");
    ////================ password table
    transact.exec0(R"crpasswdtab(
---- TABLE tb_password
CREATE TABLE IF NOT EXISTS tb_password (
  passw_id SERIAL PRIMARY KEY NOT NULL, -- unique key in this table
  passw_userid INT NOT NULL, -- the user id whose password we store
  passw_encr VARCHAR(62) NOT NULL, -- the encrypted password
  passw_mtime  TIMESTAMP NOT NULL, -- the last time that password was modified
); --- end TABLE tb_password
)crpasswdtab");
    transact.commit();
  }
  HCV_SYSLOGOUT(LOG_NOTICE, "PostGreSQL database " << uri << " successfully initialized");
} // end hcv_initialize_database



/////////// end of file hcv_database.cc in github.com/bstarynk/helpcovid
