## https://github.com/bstarynk/helpcovid file Makefile

## License:
##    This HelpCovid program is free software: you can redistribute it
##    and/or modify it under the terms of the GNU General Public
##    License as published by the Free Software Foundation, either
##    version 3 of the License, or (at your option) any later version.
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program.  If not, see <http://www.gnu.org/licences>

.PHONY: all plugins sanitized_plugins clean indent deploy localtest0


.SUFFIXES: .sanit.
.SUFFIXES: .so
.SUFFIXES: _sanit.so
HELPCOVID_SOURCES := $(wildcard hcv_*.cc)
HELPCOVID_PLUGINSOURCES := $(wildcard hcvplugin_*.cc)
HELPCOVID_OBJECTS := $(patsubst %.cc, %.o, $(HELPCOVID_SOURCES))
HELPCOVID_PLUGINS := $(patsubst %.cc, %.so, $(HELPCOVID_PLUGINSOURCES))
HELPCOVID_SANITIZED_PLUGINS := $(patsubst %.cc, %_sanit.so, $(HELPCOVID_PLUGINSOURCES))
HELPCOVID_HEADERS := $(wildcard hcv*.hh)
HELPCOVID_GIT_ID := $(shell ./generate-gitid.sh)

HELPCOVID_SANITIZED_OBJECTS := $(patsubst %.cc, %.sanit.o, $(HELPCOVID_SOURCES))

HELPCOVID_BUILD_CCACHE = ccache
HELPCOVID_BUILD_CC = gcc
HELPCOVID_BUILD_CXX = g++
HELPCOVID_BUILD_DIALECTFLAGS = -std=gnu++17
HELPCOVID_BUILD_WARNFLAGS = -Wall -Wextra
HELPCOVID_BUILD_OPTIMFLAGS = -O0 -g3
HELPCOVID_PKG_CONFIG = pkg-config
HELPCOVID_PKG_NAMES = glibmm-2.4 giomm-2.4 jsoncpp libpqxx openssl curlpp
HELPCOVID_PKG_CFLAGS:= $(shell $(HELPCOVID_PKG_CONFIG) --cflags $(HELPCOVID_PKG_NAMES))
HELPCOVID_PKG_LIBS:= $(shell $(HELPCOVID_PKG_CONFIG) --libs $(HELPCOVID_PKG_NAMES))

## for GCC address sanitizer
## https://stackoverflow.com/q/37970758/841108
## https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
HELPCOVID_SANITIZE_CXXFLAGS= -fsanitize=address -DHELPCOVID_SANITIZE=\"address\"

## it is not reasonable to link libraries statically
LIBES=  $(HELPCOVID_PKG_LIBS) -rdynamic -ldl
RM= rm -f
MV= mv
CC = $(HELPCOVID_BUILD_CCACHE) $(HELPCOVID_BUILD_CC)
CXX = $(HELPCOVID_BUILD_CCACHE) $(HELPCOVID_BUILD_CXX)
CXXFLAGS += $(HELPCOVID_BUILD_DIALECTFLAGS) $(HELPCOVID_BUILD_OPTIMFLAGS) \
            $(HELPCOVID_BUILD_CODGENFLAGS) \
	    $(HELPCOVID_BUILD_WARNFLAGS) $(HELPCOVID_BUILD_INCLUDE_FLAGS) \
	    $(HELPCOVID_PKG_CFLAGS) -DHELPCOVID_GITID=\"$(HELPCOVID_GIT_ID)\"
LDFLAGS += -rdynamic -pthread -L /usr/local/lib -L /usr/lib

all:
	if [ -f helpcovid ] ; then  $(MV) -f --backup helpcovid helpcovid~ ; fi
	$(RM) __timestamp.o __timestamp.c
	$(MAKE) $(MAKEFLAGS) helpcovid
	$(MAKE) $(MAKEFLAGS) plugins
	sync


## The usual program
helpcovid: $(HELPCOVID_OBJECTS) __timestamp.o
	$(LINK.cc) $(HELPCOVID_OBJECTS)  __timestamp.o \
           $(LIBES) -o $@-tmp
	$(MV) --backup $@-tmp $@
	$(MV) --backup __timestamp.c __timestamp.c~
	$(RM) __timestamp.o

__timestamp.c:
	./generate-timestamp.sh > $@-tmp
	printf "const char hcv_cxx_compiler[]=\"%s\";\n" "$$($(CXX) --version | head -1)" >> $@-tmp
	$(MV) --backup $@-tmp $@

## the address-sanitized variant
## https://en.wikipedia.org/wiki/AddressSanitizer
sanitized-helpcovid: $(HELPCOVID_SANITIZED_OBJECTS) __timestamp.o
	$(LINK.cc) $(HELPCOVID_SANITIZE_CXXFLAGS) \
           $(HELPCOVID_SANITIZED_OBJECTS)  __timestamp.o \
           $(LIBES) -o $@-tmp
	$(MV) --backup $@-tmp $@
	$(MV) --backup __timestamp.c __timestamp.c~
	$(RM) __timestamp.o

%.sanit.o: %.cc
	 $^ $(HELPCOVID_SANITIZE_CXXFLAGS) -o $@

%.so: %.cc
	$(LINK.cc) -fPIC -shared $^ -o $@

# to help debugging preprocessor macros
%.ii: %.cc
	 $(HELPCOVID_BUILD_CXX) -C -E $(CXXFLAGS) $^ | sed 's:^#://\0:g' > $@


%_sanit.so: %.cc
	$(LINK.cc) -fPIC -shared $(HELPCOVID_SANITIZE_CXXFLAGS) $^ -o $<

clean:
	$(RM) *~ *% *.orig *.o i*.so *.ii helpcovid *tmp core*

indent:
	./indent-cxx-files.sh $(HELPCOVID_SOURCES) $(HELPCOVID_HEADERS) $(HELPCOVID_PLUGINSOURCES)

plugins: $(HELPCOVID_PLUGINS)

sanitized_plugins: $(HELPCOVID_SANITIZED_PLUGINS)

localtest0: helpcovid
#	./generate-config.py
	./helpcovid --debug --threads=3

