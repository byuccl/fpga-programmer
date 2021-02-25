The goal of this project is to support FPGA programming without requiring installation of the commercial FPGA tools.  It uses *openocd* to program both hardware (FPGA bitstream) and software (ARM FSBL + ELF).


### Prerequisites

Install openocd:
`sudo apt install openocd`

### Running
```
usage: ./fpga_programmer.py [-h] [--bit BIT] [--fsbl FSBL] [--elf ELF] board

positional arguments:
  board

optional arguments:
  -h, --help   show this help message and exit
  --bit BIT    FPGA bitstream (.bit) path
  --fsbl FSBL  First stage bootloeader (.elf) path
  --elf ELF    Executable (.elf) path
```


Currently supported boards:
  * `zedboard`
  * `zybo`
  * `basys3`

#### Hardware Only
If you only want to program hardware, just provide the `--bit` bitstream and ignore the software arguments.

#### Running Software 
You can run software by specifying just the `--elf` option.  The `boards` folder already contains basic bitstream and FSBL files, so you don't need to provide these options unless you need specific hardware or bootloader code.


### Todo List of Features
* Add support for PYNQ and Ultra96 boards.
