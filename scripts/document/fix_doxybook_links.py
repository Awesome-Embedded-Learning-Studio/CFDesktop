#!/usr/bin/env python3
"""
Fix doxybook-generated API doc links for MkDocs.

Doxybook generates links like `Classes/xxx.md` assuming they're relative to
the api/ root, but MkDocs resolves them relative to the file's actual location.
This script adds the correct `../` prefixes.

Usage:
    python scripts/document/fix_doxybook_links.py

Run this AFTER doxybook generates the docs, BEFORE mkdocs build/serve.
"""
import os
import re
import glob

API_DIR = "document/api"

# For each subdirectory, map the link prefix to the correct relative path
DIR_FIXES = {
    "Classes": {
        "Classes/": "./",       # same directory
        "Namespaces/": "../Namespaces/",
        "Files/": "../Files/",
        "Examples/": "../Examples/",
        "Modules/": "../Modules/",
        "Pages/": "../Pages/",
    },
    "Namespaces": {
        "Classes/": "../Classes/",
        "Namespaces/": "./",    # same directory
        "Files/": "../Files/",
        "Examples/": "../Examples/",
        "Modules/": "../Modules/",
        "Pages/": "../Pages/",
    },
    "Files": {
        "Classes/": "../Classes/",
        "Namespaces/": "../Namespaces/",
        "Files/": "./",         # same directory
        "Examples/": "../Examples/",
        "Modules/": "../Modules/",
        "Pages/": "../Pages/",
    },
    "Modules": {
        "Classes/": "../Classes/",
        "Namespaces/": "../Namespaces/",
        "Files/": "../Files/",
        "Examples/": "../Examples/",
        "Modules/": "./",
        "Pages/": "../Pages/",
    },
    "Examples": {
        "Classes/": "../Classes/",
        "Namespaces/": "../Namespaces/",
        "Files/": "../Files/",
        "Examples/": "./",
        "Modules/": "../Modules/",
        "Pages/": "../Pages/",
    },
    "Pages": {
        "Classes/": "../Classes/",
        "Namespaces/": "../Namespaces/",
        "Files/": "../Files/",
        "Examples/": "../Examples/",
        "Modules/": "../Modules/",
        "Pages/": "./",
    },
}

# Pattern to match markdown links: [text](prefix_path)
LINK_PATTERN = re.compile(r'\]([^)]*\b(?:Classes|Namespaces|Files|Examples|Modules|Pages)/)')


def fix_file(filepath, fixes):
    """Apply link fixes to a single file."""
    with open(filepath, 'r') as f:
        content = f.read()

    original = content
    for wrong_prefix, correct_prefix in fixes.items():
        if correct_prefix == "./":
            # Remove the prefix entirely (same directory)
            content = content.replace(f"]({wrong_prefix}", "](")
        else:
            content = content.replace(f"]({wrong_prefix}", f"]({correct_prefix}")

    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        return True
    return False


def fix_index_files():
    """Fix links in api/ root-level index files."""
    index_files = glob.glob(os.path.join(API_DIR, "index_*.md"))
    # These files are at api/ root level, links like Classes/ are correct as-is
    # But check for HandBook/api files that have ../../examples/ links
    return 0


def main():
    total_fixed = 0

    for subdir, fixes in DIR_FIXES.items():
        dir_path = os.path.join(API_DIR, subdir)
        if not os.path.isdir(dir_path):
            continue

        md_files = glob.glob(os.path.join(dir_path, "*.md"))
        fixed_count = 0
        for md_file in md_files:
            if fix_file(md_file, fixes):
                fixed_count += 1

        print(f"  {subdir}/: {fixed_count}/{len(md_files)} files fixed")
        total_fixed += fixed_count

    # Also fix the api root-level index files if needed
    # (these are at api/ level, so Classes/ is correct - no fix needed)
    # But check for Modules/ and Pages/ directories

    print(f"\nTotal: {total_fixed} files fixed")


if __name__ == "__main__":
    main()
