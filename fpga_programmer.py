#! /usr/bin/python3

import argparse
import sys
import pathlib
import subprocess

ROOT_PATH = pathlib.Path(__file__).resolve().parent
BOARDS = ["zedboard"]
PATHS_CFG_PATH = "paths.cfg"


class TermColors:
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
    print(color + " ".join(str(item) for item in msg), TermColors.END)


def error(*msg, returncode=-1):
    """ Print an error message and exit program """
    print_color(TermColors.RED, "ERROR:", *msg)
    sys.exit(returncode)


def check_file_exists(path):
    if not path.is_file():
        error(path, "does not exist")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("board", choices=BOARDS)
    parser.add_argument("bit", help="FPGA bitstream (.bit) path")
    parser.add_argument("elf", help="FPGA bitstream (.bit) path")
    args = parser.parse_args()

    # Validate bitstream path
    if not args.bit.endswith(".bit"):
        error("bit file should have .bit suffix")
    bit_path = pathlib.Path(args.bit)
    check_file_exists(bit_path)

    # Get FSBL
    fsbl_path = ROOT_PATH / "test" / "fsbl_zedboard.elf"
    check_file_exists(fsbl_path)

    elf_path = pathlib.Path(args.elf)
    check_file_exists(elf_path)

    with open(PATHS_CFG_PATH, "w") as fp:
        fp.write("set BITSTREAM_PATH " + str(bit_path) + "\n")
        fp.write("set FSBL_PATH " + str(fsbl_path) + "\n")
        fp.write("set ELF_PATH " + str(elf_path) + "\n")

    cmd = ["openocd", "-f", PATHS_CFG_PATH, "-f", "zedboard.cfg"]
    subprocess.run(cmd, cwd=ROOT_PATH)


if __name__ == "__main__":
    main()