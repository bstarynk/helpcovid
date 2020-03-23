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


import readline
import os



# https://eli.thegreenplace.net/2016/basics-of-using-the-readline-library/
def make_completer(vocabulary):
    def custom_complete(text, state):
        results = [x for x in vocabulary if x.startswith(text)] + [None]
        return results[state] + ' '
    return custom_complete



def main():
    print('Starting HelpCovid configuration generator...')

    # Define tab completion vocabulary
    vocabulary = {
        'localhost', 'webroot', 'helpcovid_db', 'helpcovid_usr', 'helpcovid'
    }

    # Initialise GNU readline
    readline.parse_and_bind('tab: complete')
    readline.set_completer(make_completer(vocabulary))

    # Create configuration file in $HOME
    rc_path = os.path.expanduser('~') + '/.helpcovidrc'
    rc_file = open(rc_path, 'w')

    # Read keys and save to config file
    try:
        url = input('URL: ').strip()
        print('[url = {0}]'.format(url))
        rc_file.write('[web]\n\n')
        rc_file.write('url=%s\n' % url)

        root = input('Root: ').strip()
        print('[root = {0}]'.format(root))
        rc_file.write('root=%s\n' % root)

        ssl_cert = input('SSL Certificate: ').strip()
        print('[sslcert = {0}]'.format(ssl_cert))
        rc_file.write('sslcert=%s\n' % ssl_cert)

        ssl_key = input('SSL Key: ').strip()
        print('[sslkey = {0}]'.format(ssl_key))
        rc_file.write('sslkey=%s\n\n' % ssl_key)

        conn_str = input('Connection String: ').strip()
        print('[connection = {0}]'.format(conn_str))
        rc_file.write('[postgresql]\n\n')
        rc_file.write('connection=%s\n' % conn_str)
       
        # Wrap up
        print('\nConfiguration file saved to {0}]'.format(rc_path))
        rc_file.close()

    # Handle interrupt
    except (EOFError, KeyboardInterrupt) as e:
        print('\nInterrupt detected, exiting...')
        rc_file.close()


if __name__ == '__main__':
    main()

