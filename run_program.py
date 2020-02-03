#!/usr/bin/python3
import argparse
import sys
import os
import subprocess
import re
import serial

VITIS_BIN = "/opt/Xilinx/Vitis/2019.2/bin/vitis"

LABS_DIR = os.path.dirname(os.path.abspath(__file__))

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def execute(cmd, cwd = None, env = None):
    if cwd is None:
        cwd = os.getcwd()
    if env is None:
        env = os.environ
    popen = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True, env=env, cwd=cwd)
    for stdout_line in iter(popen.stdout.readline, ""):
        yield stdout_line 
    popen.stdout.close()
    return_code = popen.wait()
    if return_code:
        raise subprocess.CalledProcessError(return_code, cmd)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("elf", help="The elf file to run (ex. 'lab1/lab1.elf')")
    parser.add_argument("--serial", help="The serial device (ex. '/dev/ttyUSB1')")
    args = parser.parse_args()

    elf_file = args.elf

    # Find serial device
    if args.serial is None:
        print(bcolors.HEADER + "Finding serial device", bcolors.ENDC)
        devs = []
        for line in execute(["ls", "/dev"]):
            if re.match("^ttyUSB\d+$", line.strip()):
                devs.append("/dev/" + line.strip())
        if len(devs) == 0:
            print("No ttyUSB serial devices found in /dev")
            sys.exit(1)
        elif len(devs) > 1:
            print("Multiple ttyUSB serial devices found in /dev:", devs, "You must specify a device with --serial.")
            sys.exit(1)
        else:
            serialDev = devs[0]
        print("")
    else:
        serialDev = args.serial

    # Open serial device in nonblocking mode
    print(bcolors.HEADER + "Capturing output from serial device", serialDev, bcolors.ENDC)
    try:
        ser = serial.Serial(serialDev, baudrate = 115200, timeout = 0)
    except serial.serialutil.SerialException:
        print(bcolors.FAIL + "Could not open serial port", serialDev, ". Power the board off and on and then try again.", bcolors.ENDC)
        sys.exit(1)

    # Find elf file
    elf_path_labs = os.path.join(LABS_DIR, elf_file)
    elf_path_build = os.path.join(LABS_DIR, "build", elf_file)

    if os.path.isfile(elf_path_labs):
        elf_path = elf_path_labs
    elif os.path.isfile(elf_path_build):
        elf_path = elf_path_build
    else:
        print("ELF File", elf_file, "could not be found.  Checked for", elf_path_labs, "and", elf_path_build)
        sys.exit(1)

    print("\n" + bcolors.HEADER + "Programming elf file:", elf_path, bcolors.ENDC)

    # Execute programming and print output
    my_env = os.environ.copy()
    my_env["ELF_FILE"] = elf_path
    cmd = [VITIS_BIN, "-batch", "xil_arm_toolchain/run_elf.tcl"]
    for line in execute(cmd, cwd = LABS_DIR, env = my_env):
        print(line.strip())

    # Print serial output until Ctrl+C
    print("\n" + bcolors.HEADER + "Printing program output from serial (Ctrl+C to quit)", bcolors.ENDC)
    while True:
        try:
            line = ser.readline().decode('ascii', errors='replace')
            if line != "":
                sys.stdout.write(line)
        except KeyboardInterrupt:
            print("")
            break

if __name__ == "__main__":
    main()

