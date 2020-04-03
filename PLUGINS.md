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

A plugin is not explicitly [dlclose(3)](http://man7.org/linux/man-pages/man3/dlclose.3.html)-d.


## plugin symbols and calling conventions

A plugin should provide the following symbols (bound to literal strings)

```
extern "C" const char hcvplugin_name[];
extern "C" const char hcvplugin_version[];
extern "C" const char hcvplugin_gpl_compatible_license[];
extern "C" const char hcvplugin_gitapi[]; // our git id
```

and export the following routines

```
/// every plugin should have
extern "C" void hcvplugin_initialize_web(httplib::Server*,const char*);
```

The `hcvplugin_name` should be the name of the plugin. The
`hcvplugin_version` could be some version string. The
`hcvplugin_gpl_compatible_license` describes a GPL compatible license
(preferably `"GPLv3+"` for GPLv3+ plugins). The `hcvplugin_gitapi`
should be the GITID (latest `git commit` identifier) of the current
`./helpcovid` executable.

The `hcvplugin_initialize_web` is called with the web server and could
add services there. The second argument is the string argument (see below), if
any, passed to `--plugin` program argument.

A plugin can *optionally* define the following routine to initialize the database.

```
/// every plugin can define
extern "C" void hcvplugin_initialize_database(pqxx::connection*,const char*);
```
The second argument is the string argument (see below), if
any, passed to `--plugin` program argument.

## using plugins

Pass the `--plugin` program argument with the plugin name and
optionally, after a colon `:` a plugin argument string.

For example `--plugin=echo:foo` would `dlopen` the `hcvplugin_echo.so`
plugin and pass it the `foo` argument string.

## plugin naming conventions

The plugin should document what it requires and provide both on web aspects and on database aspects.

A plugin named `foo` may serve URLs containing `plugin_foo`.

A plugin named `foo` could `CREATE TABLE IF NOT EXISTS` SQL tables
prefixed with `tbfoo_`, `CREATE INDEX IF NOT EXISTS` SQL indexes
prefixed with `ixfoo_`, have SQL [prepared
statements](https://www.postgresql.org/docs/current/sql-prepare.html)
whose name end with `_foopstm`. For `libpqxx` prepared statements see
also [this](https://libpqxx.readthedocs.io/en/latest/a01331.html) and
use `pqxx::connection::prepare`.

It should be possible (perhaps with a companion plugin, or with the
`clear_database` argument) to remove all tables, indexes,
etc... created by some given plugin.
