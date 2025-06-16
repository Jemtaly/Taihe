from taihe.codegen.cmake.writer import CMakeWriter
from taihe.utils.analyses import AnalysisManager
from taihe.utils.outputs import FileKind, OutputManager


class CMakeCodeGenerator:
    def __init__(self, om: OutputManager, am: AnalysisManager):
        self.om = om
        self.am = am

    def post_generate(self):
        gen_taihe_cmake_file = "taihe_generated.cmake"
        with CMakeWriter(
            self.om,
            f"{gen_taihe_cmake_file}",
            FileKind.CMAKE,
        ) as gen_cmake_target:
            # TODO: input runtime path
            self.emit_runtime_files_list(gen_cmake_target)
            self.emit_generated_dir("${CMAKE_CURRENT_LIST_DIR}", gen_cmake_target)
            self.emit_generated_includes(gen_cmake_target)
            self.emit_generated_sources(gen_cmake_target)
            self.emit_set_cpp_standard(gen_cmake_target)

    def emit_runtime_files_list(
        self,
        gen_cmake_target: CMakeWriter,
    ):
        with gen_cmake_target.indented(
            f"if(NOT DEFINED TAIHE_RUNTIME_INCLUDE_INNER)",
            f"endif()",
        ):
            with gen_cmake_target.indented(
                f"set(TAIHE_RUNTIME_INCLUDE_INNER",
                f")",
            ):
                gen_cmake_target.writeln(f"{self.om.runtime_include_dir}")
        with gen_cmake_target.indented(
            f"if(NOT DEFINED TAIHE_RUNTIME_SRC_INNER)",
            f"endif()",
        ):
            with gen_cmake_target.indented(
                f"set(TAIHE_RUNTIME_SRC_INNER",
                f")",
            ):
                gen_cmake_target.writelns(
                    f"{self.om.runtime_src_dir}/string.c",
                    f"{self.om.runtime_src_dir}/object.c",
                    f"{self.om.runtime_src_dir}/runtime.cpp",
                )
        with gen_cmake_target.indented(
            f"set(TAIHE_RUNTIME_INCLUDE",
            f")",
        ):
            gen_cmake_target.writeln(f"${{TAIHE_RUNTIME_INCLUDE_INNER}}")
        with gen_cmake_target.indented(
            f"set(TAIHE_RUNTIME_SRC",
            f")",
        ):
            gen_cmake_target.writeln(f"${{TAIHE_RUNTIME_SRC_INNER}}")

    def emit_generated_dir(
        self,
        generated_path: str,
        gen_cmake_target: CMakeWriter,
    ):
        with gen_cmake_target.indented(
            f"if(NOT DEFINED TAIHE_GEN_DIR)",
            f"endif()",
        ):
            with gen_cmake_target.indented(
                f"set(TAIHE_GEN_DIR",
                f")",
            ):
                gen_cmake_target.writeln(f"{generated_path}")

    def emit_generated_includes(self, gen_cmake_target: CMakeWriter):
        with gen_cmake_target.indented(
            f"set(TAIHE_GEN_INCLUDE",
            f")",
        ):
            gen_cmake_target.writeln(f"${{TAIHE_GEN_DIR}}/include")

    def emit_generated_sources(
        self,
        gen_cmake_target: CMakeWriter,
    ):
        self.emit_generated_c_sources(gen_cmake_target)
        self.emit_generated_cpp_sources(gen_cmake_target)
        self.emit_generated_merge_source(gen_cmake_target)

    def emit_generated_c_sources(self, gen_cmake_target: CMakeWriter):
        with gen_cmake_target.indented(
            f"set(TAIHE_GEN_C_SRC",
            f")",
        ):
            for file in self.om.get_files_by_kind(FileKind.C_SOURCE):
                gen_cmake_target.writeln(f"${{TAIHE_GEN_DIR}}/{file.relative_path}")

    def emit_generated_cpp_sources(self, gen_cmake_target: CMakeWriter):
        with gen_cmake_target.indented(
            f"set(TAIHE_GEN_CXX_SRC",
            f")",
        ):
            for file in self.om.get_files_by_kind(FileKind.CPP_SOURCE):
                gen_cmake_target.writeln(f"${{TAIHE_GEN_DIR}}/{file.relative_path}")

    def emit_generated_merge_source(
        self,
        gen_cmake_target: CMakeWriter,
    ):
        with gen_cmake_target.indented(
            f"set(TAIHE_GEN_SRC",
            f")",
        ):
            gen_cmake_target.writelns(
                f"${{TAIHE_GEN_C_SRC}}",
                f"${{TAIHE_GEN_CXX_SRC}}",
            )

    def emit_set_cpp_standard(
        self,
        gen_cmake_target: CMakeWriter,
    ):
        with gen_cmake_target.indented(
            f"set_source_files_properties(",
            f")",
        ):
            gen_cmake_target.writelns(
                f"${{TAIHE_GEN_CXX_SRC}}",
                # Add taihe runtime c++ files
                f"{self.om.runtime_src_dir}/runtime.cpp",
                # setting
                f"PROPERTIES",
                f"LANGUAGE CXX",
                f'COMPILE_FLAGS "-std=c++17"',
            )
