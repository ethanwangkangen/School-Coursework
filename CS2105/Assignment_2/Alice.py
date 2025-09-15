from socket import *
import zlib
import sys

MAX_PACKET_SIZE = 64
HEADER_SIZE = 20  # Approximate size for the header (s/[seq]c/[checksum]m/)

def createPacket(message, seq):
    message_bytes = message.encode('utf-8')
    checksum = zlib.crc32(message_bytes)

    # Ensure the total packet size does not exceed MAX_PACKET_SIZE
    if len(message_bytes) + HEADER_SIZE > MAX_PACKET_SIZE:
        message_bytes = message_bytes[:MAX_PACKET_SIZE - HEADER_SIZE]  # Truncate message

    # Create the header: s/[sequence]c/[checksum]m/message
    header = f"s/{str(seq)}c/{checksum}"
    packet = f"{header}m/{message_bytes.decode()}"
    
    return packet.encode('utf-8')

def splitAck(ack_packet):
    try:
        header, body = ack_packet.split(b'm/', 1)  # Split on the first occurrence of 'm/'
        checksum_part = header.split(b'c/', 1)[1]  # Get the part after 'c/'
        checksum = int(checksum_part)  # Convert checksum to integer
        ackNumber = int(body)  # Convert body to integer
        return checksum, ackNumber  # Return as a tuple
    except Exception as e:
#        print(f"Error splitting ACK: {e}")
        return 99999, -1

def isValidAck(checksum, ackNumber):
    message_bytes = str(ackNumber).encode('utf-8')
    return checksum == zlib.crc32(message_bytes)

def main():
    serverName = 'localhost'  # All on the same host
    serverPort = int(sys.argv[1])
    
    clientSocket = socket(AF_INET, SOCK_DGRAM)
    clientSocket.settimeout(0.05)

    currentAckNumber = 1
    sendNum = 0

    while True:
        # Read from standard input buffer
        message = sys.stdin.buffer.read(MAX_PACKET_SIZE - HEADER_SIZE)
        if not message:  # Stop if no more data
            break
        
        sendNum = 1 - sendNum  # Alternate seq number
        
#        print("Sending from Alice seq num: " + str(sendNum) + " message: " + message.decode() + " to port: " + str(serverPort))
        clientSocket.sendto(createPacket(message.decode(), sendNum), (serverName, serverPort))

        while True:  # Loop to receive ack, in case of duplicate/corrupted ack
            try:
                ackPacket, serverAddress = clientSocket.recvfrom(2048)  # Size of ack packet
                checksum, ackNumber = splitAck(ackPacket)
                if isValidAck(checksum, ackNumber) and currentAckNumber == ackNumber:  # Correct ACK received
#                    print("Valid ack received by Alice")
                    currentAckNumber = 1 - ackNumber
                    break
            except timeout:
#                print("Timeout, resending packet")
#                print("Sending from Alice seq num: " + str(sendNum) + " message: " + message.decode())
                clientSocket.sendto(createPacket(message.decode(), sendNum), (serverName, serverPort))

    clientSocket.close()

if __name__ == "__main__":
    main()
