#!/bin/bash

function error_handler() {
  echo "Error occurred in script at line: ${1}."
  echo "Line exited with status: ${2}"
}

trap 'error_handler ${LINENO} $?' ERR

set -o errexit
set -o errtrace
set -o nounset

grub-file --is-x86-multiboot ./build/boot

echo "Sanity checks completed successfully"
