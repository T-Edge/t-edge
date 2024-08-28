#!/bin/bash
# set -x

PROJECT=$(pwd)
IMAGEDIR=$PROJECT/images/linux

[ -d $IMAGEDIR ] || (echo "$IMAGEDIR doesn't exist" && exit 1)

DISK=$1

if [ -z ${DISK} ]; then
    echo "Usage: $0 <disk>"
    echo "Example: $0 /dev/sdX"
    exit 1
fi

set -e

echo "************************************************************"
echo "*** Unmounting partitions ..."

# Unmount partitions if the disk is mounted
if (mount | grep ${DISK}); then
    umount $(cat /proc/mounts | grep ${DISK} | awk '{ print $2 }')
fi

echo "************************************************************"
echo "*** Checking filesystem ..."

# check filesystem
fsck -a ${DISK}1 || echo "fsck ${DISK}1 returned $?"
fsck -a ${DISK}2 || echo "fsck ${DISK}2 returned $?"

echo "************************************************************"
echo "*** Writing image to disk ..."

# prepare workdir
WORKDIR=$(realpath $(mktemp -d -p . -t .mount.XXXX))
pushd $WORKDIR

# update boot partition
echo "*** Writing boot partition ..."
mkdir boot
mount ${DISK}1 boot
cd boot
rm -f *
cp $IMAGEDIR/{BOOT.BIN,image.ub,boot.scr} .
cd $WORKDIR

# update rootfs partition
echo "*** Writing rootfs partition ..."

mkdir -p /tmp/rootfs
tar xf $IMAGEDIR/rootfs.tar.gz -C /tmp/rootfs

mkdir rootfs
mount ${DISK}2 rootfs
rsync -a --del --info=stats2 /tmp/rootfs/ rootfs/
cp $IMAGEDIR/system.bit rootfs/home/root

# sync to disk
echo "*** Syncing to disk ..."
time sync

popd

echo "************************************************************"
echo "*** Unmounting partitions ..."

# unmount partitions
while (mount | grep ${DISK}); do
    echo "Unmounting partitions ..."
    umount $(cat /proc/mounts | grep ${DISK} | awk '{ print $2 }')
done

echo "************************************************************"
echo "*** Cleaning up ..."

rm -r $WORKDIR

set +e

# check filesystem
fsck -a ${DISK}1
fsck -a ${DISK}2

date -Is

(mount | grep ${DISK}) && echo "${DISK} is not unmounted" && exit 1
