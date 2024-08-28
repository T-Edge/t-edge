#!/usr/bin/env python3
import argparse
import os
from pathlib import Path

USER = os.getenv("USER")
PETALINUX_VER = os.getenv("PETALINUX_VER")


def parse_args():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "-p",
        "--project",
        type=Path,
        default=Path.cwd(),
        help="Path to the PetaLinux project",
    )
    parser.add_argument(
        "-b",
        "--base",
        type=Path,
        default=Path(f"/home/{USER}/.plnx/{PETALINUX_VER}"),
        help="Base directory",
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    if not args.base.is_dir():
        raise NotADirectoryError(f"{args.base} does not exist.")

    os.chdir(args.project)

    USERCONF = Path("project-spec/meta-user/conf/petalinuxbsp.conf")
    assert USERCONF.is_file(), f"{USERCONF} does not exist."

    dl_dir = args.base / "downloads"
    sstate_dir = args.base / "sstate-cache/aarch64"

    with open(USERCONF, "a") as f:
        f.write(f'DL_DIR = "{dl_dir}"\n')
        f.write(f'SSTATE_DIR = "{sstate_dir}"\n')
