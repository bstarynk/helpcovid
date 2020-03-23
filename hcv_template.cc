/****************************************************************
 * file hcv_template.cc
 *
 * Description:
 *      Template machinery of https://github.com/bstarynk/helpcovid
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

extern "C" const char hcv_template_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_template_date[] = __DATE__;

extern "C" void
hcv_expand_processing_instruction(std::ostream&out, const std::string &procinstr, const char*filename, int lineno, long offset);

void
hcv_expand_processing_instruction(std::ostream&out, const std::string &procinstr, const char*filename, int lineno, long offset)
{
#warning unimplemented hcv_expand_processing_instruction
  HCV_FATALOUT("unimplemented hcv_expand_processing_instruction filename=" << (filename?:"*null*")
               << " lineno=" << lineno
               << " offset=" << offset);
} // end hcv_expand_processing_instruction

std::string
hcv_expand_template_file(const std::string& srcfilepath)
{
  static constexpr unsigned max_template_size = 128*1024;
  struct stat srcfilestat;
  memset (&srcfilestat, 0, sizeof(srcfilestat));
  if (stat(srcfilepath.c_str(), &srcfilestat))
    HCV_FATALOUT("hcv_expand_template_file: stat failure on source file " << srcfilepath);
  if (!S_ISREG(srcfilestat.st_mode))
    HCV_FATALOUT("hcv_expand_template_file: source file " << srcfilepath
                 << " is not a regular file.");
  if (srcfilestat.st_size > max_template_size)
    HCV_FATALOUT("hcv_expand_template_file: source file " << srcfilepath
                 << " is too big: "
                 << (long)srcfilestat.st_size << " bytes.");
  std::ostringstream outp;
  std::ifstream srcinp(srcfilepath);
  int lincnt = 0;
  bool gotpe = false;
  long off=0;
  for (std::string linbuf; (off=srcinp.tellg()), std::getline(srcinp, linbuf); )
    {
      gotpe = false;
      lincnt++;
      int col=0, nextcol=0, prevcol=0, lqpos=0, qrpos=0;
      while ((lqpos=linbuf.find("<?hcv ", col)>0) >=0)
        {
          outp << linbuf.substr(prevcol, lqpos-prevcol);
          qrpos = linbuf.find("?>", lqpos);
          if (qrpos<0)
            {
              HCV_SYSLOGOUT(LOG_WARNING,
                            "hcv_expand_template_file: " << srcfilepath
                            << ":" << lincnt
                            << " line has unclosed template markup:" << std::endl
                            << linbuf);
              outp << linbuf.substr(prevcol);
              continue;
            }
          std::string procinstr=linbuf.substr(prevcol, qrpos+2-prevcol);
          hcv_expand_processing_instruction(outp, procinstr, srcfilepath.c_str(), lincnt, off);
          prevcol=col;
          col = qrpos+2;
          gotpe = true;
        };
      if (gotpe)
        outp << std::fflush;
    }
  outp<<std::endl;
  return outp.str();
} // end hcv_expand_template_file

/************* end of file hcv_template in github.com/bstarynk/helpcovid*/
