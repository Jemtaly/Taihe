from dataclasses import dataclass

from taihe.driver.backend import Backend, BackendConfig
from taihe.driver.contexts import CompilerInstance


@dataclass
class CMakeBridgeBackendConfig(BackendConfig):
    NAME = "cmake-user"
    # TODO: DEPS and self var

    """Use the original function name (instead of "camelCase") in exported ArkTS sources."""

    def construct(self, instance: CompilerInstance) -> Backend:
        from taihe.codegen.cmake.gen_cmake import CMakeCodeGenerator

        class CMakeBridgeBackendImpl(Backend):
            def __init__(self, ci: CompilerInstance):
                super().__init__(ci)
                self._ci = ci

            def generate(self):
                om = self._ci.output_manager
                am = self._ci.analysis_manager
                pg = self._ci.package_group
                CMakeCodeGenerator(om, am).generate(pg)

        return CMakeBridgeBackendImpl(instance)
