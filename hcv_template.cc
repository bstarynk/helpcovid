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

static std::map<std::string, hcv_template_expanding_closure_t> hcv_template_expander_dict;
static std::recursive_mutex hcv_template_mtx;

#define HCV_TEMPLATE_NAME_MAXLEN 64
void
hcv_register_template_expander_closure(const std::string&name, const hcv_template_expanding_closure_t&expfun)
{
  if (name.empty() || !(std::isalpha(name[0])||name[0]=='_'))
    HCV_FATALOUT("hcv_register_expander_closure: invalid name '"<< name <<"' for expander.");
  if (!expfun)
    HCV_FATALOUT("hcv_register_expander_closure: name '"<< name <<"' for expander without closure.");
  if (strlen(name.c_str()) > HCV_TEMPLATE_NAME_MAXLEN)
    HCV_FATALOUT("hcv_register_expander_closure: too long name '"<< name <<"' for expander.");
  for (char c: name)
    if (!std::isalnum(c) && c!='_')
      HCV_FATALOUT("hcv_register_expander_closure: bad name '"<< name <<"' for expander.");
  std::lock_guard<std::recursive_mutex> gu(hcv_template_mtx);
  hcv_template_expander_dict.insert({name,expfun});
} // end hcv_register_expander_closure



void
hcv_forget_template_expander(const std::string&name)
{
  std::lock_guard<std::recursive_mutex> gu(hcv_template_mtx);
  auto it = hcv_template_expander_dict.find(name);
  if (it == hcv_template_expander_dict.end())
    {
      HCV_SYSLOGOUT(LOG_WARNING,"hcv_forget_template_expander: unknown name='" << name << "'");
      return;
    };
  hcv_template_expander_dict.erase(it);
} // end hcv_forget_template_expander



void
hcv_expand_processing_instruction(std::ostream&out, const std::string &procinstr, const char*filename, int lineno, long offset)
{
  char namebuf[80];
  memset (namebuf, 0, sizeof(namebuf));
  static_assert (sizeof(namebuf) >= HCV_TEMPLATE_NAME_MAXLEN, "too short namebuf");
  int endpos = -1;
  if (sscanf(procinstr.c_str(), "<?hcv %64[a-zA-Z0-9_] %n", namebuf, &endpos)<1 || endpos<0)
    {
      HCV_SYSLOGOUT(LOG_WARNING,"hcv_expand_processing_instruction: invalid procinstr='" << procinstr
                    << "' in " << (filename?:"**??**")
                    << ":" << lineno << " @" << offset);
      return;
    }
  auto endpi = strstr(procinstr.c_str()+endpos, "?>");
  if (!endpi || endpi[2])
    HCV_FATALOUT("hcv_expand_processing_instruction: corrupted procinstr='" << procinstr
                 << "' in " << (filename?:"**??**")
                 << ":" << lineno << " @" << offset);
  std::string arg = procinstr.substr(endpos,endpi-procinstr.c_str());
  std::lock_guard<std::recursive_mutex> gu(hcv_template_mtx);
  auto it = hcv_template_expander_dict.find(std::string(namebuf));
  if (it == hcv_template_expander_dict.end())
    {
      HCV_SYSLOGOUT(LOG_WARNING,"hcv_expand_processing_instruction: unknown namebuf='" << namebuf << "'");
      return;
    };
  hcv_template_expanding_closure_t clos = it->second;
  return clos(out,procinstr,filename,lineno,offset);
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
      int col=0, prevcol=0, lqpos=0, qrpos=0;
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
        outp << std::flush;
    }
  outp<<std::endl;
  return outp.str();
} // end hcv_expand_template_file

/************* end of file hcv_template in github.com/bstarynk/helpcovid*/
