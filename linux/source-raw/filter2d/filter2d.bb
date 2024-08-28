#
# This file is the filter2d recipe.
#

SUMMARY = "Simple filter2d application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI += "file://cmdlineparser.cpp"
SRC_URI += "file://cmdlineparser.h"
SRC_URI += "file://coefficients.h"
SRC_URI += "file://common.h"
SRC_URI += "file://filter2d.cpp"
SRC_URI += "file://inputImage50.jpg"
SRC_URI += "file://kernel_hw.cpp"
SRC_URI += "file://kernel_sw.cpp"
SRC_URI += "file://Makefile"

S = "${WORKDIR}"

DEPENDS += "opencv"

inherit pkgconfig

do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 filter2d ${D}${bindir}
	install -d ${D}${base_prefix}/home/root
	install -m 0644 inputImage50.jpg ${D}${base_prefix}/home/root
}

FILES_${PN} += "${base_prefix}/home/root/"
