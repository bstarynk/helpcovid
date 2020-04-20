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

////////////////////////////////////////////////////////////////


Hcv_template_data::~Hcv_template_data()
{
} // end Hcv_template_data::~Hcv_template_data

////////////////

Hcv_http_template_data::~Hcv_http_template_data()
{
  _hcvhttp_request = nullptr;
  _hcvhttp_response = nullptr;
  _hcvhttp_reqnum = 0;
} // end Hcv_http_template_data::~Hcv_http_template_data



std::string
Hcv_http_template_data::request_language(void) const
{
  if (!_hcvhttp_language.empty())
    return _hcvhttp_language;
  auto req = request();
  if (!req)
    return "";
  std::string langstr= req->get_header_value("Accept-Language");
  HCV_DEBUGOUT("Hcv_http_template_data::request_language langstr='" << langstr << "'");
  if (langstr.empty())
    return "";
  std::string reqlanstr;
#warning Hcv_http_template_data::request_language incomplete
  if (false) {}
#define HCV_WHENLANG(Str) else if (langstr.find(Str) != std::string::npos) reqlanstr= Str
  // see https://www.metamodpro.com/browser-language-codes
  // only two letter codes here
  HCV_WHENLANG("af"); // Afrikaans
  HCV_WHENLANG("hr"); // Croatian
  HCV_WHENLANG("el"); // Gree
  HCV_WHENLANG("pl"); // Polish
  HCV_WHENLANG("sx"); // Sutu
  HCV_WHENLANG("sq"); // Albanian
  HCV_WHENLANG("cs"); // Czech
  HCV_WHENLANG("gu"); // Gujurati
  HCV_WHENLANG("pt"); // Portugese
  HCV_WHENLANG("sw"); // Swahili
  HCV_WHENLANG("ar"); // Arabic (standard)
  HCV_WHENLANG("da"); // Danish
  HCV_WHENLANG("ht"); // Haitian
  HCV_WHENLANG("sv"); // Swedish
  HCV_WHENLANG("nl"); // Dutch
  HCV_WHENLANG("he"); // Hebrew
  HCV_WHENLANG("pa"); // Punjabi
  HCV_WHENLANG("hi"); // Hindi
  HCV_WHENLANG("en"); // English
  HCV_WHENLANG("hu"); // Hungarian
  HCV_WHENLANG("ta"); // Tamil
  HCV_WHENLANG("is"); // Icelandic
  HCV_WHENLANG("qe"); // Quechua
  HCV_WHENLANG("tt"); // Tatar
  HCV_WHENLANG("id"); // Indonesian
  HCV_WHENLANG("rm"); // Rhaeto-Romanic
  HCV_WHENLANG("te"); // Teluga
  HCV_WHENLANG("iu"); // Inuktitut
  HCV_WHENLANG("ro"); // Romanian
  HCV_WHENLANG("th"); // Thai
  HCV_WHENLANG("ga"); // Irish
  HCV_WHENLANG("tig"); // Tigre
  HCV_WHENLANG("it"); // Italian
  HCV_WHENLANG("ru"); // Russian
  HCV_WHENLANG("ts"); // Tsonga
  HCV_WHENLANG("tn"); // Tswana
  HCV_WHENLANG("ja"); // Japanese
  HCV_WHENLANG("sz"); // Sami
  HCV_WHENLANG("tr"); // Turkish
  HCV_WHENLANG("kn"); // Kannada
  HCV_WHENLANG("sg"); // Sango
  HCV_WHENLANG("tk"); // Turkmen
  HCV_WHENLANG("ks"); // Kashmiri
  HCV_WHENLANG("sa"); // Sanskrit
  HCV_WHENLANG("uk"); // Ukrainian
  HCV_WHENLANG("kk"); // Kazakh
  HCV_WHENLANG("sc"); // Sardinian
  HCV_WHENLANG("hsb"); // Upper Sorbian
  HCV_WHENLANG("km"); // Khmer
  HCV_WHENLANG("gd"); // Scots Gaelic
  HCV_WHENLANG("ur"); // Urdu
  HCV_WHENLANG("ky"); // Kirghiz
  HCV_WHENLANG("sd"); // Sindhi
  HCV_WHENLANG("ve"); // Venda
  HCV_WHENLANG("eo"); // Esperanto
  HCV_WHENLANG("tlh"); // Klingon
  HCV_WHENLANG("si"); // Singhalese
  HCV_WHENLANG("vi"); // Vietnamese
  HCV_WHENLANG("ar"); // Aragonese
  HCV_WHENLANG("et"); // Estonian
  HCV_WHENLANG("ko"); // Korean
  HCV_WHENLANG("sr"); // Serbian
  HCV_WHENLANG("vo"); // Volapuk
  HCV_WHENLANG("hy"); // Armenian
  HCV_WHENLANG("fo"); // Faeroese
  HCV_WHENLANG("sk"); // Slovak
  HCV_WHENLANG("wa"); // Walloon
  HCV_WHENLANG("as"); // Assamese
  HCV_WHENLANG("fa"); // Farsi
  HCV_WHENLANG("sl"); // Slovenian
  HCV_WHENLANG("cy"); // Welsh
  HCV_WHENLANG("ast"); // Asturian
  HCV_WHENLANG("fj"); // Fijian
  HCV_WHENLANG("la"); // Latin
  HCV_WHENLANG("so"); // Somani
  HCV_WHENLANG("xh"); // Xhosa
  HCV_WHENLANG("az"); // Azerbaijani
  HCV_WHENLANG("fi"); // Finnish
  HCV_WHENLANG("lv"); // Latvian
  HCV_WHENLANG("sb"); // Sorbian
  HCV_WHENLANG("ji"); // Yiddish
  HCV_WHENLANG("eu"); // Basque
  HCV_WHENLANG("fr"); // French
  HCV_WHENLANG("lt"); // Lithuanian
  HCV_WHENLANG("es"); // Spanish
  HCV_WHENLANG("zu"); // Zulu
#warning many languages code are missing in Hcv_http_template_data::request_language
#undef HCV_WHENLANG
  if (!reqlanstr.empty())
    _hcvhttp_language = reqlanstr;
  return reqlanstr;
} // end Hcv_http_template_data::request_language

////////////////

Hcv_https_template_data::~Hcv_https_template_data()
{
} // end Hcv_https_template_data::~Hcv_https_template_data

////////////////
std::atomic<long> Hcv_email_template_data::_hcvemail_counter_;
long
Hcv_email_template_data::incremented_email_counter()
{
#if __GNUC__ >= 9
  return 1+std::atomic_fetch_add(&_hcvemail_counter_, 1);
#else
  //return __sync_fetch_and_add(&hcv_web_request_counter, 1);
  return 1+__sync_fetch_and_add(reinterpret_cast<long*>(&_hcvemail_counter_),
                                1L);
#endif /* __GNUC__ >= 9 */
} // end of Hcv_email_template_data::incremented_email_counter


Hcv_email_template_data::~Hcv_email_template_data()
{
  HCV_FATALOUT("unimplemented Hcv_email_template_data::~Hcv_email_template_data");
#warning TODO: unimplemented Hcv_email_template_data::~Hcv_email_template_data
} // end Hcv_email_template_data::~Hcv_email_template_data

void
Hcv_email_template_data::send_email()
{
  std::string emailcmd = hcv_get_html_email_command();
  if (emailcmd.empty())
    HCV_FATALOUT("Hcv_email_template_data::send_email #" << email_serial() << " to " << email_to()
                 << " about " << email_subject()
                 << " missing email command. configuration [helpcovid] html_email_popen_command");
  emailcmd += " ";
  std::string quotedsubject = Glib::shell_quote(_hcvemail_subject);
  emailcmd += quotedsubject;
  emailcmd += " ";
  std::string destemail = Glib::shell_quote(_hcvemail_to);
  emailcmd += destemail;
  HCV_DEBUGOUT("Hcv_email_template_data::send_email #" << email_serial()
               << " emailcmd=" << emailcmd);
  FILE* pipmail = popen(emailcmd.c_str(), "w");
  if (!pipmail)
    HCV_FATALOUT("Hcv_email_template_data::send_email #" << email_serial()
                 << " failed popen " << emailcmd);
  std::ifstream templinf(email_template_path());
  HCV_DEBUGOUT("Hcv_email_template_data::send_email #" << email_serial()
               << " templatepath='" << email_template_path() << '"');
  std::string mailstr = hcv_expand_template_input_stream(templinf, email_template_path().c_str(), this);
  HCV_DEBUGOUT("Hcv_email_template_data::send_email #" << email_serial()
               << " mailsize=" << mailstr.size());
  if (fwrite(mailstr.c_str(), mailstr.size(), 1, pipmail) <= 0)
    HCV_FATALOUT("failed fwrite Hcv_email_template_data::send_email #" << email_serial() << " to " << email_to()
                 << " about " << email_subject()
                 << " for " << mailstr.size()
                 << " bytes.");
  int pstat = pclose(pipmail);
  pipmail = nullptr;
  if (pstat != 0)
    HCV_FATALOUT("pclose failure Hcv_email_template_data::send_email #" << email_serial() << " to " << email_to()
                 << " about " << email_subject()
                 << " status=" << pstat);
  HCV_DEBUGOUT("Hcv_email_template_data::send_email #" << email_serial()
               << " sent to " << email_to()
               << " about " << email_subject()
               << " for " << mailstr.size()
               << " bytes.");
} // end Hcv_email_template_data::send_email


////////////////////////////////////////////////////////////////
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
hcv_expand_processing_instruction(Hcv_template_data*templdata, const std::string &procinstr, const char*filename, int lineno, long offset)
{
  if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
    HCV_FATALOUT("hcv_expand_processing_instruction: missing templdata for procinstr='"
                 << procinstr << "' in " << (filename?:"??") << ":" << lineno);
  char namebuf[80];
  memset (namebuf, 0, sizeof(namebuf));
  static_assert (sizeof(namebuf) >= HCV_TEMPLATE_NAME_MAXLEN, "too short namebuf");
  int endpos = -1;
  const char*procstr = procinstr.c_str();
  if (sscanf(procstr, "<?hcv %64[a-zA-Z0-9_] %n", namebuf, &endpos)<1 || endpos<0)
    {
      HCV_SYSLOGOUT(LOG_WARNING,"hcv_expand_processing_instruction: invalid procinstr='" << procinstr
                    << "' in " << (filename?:"**??**")
                    << ":" << lineno << " @" << offset);
      return;
    }
  auto endpi = strstr(procstr+endpos, "?>");
  if (!endpi || endpi[2])
    HCV_FATALOUT("hcv_expand_processing_instruction: corrupted procinstr='" << procinstr
                 << "' in " << (filename?:"**??**")
                 << ":" << lineno << " @" << offset);
  std::string arg(procstr+endpos, endpi-(procstr+endpos));
  std::lock_guard<std::recursive_mutex> gu(hcv_template_mtx);
  auto it = hcv_template_expander_dict.find(std::string(namebuf));
  if (it == hcv_template_expander_dict.end())
    {
      if (auto httptempl = dynamic_cast<Hcv_http_template_data*>(templdata))
        HCV_SYSLOGOUT(LOG_WARNING,"hcv_expand_processing_instruction: unknown namebuf='"
                      << namebuf << "' for HTTP request "
                      << httptempl->request_method()
                      << " on " << httptempl->request_path());
      else
        HCV_SYSLOGOUT(LOG_WARNING,"hcv_expand_processing_instruction: unknown namebuf='" << namebuf);
      return;
    };
  hcv_template_expanding_closure_t clos = it->second;
  return clos(templdata,procinstr,filename,lineno,offset);
} // end hcv_expand_processing_instruction


const unsigned hcv_max_template_size = 128*1024;


std::string
hcv_expand_template_file(const std::string& srcfilepath, Hcv_template_data* templdata)
{
  struct stat srcfilestat;
  memset (&srcfilestat, 0, sizeof(srcfilestat));
  if (srcfilepath.empty())
    HCV_FATALOUT("hcv_expand_template_file with empty srcfilepath");
  if (srcfilepath[0] != '/')
    HCV_SYSLOGOUT(LOG_WARNING,
                  "hcv_expand_template_file with relative path: " << srcfilepath);
  if (stat(srcfilepath.c_str(), &srcfilestat))
    HCV_FATALOUT("hcv_expand_template_file: stat failure on source file " << srcfilepath);
  if (!S_ISREG(srcfilestat.st_mode))
    HCV_FATALOUT("hcv_expand_template_file: source file " << srcfilepath
                 << " is not a regular file.");
  if (srcfilestat.st_size > hcv_max_template_size)
    HCV_FATALOUT("hcv_expand_template_file: source file " << srcfilepath
                 << " is too big: "
                 << (long)srcfilestat.st_size << " bytes.");

  auto outp = dynamic_cast<std::ostringstream*>(templdata->output_stream());
  if (outp == nullptr)
    HCV_FATALOUT("hcv_expand_template_file: bad templdata->output_stream()");

  //std::ostringstream *outp = outstrptr;
  std::ifstream srcinp(srcfilepath);
  int lincnt = 0;
  bool gotpe = false;
  long off=0;
  for (std::string linbuf; (off=srcinp.tellg()), std::getline(srcinp, linbuf); )
    {
      gotpe = false;
      lincnt++;
      if (linbuf.empty())
        {
          *outp << std::endl;
          continue;
        }
      /// skip <!DOCTYPE html> or <!-- html comment --> in first 8 lines
      if (lincnt < 8 && linbuf.size()>4 && linbuf[0]=='<' && linbuf[1]=='!')
        {
          *outp << linbuf << std::endl;
          continue;
        }
      const char*linestr= linbuf.c_str();
      const char*startpi = nullptr;
      const char*curpc = linestr;
      while (curpc && (startpi = strstr(curpc, "<?hcv ")) != nullptr)
        {
          const char*endpi = strstr(curpc+strlen("<?hcv "), "?>");
          if (endpi == nullptr)
            {
              HCV_SYSLOGOUT(LOG_WARNING,
                            "hcv_expand_template_file: " << srcfilepath
                            << ":" << lincnt
                            << " line has unclosed template markup:" << std::endl
                            << linbuf);
              *outp << std::string(curpc);
              curpc = nullptr;
              break;
            }
          else
            {
              std::string before(curpc, startpi-curpc);
              *outp << before;
              std::string procinstr(startpi, (endpi+2)-startpi);
              hcv_expand_processing_instruction(templdata, procinstr, srcfilepath.c_str(), lincnt, off);
              curpc = endpi+2;
              gotpe = true;
            };
          if (gotpe)
            *outp << std::flush;
        } // end while curpc && (startpi=....)
      if (curpc && !startpi)
        *outp << curpc;
      *outp << std::endl;
    };
  outp->flush();

  return outp->str();
} // end hcv_expand_template_file


std::string
hcv_expand_template_input_stream(std::istream&srcinp, const char*inpname, Hcv_template_data*templdata)
{
  if (!inpname)
    inpname = "??*null*??";
  std::ostringstream outp;
  int lincnt = 0;
  bool gotpe = false;
  long off=0;
  for (std::string linbuf; (off=srcinp.tellg()), std::getline(srcinp, linbuf); )
    {
      gotpe = false;
      lincnt++;
      if (off > hcv_max_template_size)
        HCV_FATALOUT("hcv_expand_template_input_stream: source input " << inpname
                     << " is too big: "
                     << (long)off << " bytes.");
      const char*linestr= linbuf.c_str();
      const char*startpi = nullptr;
      const char*curpc = linestr;
      while (curpc && (startpi = strstr(curpc, "<?hcv ")) != nullptr)
        {
          const char*endpi = strstr(curpc+strlen("<?hcv "), "?>");
          if (endpi == nullptr)
            {
              HCV_SYSLOGOUT(LOG_WARNING,
                            "hcv_expand_template_input_stream: " << inpname
                            << ":" << lincnt
                            << " line has unclosed template markup:" << std::endl
                            << linbuf);
              outp << std::string(curpc);
              curpc = nullptr;
              break;
            }
          else
            {
              std::string before(curpc, startpi-curpc);
              outp << before;
              std::string procinstr(startpi, (endpi+2)-startpi);
              hcv_expand_processing_instruction(templdata, procinstr, inpname, lincnt, off);
              curpc = endpi+2;
              gotpe = true;
            };
          if (gotpe)
            outp << std::flush;
        } // end while curpc && (startpi=....)
      if (curpc && !startpi)
        outp << curpc;
      outp<<std::endl;
    }
  outp.flush();
  return outp.str();
} // end hcv_expand_template_input_stream



std::string
hcv_expand_template_string(const std::string&inpstr, const char*inpname, Hcv_template_data*templdata)
{
  std::istringstream inp(inpstr);
  return hcv_expand_template_input_stream(inp,inpname,templdata);
} // end hcv_expand_template_input_string


void
hcv_initialize_templates(void)
{
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv date?>
  hcv_register_template_expander_closure
  ("date",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv date?>' processing instruction "
                   << procinstr << " in "  << filename << ":" << lineno);
    time_t nowt = 0;
    time(&nowt);
    struct tm nowtm;
    memset (&nowtm, 0, sizeof(nowtm));
    char nowbuf[80];
    memset (nowbuf, 0, sizeof(nowbuf));
    localtime_r (&nowt, &nowtm);
    strftime(nowbuf, sizeof(nowbuf), "%Y, %b, %d", &nowtm);
    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, nowbuf);
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv date?>' processing instruction in "
                    << filename << ":" << lineno << " @" << offset);
  });
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv now?>
  hcv_register_template_expander_closure
  ("now",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv now?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    time_t nowt = 0;
    time(&nowt);
    struct tm nowtm;
    memset (&nowtm, 0, sizeof(nowtm));
    char nowbuf[80];
    memset (nowbuf, 0, sizeof(nowbuf));
    localtime_r (&nowt, &nowtm);
    strftime(nowbuf, sizeof(nowbuf), "%c %Z", &nowtm);
    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, nowbuf);
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv now?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  });
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv html_config configname?>
  hcv_register_template_expander_closure
  ("html_config",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv html_config ...?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    HCV_DEBUGOUT("html_config procinstr='" << procinstr
                 << "' at " << filename << ":" << lineno);
    char confname[HCV_CONFIG_HTML_NAME_MAXLEN+4];
    memset (confname, 0, sizeof(confname));
    int endpos = -1;
    if (sscanf(procinstr.c_str(),
               "<?hcv html_config %60[A-Za-z0-9_] ?>%n",
               confname, &endpos) <= 1
        || endpos<(int)procinstr.size())
      {
        HCV_SYSLOGOUT(LOG_WARNING,
                      "invalid html_config PI " << procinstr
                      << " at " << filename << ":" << lineno);
        return;
      }
    if (auto pouts = templdata->output_stream())
      *pouts << hcv_get_config_html(std::string(confname));
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv now?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  });
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv request_number?>
  hcv_register_template_expander_closure
  ("request_number",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv request_number?>' processing instruction  "
                   << procinstr << " in "
                   << filename << ":" << lineno);
    char numbuf[32];
    memset(numbuf, 0, sizeof(numbuf));
    snprintf(numbuf, sizeof(numbuf), "%ld", templdata->serial());
    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, numbuf);
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv request_number?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); // end <?hcv request_number?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv gitid?>
  hcv_register_template_expander_closure
  ("gitid",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv gitid?>' processing instruction  "
                   << procinstr << " in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, hcv_gitid);
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv gitid?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); // end of <?hcv gitid?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv half_gitid?>
  hcv_register_template_expander_closure
  ("half_gitid",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv half_gitid?>' processing instruction  "
                   << procinstr << " in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      {
        std::string gidstr(hcv_gitid);
        auto gidsiz = gidstr.size();
        HCV_ASSERT(gidsiz>4);
        bool withplus = gidstr[gidsiz-1] == '+';
        if (withplus)
          gidstr.erase(gidsiz/3, gidsiz-gidsiz/3-1);
        else
          gidstr.erase(gidsiz/3,gidsiz);
        HCV_NEVEROUT("<?hcv half_gitid?> hcv_gitid='" << hcv_gitid
                     << "' gidstr='" << gidstr << "'");
        *pouts << gidstr;
      }
    else
      HCV_SYSLOGOUT(LOG_WARNING,
                    "no output stream for '<?hcv half_gitid?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); // end <?hcv half_gitid?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv lastgitcommit?>
  hcv_register_template_expander_closure
  ("gitid",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv lastgitcommit?>' processing instruction  "
                   << procinstr << " in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, hcv_lastgitcommit);
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv lastgitcommit?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  });
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv timestamp?>
  hcv_register_template_expander_closure
  ("timestamp",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv timestamp?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, hcv_timestamp);
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv timestamp?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); // end <?hcv timestamp?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv pid?>
  hcv_register_template_expander_closure
  ("pid",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv pid?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      *pouts << (long)getpid();
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv pid?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); // end  <?hcv pid?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv hostname?>
  hcv_register_template_expander_closure
  ("hostname",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv hostname?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      *pouts << hcv_get_hostname();
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv hostname?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  });			   // end  <?hcv hostname?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv request_method?>
  hcv_register_template_expander_closure
  ("request_method",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv request_method?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      {
        if (auto httptempl = dynamic_cast<Hcv_http_template_data*>(templdata))
          {
            hcv_output_cstr_encoded_html(*pouts,httptempl->request_method().c_str());
          }
      }
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv request_method?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  });			     // end <?hcv request_method?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv request_path?>
  hcv_register_template_expander_closure
  ("request_path",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv request_path?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      {
        if (auto httptempl = dynamic_cast<Hcv_http_template_data*>(templdata))
          {
            hcv_output_cstr_encoded_html(*pouts,httptempl->request_path().c_str());
          }
      }
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv request_path?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  });				// end <?hcv request_path?>

  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv webroot?>
  hcv_register_template_expander_closure
  ("webroot",
   [](Hcv_template_data* templdata, const std::string& procinstr,
      const char* filename, int lineno, long offset)
  {
    if (!templdata
        || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv webroot?>' processing instruction  "
                   << procinstr << " in " << filename << ":" << lineno);

    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, hcv_get_web_root().c_str());

    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv webroot?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); /// end <?hcv webroot?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv filename?>
  hcv_register_template_expander_closure
  ("filename",
   [](Hcv_template_data* templdata, const std::string& procinstr,
      const char* filename, int lineno, long offset)
  {
    if (!templdata
        || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv filename?>' processing instruction  "
                   << procinstr << " in " << filename << ":" << lineno);

    if (auto pouts = templdata->output_stream())
      hcv_output_cstr_encoded_html(*pouts, filename);

    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv filename?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); /// end <?hcv filename?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv lineno?>
  hcv_register_template_expander_closure
  ("lineno",
   [](Hcv_template_data* templdata, const std::string& procinstr,
      const char* filename, int lineno, long offset)
  {
    if (!templdata
        || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv lineno?>' processing instruction  "
                   << procinstr << " in " << filename << ":" << lineno);

    if (auto pouts = templdata->output_stream())
      *pouts << lineno;

    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv lineno?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); /// end <?hcv lineno?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv offset?>
  hcv_register_template_expander_closure
  ("offset",
   [](Hcv_template_data* templdata, const std::string& procinstr,
      const char* filename, int lineno, long offset)
  {
    if (!templdata
        || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv offset?>' processing instruction  "
                   << procinstr << " in " << filename << ":" << lineno);

    if (auto pouts = templdata->output_stream())
      *pouts << offset;

    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv offset?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); /// end <?hcv offset?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv basefilepos [htmltag] [cssclass]?>
  hcv_register_template_expander_closure
  ("basefilepos",
   [](Hcv_template_data* templdata, const std::string& procinstr,
      const char* filename, int lineno, long offset)
  {
    if (!templdata
        || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv basefilepos ...?>' processing instruction  "
                   << procinstr << " in " << filename << ":" << lineno);
    HCV_NEVEROUT("<?hcv basefilepos ...?> at "<< filename << ":" << lineno
                 << " procinstr='" << procinstr << "'");
    char tagbuf[24];
    char classbuf[40];
    memset (tagbuf, 0, sizeof(tagbuf));
    memset (classbuf, 0, sizeof(classbuf));
    int endpos = -1;
    int nbs = 0;
    bool comm = !strcmp(procinstr.c_str(), "<?hcv basefilepos comment?>");
    if (!comm)
      nbs = sscanf(procinstr.c_str()+5, " basefilepos %20[A-Za-z0-9_] %38[A-Za-z0-9_] ?>%n",
                   tagbuf, classbuf, &endpos);
    if (auto pouts = templdata->output_stream())
      {
        const char*lastslash = strrchr(filename?:"??", '/');
        const char*basefilname = (lastslash&&lastslash[0])?(lastslash+1):filename;
        if (comm)
          {
            HCV_NEVEROUT("<?hcv basefilepos comment?> at "<< basefilname << ":" << lineno);
            *pouts << "<!-- @";
          }
        else if (tagbuf[0])
          {
            *pouts << "<" << tagbuf;
            if (classbuf[0])
              *pouts << " class='" << classbuf << "'";
            *pouts << ">";
            HCV_NEVEROUT("<?hcv basefilepos .. @" <<  basefilname << ":" << lineno
                         << " tagbuf='" << tagbuf << "' classbuf='" << classbuf << "'");
          }
        hcv_output_cstr_encoded_html(*pouts, basefilname);
        *pouts << ":" << lineno;
        if (comm) *pouts << " -->";
        else if (tagbuf[0])
          *pouts << "</" << tagbuf << ">";
      }

    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv basefilepos?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); /// end <?hcv basefilepos [htmltag] [cssclass]?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv register_form_token?>
  hcv_register_template_expander_closure
  ("register_form_token",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv register_form_token?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    HCV_DEBUGOUT("<?hcv register_form_token?> at "<< filename << ":" << lineno);
    if (auto pouts = templdata->output_stream())
      *pouts << hcv_view_register_form_token(dynamic_cast<Hcv_http_template_data*>(templdata));
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv register_form_token?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset);
  }); // end  <?hcv register_form_token?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv msg ...?>
  hcv_register_template_expander_closure
  ("msg",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv msg ...?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    HCV_DEBUGOUT("<?hcv msg ...?> at "<< filename << ":" << lineno << " " << procinstr);
    if (auto pouts = templdata->output_stream())
      {
        /// temporary trick to get just 16 debug messages...
        static std::atomic<std::uint32_t> atomic_count;
        std::uint32_t curcount = atomic_count.fetch_add(1);
        try
          {
            std::string str =
              hcv_view_expand_msg(dynamic_cast<Hcv_http_template_data*>(templdata), procinstr, filename, lineno, offset);
            *pouts << str;
            if (curcount > 16)
              HCV_NEVEROUT("PI " << procinstr  << " @" << filename << ":" << lineno << " =>" << std::endl << str);
            else
              HCV_DEBUGOUT("PI " << procinstr  << " @" << filename << ":" << lineno << " count#" << curcount
                           << " =>" << std::endl << str);

          }
        catch (std::exception& exc)
          {
            HCV_FATALOUT("PI " << procinstr << " @" << filename << ":" << lineno
                         << " got standard exception " << exc.what());
          }
        catch (Glib::Exception &gex)
          {
            HCV_FATALOUT("PI " << procinstr << " @" << filename << ":" << lineno
                         << " got Glib exception " << gex.what());
          }
        if (curcount > 16)
          HCV_NEVEROUT("<?hcv msg ...?> at "<< filename << ":" << lineno << " done");
        else
          HCV_NEVEROUT("<?hcv msg ...?> at "<< filename << ":" << lineno << " done count#" << curcount);
      }
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv msg ...?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset
                    << std::endl << procinstr);
  }); // end  <?hcv msg ...?>
  ////////////////////////////////////////////////////////////////
  //////////////// for <?hcv confmsg ...?>
  hcv_register_template_expander_closure
  ("confmsg",
   [](Hcv_template_data*templdata, const std::string &procinstr,
      const char*filename, int lineno,
      long offset)
  {
    if (!templdata || templdata->kind() == Hcv_template_data::TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no template data for '<?hcv confmsg ...?>' processing instruction "
                   << procinstr <<" in "
                   << filename << ":" << lineno);
    HCV_DEBUGOUT("<?hcv confmsg ...?> at "<< filename << ":" << lineno << " " << procinstr);
    if (auto pouts = templdata->output_stream())
      {
        try
          {
            std::string str =
              hcv_view_expand_confmsg(dynamic_cast<Hcv_http_template_data*>(templdata), procinstr, filename, lineno, offset);
            *pouts << str;
            HCV_NEVEROUT("PI " << procinstr  << " @" << filename << ":" << lineno << " =>" << std::endl << str);
          }
        catch (std::exception& exc)
          {
            HCV_FATALOUT("PI " << procinstr << " @" << filename << ":" << lineno
                         << " got standard exception " << exc.what());
          }
        catch (Glib::Exception &gex)
          {
            HCV_FATALOUT("PI " << procinstr << " @" << filename << ":" << lineno
                         << " got Glib exception " << gex.what());
          }
        HCV_NEVEROUT("<?hcv confmsg ...?> at "<< filename << ":" << lineno << " done");
      }
    else
      HCV_SYSLOGOUT(LOG_WARNING, "no output stream for '<?hcv confmsg ...?>' processing instruction in "
                    << filename << ":" << lineno<< " @" << offset
                    << std::endl << procinstr);
  }); // end  <?hcv confmsg ...?>
} // end hcv_initialize_templates =======================================

/************* end of file hcv_template.cc in github.com/bstarynk/helpcovid *********/
