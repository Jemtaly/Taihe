from typing_extensions import override

from taihe.utils.analyses import AnalysisManager
from taihe.utils.outputs import OutputManager


class CMakeOutputManager(OutputManager):
    @override
    def post_generate(self, am: AnalysisManager) -> None:
        from taihe.codegen.cmake.gen_cmake import CMakeCodeGenerator

        CMakeCodeGenerator(self, am).post_generate()
