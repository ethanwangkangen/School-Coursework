# !/usr/bin/env python3
import os
import sys
from Cryptodome.Hash import SHA256

if len(sys.argv) < 3:
    print("Usage: python3 ", os.path.basename(__file__), "key_file_name document_file_name")
    sys.exit()

key_file_name   = sys.argv[1]
file_name       = sys.argv[2]


# get the authentication key from the file
# TODO
with open(key_file_name, 'rb') as key_file:
    key = key_file.read(32)


# read the input file
with open(file_name, 'rb') as file:
    mac_from_file = file.read(32)  # First 32 bytes is the MAC
    data = file.read()             # Remaining bytes are the data

# First 32 bytes is the message authentication code
# TODO


# Use the remaining file content to generate the message authentication code
# TODO
h = SHA256.new()
h.update(data + key)
mac_generated = h.digest()
#print(mac_generated)

if mac_from_file == mac_generated:
    print ('yes')
else:
    print ('no')
