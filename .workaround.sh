#!/bin/bash

for binutils in $XILINX_VIVADO/tps/lnx64/binutils-*; do
    [ -f $binutils/bin/ld ] && mv $binutils/bin/ld{,.bak}
done
