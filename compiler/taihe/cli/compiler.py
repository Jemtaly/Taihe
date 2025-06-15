import argparse
import sys
from pathlib import Path

from taihe.codegen.cmake import CMakeOutputManager
from taihe.driver.backend import BackendConfig, BackendRegistry
from taihe.driver.contexts import CompilerInstance, CompilerInvocation
from taihe.utils.outputs import OutputManager


def main():
    registry = BackendRegistry()
    registry.register_all()

    parser = argparse.ArgumentParser(
        prog="taihec",
        description="generates source code from taihe files",
    )
    parser.add_argument(
        "src_files",
        type=Path,
        nargs="*",
        default=[],
        help="input .taihe files, if not provided, will read from stdin",
    )
    parser.add_argument(
        "-I",
        type=Path,
        dest="src_dirs",
        nargs="*",
        default=[],
        help="directories of .taihe source files",
    )  # deprecated
    parser.add_argument(
        "--output",
        "-O",
        type=Path,
        dest="dst_dir",
        required=True,
        help="directory for generated files",
    )
    parser.add_argument(
        "--generate",
        "-G",
        dest="backends",
        nargs="*",
        default=[],
        choices=registry.get_backend_names(),
        help="backends to generate sources, default: abi-header, abi-source, c-author",
    )
    parser.add_argument(
        "--codegen",
        "-C",
        dest="config",
        action="append",
        default=[],
        help="additional code generation configuration",
    )
    parser.add_argument(
        "--cmake",
        action="store_true",
        help="generate cmake for generated files",
    )
    args = parser.parse_args()

    resolved_backends: list[BackendConfig] = []
    for b in registry.collect_required_backends(args.backends):
        resolved_backends.append(b())

    if args.cmake:
        om = CMakeOutputManager(Path(args.dst_dir))
    else:
        om = OutputManager(Path(args.dst_dir))

    invocation = CompilerInvocation(
        src_files=args.src_files,
        src_dirs=args.src_dirs,
        out_dir=args.dst_dir,
        backends=resolved_backends,
        output_manager=om,
    )

    for config in args.config:
        k, *v = config.split("=", 1)
        if k == "sts:keep-name":
            invocation.sts_keep_name = True
        elif k == "arkts:module-prefix":
            invocation.arkts_module_prefix = v[0] if v else None
        elif k == "arkts:path-prefix":
            invocation.arkts_path_prefix = v[0] if v else None
        else:
            raise ValueError(f"unknown codegen config {k!r}")

    instance = CompilerInstance(invocation)
    if not instance.run():
        return -1
    return 0


if __name__ == "__main__":
    sys.exit(main())
