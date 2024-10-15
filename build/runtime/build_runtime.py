#!/usr/bin/env python3

import os
import argparse

def main(src_dir, out_dir):
  if not os.path.exists(out_dir):
    os.makedirs(out_dir)
  os.system(f"cd {out_dir} && cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ {src_dir} && make")

if __name__ == "__main__":
  parser = argparse.ArgumentParser(
      description="Build and run project from a target directory",
  )
  parser.add_argument(
      "src_dir",
      type=str,
      help="Specify the src dir for cmake to build",
  )
  parser.add_argument(
      "out_dir",
      type=str,
      help="Specify the out dir for cmake to build",
  )
  args = parser.parse_args()
  main(args.src_dir, args.out_dir)