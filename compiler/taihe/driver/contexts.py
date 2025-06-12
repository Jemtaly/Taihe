"""Orchestrates the compilation process.

- BackendRegistry: initializes all known backends
- CompilerInstance: runs the compilation
    - CompilerInstance: scans and parses sources files
    - Backends: post-process the IR
    - Backends: validate the IR
    - Backends: generate the output
"""

from pathlib import Path

from taihe.driver.backend import Backend, BackendConfig
from taihe.parse.convert import (
    AstConverter,
    IgnoredFileReason,
    IgnoredFileWarn,
    normalize_pkg_name,
)
from taihe.semantics.analysis import analyze_semantics
from taihe.semantics.declarations import PackageGroup
from taihe.utils.analyses import AnalysisManager
from taihe.utils.diagnostics import ConsoleDiagnosticsManager, DiagnosticsManager, Level
from taihe.utils.exceptions import AdhocNote
from taihe.utils.outputs import OutputConfig
from taihe.utils.sources import SourceFile, SourceLocation, SourceManager


class CompilerInstance:
    """Helper class for storing key objects.

    CompilerInstance holds key intermediate objects across the compilation
    process, such as the source manager and the diagnostics manager.

    It also provides utility methods for driving the compilation process.
    """

    backends: list[Backend]

    diagnostics_manager: DiagnosticsManager

    source_manager: SourceManager
    package_group: PackageGroup

    analysis_manager: AnalysisManager

    output_config: OutputConfig

    def __init__(self, output_config: OutputConfig, backends: list[BackendConfig]):
        self.diagnostics_manager = ConsoleDiagnosticsManager()
        self.analysis_manager = AnalysisManager(self.diagnostics_manager)
        self.source_manager = SourceManager()
        self.package_group = PackageGroup()
        self.output_config = output_config
        self.backends = [conf.construct(self) for conf in backends]

    def parse(self):
        for src in self.source_manager.sources:
            conv = AstConverter(src, self.diagnostics_manager)
            pkg = conv.convert()
            with self.diagnostics_manager.capture_error():
                self.package_group.add(pkg)
        for b in self.backends:
            b.post_process()

    def validate(self):
        analyze_semantics(self.package_group, self.diagnostics_manager)
        for b in self.backends:
            b.validate()

    def generate(self):
        if not self.output_config.dst_dir:
            return
        if self.diagnostics_manager.current_max_level >= Level.ERROR:
            return
        for b in self.backends:
            b.generate()

    def run(self):
        self.parse()
        self.validate()
        self.generate()
        return not self.diagnostics_manager.current_max_level >= Level.ERROR


def scan(compiler_instance: CompilerInstance, src_dirs: list[Path]):
    """Adds all `.taihe` files inside a directory. Subdirectories are ignored."""
    for src_dir in src_dirs:
        d = Path(src_dir)
        for file in d.iterdir():
            loc = SourceLocation.with_path(file)
            # subdirectories are ignored
            if not file.is_file():
                w = IgnoredFileWarn(IgnoredFileReason.IS_DIRECTORY, loc=loc)
                compiler_instance.diagnostics_manager.emit(w)
            # unexpected file extension
            elif file.suffix != ".taihe":
                target = d.with_suffix(".taihe").name
                w = IgnoredFileWarn(
                    IgnoredFileReason.EXTENSION_MISMATCH,
                    loc=loc,
                    note=AdhocNote(f"consider renaming to `{target}`", loc=loc),
                )
                compiler_instance.diagnostics_manager.emit(w)
            else:
                source = SourceFile(file)
                orig_name = source.pkg_name
                norm_name = normalize_pkg_name(orig_name)
                # invalid package name
                if norm_name != orig_name:
                    loc = SourceLocation(source)
                    compiler_instance.diagnostics_manager.emit(
                        IgnoredFileWarn(
                            IgnoredFileReason.INVALID_PKG_NAME,
                            note=AdhocNote(
                                f"consider using `{norm_name}` instead of `{orig_name}`",
                                loc=loc,
                            ),
                            loc=loc,
                        )
                    )
                # Okay...
                else:
                    compiler_instance.source_manager.add_source(source)
