#!/bin/bash

set -ex

time petalinux-build

if [ -f images/linux/tee_raw.bin ]; then
    time petalinux-package \
        --boot \
        --pmufw \
        --fpga \
        --u-boot \
        --add images/linux/tee_raw.bin \
        --cpu a53-0 \
        --file-attribute "load=0x60000000, startup=0x60000000, exception_level=el-1, trustzone" \
        --force
else
    time petalinux-package --boot --u-boot --force
fi

date -Is
