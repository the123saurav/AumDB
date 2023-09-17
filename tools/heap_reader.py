import os
import struct

# Constants for the heap file
HEADER_SIZE = 4096
MAGIC_NUMBER_SIZE = 4
VERSION_OFFSET = 4
PAGE_SIZE_OFFSET = 5
PAGE_COUNT_OFFSET = 7

def read_heap_file(filepath):
    with open(filepath, 'rb') as f:
        # Read and validate the magic number
        magic_number = f.read(MAGIC_NUMBER_SIZE)
        print(f"Magic Number: {magic_number.decode('utf-8')}")
        
        # Read and print the version
        f.seek(VERSION_OFFSET)
        version = struct.unpack('B', f.read(1))[0]
        print(f"Version: {version}")
        
        # Read and print the page size
        f.seek(PAGE_SIZE_OFFSET)
        page_size = struct.unpack('<H', f.read(2))[0]  # Assuming little endian, 2-byte integer
        print(f"Page Size: {page_size}")
        
        # Read and print the page count
        f.seek(PAGE_COUNT_OFFSET)
        page_count = struct.unpack('<H', f.read(2))[0]  # Assuming little endian, 2-byte integer
        print(f"Page Count: {page_count}")
        
        # Optionally, you can read and print the rest of the header or any other information
        
def main():
    # dir_path = input("Enter directory path: ")
    dir_path = "/tmp/AumDb"
    table_name = "users"
    # table_name = input("Enter table name: ")
    filepath = os.path.join(dir_path, table_name, "heap")
    read_heap_file(filepath)

if __name__ == "__main__":
    main()
