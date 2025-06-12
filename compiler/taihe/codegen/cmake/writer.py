from typing_extensions import override

from taihe.utils.outputs import DEFAULT_INDENT, FileKind, FileWriter, OutputManager


class CMakeWriter(FileWriter):
    """Represents a CMakeLists file."""

    @override
    def __init__(
        self,
        om: OutputManager,
        path: str,
        file_kind: FileKind,
        indent_unit: str = DEFAULT_INDENT,
    ):
        super().__init__(
            om,
            path=path,
            file_kind=file_kind,
            default_indent=indent_unit,
            comment_prefix="# ",
        )
        self.headers: dict[str, None] = {}
