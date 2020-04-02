/****************************************************************
 * file hcv_plugins.cc
 *
 * Description:
 *      Plugins machinery of https://github.com/bstarynk/helpcovid
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

extern "C" const char hcv_plugins_gitid[] = HELPCOVID_GITID;
extern "C" const char hcv_plugins_date[] = __DATE__;

static std::recursive_mutex hcv_plugin_mtx;


struct Hcv_plugin
{
  std::string hcvpl_name;
  void* hcvpl_handle; 		// result of dlopen
  const char* hcvpl_arg;	// argument passed to plugin
  std::string hcvpl_gitid;
  std::string hcvpl_license;
  hcvplugin_initializer_sig_t* hcvpl_initweb;
};

std::vector<Hcv_plugin> hcv_plugin_vect;


std::vector<std::string>
hcv_get_loaded_plugins_vector(void)
{
  std::vector<std::string> res;
  std::lock_guard<std::recursive_mutex> guplug(hcv_plugin_mtx);
  res.reserve(hcv_plugin_vect.size());
  for (auto &pl: hcv_plugin_vect)
    res.push_back(pl.hcvpl_name);
  return res;
} // end hcv_get_loaded_plugins_vector


void hcv_load_plugin(const char*plugin_name, const char*plugin_arg)
{
  if (!plugin_name || !plugin_name[0])
    HCV_FATALOUT("hcv_load_plugin: missing plugin name");
  if (strlen(plugin_name) > HCV_PLUGIN_NAME_MAXLEN)
    HCV_FATALOUT("too long plugin name: " << plugin_name);
  for (const char*pc = plugin_name; *pc; pc++)
    if (!isalnum(*pc) && *pc != '_')
      HCV_FATALOUT("invalid plugin name " << plugin_name);
  std::lock_guard<std::recursive_mutex> guplug(hcv_plugin_mtx);
  HCV_DEBUGOUT("hcv_load_plugin " << plugin_name << " starting");
  std::string pluginstr(plugin_name);
  for (auto& pl : hcv_plugin_vect)
    if (pl.hcvpl_name == pluginstr)
      HCV_FATALOUT("hcv_load_plugin duplicate plugin " << plugin_name);
  char sobuf[HCV_PLUGIN_NAME_MAXLEN + 48];
  memset(sobuf, 0, sizeof(sobuf));
  snprintf(sobuf, sizeof(sobuf), HCV_PLUGIN_PREFIX "%s" HCV_PLUGIN_SUFFIX,
           plugin_name);
  HCV_ASSERT(sobuf[sizeof(sobuf)-1]==(char)0);
  HCV_ASSERT(strstr(sobuf, ".so") != nullptr);
  HCV_DEBUGOUT("hcv_load_plugin " << plugin_name << " sobuf=" << sobuf);
  /// Notice that dlopen(3) handles specially paths without /; in that case we prepend
  /// "./" if file exists and is an ELF shared library for 64 bits
  /// (x86-64 a.k.a. amd64 ABI)
  if (!strchr(sobuf, '/') && strlen(sobuf)<sizeof(sobuf)-4)
    {
      bool needprepend = false;
      if (FILE* fplug = fopen(sobuf, "r"))
        {
          Elf64_Ehdr elfheader;
          memset (&elfheader, 0, sizeof(elfheader));
          if (fread(&elfheader, sizeof(elfheader), 1, fplug) > 0)
            {
              needprepend =
                elfheader.e_ident[EI_MAG0] == ELFMAG0
                && elfheader.e_ident[EI_MAG1] == ELFMAG1
                && elfheader.e_ident[EI_MAG2] == ELFMAG2
                && elfheader.e_ident[EI_MAG3] == ELFMAG3
                && elfheader.e_ident[EI_CLASS] == ELFCLASS64 /*since x86-64*/
                && elfheader.e_type == ET_DYN;
            };
          if (needprepend)
            {
              memmove(sobuf+2, sobuf, strlen(sobuf));
              sobuf[0] = '.';
              sobuf[1] = '/';
              HCV_DEBUGOUT("hcv_load_plugin prepended sobuf=" << sobuf);
            }
          else
            HCV_DEBUGOUT("hcv_load_plugin no prepending kept sobuf=" << sobuf);
          fclose(fplug);
        }
    }
  HCV_SYSLOGOUT(LOG_INFO, "hcv_load_plugin " << plugin_name << " is dlopen-ing " << sobuf);
  void* dlh = dlopen(sobuf, RTLD_NOW | RTLD_DEEPBIND);
  if (!dlh)
    HCV_FATALOUT("hcv_load_plugin " << plugin_name << " failed to dlopen " << sobuf
                 << " : " << dlerror());
  HCV_DEBUGOUT("hcv_load_plugin dlopened " << sobuf);
  const char* plgname
    = reinterpret_cast<const char*>(dlsym(dlh,
                                          "hcvplugin_name"));
  if (!plgname)
    HCV_FATALOUT("hcv_load_plugin " << plugin_name << " plugin " << sobuf
                 << " has no symbol hcvplugin_name: " << dlerror());
  if (strcmp(plgname, plugin_name))
    HCV_FATALOUT("hcv_load_plugin " << plugin_name << " plugin has unexpected hcvplugin_name " << plgname);
  const char* plglicense
    = reinterpret_cast<const char*>(dlsym(dlh,
                                          "hcvplugin_gpl_compatible_license"));
  if (!plglicense)
    HCV_FATALOUT("hcv_load_plugin " << plugin_name << " plugin " << sobuf
                 << " has no symbol hcvplugin_gpl_compatible_license: " << dlerror());
  const char* plgapi
    = reinterpret_cast<const char*>(dlsym(dlh, "hcvplugin_gitapi"));
  if (!plgapi)
    HCV_FATALOUT("hcv_load_plugin " << plugin_name << " plugin " << sobuf
                 << " has no symbol hcvplugin_gitapi: " << dlerror());
  const char* plgversion
    = reinterpret_cast<const char*>( dlsym(dlh, "hcvplugin_version"));
  if (!plgversion)
    HCV_FATALOUT("hcv_load_plugin " << plugin_name << " plugin " << sobuf
                 << " has no symbol hcvplugin_version: " << dlerror());
  void* plginit = dlsym(dlh, "hcvplugin_initialize_web");
  if (!plginit)
    HCV_FATALOUT("hcv_load_plugin " << plugin_name << " plugin " << sobuf
                 << " has no symbol hcvplugin_initialize_web: " << dlerror());
  HCV_SYSLOGOUT(LOG_NOTICE, "hcv_load_plugin " << plugin_name
                << " dlopened " << sobuf << " with license " << plglicense
                << " gitapi " << plgapi << " and version " << plgversion);
  if (strncmp(plgapi, hcv_gitid, 24))
    HCV_SYSLOGOUT(LOG_WARNING, "hcv_load_plugin " << plugin_name
                  << " dlopened " << sobuf
                  << " with gitapi mismatch - expected " << hcv_gitid
                  << " but got " << plgapi);
  hcv_plugin_vect.emplace_back
  (Hcv_plugin
  {
    .hcvpl_name = pluginstr,
    .hcvpl_handle= dlh,
    .hcvpl_arg= plugin_arg,
    .hcvpl_gitid= std::string(plgapi),
    .hcvpl_license = std::string(plglicense),
    .hcvpl_initweb = reinterpret_cast<hcvplugin_initializer_sig_t*>(plginit)
  });
  ////
  HCV_DEBUGOUT("hcv_load_plugin done " << pluginstr << " rank#"
               << hcv_plugin_vect.size());
} // end hcv_load_plugin



void
hcv_initialize_plugins_for_web(httplib::Server*webserv)
{
  HCV_ASSERT(webserv != nullptr);
  std::lock_guard<std::recursive_mutex> guplug(hcv_plugin_mtx);
  auto nbplugins = hcv_plugin_vect.size();
  HCV_DEBUGOUT("hcv_initialize_plugins_for_web starting with " << nbplugins
               << " plugins");
  if (nbplugins == 0)
    return;
  for (auto& pl : hcv_plugin_vect)
    {
      HCV_DEBUGOUT("hcv_initialize_plugins_for_web initializing " << pl.hcvpl_name
                   << (pl.hcvpl_arg?" with argument ":" without argument")
                   << (pl.hcvpl_arg?:"."));
      (*pl.hcvpl_initweb)(webserv,pl.hcvpl_arg);
      HCV_SYSLOGOUT(LOG_INFO, "hcv_initialize_plugins_for_web initialized plugin "
                    << pl.hcvpl_name << (pl.hcvpl_arg?" with argument ":" without argument")
                    << (pl.hcvpl_arg?:"."));
    };
  HCV_SYSLOGOUT(LOG_INFO, "hcv_initialize_plugins_for_web done with " << nbplugins
                << " plugins");
} // end hcv_initialize_plugins_for_web


/****************** end of file hcv_plugins.cc of github.com/bstarynk/helpcovid **/
