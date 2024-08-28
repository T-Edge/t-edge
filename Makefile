HARDWARE_PLATFORM=fpga/build/filter2dhw.xsa

xsa: $(HARDWARE_PLATFORM)
$(HARDWARE_PLATFORM): $(HLS_IP)
	make -C fpga

########################################################################
# Source Code for HLS
########################################################################

XILINX_VERSION=2021.2

convolution-tutorial: .$(XILINX_VERSION).tar.gz
	if [ -d $@ ]; then rm -rf $@; fi
	tar -xvf .$(XILINX_VERSION).tar.gz Vitis-Tutorials-$(XILINX_VERSION)/Hardware_Acceleration/Design_Tutorials/01-convolution-tutorial --strip-components=3
	mv 01-convolution-tutorial $@
	chmod -R -w $@

.$(XILINX_VERSION).tar.gz:
	[ -f .$(XILINX_VERSION).tar.gz ] || wget https://github.com/Xilinx/Vitis-Tutorials/archive/refs/heads/$(XILINX_VERSION).tar.gz -O .$(XILINX_VERSION).tar.gz
