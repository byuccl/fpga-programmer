The goal of this project is to support FPGA programming without requiring installation of the commercial FPGA tools.  It uses *openocd* to program both hardware (FPGA bitstream) and software (ARM FSBL + ELF).


### Prerequisites

Install openocd:
`sudo apt install openocd`

### Running
```
./fpga_programmer.py <boards> <bit_path> <elf_path>
```

Currently supported boards:
  * `zedboard`

### Todo List of Features
* Support programming hardware only (not software)
* Add support for PYNQ, Zybo and Ultra96 boards.
* Command-line argument for custom FSBL