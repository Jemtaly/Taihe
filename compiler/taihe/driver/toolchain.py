# -*- coding: utf-8 -*-
#
# Copyright (c) 2025 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import json
import logging
import os
import shutil
import subprocess
import tarfile
import time
from collections.abc import Iterable, Mapping, Sequence
from dataclasses import dataclass
from pathlib import Path

from typing_extensions import override

from taihe.driver.backend import BackendRegistry
from taihe.driver.contexts import (
    CompilerInstance,
    CompilerInvocation,
)
from taihe.driver.options import OptionRegistry
from taihe.semantics import PrettyPrintBackendConfig
from taihe.utils.diagnostics import ConsoleDiagnosticsManager
from taihe.utils.outputs import (
    BasicOutputConfig,
    BasicOutputManager,
    DebugLevel,
    GeneratedFileGroup,
    RuntimeSourceGroup,
)
from taihe.utils.resources import (
    PandaVm,
)

logger = logging.getLogger(__name__)


def run_command(
    command: Sequence[Path | str],
    capture_output: bool = False,
    env: Mapping[str, Path | str] | None = None,
) -> float:
    """Run a command with environment variables."""
    command_str = " ".join(map(str, command))

    env_str = ""
    for key, val in (env or {}).items():
        env_str += f"{key}={val} "

    logger.debug("+ %s%s", env_str, command_str)

    try:
        start_time = time.time()
        subprocess.run(
            command,
            check=True,
            text=True,
            env=env,
            capture_output=capture_output,
        )
        end_time = time.time()
        elapsed_time = end_time - start_time
        return elapsed_time
    except subprocess.CalledProcessError as e:
        logger.error("Command failed with exit code %s", e.returncode)
        if e.stdout:
            logger.error("Standard output: %s", e.stdout)
        if e.stderr:
            logger.error("Standard error: %s", e.stderr)
        raise


def create_directory(directory: Path) -> None:
    directory.mkdir(parents=True, exist_ok=True)
    logger.debug("Created directory: %s", directory)


def clean_directory(directory: Path) -> None:
    if not directory.exists():
        return
    shutil.rmtree(directory)
    logger.debug("Cleaned directory: %s", directory)


def move_directory(src: Path, dst: Path) -> None:
    if not src.exists():
        raise FileNotFoundError(f"Source directory does not exist: {src}")
    shutil.move(src, dst)
    logger.debug("Moved directory from %s to %s", src, dst)


def copy_directory(src: Path, dst: Path) -> None:
    if not src.exists():
        raise FileNotFoundError(f"Source directory does not exist: {src}")
    shutil.copytree(src, dst, dirs_exist_ok=True)
    logger.debug("Copied directory from %s to %s", src, dst)


def extract_file(
    target_file: Path,
    extract_dir: Path,
) -> None:
    """Extract a tar.gz file."""
    if not target_file.exists():
        raise FileNotFoundError(f"File to extract does not exist: {target_file}")

    create_directory(extract_dir)

    with tarfile.open(target_file, "r:gz") as tar:
        # Check for any unsafe paths before extraction
        for member in tar.getmembers():
            member_path = Path(member.name)
            if member_path.is_absolute() or ".." in member_path.parts:
                raise ValueError(f"Unsafe path in archive: {member.name}")
        # Extract safely
        tar.extractall(path=extract_dir)

    logger.info("Extracted %s to %s", target_file, extract_dir)


MANIFEST_FILENAME = ".taihe_manifest.json"


@dataclass
class TryitOutputConfig(BasicOutputConfig):
    """Output configuration for the tryit compilation flow.

    Extends BasicOutputConfig to construct a TryitOutputManager that records
    registered runtime source files and generated files by group, allowing the
    build system to query them without any type-specific logic in the manager.
    """

    def construct(self) -> "TryitOutputManager":
        return TryitOutputManager(self.dst_dir, debug_level=self.debug_level)


class TryitOutputManager(BasicOutputManager):
    """Output manager that collects compilation info for the tryit flow.

    Like CMakeOutputManager, files are stored in group-keyed dicts.  The
    build system (tryit.py) is responsible for querying the specific groups it
    needs and constructing absolute paths — keeping type-specific knowledge out
    of this class.
    """

    def __init__(
        self,
        dst_dir: Path,
        *,
        debug_level: DebugLevel,
    ):
        super().__init__(dst_dir, debug_level=debug_level)
        self.runtime_src_files: dict[RuntimeSourceGroup, list[str]] = {}
        self.gen_src_files: dict[GeneratedFileGroup, list[str]] = {}

    @override
    def register_runtime_src(self, group: RuntimeSourceGroup, relative_path: str):
        self.runtime_src_files.setdefault(group, []).append(relative_path)

    @override
    def register_generated_file(self, group: GeneratedFileGroup, relative_path: str):
        self.gen_src_files.setdefault(group, []).append(relative_path)

    @override
    def post_generate(self) -> None:
        """Write a build manifest so the build step can run independently."""
        manifest_data = {
            "runtime_src_files": {
                group.var_name: paths for group, paths in self.runtime_src_files.items()
            },
            "gen_src_files": {
                group.var_name: paths for group, paths in self.gen_src_files.items()
            },
        }
        manifest_path = self.dst_dir / MANIFEST_FILENAME
        with open(manifest_path, "w") as f:
            json.dump(manifest_data, f, indent=2)
        logger.debug("Written build manifest: %s", manifest_path)


@dataclass
class TryitManifest:
    """Persisted metadata from a generate step, consumed by a standalone build step.

    Written to ``generated/.taihe_manifest.json`` by :meth:`TryitOutputManager.post_generate`.
    """

    runtime_src_files: dict[str, list[str]]
    gen_src_files: dict[str, list[str]]

    @classmethod
    def load(cls, generated_dir: Path) -> "TryitManifest":
        """Load the manifest written by a previous generate step."""
        manifest_path = generated_dir / MANIFEST_FILENAME
        if not manifest_path.exists():
            raise FileNotFoundError(
                f"Build manifest not found at '{manifest_path}'. "
                "Run 'generate' before 'build'."
            )
        with open(manifest_path) as f:
            data = json.load(f)
        return cls(
            runtime_src_files=data["runtime_src_files"],
            gen_src_files=data["gen_src_files"],
        )


def taihec(
    dst_dir: Path,
    src_files: list[Path],
    backend_names: list[str],
    extra: list[str] | None = None,
    debug: bool = False,
) -> TryitOutputManager:
    registry = BackendRegistry()
    registry.register_all()

    dm = ConsoleDiagnosticsManager()

    backend_config_types = registry.collect_required_backends(backend_names, dm)
    option_registry = OptionRegistry()
    for backend_config_type in backend_config_types:
        backend_config_type.register(option_registry)
    options = option_registry.parse_args(extra or [], dm)
    backend_configs = [
        backend_config
        for backend_config_type in backend_config_types
        if (backend_config := backend_config_type.create(options, dm)) is not None
    ]
    if debug:
        pretty_print_backend_config = PrettyPrintBackendConfig(
            show_resolved=True,
            show_internal=True,
            target_desc="stderr",
        )
        backend_configs.append(pretty_print_backend_config)

    output_config = TryitOutputConfig(dst_dir=dst_dir)

    if dm.has_error:
        raise RuntimeError("Failed to parse options for backends")

    invocation = CompilerInvocation(
        src_files=src_files,
        backend_configs=backend_configs,
        output_config=output_config,
    )
    instance = CompilerInstance(invocation, dm)
    instance.run()

    if dm.has_error:
        raise RuntimeError("Compilation failed with errors")

    om = instance.output_manager
    if not isinstance(om, TryitOutputManager):
        raise TypeError(f"Expected TryitOutputManager, got {type(om).__name__}")
    return om


class CppToolchain:
    """Utility class for C++ toolchain operations."""

    def __init__(self):
        self.cxx = os.getenv("CXX", "clang++")
        self.cc = os.getenv("CC", "clang")

    def compile(
        self,
        output_dir: Path,
        input_files: Iterable[Path],
        include_dirs: Sequence[Path] = (),
        compile_flags: Sequence[str] = (),
    ) -> list[Path]:
        """Compile source files."""
        output_files: list[Path] = []

        for input_file in input_files:
            name = input_file.name
            output_file = output_dir / f"{name}.o"

            if name.endswith(".c"):
                compiler = self.cc
                std = "gnu11"
            else:
                compiler = self.cxx
                std = "gnu++17"

            command = [
                compiler,
                "-c",
                "-fvisibility=hidden",
                "-fPIC",
                "-Wall",
                "-Wextra",
                # "-Werror",
                f"-std={std}",
                "-o",
                output_file,
                input_file,
                *compile_flags,
            ]

            for include_dir in include_dirs:
                if include_dir.exists():  # Only include directories that exist
                    command.append(f"-I{include_dir}")

            run_command(command)

            output_files.append(output_file)

        return output_files

    def link(
        self,
        output_file: Path,
        input_files: Sequence[Path],
        shared: bool = False,
        link_options: Sequence[str] = (),
    ) -> None:
        """Link object files."""
        if len(input_files) == 0:
            logger.warning("No input files to link")
            return

        command = [
            self.cxx,
            "-fPIC",
            "-o",
            output_file,
            *input_files,
            *link_options,
        ]

        if shared:
            command.append("-shared")

        run_command(command)

    def run(
        self,
        target: Path,
        ld_lib_path: Path,
        args: Sequence[str] = (),
    ) -> float:
        """Run the compiled target."""
        command = [
            target,
            *args,
        ]

        return run_command(
            command,
            env={"LD_LIBRARY_PATH": ld_lib_path},
        )


class ArkToolchain:
    """Utility class for ABC toolchain operations."""

    def __init__(self):
        self.vm = PandaVm.resolve()

    def create_arktsconfig(
        self,
        arktsconfig_file: Path,
        app_paths: dict[str, Path] | None = None,
    ) -> None:
        """Create ArkTS configuration file."""
        vm = PandaVm.resolve()
        paths = vm.stdlib_sources | vm.sdk_sources | (app_paths or {})

        config_content = {
            "compilerOptions": {
                "baseUrl": str(vm.host_tools_dir),
                "paths": {key: [str(value)] for key, value in paths.items()},
            }
        }

        with open(arktsconfig_file, "w") as json_file:
            json.dump(config_content, json_file, indent=2)

        logger.debug("Created configuration file at: %s", arktsconfig_file)

    def compile(
        self,
        output_dir: Path,
        input_files: Iterable[Path],
        arktsconfig_file: Path,
    ) -> list[Path]:
        """Compile ETS files to ABC format."""
        output_files: list[Path] = []

        for input_file in input_files:
            name = input_file.name
            output_file = output_dir / f"{name}.abc"
            output_dump = output_dir / f"{name}.abc.dump"

            gen_abc_command = [
                self.vm.tool("es2panda"),
                input_file,
                "--output",
                output_file,
                "--extension",
                "ets",
                "--arktsconfig",
                arktsconfig_file,
            ]

            run_command(gen_abc_command)

            output_files.append(output_file)

            ark_disasm_path = self.vm.tool("ark_disasm")
            if not ark_disasm_path.exists():
                logger.warning(
                    "ark_disasm not found at %s, skipping disassembly", ark_disasm_path
                )
                continue

            gen_abc_dump_command = [
                ark_disasm_path,
                output_file,
                output_dump,
            ]

            run_command(gen_abc_dump_command)

        return output_files

    def link(
        self,
        target: Path,
        input_files: Sequence[Path],
    ) -> None:
        """Link ABC files."""
        if len(input_files) == 0:
            logger.warning("No input files to link")
            return

        command = [
            self.vm.tool("ark_link"),
            "--output",
            target,
            "--",
            *input_files,
        ]

        run_command(command)

    def run(
        self,
        abc_target: Path,
        ld_lib_path: Path,
        entry: str,
    ) -> float:
        """Run the compiled ABC file with the Ark runtime."""
        ark_path = self.vm.tool("ark")

        command = [
            ark_path,
            f"--boot-panda-files={self.vm.sdk_lib}",
            f"--boot-panda-files={self.vm.stdlib_lib}",
            f"--load-runtimes=ets",
            abc_target,
            entry,
        ]

        return run_command(
            command,
            env={"LD_LIBRARY_PATH": ld_lib_path},
        )
