from socket import *
import zlib
import sys

def splitPacket(packet):
    try:
        header, body = packet.split(b'm/', 1)  # Split on the first occurrence of 'm/'
        seq_part, checksum_part = header.split(b'c/', 1)  # Split the header on 'c/'
        
        seq = int(seq_part.split(b'/')[1])  # Get the part after 's/'
        checksum = int(checksum_part)  # Convert checksum to integer
        
        return seq, checksum, body  # Return as a tuple
    except Exception as e:
#        print(f"Error splitting packet: {e}")
        return 999, -1, b""

def createAck(ackNumber):
    ackNumberStr = str(ackNumber)
    message_bytes = ackNumberStr.encode('utf-8')
    checksum = zlib.crc32(message_bytes)

    # Create the header
    header = f"c/{checksum}"
    packet = f"{header}m/{ackNumberStr}"
    return packet.encode('utf-8')

def isValidBody(checksum, packetBody):
    # Ensure that packetBody is in bytes for checksum verification
    return checksum == zlib.crc32(packetBody)

def main():
    clientName = 'localhost'
    clientPort = None
    serverPort = int(sys.argv[1])

    serverSocket = socket(AF_INET, SOCK_DGRAM)
    serverSocket.bind(('', serverPort))

    ack = 0

    def sendAck():
        serverSocket.sendto(createAck(ack), (clientName, clientPort))

    while True:
        packet, addr = serverSocket.recvfrom(2048)
        if clientPort is None:
            clientPort = addr[1]

        seq, checksum, body = splitPacket(packet)

        # If corrupt (checksum wrong), just resend last ack
        if not isValidBody(checksum, body):
 #           print("Received corrupt packet. Resending ACK:", ack)
            sendAck()
            continue

        # Check sequence number. If correct, alternate ACK and send. Output to stdout
        if seq != ack:
            ack = 1 - ack
  #          print("Sending ACK:", ack)
            sendAck()
            if body:
                print(body.decode(), end="")
        else:  # Wrong sequence number, send old ack
#            print("Duplicate packet received. Resending ACK:", ack)
            sendAck()

if __name__ == '__main__':
    main()
