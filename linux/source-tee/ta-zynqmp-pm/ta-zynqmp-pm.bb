#
# This file is the ta-zynqmp-pm recipe.
#

SUMMARY = "Simple ta-zynqmp-pm application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

COMPATIBLE_MACHINE_zynqmp = "zynqmp-generic|ultra96-zynqmp"

DEPENDS = "optee-client optee-os python3-cryptography-native"
inherit python3native
require recipes-security/optee/optee.inc

SRC_URI += "file://host/main.c"
SRC_URI += "file://host/Makefile"
SRC_URI += "file://Makefile"
SRC_URI += "file://ta/sub.mk"
SRC_URI += "file://ta/include/fpga_load_ta.h"
SRC_URI += "file://ta/fpga_load_ta.c"
SRC_URI += "file://ta/Makefile"
SRC_URI += "file://ta/user_ta_header_defines.h"

EXTRA_OEMAKE += "TA_DEV_KIT_DIR=${TA_DEV_KIT_DIR}"
EXTRA_OEMAKE += "HOST_CROSS_COMPILE=${HOST_PREFIX}"
EXTRA_OEMAKE += "TA_CROSS_COMPILE=${HOST_PREFIX}"
EXTRA_OEMAKE += "OUTPUT_DIR=${B}"

S = "${WORKDIR}"

do_compile() {
	oe_runmake -C ${S}
}

do_install() {
    install -d ${D}${bindir}
    install -p -m 0755 ${B}/host/optee_fpga_load ${D}${bindir}
    install -d ${D}${nonarch_base_libdir}/optee_armtz
    install -p -m 0444 ${B}/ta/*.ta ${D}${nonarch_base_libdir}/optee_armtz
}

FILES_${PN} += "${nonarch_base_libdir}/optee_armtz/"

# Imports machine specific configs from staging to build
PACKAGE_ARCH = "${MACHINE_ARCH}"
