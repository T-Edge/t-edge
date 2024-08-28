SRC_URI_append += "file://0001-improve-perftime-measurement.patch"
SRC_URI_append += "file://0002-add-fsbl-hook.patch"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

# Add debug for FSBL(optional)
# XSCTH_BUILD_DEBUG = "1"

# Enable appropriate FSBL debug or compiler flags
YAML_COMPILER_FLAGS_append += "-DXPS_BOARD_ZCU106"
YAML_COMPILER_FLAGS_append += "-DFSBL_PERF_EXCLUDE_VAL=0"
