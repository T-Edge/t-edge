#!/bin/bash

PROJECT_NAME="system-conv-raw"
PLATFORM_XSA=$(realpath ../fpga/conv_filter.xsa)

if [ -z "$PETALINUX_VER" ]; then
    echo "PETALINUX_VER environment variable not set"
    exit 1
fi

if [ ! -d $PROJECT_NAME ]; then
    echo "Creating project $PROJECT_NAME"
    petalinux-create -t project -n $PROJECT_NAME --template zynqMP
fi

python3 $(dirname $0)/plnx-set-cache.py -p $PROJECT_NAME

cd $PROJECT_NAME

cat <<EOF >>"project-spec/configs/config"
CONFIG_SUBSYSTEM_ETHERNET_PSU_ETHERNET_3_MAC="0E:B1:9D:45:5C:09"
CONFIG_SUBSYSTEM_INITRAMFS_IMAGE_NAME="petalinux-initramfs-image"
CONFIG_SUBSYSTEM_MACHINE_NAME="zcu106-reva"
EOF

cat <<EOF >>"project-spec/configs/rootfs_config"
CONFIG_bash-completion=y
CONFIG_bash=y
CONFIG_glibc=y
CONFIG_opencv-dev=y
CONFIG_opencv=y
CONFIG_packagegroup-core-buildessential=y
CONFIG_rsync=y
CONFIG_vim=y
EOF

if [ ! -f project-spec/hw-description/system.xsa ]; then
    petalinux-config --get-hw-description $PLATFORM_XSA --silentconfig
else
    echo "Hardware description already exists"
    petalinux-config --silentconfig
fi

if [ ! -e project-spec/meta-user/recipes-bsp/embeddedsw ]; then
    ln -s ../source-raw/embeddedsw project-spec/meta-user/recipes-bsp
else
    echo "embeddedsw already exists"
fi

if [ ! -f project-spec/meta-user/recipes-modules/xfilter2dkernel/xfilter2dkernel.bb ]; then
    petalinux-create -t modules -n xfilter2dkernel --enable

    pushd project-spec/meta-user/recipes-modules/
    rm -rf xfilter2dkernel
    ln -s ../../../../source-raw/xfilter2dkernel xfilter2dkernel
    popd
else
    echo "xfilter2dkernel already exists"
fi

if [ ! -f project-spec/meta-user/recipes-apps/filter2d/filter2d.bb ]; then
    petalinux-create -t apps -n filter2d --enable

    pushd project-spec/meta-user/recipes-apps
    rm -rf filter2d
    ln -s ../../../../source-raw/filter2d filter2d
    popd
else
    echo "filter2d already exists"
fi
