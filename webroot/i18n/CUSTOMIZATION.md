# customization of HelpCoVid

The `./generate-i18n.py` script of
[HelpCovid](https://github.com/bstarynk/helpcovid) generates message
files suitable as input to
[msgfmt(1)](http://man7.org/linux/man-pages/man1/msgfmt.1.html) by
parsing HTML files for `<?hcv msg` processing instructions.

The `./generate-i18n.py` script of
[HelpCovid](https://github.com/bstarynk/helpcovid) accepts both
`--help` and `--subscript` program argument. The later is for
debugging purposes and should write subscript serial numbers like `₀₆`
in `msgstr`. Since `<?hcv msg` processing instructions in our
`webroot/html/*.html` files are expanded with
[dgettext(3)](http://man7.org/linux/man-pages/man3/dgettext.3.html) :
see our C++ file `hcv_views.cc` function `hcv_view_expand_msg`.

The message files go under `helpcovid/webroot/i18n/` 
