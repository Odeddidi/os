#!/usr/bin/env bash

# check if there are exactly two arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input.pgn> <output_dir>"
    exit 1
fi

input_file="$1"
dest_dir="$2"

# check if the input file exists
if [ ! -f "$input_file" ]; then
   echo "Error: File '$input_file' does not exist."
   exit 1
fi

# check if the dest directory exists, if not create it
if [ ! -d "$dest_dir" ]; then
    mkdir -p "$dest_dir"
    echo "Created directory $dest_dir"
fi

game_number=0
current_file=""

# Read the input file line by line
# and split the games into separate files
while IFS= read -r line || [[ -n "$line" ]]; do
    if [[ "$line" == *'[Event "'* ]]; then
        game_number=$((game_number + 1))

        current_file="${dest_dir}/$(basename "$input_file" .pgn)_${game_number}.pgn"
        echo "Saved game to ${dest_dir}/$(basename "$input_file" .pgn)_${game_number}.pgn"
        echo "$line" > "$current_file"
    else
        if [ -n "$current_file" ]; then
            echo "$line" >> "$current_file"
        fi
    fi
done < "$input_file"

echo "All games have been split and saved to '$dest_dir'."
