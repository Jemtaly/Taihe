from taihe.codegen.abi.analyses import PackageABIInfo
from taihe.codegen.ani.analyses import PackageANIInfo
from taihe.codegen.cmake.analyses import PackageCMakeInfo
from taihe.codegen.cmake.writer import CMakeWriter
from taihe.semantics.declarations import (
    PackageDecl,
    PackageGroup,
)
from taihe.utils.analyses import AnalysisManager
from taihe.utils.outputs import OutputManager, FileKind


class CMakeCodeGenerator:
    def __init__(self, om: OutputManager, am: AnalysisManager):
        self.om = om
        self.am = am

    def generate(self, pg: PackageGroup):
        gen_taihe_cmake_file = "taihe_generated.cmake"
        with CMakeWriter(
            self.om,
            f"{gen_taihe_cmake_file}",
            FileKind.CMAKE,
        ) as gen_cmake_target:
            # TODO: input runtime path
            self.emit_runtime_files_list("${OHOS_SDK_NATIVE}/../toolchains/taihe", gen_cmake_target)
            self.emit_generated_includes(gen_cmake_target)
            self.emit_generated_sources(pg, gen_cmake_target)
            self.emit_set_cpp_standard(pg, gen_cmake_target)

    def emit_runtime_files_list(
        self,
        runtime_path: str,
        gen_cmake_target: CMakeWriter,
    ):
        with gen_cmake_target.indented(
            f"set(TAIHE_RUNTIME_DIR",
            f")",
        ):
            gen_cmake_target.writeln(f"{runtime_path}")
        with gen_cmake_target.indented(
            f"set(TAIHE_RUNTIME_INCLUDE",
            f")",
        ):
            gen_cmake_target.writeln(f"${{TAIHE_RUNTIME_DIR}}/include")
        with gen_cmake_target.indented(
            f"set(TAIHE_RUNTIME_SRC",
            f")",
        ):
            gen_cmake_target.writelns(
                f"${{TAIHE_RUNTIME_DIR}}/src/taihe/runtime/string.c",
                f"${{TAIHE_RUNTIME_DIR}}/src/taihe/runtime/object.c",
                f"${{TAIHE_RUNTIME_DIR}}/src/taihe/runtime/runtime.cpp",
            )

    def emit_generated_includes(self, gen_cmake_target: CMakeWriter):
        with gen_cmake_target.indented(
            f"set(TAIHE_GEN_INCLUDE",
            f")",
        ):
            gen_cmake_target.writeln(f"${{CMAKE_CURRENT_LIST_DIR}}/include")

    def emit_generated_sources(
        self,
        pg: PackageGroup,
        gen_cmake_target: CMakeWriter,
    ):
        for pkg in pg.packages:
            self.emit_pkg_source(pkg, gen_cmake_target)
        self.emit_generated_merge_source(pg, gen_cmake_target)

    def emit_generated_merge_source(
        self,
        pg: PackageGroup,
        gen_cmake_target: CMakeWriter,
    ):
        with gen_cmake_target.indented(
            f"set(TAIHE_GEN_SRC",
            f")",
        ):
            for pkg in pg.packages:
                pkg_cmake_info = PackageCMakeInfo.get(self.am, pkg)
                gen_cmake_target.writeln(f"${{{pkg_cmake_info.all_src}}}")

    def emit_set_cpp_standard(
        self,
        pg: PackageGroup,
        gen_cmake_target: CMakeWriter,
    ):
        with gen_cmake_target.indented(
            f"set_source_files_properties(",
            f")",
        ):
            for pkg in pg.packages:
                pkg_cmake_info = PackageCMakeInfo.get(self.am, pkg)
                # TODO: all c++ generated files
                # Add c++ generated files
                gen_cmake_target.writeln(f"${{{pkg_cmake_info.ani_src}}}")
            gen_cmake_target.writelns(
                # Add taihe runtime c++ files
                f"${{TAIHE_RUNTIME_DIR}}/src/taihe/runtime/runtime.cpp",
                # setting
                f"PROPERTIES",
                f"LANGUAGE CXX",
                f'COMPILE_FLAGS "-std=c++17"',
            )

    def emit_pkg_source(
        self,
        p: PackageDecl,
        gen_cmake_target: CMakeWriter,
    ):
        self.emit_pkg_abi_source(p, gen_cmake_target)
        self.emit_pkg_ani_source(p, gen_cmake_target)
        self.emit_pkg_merged_sources(p, gen_cmake_target)

    def emit_pkg_merged_sources(
        self,
        p: PackageDecl,
        gen_cmake_target: CMakeWriter,
    ):
        pkg_cmake_info = PackageCMakeInfo.get(self.am, p)
        with gen_cmake_target.indented(
            f"set({pkg_cmake_info.all_src}",
            f")",
        ):
            gen_cmake_target.writelns(
                f"${{{pkg_cmake_info.abi_src}}}",
                f"${{{pkg_cmake_info.ani_src}}}",
            )

    def emit_pkg_abi_source(
        self,
        p: PackageDecl,
        gen_cmake_target: CMakeWriter,
    ):
        """The generated abi file only has C language source file(.c)."""
        pkg_abi_info = PackageABIInfo.get(self.am, p)
        pkg_cmake_info = PackageCMakeInfo.get(self.am, p)
        with gen_cmake_target.indented(
            f"set({pkg_cmake_info.abi_src}",
            f")",
        ):
            gen_cmake_target.writeln(
                f"${{CMAKE_CURRENT_LIST_DIR}}/src/{pkg_abi_info.src}"
            )

    def emit_pkg_ani_source(
        self,
        p: PackageDecl,
        gen_cmake_target: CMakeWriter,
    ):
        """The generated ani file only has C++ language source file(.cpp)."""
        pkg_ani_info = PackageANIInfo.get(self.am, p)
        pkg_cmake_info = PackageCMakeInfo.get(self.am, p)
        with gen_cmake_target.indented(
            f"set({pkg_cmake_info.ani_src}",
            f")",
        ):
            gen_cmake_target.writeln(
                f"${{CMAKE_CURRENT_LIST_DIR}}/src/{pkg_ani_info.source}"
            )
