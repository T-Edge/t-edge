#!/bin/bash

PROJECT_NAME="system-conv-tee"
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
CONFIG_strace=y
CONFIG_vim=y
EOF

if [ ! -f project-spec/hw-description/system.xsa ]; then
    petalinux-config --get-hw-description $PLATFORM_XSA --silentconfig
else
    echo "Hardware description already exists"
    petalinux-config --silentconfig
fi

if [ ! -e project-spec/meta-user/recipes-bsp/embeddedsw ]; then
    echo "Creating embeddedsw"
    ln -s -r ../source-tee/embeddedsw project-spec/meta-user/recipes-bsp
else
    echo "embeddedsw already exists"
fi

if [ ! -e project-spec/meta-user/recipes-security/optee ]; then
    echo "Creating optee"
    mkdir -p project-spec/meta-user/recipes-security
    ln -s -r ../source-tee/optee project-spec/meta-user/recipes-security/optee
    ln -s -r -f ../source-tee/system-user.dtsi project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
    echo 'IMAGE_INSTALL_append += "optee-os optee-client optee-test optee-examples"' >>project-spec/meta-user/conf/petalinuxbsp.conf
else
    echo "optee already exists"
fi

if [ ! -f project-spec/meta-user/recipes-apps/ta-zynqmp-pm/ta-zynqmp-pm.bb ]; then
    petalinux-create -t apps -n ta-zynqmp-pm --enable
    rm -rf project-spec/meta-user/recipes-apps/ta-zynqmp-pm
    ln -s -r ../source-tee/ta-zynqmp-pm project-spec/meta-user/recipes-apps/ta-zynqmp-pm
else
    echo "filter2d already exists"
fi

if $(grep -q "CONFIG_OPTEE=y" project-spec/meta-user/recipes-kernel/linux/linux-xlnx/bsp.cfg); then
    echo "CONFIG_OPTEE already set"
else
    cat ../source-tee/enable_optee.cfg >>project-spec/meta-user/recipes-kernel/linux/linux-xlnx/bsp.cfg
fi

if [ ! -f project-spec/meta-user/recipes-bsp/arm-trusted-firmware/arm-trusted-firmware_%.bbappend ]; then
    mkdir -p project-spec/meta-user/recipes-bsp/arm-trusted-firmware
    cat <<EOF >>project-spec/meta-user/recipes-bsp/arm-trusted-firmware/arm-trusted-firmware_%.bbappend
EXTRA_OEMAKE_append = " LOG_LEVEL=LOG_LEVEL_INFO"
EXTRA_OEMAKE_append = " NEED_BL32=yes"
EXTRA_OEMAKE_append = " SPD=opteed"
EOF
else
    echo "arm-trusted-firmware_%.bbappend already exists"
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
