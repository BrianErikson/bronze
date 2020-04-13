#!/bin/bash

function error_handler() {
  echo "Error occurred in script at line: ${1}."
  echo "Line exited with status: ${2}"
}

trap 'error_handler ${LINENO} $?' ERR

set -o errexit
set -o errtrace
set -o nounset

mkdir -p build/iso/boot/grub
rsync -chav build/bronze build/iso/boot/bronze
rsync -chav grub.cfg build/iso/boot/grub/grub.cfg
grub-mkrescue -o build/bronze.iso build/iso
