#! /usr/bin/python3

import argparse
import sys
import pathlib
import subprocess

ROOT_PATH = pathlib.Path(__file__).resolve().parent
BOARDS_PATH = ROOT_PATH / "boards"
SETUP_CFG_PATH = ROOT_PATH / "setup.cfg"


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


class Board:
    def __init__(self, name, supports_sw):
        self.name = name
        board_path = BOARDS_PATH / name
        self.cfg_path = board_path / (name + ".cfg")
        check_file_exists(self.cfg_path)

        self.fsbl_path = board_path / "fsbl.elf"
        self.bit_path = board_path / (name + ".bit")

        self.supports_sw = supports_sw


def main():
    boards = []

    boards.append(Board("zedboard", supports_sw=True))
    boards.append(Board("zybo", supports_sw=True))
    boards.append(Board("basys3", supports_sw=False))

    parser = argparse.ArgumentParser()
    parser.add_argument("board", choices=[b.name for b in boards])
    parser.add_argument("--bit", help="FPGA bitstream (.bit) path")
    parser.add_argument("--fsbl", help="First stage bootloeader (.elf) path")
    parser.add_argument("--elf", help="Executable (.elf) path")
    args = parser.parse_args()

    # Board
    board = [b for b in boards if b.name == args.board][0]
    cfg_path = board.cfg_path

    # Validate bitstream path
    if args.bit:
        if not args.bit.endswith(".bit"):
            error("bit file should have .bit suffix")
        bit_path = pathlib.Path(args.bit).resolve()
    else:
        bit_path = board.bit_path
        print_color(TermColors.YELLOW, "Using default bitstream", bit_path.relative_to(ROOT_PATH))
    check_file_exists(bit_path)

    # Check if we are programming software
    sw = args.fsbl or args.elf
    if sw and not board.supports_sw:
        error("Board does not support software.")

    if sw:
        # Get FSBL
        if args.fsbl:
            if not args.fsbl.endswith(".elf"):
                error("FSBL file should have .elf suffix")
            fsbl_path = pathlib.Path(args.fsbl).resolve()
        else:
            fsbl_path = board.fsbl_path
            print_color(TermColors.YELLOW, "Using default FSBL", fsbl_path.relative_to(ROOT_PATH))
        check_file_exists(fsbl_path)

        # Get ELF
        if args.elf is None:
            error("You must provide an --elf if you provide an --fsbl")
        elf_path = pathlib.Path(args.elf).resolve()
        check_file_exists(elf_path)

    with open(SETUP_CFG_PATH, "w") as fp:
        fp.write("set BITSTREAM_PATH " + str(bit_path) + "\n")
        fp.write("set SW " + ("1" if sw else "0") + "\n")
        if sw:
            fp.write("set FSBL_PATH " + str(fsbl_path) + "\n")
            fp.write("set ELF_PATH " + str(elf_path) + "\n")

    cmd = ["openocd", "-f", SETUP_CFG_PATH, "-f", cfg_path]
    subprocess.run(cmd, cwd=ROOT_PATH)


if __name__ == "__main__":
    main()
