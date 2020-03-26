/****************************************************************
 * file hcv_background.cc of https://github.com/bstarynk/helpcovid
 *
 * Description:
 *      Implementation of background processing
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


extern "C" const char hcv_background_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_background_date[] = __DATE__;

void hcv_start_background_thread(void)
{
#warning unimplemented hcv_start_background_thread
  HCV_SYSLOGOUT(LOG_WARNING, "unimplemented hcv_start_background_thread");
} // end of hcv_start_background_thread

/************************ end of file hcv_background.cc in github.com/bstarynk/helpcovid ***/
