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


import datetime
import os
import readline


# https://eli.thegreenplace.net/2016/basics-of-using-the-readline-library/
def make_completer(vocabulary):
    def custom_complete(text, state):
        results = [x for x in vocabulary if x.startswith(text)] + [None]
        return results[state] + ' '
    return custom_complete



def initialize_readline():
    # Define tab completion vocabulary
    vocabulary = {
        'localhost', 'webroot', 'helpcovid_db', 'helpcovid_usr', 'helpcovid'
    }

    # Initialise GNU readline
    readline.parse_and_bind('tab: complete')
    readline.set_completer(make_completer(vocabulary))



def write_key_value_pair(key, conf_file):
    val =  input('{0}: '.format(key)).strip()
    print('[{0} = {1}]'.format(key, val))
    conf_file.write('{0}={1}\n'.format(key, val))

    return val



def create_configuration_file():
    # Create configuration file in $HOME
    rc_path = os.path.expanduser('~') + '/.helpcovidrc'
    rc_file = open(rc_path, 'w')

    # Read web keys and save to config file
    rc_file.write('[web]\n\n')
    write_key_value_pair('url', rc_file)
    write_key_value_pair('root', rc_file)
    write_key_value_pair('sslcert', rc_file)
    write_key_value_pair('sskey', rc_file)

    # Read postgresql keys
    rc_file.write('\n[postgresql]\n\n')
    conn = write_key_value_pair('connection', rc_file)

    # Generate timestamp
    ts = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
    rc_file.write('\n# generated on %s\n' % ts)
    rc_file.write('# end of generated file ~/.helpcovidrc\n');

    # Ensure chmod 600
    rc_file.close()
    os.chmod(rc_path, 0o600)

    # Wrap up
    print('\nConfiguration file saved to {0}]'.format(rc_path))
    return conn



def create_connection_dict(conn):
    split = conn.split(':')

    d = dict()
    d['database'] = split[0]
    d['user'] = split[1]
    d['password'] = split[2]

    return d


def create_temp_sql(keys):
    sql_path = '/tmp/helpcovid.sql'
    sql_file = open(sql_path, 'w')

    sql_file.write('CREATE DATABASE ')
    sql_file.write(keys['database'])

    sql_file.write(';\nCREATE USER ')
    sql_file.write(keys['user'])
    sql_file.write(' WITH PASSWORD \'')
    sql_file.write(keys['password'])

    sql_file.write('\';\nALTER ROLE ')
    sql_file.write(keys['user'])
    sql_file.write(' SET client_encoding TO \'utf8\';')

    sql_file.write('\nALTER ROLE ')
    sql_file.write(keys['user'])
    sql_file.write(' SET default_transaction_isolation TO \'read committed\';')

    sql_file.write('\nALTER ROLE ')
    sql_file.write(keys['user'])
    sql_file.write(' SET timezone to \'UTC\';')

    sql_file.write('\nGRANT ALL PRIVILEGES ON DATABASE ')
    sql_file.write(keys['database'])
    sql_file.write(' TO ')
    sql_file.write(keys['user'])
    sql_file.write(';\n')

    sql_file.close
    return sql_path



def create_database(sql_path):
    print('Creating database...')
    os.system('sudo -u postgres psql -f ' + sql_path)



def create_password_file(keys):
    pass_path = os.path.expanduser('~') + '/.pgpasswd_helpcovid'
    pass_file = open(pass_path, 'w')
    pass_file.write(keys['password'])
    pass_file.close()

    print('Password file created at ' + pass_path)



def main():
    print('Starting HelpCovid configuration generator...')
    print('See https://github.com/bstarynk/helpcovid and its README.md\n')


    initialize_readline()
    conn = create_configuration_file()
    keys = create_connection_dict(conn)
    sql = create_temp_sql(keys)
    create_database(sql)
    create_password_file(keys)



if __name__ == '__main__':
    main()

