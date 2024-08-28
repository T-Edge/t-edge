require optee-os.inc

SRCREV = "afacf356f9593a7f83cae9f96026824ec242ff52"

SRC_URI_append = "                          \
    file://0001-add-fpga_load-driver.patch  \
    file://0002-sign-bitstream.patch        \
"
