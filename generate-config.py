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
import pwd
import readline
import tempfile

config_dict = dict()


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


def write_key_value_pair(key, conf_file, suffix_slash=False, prefill=""):
    val = input('{0}: '.format(key)).strip() or prefill
    print(val)
    if suffix_slash == True:
        if not val.endswith('/'):
            val += '/'

    print('[{0} = {1}]'.format(key, val))
    conf_file.write('{0}={1}\n'.format(key, val))

    return val


def create_configuration_file():
    # Create configuration file in $HOME
    rc_path = os.path.expanduser('~') + '/.helpcovidrc'
    if os.access(rc_path, os.F_OK):  os.rename(rc_path, rc_path + '~')
    rc_file = open(rc_path, 'w')

    # Read web keys and save to config file
    rc_file.write('[web]\n\n')
    print("\n ==== HelpCovid web configuration =====\n")
    print("Enter web served URL, e.g. http://localhost:8089/ or https://example.com/\n")
    write_key_value_pair('url', rc_file, True, "http://localhost:8089/")
    print(
        "Enter web document root, a local directory containing web served resources (e.g. HTML files, CSS stylesheets)\n");
    print("... e.g. " + os.getcwd() + "/webroot/")
    write_key_value_pair('root', rc_file, True, os.getcwd() + "/webroot/")
    print("Enter the OpenSSL public certificate, for HTTPS - can be left empty; see https://www.openssl.org/\n")
    write_key_value_pair('sslcert', rc_file)
    print("Enter the OpenSSL private key, for HTTPS")
    write_key_value_pair('sskey', rc_file)

    # Read postgresql keys
    print("\n ==== HelpCovid PostGreSQL database configuration, see https://www.postgresql.org/ ====\n")
    rc_file.write('\n[postgresql]\n\n')
    print("Enter the PostGreSQL connnection string, see https://www.postgresql.org/docs/current/libpq-connect.html")
    print('Example: dbname=helpcovid_db user=helpcovid_usr'
          ' password=passwd1234 hostaddr=127.0.0.1 port=5432')
    conn = write_key_value_pair('connection', rc_file,
                                prefill="dbname=helpcovid_db user=helpcovid_usr password=passwd1234 hostaddr=127.0.0.1 port=5432")
    print('Enter *full path* of the HelpCovid PostgreSQL password file')
    print("... e.g. " + os.path.expanduser('~') + "/.helpcovid-pg-password")
    config_dict['passfile'] = write_key_value_pair('passfile', rc_file,
                                                   prefill=os.path.expanduser('~') + "/.helpcovid-pg-password")

    # Read HTML template tag keys
    print("\n ==== HelpCovid HTML template tags configuration ====\n")
    rc_file.write('\n[html]\n\n')
    print("Enter HTML for <?hcv_datacontroller?>")
    print('Example: <a href="mailto:john.doe@example.org>John Doe</a>')
    write_key_value_pair("hcv_datacontroller_tag", rc_file, prefill='<a href="mailto:john.doe@example.org>John Doe</a>')

    # Generate timestamp
    ts = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
    rc_file.write('\n# generated on %s\n' % ts)
    rc_file.write('# end of generated file ~/.helpcovidrc\n')

    # Ensure chmod 600
    rc_file.close()
    os.chmod(rc_path, 0o600)

    # Wrap up
    print('\nConfiguration file saved to {0}]'.format(rc_path))
    return conn


def write_connection_keys(conn):
    split = conn.split()

    config_dict['database'] = split[0].split('=')[1]
    config_dict['user'] = split[1].split('=')[1]
    config_dict['password'] = split[2].split('=')[1]


def create_temp_sql():
    sql_file, sql_path = tempfile.mkstemp(suffix='helpcovid', text=True)
    sql_file = open(sql_file, 'w')

    # https://stackoverflow.com/questions/18389124/
    sql = 'SELECT \'CREATE DATABASE {0}\' WHERE NOT EXISTS \
        (SELECT FROM pg_database WHERE datname = \'{0}\')\\gexec\n'.format(
        config_dict['database'])
    sql_file.write(sql)

    sql = 'create extension if not exists pgtap;'
    sql_file.write(sql)
    sql = 'create extension if not exists pgcrypto;'
    sql_file.write(sql)

    # https://stackoverflow.com/questions/8092086/
    sql_file.write('DO $$\n BEGIN\n')
    sql_file.write('CREATE USER ')
    sql_file.write(config_dict['user'])
    sql_file.write(' WITH PASSWORD \'')
    sql_file.write(config_dict['password'])
    sql_file.write('\';\nEXCEPTION WHEN DUPLICATE_OBJECT THEN\n')
    sql_file.write('RAISE NOTICE \'user already exists; skipping...\';\n')
    sql_file.write('END\n$$;')

    sql_file.write('ALTER ROLE ')
    sql_file.write(config_dict['user'])
    sql_file.write(' SET client_encoding TO \'utf8\';')

    sql_file.write('\nALTER ROLE ')
    sql_file.write(config_dict['user'])
    sql_file.write(' SET default_transaction_isolation TO \'read committed\';')

    sql_file.write('\nALTER ROLE ')
    sql_file.write(config_dict['user'])
    sql_file.write(' SET timezone to \'UTC\';')

    sql_file.write('\nGRANT ALL PRIVILEGES ON DATABASE ')
    sql_file.write(config_dict['database'])
    sql_file.write(' TO ')
    sql_file.write(config_dict['user'])
    sql_file.write(';\n')

    sql_file.close
    os.system('sudo /bin/chown postgres:postgres ' + sql_path)
    return sql_path


def create_database(sql_path):
    print('Creating database...')
    os.system('sudo -u postgres /usr/bin/psql -f ' + sql_path)
    os.system('sudo /bin/rm -v ' + sql_path)
    os.system('psql -d {0} -U {1} -f pgtap.sql'.format(config_dict['database'],
        config_dict['user']))
    # os.remove(sql_path) <-- encounters permission error


def create_password_file():
    if os.access(config_dict['passfile'], os.F_OK):
        os.rename(config_dict['passfile'], config_dict['passfile'] + '~')
    pass_file = open(config_dict['passfile'], 'w')
    pass_file.write(config_dict['password'])
    pass_file.write("\n")
    pass_file.close()

    print('Password file created at ' + config_dict['passfile'])


def check_postgres_exists():
    try:
        pwd.getpwnam('postgres')
    except(KeyError):
        print('postgres user not available. Please install PostgreSQL first...')


def main():
    print('Starting HelpCovid configuration generator...')
    print('See https://github.com/bstarynk/helpcovid and its README.md\n')

    check_postgres_exists()
    initialize_readline()

    conn = create_configuration_file()
    write_connection_keys(conn)
    sql_path = create_temp_sql()
    create_database(sql_path)
    create_password_file()


if __name__ == '__main__':
    main()
