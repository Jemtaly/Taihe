#!/usr/bin/env python3

import shutil
from pathlib import Path
import argparse
from taihe.compilation import compile as taihec


def clean_directory(directory):
    if directory.exists():
        shutil.rmtree(directory)


def main(idl_dir: str, target_dir: str, author: bool, user: bool):
    idl_dir = Path(idl_dir)
    target_dir = Path(target_dir)

    if not idl_dir.is_dir():
        raise FileNotFoundError(f"'{idl_dir}' is not an valid directory.")

    if user and author:
        raise ValueError(f"Invalid to set both author and user args as true")

    print("Generating codes...")
    taihec(
        src_dirs=[idl_dir],
        dst_dir=target_dir,
        gen_author=author,
        gen_user=user,
    )
    print("generate done")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Build and run project from a target directory",
    )
    parser.add_argument(
        "idl_dir",
        type=str,
        help="The idl directory containing .taihe files for the project",
    )
    parser.add_argument(
        "target_dir",
        type=str,
        help="The directory used to store generated header files.",
    )
    parser.add_argument(
        "-a",
        "--author",
        action="store_true",
        help="Generte author code",
    )
    parser.add_argument(
        "-u",
        "--user",
        action="store_true",
        help="Generte user code",
    )
    args = parser.parse_args()

    main(args.idl_dir, args.target_dir, args.author, args.user)
