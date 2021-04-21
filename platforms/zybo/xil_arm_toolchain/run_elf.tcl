connect -url tcp:127.0.0.1:3121

# puts "Cable targets:\n [targets ]"

# Get a list of targets 
set target_list [targets -nocase -filter {name =~ "xc7z010*"} -target-properties]

if {[llength $target_list] > 1} {
	puts "Error: Multiple matching targets: $target_list (Make sure only one board is plugged in)"
	exit 1
} elseif {[llength $target_list] == 0} {
    puts "Error: No targets.  Make sure board is plugged in and driver is installed."
    exit 1
}

targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -index 1
fpga -file platforms/hw/330_hw_system.bit 
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw platforms/hw/330_hw_system.xsa -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source platforms/hw/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow $::env(ELF_FILE)
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
