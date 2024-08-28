open_project -reset conv_filter_hls
set_top Filter2DKernel

set src_dir "../convolution-tutorial/src"

add_files -tb $src_dir/hls_testbench.cpp  -cflags "-I$src_dir/src"
add_files -tb $src_dir/cmdlineparser.cpp  -cflags "-I$src_dir/src"
add_files -tb $src_dir/filter2d_sw.cpp    -cflags "-I$src_dir/src"
add_files     $src_dir/filter2d_hw.cpp    -cflags "-I$src_dir/src"

open_solution "solution" -flow_target vitis
set_part {xczu7ev-ffvc1156-2-e}

create_clock -period 3.33333333 -name default
csim_design
csynth_design

# cosim_design
export_design -format ip_catalog
exit
