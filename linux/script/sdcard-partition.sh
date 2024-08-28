#!/bin/bash
# set -x

DISK=$1

if [ -z ${DISK} ]; then
    echo "Usage: $0 <disk>"
    echo "Example: $0 /dev/sdX"
    exit 1
fi

set -e

# Ensure the size of the disk is smaller than 16GB
SIZE=$(blockdev --getsize64 ${DISK})
if [ $SIZE -gt 17179869184 ]; then
    echo "Disk size is larger than 16GB."
    echo "This limitation is to avoid overwriting the wrong disk."
    echo "Please modify the script to continue."
    exit
fi

# Unmount partitions if the disk is mounted
if (mount | grep ${DISK}); then
    umount $(cat /proc/mounts | grep ${DISK} | awk '{ print $2 }')
fi

# Delete partitions using wipefs
wipefs --all ${DISK}

# Create two partitions using parted
#  1. 512 MiB FAT32
#  2. Remainder Linux
parted -s ${DISK} \
    mklabel msdos \
    mkpart primary fat32 1MiB 513MiB \
    mkpart primary ext2 513MiB 100%

fdisk -l ${DISK}

# Format partitions
mkfs.vfat -F 32 ${DISK}1
mkfs.ext2 ${DISK}2
