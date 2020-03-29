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
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <dlfcn.h>
#include <dirent.h>
#include <pthread.h>
#include <pwd.h>
#include <poll.h>
#include <regex>


// PostGresQL C++ http://pqxx.org/development/libpqxx
#include <pqxx/pqxx>
#include <pqxx/prepared_statement.hxx>

//  cpp-httplib https://github.com/yhirose/cpp-httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT 1
extern "C" unsigned hcv_http_max_threads;
extern "C" unsigned hcv_http_payload_max;

#define CPPHTTPLIB_THREAD_POOL_COUNT     hcv_http_max_threads
#define CPPHTTPLIB_PAYLOAD_MAX_LENGTH    hcv_http_payload_max

#include "httplib.h"

// JsonCPP https://github.com/open-source-parsers/jsoncpp
#include "json/json.h"

// Glibmm https://developer.gnome.org/glibmm/stable/
#include "glibmm.h"
#include "giomm.h"

// in generated __timestamp.c
extern "C" const char hcv_timestamp[];
extern "C" const unsigned long hcv_timelong[];
extern "C" const char hcv_topdirectory[];
extern "C" const char hcv_gitid[];
extern "C" const char hcv_lastgittag[];
extern "C" const char hcv_lastgitcommit[];
extern "C" const char hcv_md5sum[];
extern "C" const char*const hcv_files[];
extern "C" const char hcv_makefile[];
extern "C" const char hcv_cxx_compiler[];

// start time.
extern "C" char hcv_startimbuf[];
//////////////// fatal error - aborting
extern "C" void hcv_fatal_stop_at (const char *, int, int) __attribute__((noreturn));

extern "C" std::recursive_mutex hcv_fatalmtx;

extern "C" const char*hcv_get_hostname(void);

// mark unlikely conditions to help optimization
#ifdef __GNUC__
#define HCV_UNLIKELY(P) __builtin_expect(!!(P),0)
#define HCV_LIKELY(P) !__builtin_expect(!(P),0)
#define HCV_UNUSED __attribute__((unused))
#else
#define HCV_UNLIKELY(P) (P)
#define HCV_LIKELY(P) (P)
#define HCV_UNUSED
#endif


////////////////////////////////////////////////////////////////
// fatal error
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


//////////////// assert
#ifndef NDEBUG
#define HCV_ASSERT_AT_BIS(Fil,Lin,Func,Cond) do {		\
    if (HCV_UNLIKELY(!(Cond))) {				\
    HCV_FATALOUT("*** RefPerSys ASSERT failed:" << #Cond	\
<< " @" << Func); }} while(0)

#define HCV_ASSERT_AT(Fil,Lin,Func,Cond) HCV_ASSERT_AT_BIS(Fil,Lin,Func,Cond)
#define HCV_ASSERT(Cond) HCV_ASSERT_AT(__FILE__,__LINE__,__PRETTY_FUNCTION__,(Cond))


#define HCV_ASSERT_OUT_AT_BIS(Fil,Lin,Func,Cond,Out) do {	\
    if (HCV_UNLIKELY(!(Cond))) {				\
    HCV_FATALOUT("*** RefPerSys ASSERT_OUT failed: "		\
		 << #Cond << std::endl				\
		 << " @" << Func << "::" << Out); }} while(0)

#define HCV_ASSERT_OUT_AT(Fil,Lin,Func,Cond,Out) HCV_ASSERT_OUT_AT_BIS(Fil,Lin,Func,Cond,Out)
#define HCV_ASSERT_OUT(Cond,Fmt,Out) HCV_ASSERT_OUT_AT(__FILE__,__LINE__,__PRETTY_FUNCTION__,(Cond),Out)
#else
#define HCV_ASSERT(Cond) do { if (false && (Cond)) hcv_fatal_stop_at(__FILE_,__LINE__); } while(0)
#define HCV_ASSERT_OUT(Cond,Fmt,Out)  do { if (false && (Cond)) \
      std::clog << Out; } while(0)
#endif /*NDEBUG*/


////////////////////////////////////////////////////////////////
// syslog facility

extern "C" std::recursive_mutex hcv_syslogmtx;
extern "C" void hcv_syslog_at (const char *fil, int lin, int prio,const std::string&str);
#define HCV_SYSLOGOUT_AT_BIS(Fil,Lin,Prio,...) do {		\
  int err##Lin = errno;						\
  std::lock_guard<std::recursive_mutex>				\
    gu##Lin(hcv_syslogmtx);					\
  std::ostringstream outs##Lin;					\
  outs##Lin << " !! "						\
	 << __VA_ARGS__ << std::endl;				\
  if (err##Lin)							\
    outs##Lin << "-: " << strerror(err##Lin);			\
  hcv_syslog_at ((Fil), (Lin), (Prio), (outs##Lin.str()));	\
  } while(0)

#define HCV_SYSLOGOUT_AT(Fil,Lin,Prio,...) HCV_SYSLOGOUT_AT_BIS(Fil,Lin,(Prio),##__VA_ARGS__)

// typical usage would be HCV_SYSLOGOUT(LOG_NOTICE,"x=" << x)
#define HCV_SYSLOGOUT(Prio,...) HCV_SYSLOGOUT_AT(__FILE__,__LINE__,(Prio),##__VA_ARGS__)



// debug facility
extern "C" std::atomic<bool> hcv_debugging;


extern "C" void hcv_debug_at (const char *fil, int lin, std::ostringstream&outs);
#define HCV_DEBUGOUT_AT_BIS(Fil,Lin,...) do {	\
  if (hcv_debugging.load()) {			\
  std::ostringstream outs##Lin;			\
  outs##Lin << " "				\
	    << __VA_ARGS__ << std::flush;	\
  hcv_debug_at ((Fil),(Lin),(outs##Lin));	\
  } } while(0)

#define HCV_DEBUGOUT_AT(Fil,Lin,...) HCV_DEBUGOUT_AT_BIS(Fil,Lin,##__VA_ARGS__)

// typical usage would be HCV_DEBUGOUT("x=" << x)
#define HCV_DEBUGOUT(...) HCV_DEBUGOUT_AT(__FILE__,__LINE__,##__VA_ARGS__)



////////////////////////////////////////////////////////////////
///// configuration file https://developer.gnome.org/glibmm/stable/classGlib_1_1KeyFile.html
#define HCV_DEFAULT_CONFIG_PATH "/etc/helpcovid.conf"
extern "C" std::string hcv_get_config_file_path(void);
extern "C" bool hcv_config_has_group(const char*grpname);
extern "C" bool hcv_config_has_key(const char*grpname, const char*keyname);
/*** Do something with the configuration file, serialized by a mutex. For example:
     int foo=0;
     hcv_config_do([&foo](const Glib::KeyFile*kf) { foo = kf->get_integer("foo"); });
 ***/
extern "C" void hcv_config_do(const std::function<void(const Glib::KeyFile*)>&dofun);

extern "C" void hcv_load_config_file(const char*configfile=nullptr);

////////////////////////////////////////////////////////////////

class Hcv_PreparedStatement
{
public:
  Hcv_PreparedStatement(const std::string& name);
  ~Hcv_PreparedStatement();

  void bind(const std::string& arg);
  void bind(char arg);
  void bind(std::int64_t arg);
  pqxx::result query();

private:
  std::string m_name;
  pqxx::prepare::invocation* m_inv;
  pqxx::work* m_txn;
};


//// PostGreSQL database
extern "C" std::unique_ptr<pqxx::connection> hcv_dbconn;
extern "C" std::recursive_mutex hcv_dbmtx;
extern "C" void hcv_initialize_database(const std::string&uri);

extern "C" const std::string hcv_postgresql_version(void);

// register a prepared statement with the database
extern "C" void
hcv_database_register_prepared_statement(const std::string& name,
    const std::string& sql);


// query if an email is known or not
extern "C" bool hcv_database_with_known_email(const std::string&emailstr);

// INSERT some web cookie in the database, returning its serial
extern "C" long
hcv_database_get_id_of_added_web_cookie(const std::string& randomstr,
                                        time_t exptime, int webagenthash);
////////////////////////////////////////////////////////////////

//// Web service
class Hcv_http_template_data; /// forward declaration

// the first bytes of every HTML5 file
#define HCV_HTML5_START "<!DOCTYPE html"

// if the first 8 lines of some HTML file contain the following string, it should be expanded for <?hcv ...?> processing instructions
#define HCV_HTML_DYNAMIC_MAGIC "!HelpCoVidDynamic!"

/// this could be run with root privilege if we need to serve the :80
/// HTTP TCP port.
void hcv_initialize_web(const std::string&weburl, const std::string&webroot,
                        const std::string&sslcert, const std::string&sslkey);
void hcv_stop_web(void);

extern "C" void hcv_webserver_run(void);

extern "C" void hcv_output_encoded_html(std::ostream&out, const std::string&str);
extern "C" void hcv_output_cstr_encoded_html(std::ostream&out, const char*cstr);

extern "C" std::string hcv_get_web_root(void);

extern "C" long hcv_get_web_request_counter(void);

/// return a string, perhaps 0123-9wI1QOXiH0M03Pf1ef14ab69-1abc4, for a fresh web cookie.
extern "C" std::string hcv_web_register_fresh_cookie(Hcv_http_template_data*);


////////////////////////////////////////////////////////////////

//// template machinery: in some quasi HTML file starting with
//// '<!DOCTYPE html' expand every occurrence of <?hcv markup...?>
//// where <?hcv is verbatim; and return the expanded string.

extern "C" const unsigned hcv_max_template_size;

class Hcv_template_data
{
protected:
  virtual ~Hcv_template_data();
public:
  enum class TmplKind_en
  {
    hcvtk_none=0,
    hcvtk_http,
    hcvtk_https,
    hcvtk_websocket
  };
  virtual std::ostream* output_stream() const =0;
  virtual long serial() const =0;
private:
  const TmplKind_en _hcvt_kind;
protected:
  Hcv_template_data(TmplKind_en knd)
    : _hcvt_kind(knd)
  {
    if (knd == TmplKind_en::hcvtk_none)
      HCV_FATALOUT("no kind in Hcv_template_data @" << (void*)this);
  };
public:
  TmplKind_en kind() const
  {
    return _hcvt_kind;
  };
};				// end of Hcv_template_data


class Hcv_http_template_data : public Hcv_template_data
{
protected:
  const httplib::Request* _hcvhttp_request;
  httplib::Response* _hcvhttp_response;
  long _hcvhttp_reqnum;
  mutable std::ostringstream _hcvhttp_outs;
public:
  Hcv_http_template_data(const httplib::Request& req, httplib::Response&resp, long reqnum)
    : Hcv_template_data(TmplKind_en::hcvtk_http),
      _hcvhttp_request(&req),
      _hcvhttp_response(&resp),
      _hcvhttp_reqnum(reqnum),
      _hcvhttp_outs()
  {
  };
protected:
  Hcv_http_template_data(TmplKind_en kind, const httplib::Request& req, httplib::Response&resp, long reqnum)
    : Hcv_template_data(kind),
      _hcvhttp_request(&req),
      _hcvhttp_response(&resp),
      _hcvhttp_reqnum(reqnum),
      _hcvhttp_outs()
  {
  };
public:


/// this reaches the getsockname call in httplib.h
/// http://man7.org/linux/man-pages/man2/getsockname.2.html
  void get_request_socket_ip(struct sockaddr *addr, socklen_t *addrlen);

  long request_number() const
  {
    return _hcvhttp_reqnum;
  };
  //

  std::string remote_address() const
  {
    if (_hcvhttp_request)
      return _hcvhttp_request->get_header_value("REMOTE_ADDR");
    else
      return "";
  }

  std::string request_method() const
  {
    if (_hcvhttp_request)
      return _hcvhttp_request->method;
    else
      return "";
  };
  std::string request_path() const
  {
    if (_hcvhttp_request)
      return _hcvhttp_request->path;
    else
      return "";
  };
  virtual std::ostream* output_stream() const
  {
    return &_hcvhttp_outs;
  };
  virtual long serial() const
  {
    return _hcvhttp_reqnum;
  };
  const httplib::Request*request() const
  {
    return _hcvhttp_request;
  };
  httplib::Response*response() const
  {
    return _hcvhttp_response;
  };
  void set_http_response_header(const char *key, const std::string &val)
  {
    if ( _hcvhttp_response)
      {
        HCV_DEBUGOUT("Hcv_http_template_data #" << request_number()
                     << " set_http_response_header key=" << key
                     << " val=" << val);
        _hcvhttp_response->set_header(key, val);
      }
  };
  virtual ~Hcv_http_template_data();
};				// end of Hcv_http_template_data


class Hcv_https_template_data : public Hcv_http_template_data
{
public:
  Hcv_https_template_data(const httplib::Request& req, httplib::Response&resp, long reqnum) :
    Hcv_http_template_data(TmplKind_en::hcvtk_https, req, resp, reqnum)
  {
  };
  virtual ~Hcv_https_template_data();
};				// end of Hcv_https_template_data


#warning TODO: add class Hcv_websocket_template_data
#if 0
class Hcv_websocket_template_data : public Hcv_template_data
{
};				// end Hcv_websocket_template_data
#endif /*missing Hcv_websocket_template_data*/



extern "C" std::string hcv_expand_template_file(const std::string& filepath,Hcv_template_data*templdata);

extern "C" std::string hcv_expand_template_input_stream(std::istream&srcinp, const char*inpname, Hcv_template_data*templdata);


extern "C" std::string hcv_expand_template_string(const std::string&inpstr, const char*inpname, Hcv_template_data*templdata);

typedef std::function<void(Hcv_template_data*templdata, const std::string &procinstr, const char*filename, int lineno, long offset)> hcv_template_expanding_closure_t;
// the name should be like a C identifier
extern "C" void hcv_register_template_expander_closure(const std::string&name, const hcv_template_expanding_closure_t&expfun);

extern "C" void hcv_forget_template_expander(const std::string&name);

extern "C" void
hcv_expand_processing_instruction(Hcv_template_data*templdata, const std::string &procinstr, const char*filename, int lineno, long offset);

extern "C" void hcv_initialize_templates(void);

////////////////////////////////////////////////////////////////
//////////////// timing functions
// see http://man7.org/linux/man-pages/man2/clock_gettime.2.html
extern "C" double hcv_monotonic_start_time;
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

///////////////////////////////////////////////////////////////////////////////
// random numbers - shameless copied from code of http://refpersys.org/

class Hcv_Random
{
  static thread_local Hcv_Random _rand_thr_;
  static bool _rand_is_deterministic_;
  static std::ranlux48 _rand_gen_deterministic_;
  static std::mutex _rand_mtx_deterministic_;
  /// the thread local random state
  unsigned long _rand_count;
  std::mt19937 _rand_generator;
  /// we could need very quick and poor small random numbers on just 4
  /// bits. For these, we care less about the random quality, but even
  /// more about speed. So we keep one 32 bits of random number in
  /// advance, and a count of the remaining random bits in it.
  uint32_t _rand_advance;
  uint8_t _rand_remainbits;
  unsigned _rand_threadrank;
  static std::atomic<unsigned> _rand_threadcount;
  static constexpr const unsigned _rand_reseed_period_ = 65536;
  /// private initializer
  void init_deterministic (void);
  /// private deterministic reseeder
  void deterministic_reseed (void);
  /// private constructor
  Hcv_Random () :
    _rand_count(0), _rand_generator(), _rand_advance(0), _rand_remainbits(0),
    _rand_threadrank(std::atomic_fetch_add(&_rand_threadcount,1U))
  {
    if (_rand_is_deterministic_)
      init_deterministic();
  };
  ///
  uint32_t generate_32u(void)
  {
    if (HCV_UNLIKELY(_rand_count++ % _rand_reseed_period_ == 0))
      {
        if (HCV_UNLIKELY(_rand_is_deterministic_))
          deterministic_reseed();
        else
          {
            std::random_device randev;
            auto s1=randev(), s2=randev(), s3=randev(), s4=randev(),
                 s5=randev(), s6=randev(), s7=randev();
            std::seed_seq seq {s1,s2,s3,s4,s5,s6,s7};
            _rand_generator.seed(seq);
          }
      }
    return _rand_generator();
  };
  uint32_t generate_nonzero_32u(void)
  {
    uint32_t r = 0;
    do
      {
        r = generate_32u();
      }
    while (HCV_UNLIKELY(r==0));
    return r;
  };
  uint64_t generate_64u(void)
  {
    return (static_cast<uint64_t>(generate_32u())<<32) | static_cast<uint64_t>(generate_32u());
  };
  uint8_t generate_quickly_4bits()
  {
    if (HCV_UNLIKELY(_rand_remainbits < 4))
      {
        _rand_advance = generate_32u();
        _rand_remainbits = 32;
      }
    uint8_t res = _rand_advance & 0xf;
    _rand_remainbits -= 4;
    _rand_advance = _rand_advance>>4;
    return res;
  };
  uint8_t generate_quickly_8bits()
  {
    if (HCV_UNLIKELY(_rand_remainbits < 8))
      {
        _rand_advance = generate_32u();
        _rand_remainbits = 32;
      }
    uint8_t res = _rand_advance & 0xff;
    _rand_advance = _rand_advance>>8;
    _rand_remainbits -= 8;
    return res;
  };
public:
  static void start_deterministic(long seed); // to be called from main
  static uint32_t random_32u(void)
  {
    return _rand_thr_.generate_32u();
  };
  static uint64_t random_64u(void)
  {
    return _rand_thr_.generate_64u();
  };
  static uint32_t random_nonzero_32u(void)
  {
    return _rand_thr_.generate_nonzero_32u();
  };
  static uint8_t random_quickly_4bits()
  {
    return _rand_thr_.generate_quickly_4bits();
  };
  static uint8_t random_quickly_8bits()
  {
    return _rand_thr_.generate_quickly_8bits();
  };
};				// end class Hcv_Random



///////////////////////////////////////////////////////////////////////////////
// background thread

/*******
 * A background thread is needed for various cleanup tasks, like
 * removing old cookies server-side in the database. It sleeps most of
 * the time, and wakes up every ten seconds to run some cleanup.
 *******/
extern "C" void hcv_start_background_thread(void);


// register a closure and some data to be executed in the background
// thread postponed by some delay (at least 0.01 seconds, at most 1000
// seconds).
#define HCV_POSTPONE_MINIMAL_DELAY 0.01
#define HCV_POSTPONE_MAXIMAL_DELAY 1000.0
extern "C" void hcv_do_postpone_background(double delay, const std::string&name, void*data,
    const std::function<void(void*)>& todofun);
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Database models
///////////////////////////////////////////////////////////////////////////////


extern "C" bool
hcv_model_validator_required(const std::string& field, const std::string& tag,
                             std::string& msg);


extern "C" bool
hcv_model_validator_email(const std::string& field, const std::string& tag,
                          std::string& msg);

///////////////////////////////////////////////////////////////////////////////
// Database models
///////////////////////////////////////////////////////////////////////////////


struct hcv_user_model
{
  std::string user_first_name;
  std::string user_family_name;
  std::string user_email;
  std::string user_gender;
};


extern "C" bool
hcv_user_model_validate(const hcv_user_model& model, hcv_user_model& status);

extern "C" bool
hcv_user_model_create(const hcv_user_model& model, hcv_user_model& status);

extern "C" bool
hcv_user_model_authenticate(const std::string& email,
                            const std::string& passwd);

extern "C" std::int64_t
hcv_user_model_find_by_email(const std::string& email);

extern "C" void
hcv_user_model_update(std::int64_t id, const hcv_user_model& model);

extern "C" void
hcv_user_model_update_password(const std::string& email,
                               const std::string& password);

///////////////////////////////////////////////////////////////////////////////
// Login views
///////////////////////////////////////////////////////////////////////////////

extern "C" std::string
hcv_login_view_get(const httplib::Request& req, httplib::Response& resp, long reqnum);


extern "C" std::string
hcv_login_view_post(const httplib::Request& req, httplib::Response& resp, long reqnum);


///////////////////////////////////////////////////////////////////////////////
// Home views
///////////////////////////////////////////////////////////////////////////////

extern "C" std::string
hcv_home_view_get(const httplib::Request& req, httplib::Response& resp, long reqnum);


#endif /*HELPCOVID_HEADER*/
