#!/bin/bash
# Find all .h and .cpp files in the current directory and its subdirectories

files=$(find . -type f \( -name "*.hpp" -o -name "*.cpp" \))

# Run clang-format on each file
for file in $files; do
    clang-format -i "$file"
done

echo "Code formatting completed."

