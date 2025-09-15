import zlib
import sys

def calculate_checksum(file_path):
    with open(file_path, "rb") as f:
        bytes = f.read()
    return zlib.crc32(bytes)


def main():
    file_path = sys.argv[1]
    checksum = calculate_checksum(file_path)
    print(checksum)

if __name__ == "__main__":
    main()



