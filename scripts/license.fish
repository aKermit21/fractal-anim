#!/usr/bin/env fish
# Add license header to source files in current directory
# 
for file in *.cpp *.h
    # echo $file
    # Not needed actually
    touch {$file}.lic
    cat ../scripts/license-header.txt >{$file}.lic
    cat $file >>{$file}.lic
    mv {$file}.lic $file
end
