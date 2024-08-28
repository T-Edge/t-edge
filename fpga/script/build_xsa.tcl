# Check if script is running in correct Vivado version.
set scripts_vivado_version 2021.2
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
    puts ""
    catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

    return 1
}

# Create project
set proj_name conv_filter_hw
create_project ${proj_name} ./${proj_name} -part xczu7ev-ffvc1156-2-e -force

# Set project properties
set_property -name "board_part" -value "xilinx.com:zcu106:part0:2.6" -objects [current_project]

# Set IP repository paths & Update IP Catalog
set_property "ip_repo_paths" "[file normalize "conv_filter_hls/solution/impl/ip"]" [current_project]
update_ip_catalog

# Create block design
source script/block_design.tcl

# Call make_wrapper to create wrapper files
set wrapper_path [make_wrapper -fileset sources_1 -files [ get_files -norecurse system.bd] -top]
add_files -norecurse -fileset sources_1 $wrapper_path

# Generate bitstream
launch_runs impl_1 -to_step write_bitstream -jobs 12
wait_on_run impl_1

# Export hardware platform
write_hw_platform -fixed -include_bit -force -file conv_filter.xsa
