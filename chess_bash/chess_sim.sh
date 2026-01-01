#!/usr/bin/env bash

init_board () {
    # Initialize the chess board
    # 8x8 array representing the chess board
    board=(
        "r n b q k b n r"
        "p p p p p p p p"
        ". . . . . . . ."
        ". . . . . . . ."
        ". . . . . . . ."
        ". . . . . . . ."
        "P P P P P P P P"
        "R N B Q K B N R"
    )
}

print_board () {
    echo "  a b c d e f g h"
    for i in {0..7}; do
        printf "%d %s %d\n" $((8 - i)) "${board[$i]}" $((8 - i))
    done
    echo "  a b c d e f g h"
}

# Function to parse moves from PGN file
parse_moves() {
    local pgn_file="$1"

    local script_dir
    script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

    parsed_moves=$(python3 "$script_dir/parse_moves.py" "$pgn_file")
    IFS=' ' read -r -a moves_array <<< "$parsed_moves"
}

# Function to convert a character to a number (a=0, b=1, ..., h=7)
char_to_num() {
    echo $(( $(printf "%d" "'$1'") - 97 ))
}

# Function to get an item from the board
get_item () {
    row=$1
    col=$2
    clean_line=$(echo "${board[$row]}" | tr -s ' ')
    IFS=' ' read -r -a row_array <<< "$clean_line"
    echo "${row_array[$col]}"
}

# Function to set an item on the board
set_item () {
    local row=$1
    local col=$2
    local item=$3
    clean_line=$(echo "${board[$row]}" | tr -s ' ')
    IFS=' ' read -r -a row_array <<< "$clean_line"
    row_array[$col]="$item"
    board[$row]="${row_array[*]}"
}

# Function to move an item on the board
move_item () {
    local move=$1
    # from is the first two characters, to is the next two characters, promotion is the last character
    local from="${move:0:2}"
    local to="${move:2:2}"
    local promotion="${move:4:1}"

    # Convert from and to to row and column indices
    from_col=$(char_to_num "${from:0:1}")
    from_row=$((8 - ${from:1:1}))
    to_col=$(char_to_num "${to:0:1}")
    to_row=$((8 - ${to:1:1}))

    # get the item at the from position
    item=$(get_item "$from_row" "$from_col")

    # en passant case
    if [[ "$item" == 'P' || "$item" == 'p' ]]; then
        if [[ "$from_row" != "$to_row" && "$from_col" != "$to_col" ]]; then
            temp_item=$(get_item "$to_row" "$to_col")
            if [[ "$temp_item" == "." ]]; then
                set_item "$from_row" "$to_col" "."
            fi
        fi
    fi

    # short white casteling
    if [[ "$item" == "K" && "$from" == "e1" && "$to" == "g1" ]]; then
        set_item 7 7 "."
        set_item 7 5 "R"
    # long white casteling
    elif [[ "$item" == "K" && "$from" == "e1" && "$to" == "c1" ]]; then
        set_item 7 0 "."
        set_item 7 3 "R"
    # short black casteling
    elif [[ "$item" == "k" && "$from" == "e8" && "$to" == "g8" ]]; then
        set_item 0 7 "."
        set_item 0 5 "r"
    # long black casteling
    elif [[ "$item" == "k" && "$from" == "e8" && "$to" == "c8" ]]; then
        set_item 0 0 "."
        set_item 0 3 "r"
    fi

    if [ -n "$promotion" ]; then
        if [ "$item" == "P" ]; then
            item="Q"
        elif [ "$item" == "p" ]; then
            item="q"
        fi
    fi

    # set the item at the to position
    set_item "$from_row" "$from_col" "."
    set_item "$to_row" "$to_col" "$item"
}

# Function to make moves on the board
make_moves () {
    init_board
    for ((i = 0; i < $1; i++)); do
        move_item "${moves_array[$i]}"
    done
}

# Main function
# Check if the script is run with a PGN file as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input.pgn>"
    exit 1
fi

input_file="$1"
if [ ! -f "$input_file" ]; then
    echo "File does not exist: $input_file"
    exit 1
fi

# print the metadata from the PGN file
printf "Metadata from PGN file:\n"
head -n 11 "$input_file" | sed '/^$/d'

# parse the moves from the PGN file
parse_moves "$input_file"
num_of_moves=${#moves_array[@]}
current_move=0
pre_message=True

# loop to display the board and handle user input
while true; do
    if [ "$pre_message" = True ]; then
        echo "Move $current_move/$num_of_moves"
        make_moves "$current_move"
        print_board
    fi

    pre_message=True

    # Prompt the user for input
    printf "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit: "
    IFS= read -r key

    case "$key" in
        # case d to move forward if there are moves left
        d)
            if [ "$current_move" -lt "$num_of_moves" ]; then
                ((current_move++))
                pre_message=True
            else
                echo "No more moves available."
                pre_message=False
            fi
            ;;
        # case a to move back if there are moves left
        a)
            if [ "$current_move" -gt 0 ]; then
                ((current_move--))
            fi
            ;;
        # case w to go to the start
        w)
            current_move=0
            ;;
        # case s to go to the end
        s)
            current_move=$num_of_moves
            ;;
        # case q to quit
        q)
            echo "Exiting."
            echo "End of game."
            break
            ;;
        # case * to handle invalid input
        *)
            echo "Invalid key pressed: $key"
            pre_message=False
            ;;
    esac
done
