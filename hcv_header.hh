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


// PostGresQL C++ http://pqxx.org/development/libpqxx
#include <pqxx/pqxx>


//  cpp-httplib https://github.com/yhirose/cpp-httplib


extern "C" const char hcv_timestamp[];
extern "C" const char hcv_topdirectory[];
extern "C" const char hcv_gitid[];
extern "C" const char hcv_lastgittag[];
extern "C" const char hcv_lastgitcommit[];
extern "C" const char hcv_md5sum[];
extern "C" const char*const hcv_files[];
extern "C" const char hcv_makefile[];
#endif /*HELPCOVID_HEADER*/
