# HelpCovid plugins.

The plugin machinery of  [https://github.com/bstarynk/helpcovid/](HelpCovid)

## plugin license

[https://github.com/bstarynk/helpcovid/](HelpCovid) is
[GPLv3+](https://www.gnu.org/licenses/gpl-3.0.en.html) licensed, so
plugins should be, if possible, compatible with that license, and
preferably under that same license too.

## plugin loading

[Plugins](https://en.wikipedia.org/wiki/Plug-in_(computing)) are
loaded only at [https://github.com/bstarynk/helpcovid/](HelpCovid)
startup, by the `hcv_load_plugin` function (called in `hcv_main.cc`).

Each plugin has a unique name (a C-like identifier). So a plugin named
`foo_23_bar` corresponds to a `hcvplugin_foo_23_bar.so` file.  Use the
`LD_LIBRARY_PATH` facility to store the plugin in some other directory.

A plugin is a Linux shared
[library](https://en.wikipedia.org/wiki/Library_(computing)) with a
`.so` suffix loaded by
[dlopen(3)](http://man7.org/linux/man-pages/man3/dlopen.3.html). Read
the [Program Library
HOWTO](https://tldp.org/HOWTO/Program-Library-HOWTO/), the [C++ dlopen
mini HOWTO](https://www.tldp.org/HOWTO/C++-dlopen/index.html) and
Drepper's paper [How to write shared
libraries](https://www.akkadia.org/drepper/dsohowto.pdf) (december,
2011).