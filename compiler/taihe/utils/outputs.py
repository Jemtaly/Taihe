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

"""Manage output files."""

import os
from abc import ABC, abstractmethod
from collections.abc import Iterator, Sequence
from contextlib import AbstractContextManager, contextmanager
from dataclasses import dataclass, field
from enum import Enum, auto
from io import StringIO
from pathlib import Path
from sys import _getframe, stderr, stdout  # type: ignore
from types import FrameType, TracebackType
from typing import Literal, TextIO

from typing_extensions import Self, override


class DebugLevel(Enum):
    """Controls the code-generator debug info.

    When enabled, the generated code would contain comments, representing the
    location of Python code which generates.
    """

    NONE = auto()
    """Don't print any debug info."""
    CONCISE = auto()
    """Prints function and line number."""
    VERBOSE = auto()
    """Besides CONSICE, also prints code snippet. Could be slow."""


@dataclass(frozen=True)
class RepeatablePathKind:
    """A path kind that allows multiple registrations.

    Represents a category of output paths that can be registered multiple times,
    e.g., generated C source files, runtime C++ source files, etc.
    """

    cmake_var: str | None = None
    """CMake variable name to use when emitting this kind. None means don't emit."""
    overridable: bool = False
    """Whether the CMake variable should be wrapped in if(NOT DEFINED ...)."""
    cxx_standard: bool = False
    """Whether paths in this kind need C++17 standard set in CMake."""


@dataclass(frozen=True)
class UniquePathKind:
    """A path kind that allows only one registration.

    Represents a category of output paths that can only be registered once,
    e.g., a CMake configuration file.
    """

    cmake_var: str | None = None
    """CMake variable name to use when emitting this kind. None means don't emit."""
    overridable: bool = False
    """Whether the CMake variable should be wrapped in if(NOT DEFINED ...)."""


@dataclass(frozen=True)
class RepeatableVarKind:
    """A variable kind that allows multiple registrations.

    Represents a category of string values that can be registered multiple times.
    """

    cmake_var: str | None = None
    """CMake variable name to use when emitting this kind. None means don't emit."""
    overridable: bool = False
    """Whether the CMake variable should be wrapped in if(NOT DEFINED ...)."""


@dataclass(frozen=True)
class UniqueVarKind:
    """A variable kind that allows only one registration.

    Represents a category of string values that can only be registered once.
    """

    cmake_var: str | None = None
    """CMake variable name to use when emitting this kind. None means don't emit."""
    overridable: bool = False
    """Whether the CMake variable should be wrapped in if(NOT DEFINED ...)."""


PathKind = RepeatablePathKind | UniquePathKind
VarKind = RepeatableVarKind | UniqueVarKind


IndentationLevel = tuple[str, ...]


class BaseWriter:
    def __init__(
        self,
        out: TextIO,
        *,
        comment_prefix: str,
        default_indent: str,
        debug_level: DebugLevel = DebugLevel.NONE,
    ):
        """Initialize a code writer with a writable output stream.

        Args:
            out: A writable stream object
            comment_prefix: The prefix for line-comment
            default_indent: The default indentation string for each level of indentation
            newline: The newline character(s) to use
            debug_level: see `DebugLevel` for details
        """
        self._out = out
        self._comment_prefix = comment_prefix
        self._default_indent = default_indent
        self._current_indent: IndentationLevel = ()
        self._indent_stack: list[IndentationLevel] = []
        self._debug_level = debug_level

    @property
    def comment_prefix(self) -> str:
        return self._comment_prefix

    @property
    def default_indent(self) -> str:
        return self._default_indent

    @property
    def current_indent(self) -> IndentationLevel:
        return self._current_indent

    @property
    def indent_stack(self) -> Sequence[IndentationLevel]:
        return self._indent_stack

    def newline(self, _show_debug: bool = True):
        """Writes a newline character."""
        if _show_debug:
            self._write_debug(_getframe(1))  # type: ignore

        self._out.write("\n")

    def writeln(self, line: str = "", _show_debug: bool = True):
        """Writes a single-line string.

        Args:
            line: The line to write (must not contain newlines)
        """
        if _show_debug:
            self._write_debug(_getframe(1))  # type: ignore

        assert "\n" not in line, "use write_block to write multi-line text block"

        # don't write indentation for empty lines
        if line:
            self._out.write("".join(self._current_indent))
            self._out.write(line)

        self._out.write("\n")

    def writelns(self, *lines: str, _show_debug: bool = True):
        """Writes multiple one-line strings.

        Args:
            *lines: One or more lines to write
        """
        if _show_debug:
            self._write_debug(_getframe(1))  # type: ignore

        for line in lines:
            self.writeln(line, _show_debug=False)

    def write_block(self, text_block: str, *, _show_debug: bool = True):
        """Writes a potentially multi-line text block.

        Args:
            text_block: The block of text to write
        """
        if _show_debug:
            self._write_debug(_getframe(1))  # type: ignore

        for line in text_block.splitlines():
            self.writeln(line, _show_debug=False)

    def write_comment(self, comment: str, *, _show_debug: bool = True):
        """Writes a comment block, prefixing each line with the comment prefix.

        Indents the comment block according to the current indentation level.
        Handles multi-line comments by splitting the input string.

        Args:
            comment: The comment text to write. Can be multi-line.
        """
        if _show_debug:
            self._write_debug(_getframe(1))  # type: ignore

        for line in comment.splitlines():
            self.writeln(self._comment_prefix + line, _show_debug=False)

    @contextmanager
    def set_indent(
        self,
        indent: IndentationLevel,
        *,
        _show_debug: bool = True,
    ) -> Iterator[Self]:
        if _show_debug:
            self._write_debug(_getframe(2))  # type: ignore

        self._indent_stack.append(self._current_indent)
        self._current_indent = indent
        try:
            yield self
        finally:
            self._current_indent = self._indent_stack.pop()

    @contextmanager
    def inc_indent(
        self,
        indent: str | None = None,
        *,
        _show_debug: bool = True,
    ) -> Iterator[Self]:
        if _show_debug:
            self._write_debug(_getframe(2))  # type: ignore

        if indent is None:
            indent = self._default_indent
        self._indent_stack.append(self._current_indent)
        self._current_indent = (*self._current_indent, indent)
        try:
            yield self
        finally:
            self._current_indent = self._indent_stack.pop()

    @contextmanager
    def indented(
        self,
        prologue: str | None,
        epilogue: str | None,
        *,
        indent: str | None = None,
        _show_debug: bool = True,
    ) -> Iterator[Self]:
        """Context manager that indents code within its scope.

        Args:
            prologue: Optional text to write before indentation
            epilogue: Optional text to write after indentation
            indent: Optional string to use for indentation (overrides default)

        Returns:
            A context manager that yields this BaseWriter
        """
        if _show_debug:
            self._write_debug(_getframe(2))  # type: ignore

        if prologue is not None:
            self.writeln(prologue, _show_debug=False)

        with self.inc_indent(indent, _show_debug=False):
            yield self

        if epilogue is not None:
            self.writeln(epilogue, _show_debug=False)

    def _write_debug(self, f: FrameType):
        if self._debug_level == DebugLevel.NONE:
            return

        taihe_dir = Path(__file__).parent.parent
        file_name = Path(f.f_code.co_filename).relative_to(taihe_dir).as_posix()
        self.write_comment(
            f"CODEGEN-DEBUG: {f.f_code.co_name} in {file_name}:{f.f_lineno}",
            _show_debug=False,
        )


class FileWriter(BaseWriter):
    def __init__(
        self,
        om: "OutputManager",
        relative_path: str,
        file_kind: PathKind,
        *,
        default_indent: str,
        comment_prefix: str,
    ):
        super().__init__(
            out=StringIO(),
            default_indent=default_indent,
            comment_prefix=comment_prefix,
            debug_level=om.debug_level,
        )
        self._om = om
        self.relative_path = relative_path
        self.kind = file_kind

    def __enter__(self):
        return self

    def __exit__(
        self,
        exc_type: type[BaseException] | None,
        exc_val: BaseException | None,
        exc_tb: TracebackType | None,
    ) -> bool:
        del exc_val, exc_tb, exc_type
        with self._om.open(self.kind, self.relative_path) as f:
            self.write_prologue(f)
            self.write_body(f)
            self.write_epilogue(f)
        return False

    def write_body(self, f: TextIO):
        assert isinstance(self._out, StringIO)
        f.write(self._out.getvalue())

    def write_prologue(self, f: TextIO):
        del f

    def write_epilogue(self, f: TextIO):
        del f


@dataclass
class OutputConfig(ABC):
    debug_level: DebugLevel = field(default=DebugLevel.NONE, kw_only=True)

    @abstractmethod
    def construct(self) -> "OutputManager":
        """Constructs an OutputManager based on this configuration."""


class OutputManager(ABC):
    """Abstract base class for output managers."""

    _path_entries: dict[PathKind, list[str]]
    _var_entries: dict[VarKind, list[str]]

    debug_level: DebugLevel

    def __init__(self, *, debug_level: DebugLevel):
        self._path_entries = {}
        self._var_entries = {}
        self.debug_level = debug_level

    def get_paths(self, kind: PathKind) -> list[str]:
        """Get all registered paths of the given kind."""
        return self._path_entries.get(kind, [])

    def get_vars(self, kind: VarKind) -> list[str]:
        """Get all registered values of the given kind."""
        return self._var_entries.get(kind, [])

    def register_path(self, kind: PathKind, path: Path) -> None:
        """Register an absolute path with the given kind."""
        self._add_path_entry(kind, path.as_posix())

    def _add_path_entry(self, kind: PathKind, path_str: str) -> None:
        """Register a path string with the given kind."""
        if isinstance(kind, UniquePathKind):
            if kind in self._path_entries:
                raise ValueError(
                    f"Path kind {kind!r} already has a registered path, "
                    f"cannot register another."
                )
            self._path_entries[kind] = [path_str]
        else:
            self._path_entries.setdefault(kind, []).append(path_str)

    def register_var(self, kind: VarKind, value: str) -> None:
        """Register a variable value with the given kind."""
        if isinstance(kind, UniqueVarKind):
            if kind in self._var_entries:
                raise ValueError(
                    f"Var kind {kind!r} already has a registered value, "
                    f"cannot register another."
                )
            self._var_entries[kind] = [value]
        else:
            self._var_entries.setdefault(kind, []).append(value)

    def register_runtime_path(self, kind: PathKind, relative: str) -> None:
        """Register a runtime path relative to the runtime directory.

        The base implementation is a no-op. Subclasses like CMakeOutputManager
        override this to resolve relative paths against the runtime directory.
        """
        return

    def register_generated_path(self, kind: PathKind, relative: str) -> None:
        """Register a generated path relative to the generated output directory.

        The base implementation is a no-op. Subclasses like BasicOutputManager
        and CMakeOutputManager override this to resolve relative paths against
        the output directory.
        """
        return

    def post_generate(self) -> None:
        """Hook called after all files have been generated."""
        return

    @contextmanager
    def open(self, kind: PathKind, relative_path: str) -> Iterator[TextIO]:
        """Opens a file for writing and registers it as a generated path."""
        self.register_generated_path(kind, relative_path)

        with self._open_impl(relative_path) as f:
            yield f

    @abstractmethod
    def _open_impl(self, relative_path: str) -> AbstractContextManager[TextIO]:
        """Opens a file for writing."""


@dataclass
class NullOutputConfig(OutputConfig):
    def construct(self) -> OutputManager:
        class NullOutputManager(OutputManager):
            def __init__(self, *, debug_level: DebugLevel):
                super().__init__(debug_level=debug_level)

            def _open_impl(self, relative_path: str):
                return Path(os.devnull).open("w", encoding="utf-8")

        return NullOutputManager(debug_level=self.debug_level)


@dataclass
class DebugOutputConfig(OutputConfig):
    target_desc: Literal["stderr", "stdout"]

    def construct(self) -> OutputManager:
        class DebugOutputManager(OutputManager):
            def __init__(
                self,
                target_desc: Literal["stderr", "stdout"],
                *,
                debug_level: DebugLevel,
            ):
                super().__init__(debug_level=debug_level)
                match target_desc:
                    case "stderr":
                        self.target = stderr
                    case "stdout":
                        self.target = stdout

            @contextmanager
            def _open_impl(self, relative_path: str):
                self.target.write(f"// File: {relative_path}\n")
                yield self.target
                self.target.write(f"// End of file: {relative_path}\n\n")

        return DebugOutputManager(self.target_desc, debug_level=self.debug_level)


@dataclass
class BasicOutputConfig(OutputConfig):
    dst_dir: Path

    def construct(self) -> OutputManager:
        return BasicOutputManager(
            self.dst_dir,
            debug_level=self.debug_level,
        )


class BasicOutputManager(OutputManager):
    """Manages the creation and saving of output files."""

    def __init__(
        self,
        dst_dir: Path,
        *,
        debug_level: DebugLevel,
    ):
        super().__init__(debug_level=debug_level)
        self.dst_dir = dst_dir

    @override
    def register_generated_path(self, kind: PathKind, relative: str) -> None:
        self._add_path_entry(kind, (self.dst_dir / relative).as_posix())

    def _open_impl(self, relative_path: str):
        file_path = self.dst_dir / relative_path
        file_path.parent.mkdir(exist_ok=True, parents=True)
        return file_path.open("w", encoding="utf-8")


#################################
# Cmake code generation related #
#################################


# Well-known path kinds for generated files
GENERATED_C_SRC = RepeatablePathKind(cmake_var="TAIHE_GEN_C_SRC")
GENERATED_CXX_SRC = RepeatablePathKind(cmake_var="TAIHE_GEN_CXX_SRC", cxx_standard=True)

# Well-known path kinds for runtime files
RUNTIME_INCLUDE = RepeatablePathKind(
    cmake_var="TAIHE_RUNTIME_INCLUDE_INNER", overridable=True
)
RUNTIME_C_SRC = RepeatablePathKind(
    cmake_var="TAIHE_RUNTIME_C_SRC_INNER", overridable=True
)
RUNTIME_CXX_SRC = RepeatablePathKind(
    cmake_var="TAIHE_RUNTIME_CXX_SRC_INNER", overridable=True, cxx_standard=True
)

# Well-known path kinds for generated include directories
GENERATED_INCLUDE = RepeatablePathKind(cmake_var="TAIHE_GEN_INCLUDE")

# Well-known var kinds for macro definitions
MACRO_DEFINITION = RepeatableVarKind(cmake_var="TAIHE_MACRO_DEFINITIONS")

# Path kinds without CMake variables (just categorization)
GENERATED_C_HEADER = RepeatablePathKind()
GENERATED_CPP_HEADER = RepeatablePathKind()
GENERATED_C_TEMPLATE = RepeatablePathKind()
GENERATED_ETS = RepeatablePathKind()
GENERATED_DTS = RepeatablePathKind()
GENERATED_CMAKE = UniquePathKind()
GENERATED_TAIHE = RepeatablePathKind()
GENERATED_OTHER = RepeatablePathKind()


class CMakeWriter(FileWriter):
    """Represents a CMake file."""

    def __init__(
        self,
        om: OutputManager,
        relative_path: str,
        file_kind: PathKind,
    ):
        super().__init__(
            om,
            relative_path=relative_path,
            file_kind=file_kind,
            default_indent="    ",
            comment_prefix="# ",
        )
        self.headers: dict[str, None] = {}


@dataclass
class CMakeOutputConfig(BasicOutputConfig):
    runtime_dir: Path = field(kw_only=True)

    def construct(self) -> OutputManager:
        return CMakeOutputManager(
            self.dst_dir,
            debug_level=self.debug_level,
            runtime_dir=self.runtime_dir,
        )


class CMakeOutputManager(BasicOutputManager):
    """Manages the generation of CMake files for Taihe runtime."""

    def __init__(
        self,
        dst_dir: Path,
        *,
        debug_level: DebugLevel,
        runtime_dir: Path,
    ):
        super().__init__(dst_dir, debug_level=debug_level)
        self.runtime_dir = runtime_dir
        self.target = CMakeWriter(self, "TaiheGenerated.cmake", GENERATED_CMAKE)
        self.generated_path = "${CMAKE_CURRENT_LIST_DIR}"

    @override
    def register_runtime_path(self, kind: PathKind, relative: str) -> None:
        full_path = self.runtime_dir / relative
        self.register_path(kind, full_path)

    @override
    def register_generated_path(self, kind: PathKind, relative: str) -> None:
        self._add_path_entry(kind, f"${{TAIHE_GEN_DIR}}/{relative}")

    @override
    def post_generate(self):
        with self.target:
            self._emit_generated_dir()
            self._emit_all_path_entries()
            self._emit_all_var_entries()
            self._emit_aggregates()
            self._emit_set_cxx_standard()

    def _emit_generated_dir(self):
        with self.target.indented(
            "if(NOT DEFINED TAIHE_GEN_DIR)",
            "endif()",
        ):
            with self.target.indented(
                "set(TAIHE_GEN_DIR",
                ")",
            ):
                self.target.writelns(
                    f"{self.generated_path}",
                )

    def _emit_all_path_entries(self):
        """Emit CMake set() calls for all registered path kinds with cmake_var."""
        for kind, paths in self._path_entries.items():
            if kind.cmake_var is None:
                continue
            if kind.overridable:
                with self.target.indented(
                    f"if(NOT DEFINED {kind.cmake_var})",
                    "endif()",
                ):
                    self._emit_set(kind.cmake_var, paths)
            else:
                self._emit_set(kind.cmake_var, paths)

    def _emit_all_var_entries(self):
        """Emit CMake set() calls for all registered var kinds with cmake_var."""
        for kind, values in self._var_entries.items():
            if kind.cmake_var is None:
                continue
            if kind.overridable:
                with self.target.indented(
                    f"if(NOT DEFINED {kind.cmake_var})",
                    "endif()",
                ):
                    self._emit_set(kind.cmake_var, values)
            else:
                self._emit_set(kind.cmake_var, values)

    def _emit_aggregates(self):
        """Emit aggregate CMake variables that combine other variables."""
        # TAIHE_RUNTIME_INCLUDE aggregates TAIHE_RUNTIME_INCLUDE_INNER
        runtime_include_refs: list[str] = []
        for kind in self._path_entries:
            if kind.cmake_var == "TAIHE_RUNTIME_INCLUDE_INNER":
                runtime_include_refs.append("${TAIHE_RUNTIME_INCLUDE_INNER}")
        if runtime_include_refs:
            self._emit_set("TAIHE_RUNTIME_INCLUDE", runtime_include_refs)

        # TAIHE_RUNTIME_SRC aggregates runtime C and CXX sources
        runtime_src_refs: list[str] = []
        for kind in self._path_entries:
            if kind.cmake_var in (
                "TAIHE_RUNTIME_C_SRC_INNER",
                "TAIHE_RUNTIME_CXX_SRC_INNER",
            ):
                runtime_src_refs.append(f"${{{kind.cmake_var}}}")
        if runtime_src_refs:
            self._emit_set("TAIHE_RUNTIME_SRC", runtime_src_refs)

        # TAIHE_GEN_SRC aggregates generated C and CXX sources
        gen_src_refs: list[str] = []
        for kind in self._path_entries:
            if kind.cmake_var in ("TAIHE_GEN_C_SRC", "TAIHE_GEN_CXX_SRC"):
                gen_src_refs.append(f"${{{kind.cmake_var}}}")
        if gen_src_refs:
            self._emit_set("TAIHE_GEN_SRC", gen_src_refs)

    def _emit_set_cxx_standard(self):
        """Emit set_source_files_properties for C++ standard."""
        cxx_vars: list[str] = []
        for kind in self._path_entries:
            if kind.cmake_var and kind.cxx_standard:
                cxx_vars.append(f"${{{kind.cmake_var}}}")
        if cxx_vars:
            with self.target.indented(
                "set_source_files_properties(",
                ")",
            ):
                self.target.writelns(
                    *cxx_vars,
                    "PROPERTIES",
                    "LANGUAGE CXX",
                    'COMPILE_FLAGS "-std=c++17"',
                )

        # Emit compile definitions from MACRO_DEFINITION var kind
        macro_defs: list[str] = self._var_entries.get(MACRO_DEFINITION, [])
        if macro_defs:
            with self.target.indented(
                "add_compile_definitions(",
                ")",
            ):
                for macro in macro_defs:
                    self.target.writelns(macro)

    def _emit_set(self, var_name: str, values: list[str]):
        with self.target.indented(
            f"set({var_name}",
            ")",
        ):
            for v in values:
                self.target.writelns(v)
