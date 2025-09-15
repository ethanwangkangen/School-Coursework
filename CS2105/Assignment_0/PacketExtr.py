import sys

# If there is a whole packet in buffer, return packet, buffer
# else return None, buffer
def extract_packetData(buffer):
    # Buffer will look like: "Size: <size>B"XXXXXXX etc.
    # Think as a queue, input from the right and output out the left.
    
    header_end = buffer.find(b'B')
    # find() gives the index of the first occurrence (from the left)
        
    header_space = buffer.find (b' ')
    
    if header_end == -1 or header_space == -1:
        return None, buffer
    
    numBytes = int(buffer[header_space + 1 : header_end])
    # num of bytes the packet is supposed to have
    
    remainderBytes = (buffer[header_end + 1 :]) 
    # Everything after the header, may or may not contain entire packet.
    
    if len(remainderBytes) < numBytes: # Does not contain the entire packet yet
        return None, buffer
    else: 
        packet = (remainderBytes[:numBytes])
        return packet, remainderBytes[numBytes:]
        

def main():
    buffer = (b'') # Empty buffer. Holds bytes (class type)
    while True:
        # Read at most 5 bytes from stdin
        data = sys.stdin.buffer.read1(2000) # data is of bytes class
        
        if not data:  # If data is empty, EOF is reached
            break
        
        # Push into buffer
        buffer += (data)
        
        while True:
            # Check buffer for a full packet
            # If packet found in buffer, send the packet to stdout and remove from buffer
            
            packetData, bufferNew = extract_packetData(buffer)
            if packetData is not None:     
            # Must specify not None!!! Because packetData could be '' and I still want to update buffer.
                sys.stdout.buffer.write(packetData)
                sys.stdout.buffer.flush()
                buffer = bufferNew # Update the buffer    
            else: # No packet found, continue with the reading
                break
        if not buffer and not data:
            return
    
if __name__ == "__main__":
    main()
