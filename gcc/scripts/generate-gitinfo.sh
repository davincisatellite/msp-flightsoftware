#!/bin/bash

# Check if exactly one argument is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <output_header_file>"
    exit 1
fi

# Output header file
output_file="$1"

# Ensure the script is being run in a Git repository
if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    echo "Error: This script must be run inside a Git repository."
    exit 1
fi

# Start creating the header file
echo "#ifndef GENERATED_GIT_INFO_H_" > "$output_file"
echo "#define GENERATED_GIT_INFO_H_" >> "$output_file"
echo >> "$output_file"

# Get the current branch name
branch_name=$(git rev-parse --abbrev-ref HEAD)
# Get the latest commit long hash
commit_hash_long=$(git rev-parse HEAD)
# Get the latest commit short hash
commit_hash_short=$(git rev-parse --short HEAD)
# Get the latest commit message
commit_message=$(git log -1 --pretty=%B | tr '\n' ' ' | sed 's/\s\+/ /g')
# Get the repository's remote URL
remote_url=$(git config --get remote.origin.url)

# Add the Git information to the header file
echo "#define GIT_BRANCH \"$branch_name\"" >> "$output_file"
echo "#define GIT_COMMIT_HASH_LONG \"$commit_hash_long\"" >> "$output_file"
echo "#define GIT_COMMIT_HASH_SHORT \"$commit_hash_short\"" >> "$output_file"
echo "#define GIT_COMMIT_MESSAGE \"$commit_message\"" >> "$output_file"
echo "#define GIT_REMOTE_URL \"$remote_url\"" >> "$output_file"

# End the header file
echo >> "$output_file"
echo "#endif // GENERATED_GIT_INFO_H_" >> "$output_file"

# echo "GIT_BRANCH: $branch_name" >> "$output_file"
# echo "GIT_COMMIT_HASH_LONG: $commit_hash_long" >> "$output_file"
# echo "GIT_COMMIT_HASH_SHORT: $commit_hash_short" >> "$output_file"
# export GIT_COMMIT_HASH_SHORT="$commit_hash_short"
# echo "GIT_COMMIT_MESSAGE: $commit_message" >> "$output_file"
# echo "GIT_REMOTE_URL: $remote_url" >> "$output_file"