import os

def write_cpp_files_contents_to_text(folders, output_file):
    """
    This function writes the contents of all C++ files in the specified list of folders
    to a single text file.

    :param folders: List of paths to folders containing C++ files
    :param output_file: Path to the output text file
    """
    with open(output_file, 'w') as outfile:
        for folder_path in folders:
            for root, _, files in os.walk(folder_path):
                for file in files:
                    if file.endswith('.cpp') or file.endswith('.hpp'):
                        file_path = os.path.join(root, file)
                        with open(file_path, 'r') as infile:
                            outfile.write(f"// File: {file_path}\n")
                            outfile.write("/*\n")
                            outfile.write(infile.read())
                            outfile.write("\n*/\n\n\n\n\n")  # Add some space between file contents

if __name__ == "__main__":
    # List of folders to process
    folders = ["src", "include"]
    output_file = "prompt.txt"
    write_cpp_files_contents_to_text(folders, output_file)
    print(f"Contents of C++ files in the specified folders written to '{output_file}'")
