from dataclasses import dataclass
from typing import ClassVar

from taihe.driver.backend import Backend, BackendConfig
from taihe.driver.contexts import CompilerInstance


@dataclass
class CJBridgeBackendConfig(BackendConfig):
    NAME = "cj-bridge"
    DEPS: ClassVar = ["abi-header"]

    keep_name: bool = False
    """Use the original function name (instead of "camelCase") in exported Cangjie sources."""

    def construct(self, instance: "CompilerInstance") -> Backend:
        from taihe.codegen.cj.gen_cj import CJCodeGenerator

        class CJBackendImpl(Backend):
            def __init__(self, ci: "CompilerInstance"):
                super().__init__(ci)
                self._ci = ci

            def generate(self):
                om = self._ci.output_manager
                am = self._ci.analysis_manager
                pg = self._ci.package_group
                CJCodeGenerator(om, am).generate(pg)

        return CJBackendImpl(instance)
