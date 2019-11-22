#!/bin/bash

file="$1"
shift

if [[ ! -f "$file" ]]
then
    echo "$file" not found
    exit 1
fi

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./"$file" "$@"

