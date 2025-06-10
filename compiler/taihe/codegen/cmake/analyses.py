from taihe.semantics.declarations import PackageDecl
from taihe.utils.analyses import AbstractAnalysis, AnalysisManager


class PackageCMakeInfo(AbstractAnalysis[PackageDecl]):
    def __init__(self, am: AnalysisManager, p: PackageDecl) -> None:
        super().__init__(am, p)
        cmake_safe_name = p.name.replace(".", "_").upper()
        self.abi_src = f"TAIHE_{cmake_safe_name}_ABI_SOURCE"
        self.ani_src = f"TAIHE_{cmake_safe_name}_ANI_SOURCE"
        self.all_src = f"TAIHE_{cmake_safe_name}_SOURCE"
