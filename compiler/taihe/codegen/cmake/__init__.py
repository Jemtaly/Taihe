from dataclasses import dataclass

from taihe.driver.backend import Backend, BackendConfig
from taihe.driver.contexts import CompilerInstance


@dataclass
class CMakeBridgeBackendConfig(BackendConfig):
    NAME = "cmake-user"
    gen_abi: bool = False
    gen_ani: bool = False
    # TODO: DEPS and self var

    """Use the original function name (instead of "camelCase") in exported ArkTS sources."""

    def construct(self, instance: CompilerInstance) -> Backend:
        from taihe.codegen.cmake.gen_cmake import CMakeCodeGenerator

        class CMakeBridgeBackendImpl(Backend):
            def __init__(self, ci: CompilerInstance, config: CMakeBridgeBackendConfig):
                super().__init__(ci)
                self._ci = ci
                self.gen_abi = config.gen_abi
                self.gen_ani = config.gen_ani

            def generate(self):
                oc = self._ci.output_config
                am = self._ci.analysis_manager
                pg = self._ci.package_group
                CMakeCodeGenerator(oc, am).generate(pg)

        return CMakeBridgeBackendImpl(instance, self)
