#!/usr/bin/python3

"""
This module is used to verify that your lab solution will build with the lab
submission system.
"""

import pathlib
import argparse
import shutil
import subprocess
import sys
import zipfile
import getpass
import re

repo_path = pathlib.Path(__file__).absolute().parent.resolve()
test_repo_path = (repo_path / "test_repo").resolve()
build_path = test_repo_path / "build"
checker_path = repo_path / "tools" / "checker"


class TermColors:
    """ Terminal codes for printing in color """

    # pylint: disable=too-few-public-methods

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

    print_color(TermColors.RED, "ERROR:", " ".join(str(item) for item in msg))
    sys.exit(returncode)


def format_code():
    """ Run ./format.py to format student code """
    print_color(TermColors.BLUE, "Formatting code")

    subprocess.run(
        [
            "./format.py",
        ],
        cwd=repo_path,
        check=True,
    )


def clone_student_repo():
    """ Clone a clean 330 student repo into 'test_repo_path', deleting existing one if it exists """

    # Delete existing repo
    shutil.rmtree(test_repo_path, ignore_errors=True)

    if test_repo_path.is_dir():
        error("Could not delete", test_repo_path)

    print_color(TermColors.BLUE, "Cloning ecen330 base repo into", test_repo_path)
    proc = subprocess.run(
        [
            "git",
            "clone",
            "https://github.com/byu-cpe/ecen330_student",
            str(test_repo_path),
        ],
        cwd=repo_path,
        check=False,
    )
    if proc.returncode:
        return False
    return True


def get_files_to_copy_and_zip(lab):
    """ Build a list of (src,dest) files to copy into the temp repo given the lab """

    print_color(TermColors.BLUE, "Enumerating files to copy/zip")

    chk_lab_path = checker_path / lab
    src_lab_path = repo_path / lab
    src_libs_path = repo_path / "my_libs"
    dest_libs_path = test_repo_path / "my_libs"
    dest_lab_path = test_repo_path / lab
    lasertag_path = repo_path / "lasertag"

    # Build a list of files
    # Each entry in this list is a tuple in format (src - pathlib.Path, dest - pathlib.Path, include_in_zip? - boolean)
    files = []
    files.append((checker_path / "CMakeLists.txt", test_repo_path, False))
    files.append((chk_lab_path / "checker.cmake", test_repo_path, False))
    if lab == "lab1":
        files.append((src_lab_path / "main.c", dest_lab_path, True))
    elif lab == "lab2":
        files.append((chk_lab_path / "my_libs.cmake", dest_libs_path / "CMakeLists.txt", False))
        files.append((src_libs_path / "buttons.c", dest_libs_path, True))
        files.append((src_libs_path / "switches.c", dest_libs_path, True))
    elif lab == "lab3":
        files.append((chk_lab_path / "my_libs.cmake", dest_libs_path / "CMakeLists.txt", False))
        files.append((chk_lab_path / "cmake", dest_lab_path / "CMakeLists.txt", False))
        files.append((src_libs_path / "buttons.c", dest_libs_path, False))
        files.append((src_libs_path / "switches.c", dest_libs_path, False))
        files.append((src_libs_path / "intervalTimer.c", dest_libs_path, True))
    elif lab == "lab4":
        files.append((chk_lab_path / "my_libs.cmake", dest_libs_path / "CMakeLists.txt", False))
        files.append((chk_lab_path / "cmake", dest_lab_path / "CMakeLists.txt", False))
        files.append((src_libs_path / "buttons.c", dest_libs_path, False))
        files.append((src_libs_path / "switches.c", dest_libs_path, False))
        files.append((src_libs_path / "intervalTimer.c", dest_libs_path, False))
        files.append((src_lab_path / "clockControl.c", dest_lab_path, True))
        files.append((src_lab_path / "clockDisplay.c", dest_lab_path, True))
    elif lab == "lab5":
        files.append((chk_lab_path / "my_libs.cmake", dest_libs_path / "CMakeLists.txt", False))
        files.append((chk_lab_path / "cmake", dest_lab_path / "CMakeLists.txt", False))
        files.append((src_libs_path / "buttons.c", dest_libs_path, False))
        files.append((src_libs_path / "switches.c", dest_libs_path, False))
        files.append((src_libs_path / "intervalTimer.c", dest_libs_path, False))
        files.append((src_lab_path / "ticTacToeControl.c", dest_lab_path, True))
        files.append((src_lab_path / "ticTacToeDisplay.c", dest_lab_path, True))
        files.append((src_lab_path / "minimax.c", dest_lab_path, True))
        files.append((src_lab_path / "testBoards.c", dest_lab_path, True))

    elif lab == "lab6":
        files.append((chk_lab_path / "my_libs.cmake", dest_libs_path / "CMakeLists.txt", False))
        files.append((chk_lab_path / "cmake", dest_lab_path / "CMakeLists.txt", False))
        files.append((src_libs_path / "buttons.c", dest_libs_path, False))
        files.append((src_libs_path / "switches.c", dest_libs_path, False))
        files.append((src_libs_path / "intervalTimer.c", dest_libs_path, False))
        files.append((src_lab_path / "buttonHandler.c", dest_lab_path, True))
        files.append((src_lab_path / "flashSequence.c", dest_lab_path, True))
        files.append((src_lab_path / "verifySequence.c", dest_lab_path, True))
        files.append((src_lab_path / "simonDisplay.c", dest_lab_path, True))
        files.append((src_lab_path / "simonControl.c", dest_lab_path, True))
        files.append((src_lab_path / "globals.c", dest_lab_path, True))
    elif lab == "lab7":
        files.append((chk_lab_path / "my_libs.cmake", dest_libs_path / "CMakeLists.txt", False))
        files.append((chk_lab_path / "cmake", dest_lab_path / "CMakeLists.txt", False))
        files.append((src_libs_path / "buttons.c", dest_libs_path, False))
        files.append((src_libs_path / "switches.c", dest_libs_path, False))
        files.append((src_libs_path / "intervalTimer.c", dest_libs_path, False))
        files.append((src_lab_path / "wamControl.c", dest_lab_path, True))
        files.append((src_lab_path / "wamDisplay.c", dest_lab_path, True))
    elif lab == "390m3-1":
        files.append((lasertag_path / "filter.c", None, True))
    elif lab == "390m3-2":
        files.append((lasertag_path / "filter.c", None, True))
        files.append((lasertag_path / "hitLedTimer.c", None, True))
        files.append((lasertag_path / "lockoutTimer.c", None, True))
        files.append((lasertag_path / "transmitter.c", None, True))
        files.append((lasertag_path / "trigger.c", None, True))
    elif lab == "390m3-3":
        files.append((lasertag_path / "detector.c", None, True))
        #        files.append((lasertag_path / "autoReloadTimer.c", None, True))
        #        files.append((lasertag_path / "invincibilityTimer.c", None, True))
        #        files.append((lasertag_path / "ledTimer.c", None, True))
        files.append((lasertag_path / "lockoutTimer.c", None, True))
        files.append((lasertag_path / "isr.c", None, True))
    elif lab == "390m5":
        files.append((lasertag_path / "gameModes.c", None, True))

    if lab.startswith("390"):
        print(
            len([f for f in files if f[2]]), "files to be included in the submission zip archive."
        )
    else:
        print(
            len(files),
            "files to copy,",
            len([f for f in files if f[2]]),
            "of these will be included in the submission zip archive.",
        )
    return files


def copy_solution_files(files_to_copy):
    """ Copy student files to the temp repo """

    print_color(TermColors.BLUE, "Copying your solution files to the test_repo")

    # files_to_copy provides a list of files in (src_path, dest_path, include_in_zip?) format
    for (src, dest, _) in files_to_copy:
        print("Copying", src.relative_to(repo_path), "to", dest.relative_to(repo_path))
        if not src.is_file():
            error("Required file", src, "does not exist.")
        shutil.copy(src, dest)


def build(milestone):
    """ Run cmake/make """

    if milestone:
        print_color(TermColors.BLUE, "Trying to build (-D" + milestone + "=1)")
    else:
        print_color(TermColors.BLUE, "Trying to build")

    print_color(TermColors.BLUE, "Removing build directory (" + str(build_path) + ")")
    shutil.rmtree(build_path)
    print_color(TermColors.BLUE, "Creating build directory (" + str(build_path) + ")")
    build_path.mkdir()

    # Run cmake
    cmake_cmd = ["cmake", "..", "-DEMU=1"]
    if milestone is not None:
        cmake_cmd.append("-D" + milestone + "=1")
    proc = subprocess.run(cmake_cmd, cwd=build_path, check=False)
    if proc.returncode:
        return False

    # Run make
    proc = subprocess.run(
        ["make", "-j4"],
        cwd=build_path,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    build_output = proc.stdout.decode()
    print(build_output)
    if proc.returncode:
        return False

    # If user code has warnings, ask if they still want to compile.
    matches = re.findall(" warning: ", build_output)
    if matches:
        input_txt = ""
        while input_txt not in ["y", "n"]:
            input_txt = input(
                TermColors.YELLOW
                + "Your code has "
                + str(len(matches))
                + " warning(s).  You will lose a coding standard point for each warning.  Are you sure you want to continue? (y/n) "
                + TermColors.END
            ).lower()
        if input_txt == "n":
            error("User cancelled zip process.")

    return True


def run(lab):
    """ Run the lab program in the emulator """
    try:
        subprocess.run([str(build_path / lab / (lab + ".elf"))], check=True)
    except KeyboardInterrupt:
        print()


def zip(lab, files):
    """ Zip the lab files """

    zip_path = repo_path / (getpass.getuser() + "_" + lab + ".zip")
    print_color(TermColors.BLUE, "Creating zip file", zip_path.relative_to(repo_path))
    if zip_path.is_file():
        print("Deleting existing file.")
        zip_path.unlink()
    with zipfile.ZipFile(zip_path, "w") as zf:
        print("Created new zip file")
        # Loop through files that are marked for zip (f[2] == True)
        for f in (f for f in files if f[2]):
            if not f[0].is_file():
                error(f[0].relative_to(repo_path), "does not exist")
            print("Adding", f[0].relative_to(repo_path))
            zf.write(f[0], arcname=f[0].name)

    return zip_path.relative_to(repo_path)


def get_milestones(lab):
    """ Return the different milestones for the lab.  """

    # Return list of configurations in (name, CMAKE_DEFINE) format
    if lab == "lab3":
        return [
            ("MILESTONE_1", "RUN_PROGRAM_MILESTONE_1"),
            ("MILESTONE_2", "RUN_PROGRAM_MILESTONE_2"),
        ]
    elif lab == "lab4":
        return [
            ("MILESTONE_1_SIZE6", "RUN_PROGRAM_MILESTONE_1_SIZE6"),
            ("MILESTONE_1_SIZE5", "RUN_PROGRAM_MILESTONE_1_SIZE5"),
            ("MILESTONE_1_SIZE4", "RUN_PROGRAM_MILESTONE_1_SIZE4"),
            ("MILESTONE_1_SIZE3", "RUN_PROGRAM_MILESTONE_1_SIZE3"),
            ("MILESTONE_2", "RUN_PROGRAM_MILESTONE_2"),
        ]
    elif lab == "lab5":
        return [
            ("MILESTONE_1", "RUN_PROGRAM_MILESTONE_1"),
            ("MILESTONE_2", "RUN_PROGRAM_MILESTONE_2"),
            ("MILESTONE_3", "RUN_PROGRAM_MILESTONE_3"),
        ]
    elif lab == "lab6":
        return [
            ("MILESTONE_1", "RUN_PROGRAM_MILESTONE_1"),
            ("MILESTONE_2", "RUN_PROGRAM_MILESTONE_2"),
            ("MILESTONE_3", "RUN_PROGRAM_MILESTONE_3"),
            ("MILESTONE_4", "RUN_PROGRAM_MILESTONE_4"),
        ]
    elif lab == "lab7":
        return [
            ("MILESTONE_1", "RUN_PROGRAM_MILESTONE_1"),
            ("MILESTONE_2", "RUN_PROGRAM_MILESTONE_2"),
        ]
    else:
        return [("main", None)]


def main():
    """ Copy files into temp repo, build and run lab """

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "lab",
        choices=[
            "lab1",
            "lab2",
            "lab3",
            "lab4",
            "lab5",
            "lab6",
            "lab7",
            "390m3-1",
            "390m3-2",
            "390m3-3",
            "390m5",
        ],
    )
    parser.add_argument(
        "--no_run", action="store_true", help="Test the lab build, but don't run the emualtor"
    )
    args = parser.parse_args()

    # First format student's code
    format_code()

    # Get a list of files need to build and zip
    files = get_files_to_copy_and_zip(args.lab)

    # For 390, we don't build/run anything, so just skip to the zip process
    if not args.lab.startswith("390"):
        # Clone/clean 330 repo
        if not clone_student_repo():
            input_txt = ""
            while input_txt not in ["y", "n"]:
                input_txt = input(
                    TermColors.YELLOW
                    + "Could not clone Github repo.  Perhaps you are not connected to the internet. "
                    "It is recommended that you cancel the process, connect to the internet, and retry. "
                    "If you proceed, the generated zip file will be untested, and may not build properly on the TA's evaluation system. "
                    "Are you sure you want to proceed? (y/n) " + TermColors.END
                ).lower()
            if input_txt == "n":
                error("User cancelled zip process.")

        else:
            # Copy over necessary files to test repo
            copy_solution_files(files)

            # Loop through configs
            for (config_name, config_define) in get_milestones(args.lab):
                build_and_run = True
                if args.no_run:
                    print_color(TermColors.BLUE, "Now Testing", config_name)
                else:
                    input(
                        TermColors.BLUE
                        + "Now Testing "
                        + config_name
                        + ". Hit <Enter> to continue."
                        + TermColors.END
                    )

                # See if the code builds
                if build(config_define):
                    # Run it
                    if not args.no_run:
                        print_color(TermColors.BLUE, "Running", args.lab, config_name)
                        print_color(
                            TermColors.BLUE,
                            "If the emulator won't close, press Ctrl+C in this terminal.",
                        )
                        run(args.lab)
                else:
                    s = ""
                    while s not in ("y", "n"):
                        s = input(
                            TermColors.RED
                            + "Build failed for "
                            + config_name
                            + ". Continue? (y/n)"
                            + TermColors.END
                        ).lower()
                    if s == "n":
                        sys.exit(0)

    # Zip it
    zip_relpath = zip(args.lab, files)

    # Delete test repo
    print_color(TermColors.BLUE, "Removing", test_repo_path.name)
    shutil.rmtree(test_repo_path, ignore_errors=True)

    print_color(TermColors.BLUE, "Created", zip_relpath, "\nDone.")


if __name__ == "__main__":
    main()
