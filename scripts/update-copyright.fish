#!/usr/bin/env fish
# Update year of copyright info in license header to source files in current directory
# 
for file in *.cpp *.h
    if test -f "$file"
        # Check if file contains the pattern before modifying
        if grep -q 'Copyright (c) 2025 ' "$file"
            echo "Updating: $file"
            # Use sed to replace the pattern
            sed -i 's/Copyright (c) 2025 /Copyright (c) 2025-2026 /g' "$file"
        end
    end
end
