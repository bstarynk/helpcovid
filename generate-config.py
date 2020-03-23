#!/usr/bin/python3
##***************************************************************
## file generate-config.py
##
## Description:
##      Configuration script for https://github.com/bstarynk/helpcovid
##
## Author(s):
##      © Copyright 2020
##      Basile Starynkevitch <basile@starynkevitch.net>
##      Abhishek Chakravarti <abhishek@taranjali.org>
##      Nimesh Neema <nimeshneema@gmail.com>
##
##
## License:
##    This HELPCOVID program is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation, either version 3 of the License, or
##    (at your option) any later version.
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program.  If not, see <http://www.gnu.org/licenses/>.
##****************************************************************************
import os

print('Starting HelpCovid configuration generator...')
print('See https://github.com/bstarynk/helpcovid and its README.md')
print("\n")

# Ask for keys
url = input('web URL: ')
root = input('web document root: ')
ssl_cert = input('SSL Certificate: ')
ssl_key = input('SSL Key: ')
conn_str = input('Connection String: ')

db_conn = input('PostGreSQL connection: ')
# Create $HOME/.helpcovidrc file
rc_path = os.path.expanduser('~') + '/.helpcovidrc'
rc_file = open(rc_path, 'a')
rc.file.write('# file ~/.helpcovidrc generated');
## please emit the generation date
# Write keys for web group
rc_file.write('\n[web]\n')
rc_file.write('url=%s\n' % url)
rc_file.write('root=%s\n' % root)
rc_file.write('sslcert=%s\n' % ssl_cert)
rc_file.write('sslkey=%s\n\n' % ssl_key)

# Write keys for postgresql group
rc_file.write('\n[postgresql]\n')
rc_file.write('connection=%s\n' % conn_str)

# Write keys for PostGreSQL group
rc_file.write('\n[postgresql]\n');
rc_file.write('connection=%s\n', db_conn);
rc.file.write('# end of generated file ~/.helpcovidrc\n');
# Wrap up
rc_file.close()

