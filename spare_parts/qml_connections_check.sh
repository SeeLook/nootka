#!/bin/bash

# Find all .qml files
find . -type f -name "*.qml" | while read -r file; do
    inside_connections=0
    while IFS= read -r line || [[ -n "$line" ]]; do
        ((line_number++))

        # Detect the start of a Connections block
        if [[ $line =~ "Connections"[[:space:]]*\{ ]]; then
            inside_connections=1
        fi

        # Detect the end of a Connections block
        if [[ $inside_connections -eq 1 && $line =~ \} ]]; then
            inside_connections=0
        fi

        # Check for incorrect signal handlers inside Connections {}
        if [[ $inside_connections -eq 1 && $line =~ ^[[:space:]]*on[A-Z] ]]; then
            echo "Incorrect signal handler in $file at line $line_number: $line"
        fi
    done < "$file"
    line_number=0
done
