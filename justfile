default:
  @just --list

all:
  just build
  just run

build:
  #! /usr/bin/bash
  cmake -B build -G Ninja
  cmake --build build

clean:
  rm -rf build
  @echo "Build dir removed"

run:
    #!/usr/bin/env bash
    set -e

    echo "Scanning executables in all build directories..."

    selected_path=$(find build/bin -type f -executable 2>/dev/null \
                    | grep -v "CMakeFiles" \
                    | fzf --prompt="Select Target to Run > " \
                          --preview "ls -lh {}" \
                          --header "Finding executables in build/bin/ :")

    if [ -n "$selected_path" ]; then
        echo "------------------------------------------------"
        [[ "$selected_path" == *"build_release"* ]] && echo "[MODE: RELEASE]" || echo "[MODE: DEBUG]"
        echo "Running: $selected_path"
        echo "------------------------------------------------"
        "$selected_path"
    else
        echo "No target selected."
    fi
