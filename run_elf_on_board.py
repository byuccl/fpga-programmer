#!/usr/bin/python3
import argparse
import sys
import os
import subprocess
import re
import serial
import shutil
import pathlib

VITIS_BIN = "/opt/Xilinx/Vitis/2019.2/bin/vitis"
# VITIS_BIN = "/opt/Xilinx/Vitis/2019.2/bin/vitis"
XSCT_BIN_WINDOWS = "C:/Xilinx/Vitis/2019.2/bin/xsct"

LABS_DIR = pathlib.Path(__file__).resolve().parent


class TermColor:
    """ Terminal codes for printing in color """

    PURPLE = "\033[95m"
    BLUE = "\033[94m"
    GREEN = "\033[92m"
    YELLOW = "\033[93m"
    RED = "\033[91m"
    END = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


def print_color(color, *msg):
    """ Print a message in color """
    print(color + " ".join(str(item) for item in msg), TermColor.END)


def error(*msg, returncode=-1):
    """ Print an error message and exit program """
    print_color(TermColor.RED, "ERROR:", *msg)
    sys.exit(returncode)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--windows",
        action="store_true",
        help="This flag indicates the Xilinx tools are installed in Windows",
    )
    parser.add_argument("elf", help="The elf file to run (ex. 'lab1/lab1.elf')")
    args = parser.parse_args()

    elf_path = pathlib.Path(args.elf).absolute()

    if not elf_path.is_file():
        error("File", elf_path, "does not exist")

    print_color(TermColor.PURPLE, "\nProgramming elf file:", elf_path)

    if args.windows:
        win_temp_path = pathlib.Path("/mnt/c/temp/ecen330")
        win_temp_path.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(elf_path, win_temp_path / "program.elf")
        shutil.copyfile(
            LABS_DIR / "zybo/xil_arm_toolchain/run_elf_windows.tcl",
            win_temp_path / "run_elf_windows.tcl",
        )
        shutil.copyfile(LABS_DIR / "hw/330_hw_system.bit", win_temp_path / "330_hw_system.bit")
        shutil.copyfile(LABS_DIR / "hw/330_hw_system.xsa", win_temp_path / "330_hw_system.xsa")

    # Execute programming and print output
    my_env = os.environ.copy()
    my_env["ELF_FILE"] = elf_path
    if args.windows:
        cmd = [
            "cmd.exe",
            "/C",
            "cd C:/temp/ecen330 && " + XSCT_BIN_WINDOWS + " C:/temp/ecen330/run_elf_windows.tcl",
        ]
    else:
        cmd = [VITIS_BIN, "-batch", str(LABS_DIR / "zybo/xil_arm_toolchain/run_elf.tcl")]
    print(cmd)
    subprocess.run(cmd, cwd=LABS_DIR, env=my_env)


if __name__ == "__main__":
    main()

