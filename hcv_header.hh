/****************************************************************
 * file hcv_header.hh
 *
 * Description:
 *      Header file of https://github.com/bstarynk/helpcovid
 *
 * Author(s):
 *      © Copyright 2020
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *
 *
 * License:
 *    This program is free software: you can redistribute it and/or modify
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
#ifndef HELPCOVID_HEADER
#define HELPCOVID_HEADER 1

#include <set>
#include <map>
#include <deque>
#include <variant>
#include <unordered_map>
#include <unordered_set>
#include <new>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <initializer_list>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <stdexcept>
#include <functional>
#include <typeinfo>
#include <locale>

#include <cassert>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <clocale>

#include <argp.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/syslog.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <dlfcn.h>
#include <dirent.h>
#include <pthread.h>
#include <pwd.h>
#include <poll.h>


// PostGresQL C++ http://pqxx.org/development/libpqxx
#include <pqxx/pqxx>


//  cpp-httplib https://github.com/yhirose/cpp-httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT 1
#include "httplib.h"

// JsonCPP https://github.com/open-source-parsers/jsoncpp
#include "json/json.h"

// Glibmm https://developer.gnome.org/glibmm/stable/
#include "glibmm.h"
#include "giomm.h"

// in generated __timestamp.c
extern "C" const char hcv_timestamp[];
extern "C" const char hcv_topdirectory[];
extern "C" const char hcv_gitid[];
extern "C" const char hcv_lastgittag[];
extern "C" const char hcv_lastgitcommit[];
extern "C" const char hcv_md5sum[];
extern "C" const char*const hcv_files[];
extern "C" const char hcv_makefile[];

// start time.
extern "C" char hcv_startimbuf[];
//////////////// fatal error - aborting
extern "C" void hcv_fatal_stop_at (const char *, int, int) __attribute__((noreturn));

extern "C" std::recursive_mutex hcv_fatalmtx;


#define HCV_FATALOUT_AT_BIS(Fil,Lin,...) do {		\
  int err##Lin = errno;					\
  std::lock_guard<std::recursive_mutex>			\
    gu##Lin(hcv_fatalmtx);				\
    std::clog << "** HELPCOVID FATAL! "			\
	      << (Fil) << ":" << Lin << ":: "		\
	      << __VA_ARGS__ << std::endl;		\
    hcv_fatal_stop_at (Fil,Lin, err##Lin); } while(0)

#define HCV_FATALOUT_AT(Fil,Lin,...) HCV_FATALOUT_AT_BIS(Fil,Lin,##__VA_ARGS__)

// typical usage would be HCV_FATALOUT("x=" << x)
#define HCV_FATALOUT(...) HCV_FATALOUT_AT(__FILE__,__LINE__,##__VA_ARGS__)



// syslog facility

extern "C" std::recursive_mutex hcv_syslogmtx;
extern "C" void hcv_syslog_at (const char *fil, int lin, int prio,const std::string&str);
#define HCV_SYSLOGOUT_AT_BIS(Fil,Lin,Prio,...) do {	\
  int err##Lin = errno;					\
  std::lock_guard<std::recursive_mutex>			\
    gu##Lin(hcv_syslogmtx);				\
  std::ostringstream outs##Lin;				\
  outs##Lin << " !! "					\
	 << __VA_ARGS__ << std::endl;			\
  if (err##Lin)						\
    outs##Lin << "-: " << strerror(err##Lin);		\
  hcv_syslog_at ((Fil),(Lin),(Prio),(outs##Lin.str()));	\
  } while(0)

#define HCV_SYSLOGOUT_AT(Fil,Lin,Prio,...) HCV_SYSLOGOUT_AT_BIS(Fil,Lin,(Prio),##__VA_ARGS__)

// typical usage would be HCV_SYSLOGOUT(LOG_NOTICE,"x=" << x)
#define HCV_SYSLOGOUT(Prio,...) HCV_SYSLOGOUT_AT(__FILE__,__LINE__,(Prio),##__VA_ARGS__)



////////////////////////////////////////////////////////////////
///// configuration file https://developer.gnome.org/glibmm/stable/classGlib_1_1KeyFile.html
#define HCV_DEFAULT_CONFIG_PATH "/etc/helpcovid.conf"
extern "C" bool hcv_config_has_group(const char*grpname);
extern "C" bool hcv_config_has_key(const char*grpname, const char*keyname);
/*** Do something with the configuration file, serialized by a mutex. For example:
     int foo=0;
     hcv_config_do([&foo](const Glib::KeyFile*kf) { foo = kf->get_integer("foo"); });
 ***/
extern "C" void hcv_config_do(const std::function<void(const Glib::KeyFile*)>&dofun);

extern "C" void hcv_load_config_file(const char*configfile=nullptr);

////////////////////////////////////////////////////////////////

//// PostGreSQL database
extern "C" std::unique_ptr<pqxx::connection> hcv_dbconn;
extern "C" std::recursive_mutex hcv_dbmtx;
extern "C" void hcv_initialize_database(const std::string&uri);




////////////////////////////////////////////////////////////////

//// Web service

/// this could be run with root privilege if we need to serve the :80
/// HTTP TCP port.
void hcv_initialize_web(const std::string&weburl, const std::string&webroot,
                        const std::string&sslcert, const std::string&sslkey);
void hcv_stop_web(void);

extern "C" void hcv_webserver_run(void);


////////////////////////////////////////////////////////////////

//// template machinery: in some quasi HTML file starting with
//// '<!DOCTYPE html' expand every occurrence of <?hcv markup...?>
//// where <?hcv is verbatim; and return the expanded string.

extern "C" std::string hcv_expand_template_file(const std::string& filepath);

////////////////////////////////////////////////////////////////
//////////////// timing functions
// see http://man7.org/linux/man-pages/man2/clock_gettime.2.html
static inline double
hcv_wallclock_real_time(void)
{
  struct timespec ts =  {0,0};
  if (clock_gettime(CLOCK_REALTIME, &ts))
    return NAN;
  return 1.0*ts.tv_sec + 1.0e-9*ts.tv_nsec;
} // end hcv_wallclock_real_time

static inline double
hcv_monotonic_real_time(void)
{
  struct timespec ts =  {0,0};
  if (clock_gettime(CLOCK_MONOTONIC, &ts))
    return NAN;
  return 1.0*ts.tv_sec + 1.0e-9*ts.tv_nsec;
} // end hcv_monotonic_real_time


static inline double
hcv_process_cpu_time(void)
{
  struct timespec ts =  {0,0};
  if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts))
    return NAN;
  return 1.0*ts.tv_sec + 1.0e-9*ts.tv_nsec;
} // end hcv_process_cpu_time


static inline double
hcv_thread_cpu_time(void)
{
  struct timespec ts =  {0,0};
  if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts))
    return NAN;
  return 1.0*ts.tv_sec + 1.0e-9*ts.tv_nsec;
} // end hcv_thread_cpu_time



#endif /*HELPCOVID_HEADER*/
