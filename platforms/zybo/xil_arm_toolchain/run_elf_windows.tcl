connect -url tcp:127.0.0.1:3121

# puts "Cable targets:\n [targets ]"

# Get a list of targets that have the jtag_cable_name =~ $cable_filter
# name == "fpga_name" is used because a single cable can have multiple targets (fpga, arm0, arm1, etc)
# -target properties is used to turn a list with 1 item per target.  Otherwise, even if there is only 
# one target, the return value is a space separates list of values and tcl considers the list length > 1
set target_list [targets -nocase -filter {name =~ "xc7z010*"} -target-properties]

if {[llength $target_list] > 1} {
	puts "Error: Multiple matching targets: $target_list (Use JTAG_BOARD_LOC or JTAG_BOARD_FILTER to filter to only one board)"
	exit 1
} elseif {[llength $target_list] == 0} {
    puts "Error: No targets matching filter: $cable_filter"
    exit 1
}

targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -index 1
fpga -file C:/temp/ecen330/330_hw_system.bit 
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw C:/temp/ecen330/330_hw_system.xsa -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source C:/temp/ecen330/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow C:/temp/ecen330/program.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con