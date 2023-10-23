#!/bin/bash
# Copyright (c) 2022 The Honeycomb Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file.

if ! command -v doxygen &> /dev/null
then
  echo "ERROR: Please install Doxygen" 1>&2
  exit 1
fi

# Determine the absolute path to the script directory.
SCRIPT_DIR="$( cd "$( dirname "$0" )" && pwd )"

# Determine the top-level Honeycomb directory.
HONEYCOMB_DIR="${SCRIPT_DIR}/.."

# Environment variables inserted into the Doxyfile via `$(VAR_NAME)` syntax.
export PROJECT_NUMBER=$(python3 ${SCRIPT_DIR}/honey_version.py current)

# Generate documentation in the docs/html directory.
# Run from the top-level Honeycomb directory so that relative paths resolve correctly.
( cd ${HONEYCOMB_DIR} && doxygen Doxyfile )

# Write a docs/index.html file.
echo "<html><head><meta http-equiv=\"refresh\" content=\"0;URL='html/index.html'\"/></head></html>" > ${HONEYCOMB_DIR}/docs/index.html

