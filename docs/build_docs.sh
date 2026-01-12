#!/usr/bin/env bash
#
# Copyright (c) 2025 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
# Script to build Zigbee documentation locally.
# This script creates a Python virtual environment, installs dependencies,
# optionally generates Doxygen API documentation, and builds Sphinx documentation.
#
# Usage:
#   ./build_docs.sh           # Build Sphinx docs only
#   ./build_docs.sh --doxygen # Build with Doxygen API docs
#   ./build_docs.sh --run     # Open in browser
#
# The output is generated in _build_sphinx/html/

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_DOXYGEN=false
RUN_BROWSER=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --doxygen)
            BUILD_DOXYGEN=true
            shift
            ;;
        --run)
            RUN_BROWSER=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Build Zigbee R23 add-on documentation locally."
            echo ""
            echo "Options:"
            echo "  --doxygen    Generate Doxygen API documentation before building Sphinx docs"
            echo "  --run        Open the documentation in browser"
            echo "  -h, --help   Show this help message"
            echo ""
            echo "Output: _build_sphinx/html/"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Open in browser if requested
if [ "$RUN_BROWSER" = true ]; then
    echo "Opening documentation in browser..."
    xdg-open "$SCRIPT_DIR/_build_sphinx/html/index.html"
    exit 0
fi


echo "=== Setting up Python virtual environment ==="
python3 -m venv venv
source ./venv/bin/activate

echo "=== Installing documentation dependencies ==="
pip install --upgrade pip
pip install -r "$SCRIPT_DIR/requirements-doc.txt"

# Build Doxygen if requested and Doxyfile exists
if [ "$BUILD_DOXYGEN" = true ] && [ -f "Doxyfile" ]; then
    echo "=== Generating Doxygen API documentation ==="
    if command -v doxygen &> /dev/null; then
        doxygen Doxyfile
    else
        echo "Warning: doxygen is not installed. Skipping API documentation generation."
        echo "Install doxygen with: sudo apt-get install doxygen"
    fi
fi

echo "=== Building Sphinx documentation ==="
sphinx-build -M html . _build_sphinx

echo ""
echo "=== Documentation built successfully! ==="
