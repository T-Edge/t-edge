SUMMARY = "Recipe for  build an external xfilter2dkernel Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

INHIBIT_PACKAGE_STRIP = "1"

SRC_URI += "file://COPYING"
SRC_URI += "file://Makefile"
SRC_URI += "file://coefficients.h"
SRC_URI += "file://xfilter2dkernel_hw.h"
SRC_URI += "file://xfilter2dkernel.c"

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
