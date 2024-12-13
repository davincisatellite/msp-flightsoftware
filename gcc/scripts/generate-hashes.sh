#!/bin/bash

# Check if at least two arguments are provided (header file and input files)
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <output_header_file> <input_file1> [<input_file2> ...]"
    exit 1
fi

# Output header file
output_file="$1"
shift  # Remove the first argument, leaving only the input files

# Start creating the header file
echo "#ifndef GENERATED_HASHES_H_" > "$output_file"
echo "#define GENERATED_HASHES_H_" >> "$output_file"
echo >> "$output_file"

# Process each input file
for file in "$@"; do
    if [ -f "$file" ]; then
        # Compute the SHA1 hash
        hash=$(sha1sum -t "$file" | awk '{print $1}')
        # Extract the file name without the path and handle whitespaces, dots, and extension
        file_name=$(basename "$file" | tr -s ' ' '_' | tr '.' '_' | tr - _ | tr '[:lower:]' '[:upper:]')
        # Add the hash definition to the header file
        echo "#define HASH_$file_name \"$hash\"" >> "$output_file"
    else
        echo "Warning: File '$file' does not exist and will be skipped."
    fi
done

# End the header file
echo >> "$output_file"
echo "#endif // GENERATED_HASHES_H_" >> "$output_file"

