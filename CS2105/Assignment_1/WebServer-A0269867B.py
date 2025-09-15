import socket
import os
import sys

TCP_IP = "127.0.0.1"
BUFFER_SIZE = 1024

class StorageObj(object):
    def __init__(self):
        self.dict = {}
        self.counts = {}  # subset of self.dict, contains temporary ones
    
    def exists(self, key):
        return key in self.dict
    
    def is_temp(self, key):
        return key in self.counts and self.counts[key] > 0 
    
    def decrement(self, key):
        self.counts[key] -= 1
        print("decrementing key " + key + "to " + str(self.counts[key]))
        if self.counts[key] <= 0:
            self.delete(key)
        
    
    def get_value(self, key):
        return self.dict[key]
    
    def get_count(self, key):
        if key in self.counts:
            return self.counts[key]
        else:
            return 'Infinity'
    
    def add(self, key, value):
        self.dict[key] = value

    def update_counter(self, key, num):
        if key not in self.counts:
            self.counts[key] = num
            print("setting count to " + str(num))
        else:
            self.counts[key] += num
            print("adding " + str(num) + "to count")

    def delete(self, key):
        print("deleting key" + key)
        if key in self.dict:
            del self.dict[key]
        if key in self.counts:
            del self.counts[key]

    def deleteCount(self, key):
        if key in self.counts:
            del self.counts[key]


class RequestObj(object):
    def __init__(self, header):
        self.header = header
        self.type = None
        self.getting_key = False
        self.header_string = header.decode()  # Convert bytes to string
        self.post_content = None

        if b'GET' in header.upper():
            self.type = "GET"
        elif b'POST' in header.upper():
            self.type = "POST"
        elif b'DELETE' in header.upper():
            self.type = "DELETE"
        else:
            self.type = None

        if b'/key/' in header:
            self.getting_key = True
        elif b'/counter' in header:
            self.getting_key = False

        parts = self.header_string.split()  # Split by whitespace
        if len(parts) > 1:
            self.item = parts[1].split('/')[2]  # Extract item from path, e.g. 2105
        else:
            self.item = None

    def get_type(self):
        return self.type

    def is_getting_key(self):
        return self.getting_key
    
    def get_content_length(self):
        # Return content length for POST command, else -1
        if self.type != "POST":
            return -1
        else:
            parts = self.header_string.split()
            lowercase_parts = [part.lower() for part in parts]

            while 'content-length' in lowercase_parts:
                length_index = lowercase_parts.index('content-length')
                try:
                    ans = int(parts[length_index + 1])
                    print("content length in post is " + str(ans))
                    return ans
                except (ValueError, IndexError):
                    lowercase_parts[length_index] = None
            return -1

    def get_item(self):
        return self.item
    
    def add_post_content(self, content):
        self.post_content = content

    def get_post_content(self):
        return self.post_content


class MyServer(object):
    def __init__(self, port):
        self.port = port
        self.storage = StorageObj()
        self.conn = None

    def start(self):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((TCP_IP, self.port))
        self.socket.listen()  # This is a blocking call

        while True:
            try:
                conn, address = self.socket.accept()
                self.conn = conn
                self.handle_connection(conn)
            except (KeyboardInterrupt, SystemExit):
                break

        self.socket.shutdown(socket.SHUT_RDWR)
        self.socket.close()
    
    def handle_connection(self, conn):
        buffer = b''  # Buffer for incoming data
        while True:
            data = conn.recv(BUFFER_SIZE)
            if not data:
                break
            buffer += data
            while True:
                
                buffer, request = self.parse_request(buffer)
                if request is None:
                    break
                self.handle_request(request)

    def parse_request(self, buffer):
        # Look for the end of headers (denoted by "  ")
        header_end_idx = buffer.find(b'  ')
        if header_end_idx == -1:
            return buffer, None  # Header not yet fully received

        header_content = buffer[:header_end_idx]
        request = RequestObj(header_content)

        # For POST, handle content-length and body
        if request.get_type() == "POST":
            length = request.get_content_length()
            if len(buffer) < header_end_idx + 2 + length:
                return buffer, None  # Full body not received yet
            request.add_post_content(buffer[header_end_idx + 2:header_end_idx + 2 + length])
            new_buffer = buffer[header_end_idx + 2 + length:]
        else:
            new_buffer = buffer[header_end_idx + 2:]
        return new_buffer, request
    
    def handle_request(self, request):
        if request.get_type() == "GET":
            self.handle_get(request.is_getting_key(), request.get_item())
        elif request.get_type() == "POST":
            self.handle_post(request.is_getting_key(), request.get_item(), request.get_post_content())
        elif request.get_type() == "DELETE":
            self.handle_delete(request.is_getting_key(), request.get_item())
            
    def handle_get(self, is_key, item):
        print(self.storage.exists(item))
        if is_key:
            if not self.storage.exists(item):
                print("get, key, not exist")
                self.respond(404, None)
                return
            if self.storage.is_temp(item):
                self.respond(200, self.storage.get_value(item))
                self.storage.decrement(item)
            else:    
                self.respond(200, self.storage.get_value(item))
            return
        if not self.storage.exists(item):
            self.respond(404, None)
        else:
            count = self.storage.get_count(item)
            self.respond(200, count)

    def handle_post(self, is_key, item, post_content):
        if is_key:
            if not self.storage.exists(item):
                self.storage.add(item, post_content)
                self.respond(200, None)
            else:
                if self.storage.is_temp(item):
                    self.respond(405, None)
                else:
                    self.storage.add(item, post_content)
                    self.respond(200, None)
        else:
            if not self.storage.exists(item):
                self.respond(405, None)
            else:
                count = int(post_content.decode('utf-8'))
                self.storage.update_counter(item, count)
                self.respond(200, None)

    def handle_delete(self, is_key, item):
        if is_key:
            if not self.storage.exists(item):
                self.respond(404, None)
            elif self.storage.is_temp(item):
                self.respond(405, None)
            else:
                value = self.storage.get_value(item)
                self.storage.delete(item)
                self.respond(200, value)
        else:
            if not self.storage.exists(item):
                self.respond(404, None)
            else:
                count = self.storage.get_count(item)
                self.storage.deleteCount(item)
                self.respond(200, count)

    def respond(self, code, item):
        print("code is " + str(code) + " and item is " + str(item))
        response = ""
        if code == 404:
            response += '404 NotFound'
        elif code == 405:
            response += '405 MethodNotAllowed'
        elif code == 200:
            response += '200 OK'

        if item is not None:  # Make sure item is not None
            if isinstance(item, bytes):
                item_str = item
            else:
                item_str = str(item).encode()
            response += f" content-length {len(item_str)}  "
            response = response.encode() + item_str  # Combine response and item bytes
        else:
            response = response.encode() + b'  '

        print(response)
        self.conn.sendall(response)        
if __name__ == "__main__":
    port = int(sys.argv[1])
    server = MyServer(port)
    server.start()
