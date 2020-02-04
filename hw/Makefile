proj:	
	mkdir 330_hw_system && cd 330_hw_system && vivado -source ../create_hw_proj.tcl

# After you create the project, build the bitstream, and export the hardware/bitstream, this copies the hw files to the permanent location (so you can delete the Vivado project)
copy_hw:
	cp 330_hw_system/330_hw_system/system_top.xsa 330_hw_system.xsa
	cp 330_hw_system/330_hw_system/330_hw_system.runs/impl_1/system_top.bit 330_hw_system.bit

clean:
	rm -rf 330_hw_system *.log *.jou

.PHONY: copy_hw