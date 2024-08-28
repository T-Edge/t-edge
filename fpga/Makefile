IP_REPO=conv_filter_hls/solution/impl/ip
IP=xilinx_com_hls_Filter2DKernel_1_0.zip

xsa: conv_filter.xsa
hls: $(IP_REPO)/$(IP)

$(IP_REPO)/$(IP): script/build_hls.tcl
	vitis_hls -f script/build_hls.tcl

conv_filter.xsa: script/build_xsa.tcl $(IP_REPO)/$(IP)
	vivado -mode batch -source script/build_xsa.tcl
