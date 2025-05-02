#!/usr/bin/env fish
# Creates table of list of sources, shall be called in source directory
# 
ls *.cpp | awk '{print}' ORS="',\n '"
