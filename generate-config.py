#!/usr/bin/python

import os

print('Starting HelpCovid configuration generator...')

# Ask for keys
url = input('URL: ')
root = input('Root: ')
ssl_cert = input('SSL Certificate: ')
ssl_key = input('SSL Key: ')

# Create $HOME/.helpcovidrc file
rc_path = os.path.expanduser('~') + '/.helpcovidrc'
rc_file = open(rc_path, 'a')

# Write keys for web group
rc_file.write('[web]\n\n')
rc_file.write('url=%s\n' % url)
rc_file.write('root=%s\n' % root)
rc_file.write('sslcert=%s\n' % ssl_cert)
rc_file.write('sslkey=%s\n', ssl_key)

# Wrap up
rc_file.close()

