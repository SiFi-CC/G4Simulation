#!/bin/bash

find . \( -iname "*.h" -or -iname "*.hh" -or -iname "*.cc" -or -iname "*.C" -or -iname "*.cpp" \) -exec clang-format --style=file -i {} \;
