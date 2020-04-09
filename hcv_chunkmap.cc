/****************************************************************
 * file hcv_chunkmap.cc
 *
 * Description:
 *      Parsing a file containing literal strings for customization
 *      for  https://github.com/bstarynk/helpcovid
 *      see its file CUSTOMIZATION.md
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


extern "C" const char hcv_chunkmap_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_chunkmap_date[] = __DATE__;


static constexpr unsigned hcv_max_chunkmap_size = 512*1024;
static constexpr unsigned hcv_max_chunkmap_linelen = 65536;


/// see documentation in CUSTOMIZATION.md
std::map<std::string,std::string>
hcv_parse_chunk_map(const std::string& filepath)
{
#define HCVSTATE_ENDLABEL_LEN 32
  std::map<std::string,std::string> resultmap;
  HCV_DEBUGOUT("hcv_parse_chunk_map start filepath=" << filepath);
  if (access(filepath.c_str(), R_OK))
    HCV_FATALOUT("hcv_parse_chunk_map cannot access " << filepath);
  // https://en.wikipedia.org/wiki/X_Macro trick
#define HCVSTATE_XMACRO(Mac)			\
  Mac(_NONE),					\
    Mac(SKIPSPACES),				\
    Mac(HEADERLINE),				\
    Mac(INSIDECHUNK)
  // define HCVCHUNKSTATE__NONE, etc...
  enum hcvchunkstate_en
  {
#define HCVSTATE_XDEFINE(Nam) HCVCHUNKSTATE_##Nam
    HCVSTATE_XMACRO(HCVSTATE_XDEFINE)
#undef HCVSTATE_XDEFINE
  };
  const char*const statenames[] =
  {
#define HCVSTATE_XNAME(Nam)  #Nam
    HCVSTATE_XMACRO(HCVSTATE_XNAME),
#undef HCVSTATE_XNAME
    nullptr
  };
  enum hcvchunkstate_en state = HCVCHUNKSTATE__NONE;
  std::ifstream inp(filepath);
  int lincnt = 0;
  long off = 0;
  state = HCVCHUNKSTATE_SKIPSPACES;
  std::string chunkname;
  std::string chunkbody;
  for (std::string linbuf; (off=inp.tellg()), std::getline(inp, linbuf); )
    {
      char endstrbuf[HCVSTATE_ENDLABEL_LEN]; // that size appears in sscanf below
      memset(endstrbuf, 0, sizeof(endstrbuf));
      lincnt++;
      if (lincnt >= (int)hcv_max_chunkmap_linelen)
        HCV_FATALOUT("hcv_parse_chunk_map  " << filepath << " has too many lines " << lincnt);
      if (off >  hcv_max_chunkmap_size)
        HCV_FATALOUT("hcv_parse_chunk_map  " << filepath << " has too many bytes " << off);
      HCV_DEBUGOUT("hcv_parse_chunk_map line#" << lincnt
                   << " state=" << statenames[state] << " line:" << linbuf);
      bool allspaces = false;
      switch (state)
        {
        case HCVCHUNKSTATE_SKIPSPACES:
        {
          chunkname.clear();
          chunkbody.clear();
          if (linbuf.empty())
            continue;
          // lines starting with # are comments
          if (linbuf[0] == '#')
            continue;
          // line with only spaces should be skipped
          allspaces = true;
          for (auto c : linbuf)
            if (!isspace(c))
              {
                allspaces = false;
                break;
              };
          if (allspaces)
            continue;
          if (linbuf.size() >= 3
              && linbuf[0] == '!'
              && isalpha(linbuf[1]))
            {
              state = HCVCHUNKSTATE_HEADERLINE;
              goto parse_headerline;
            }
          else
            HCV_FATALOUT("hcv_parse_chunk_map, in file " << filepath
                         << ", line# " << lincnt
                         << " is unexpected:" << linbuf);
        };
        break;
        ////
        case HCVCHUNKSTATE_HEADERLINE:
parse_headerline:
          {
            char chunknamebuf[64];
            memset (chunknamebuf, 0, sizeof(chunknamebuf));
            int endpos= -1;
            if (linbuf.size() >= 3
                && linbuf[0] == '!'
                && isalpha(linbuf[1]))
              {
                if (sscanf(linbuf.c_str(),
                           "!%60[A-Za-z0-9_]'%n", chunknamebuf, &endpos) >= 1
                    && endpos>2 && linbuf[endpos-1]=='\'')
                  {
                    /**
                     * single-line chunk line such as
                     * !FOO'some chunk on single line up to end-of-line
                     **/
                    chunkbody.assign(linbuf.c_str()+endpos);
                    chunkname.assign(chunknamebuf);
                    HCV_DEBUGOUT("hcv_parse_chunk_map line#" << lincnt
                                 << ", file:" << filepath
                                 << ", single line chunk " << chunkname
                                 << ":" << chunkbody);
                    resultmap.insert({chunkname,chunkbody});
                    state = HCVCHUNKSTATE_SKIPSPACES;
                    continue;

                  }
                else if (sscanf(linbuf.c_str(),
                                "!%60[A-Za-z0-9_]\"%30[A-Za-z0-9_](%n",
                                chunknamebuf, endstrbuf, &endpos) >= 2
                         && endpos>4 && isalpha(endstrbuf[0])
                         && linbuf[endpos-1]=='(')
                  {
                    /**
                     * multi-line chunk line starting with
                     * !MULTIBAR"abc(
                     **/
                    chunkname.assign(chunknamebuf);
                    chunkbody.clear();
                    if ((int)linbuf.size() > endpos
                        && !isspace(linbuf[endpos]))
                      chunkbody.assign(linbuf.c_str()+endpos);
                    HCV_DEBUGOUT("hcv_parse_chunk_map line#" << lincnt
                                 << ", file:" << filepath
                                 << ", multiline line chunk " << chunkname
                                 << (chunkbody.empty()?" eol":" starting with:")
                                 << chunkbody);
                    state = HCVCHUNKSTATE_INSIDECHUNK;
                  }
                else goto badheaderline;
              }
            else
badheaderline:
              HCV_FATALOUT("hcv_parse_chunk_map line#" << lincnt
                           << " bad header line state=" << statenames[state]
                           << " line:" << linbuf);
          }
          break;
        ////
        case HCVCHUNKSTATE_INSIDECHUNK:
          /**
           * Inside a chunk started with something like !MULTIBAR"abc(
           *
           * which should be ended by )abc"
           **/
        {
          int endpos= -1;
          char parsendstrbuf[HCVSTATE_ENDLABEL_LEN]; // that size appears in sscanf below
          memset (parsendstrbuf, 0, sizeof(parsendstrbuf));
          if (sscanf(linbuf.c_str(), ")%30[A-Za-z0-9_] %n", parsendstrbuf, &endpos) >= 1
              && endpos>2 && isalpha(parsendstrbuf[0])
              && !strncmp(parsendstrbuf, endstrbuf, HCVSTATE_ENDLABEL_LEN))
            {
              // end of chunk
              resultmap.insert({chunkname,chunkbody});
              state = HCVCHUNKSTATE_SKIPSPACES;
              continue;

            }
          else
            {
              chunkbody += linbuf;
#warning should hcv_parse_chunk_map add a newline here to chunkbody?
              continue;
            }
          //
        }
          // not implemented
          ////
#warning very incomplete hcv_parse_chunk_map
        case HCVCHUNKSTATE__NONE:
          HCV_FATALOUT("hcv_parse_chunk_map line#" << lincnt
                       << " unimplemented state=" << statenames[state]
                       << " line:" << linbuf);
        };
    };				// end for linbuf....
  HCV_DEBUGOUT("hcv_parse_chunk_map end filepath=" << filepath
               << " with " << resultmap.size() << " keys");
  if (state != HCVCHUNKSTATE_SKIPSPACES)
    HCV_SYSLOGOUT(LOG_WARNING, "hcv_parse_chunk_map line#" << lincnt
                  << " of " << filepath
                  << " is not a space line, in state " << statenames[state]
                  << std::endl);
  return resultmap;
} // end hcv_parse_chunk_map


/// end of file hcv_chunkmap.cc
