#!/usr/bin/env python3

import shutil
from pathlib import Path
import argparse
import os
from taihe.compilation import compile as taihec


def main(taihe_file: str, gen_dir: str, author: bool, user: bool):
    idl_dir = os.path.dirname(taihe_file)
    idl_dir = Path(idl_dir)
    gen_dir = Path(gen_dir)

    if not idl_dir.is_dir():
        raise FileNotFoundError(f"'{idl_dir}' is not an valid directory.")

    if user and author:
        raise ValueError(f"Invalid to set both author and user args as true")

    taihec(
        src_dirs=[idl_dir],
        dst_dir=gen_dir,
        gen_author=author,
        gen_user=user,
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Build and run project from a target directory",
    )
    parser.add_argument(
        "taihe_file",
        type=str,
        help="The .taihe file need to compile",
    )
    parser.add_argument(
        "gen_headers_dir",
        type=str,
        help="The directory used to generate header files",
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

    main(args.taihe_file, args.gen_headers_dir, args.author, args.user)
