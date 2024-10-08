SUMMARY = "OP-TEE Trusted OS"
DESCRIPTION = "Open Portable Trusted Execution Environment - Trusted side of the TEE"
HOMEPAGE = "https://www.op-tee.org/"

LICENSE = "BSD-2-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=c1f21c4f72f372ef38a5a4aee55ec173"

inherit deploy python3native
require optee.inc

DEPENDS = "python3-cryptography-native python3-pyelftools-native"

DEPENDS_append_toolchain-clang = " compiler-rt"

SRC_URI = "git://github.com/OP-TEE/optee_os.git;protocol=https"

SRC_URI_append = "                                              \
    file://0006-allow-setting-sysroot-for-libgcc-lookup.patch   \
    file://0007-allow-setting-sysroot-for-clang.patch           \
"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

EXTRA_OEMAKE += " \
    PLATFORM=${OPTEEMACHINE} \
    CFG_${OPTEE_CORE}_core=y \
    CROSS_COMPILE_core=${HOST_PREFIX} \
    CROSS_COMPILE_ta_${OPTEE_ARCH}=${HOST_PREFIX} \
    NOWERROR=1 \
    ta-targets=ta_${OPTEE_ARCH} \
    O=${B} \
"

CFLAGS[unexport] = "1"
LDFLAGS[unexport] = "1"
CPPFLAGS[unexport] = "1"
AS[unexport] = "1"
LD[unexport] = "1"

do_configure[noexec] = "1"

do_compile() {
    oe_runmake -C ${S} all
}
do_compile[cleandirs] = "${B}"

do_install() {
    #install core in firmware
    install -d ${D}${nonarch_base_libdir}/firmware/
    install -m 644 ${B}/core/*.bin ${D}${nonarch_base_libdir}/firmware/

    #install TA devkit
    install -d ${D}${includedir}/optee/export-user_ta/
    for f in ${B}/export-ta_${OPTEE_ARCH}/* ; do
        cp -aR $f ${D}${includedir}/optee/export-user_ta/
    done
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

do_deploy() {
    install -d ${DEPLOYDIR}/optee
    install -m 644 ${D}${nonarch_base_libdir}/firmware/* ${DEPLOYDIR}/optee/
}

addtask deploy before do_build after do_install

SYSROOT_DIRS += "${nonarch_base_libdir}/firmware"

FILES_${PN} = "${nonarch_base_libdir}/firmware/"
FILES_${PN}-dev = "${includedir}/optee/"

INSANE_SKIP_${PN}-dev = "staticdev"

INHIBIT_PACKAGE_STRIP = "1"
