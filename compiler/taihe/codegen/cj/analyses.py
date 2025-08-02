from taihe.utils.analyses import AbstractAnalysis, AnalysisManager
from taihe.semantics.declarations import (
    EnumDecl,
    GlobFuncDecl,
    IfaceDecl,
    IfaceMethodDecl,
    IfaceParentDecl,
    PackageDecl,
    PackageGroup,
    ParamDecl,
    StructDecl,
    StructFieldDecl,
    UnionDecl,
    UnionFieldDecl,
)
from typing_extensions import override
class PackageCJInfo(AbstractAnalysis[PackageDecl]):
    def __init__(self, am: AnalysisManager, p: PackageDecl) -> None:
        self.source = f"{p.name}.cj"

    @classmethod
    @override
    def _create(cls, am: AnalysisManager, p: PackageDecl) -> "PackageCJInfo":
        return PackageCJInfo(am, p)
    

class GlobFuncAbiInfo(AbstractAnalysis[GlobFuncDecl]):
    def __init__(self, am: AnalysisManager, f: GlobFuncDecl) -> None:
        segments = [*f.parent_pkg.segments, f.name]
        self.mangled_name = encode(segments, DeclKind.FUNC)

    @classmethod
    @override
    def _create(cls, am: AnalysisManager, f: GlobFuncDecl) -> "GlobFuncAbiInfo":
        return GlobFuncAbiInfo(am, f)