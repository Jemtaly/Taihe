from typing_extensions import override

from taihe.utils.outputs import DEFAULT_INDENT, FileWriter, OutputConfig


class CMakeWriter(FileWriter):
    """Represents a CMakeLists file."""

    @override
    def __init__(self, oc: OutputConfig, path: str, indent_unit: str = DEFAULT_INDENT):
        super().__init__(
            oc,
            path=path,
            default_indent=indent_unit,
            comment_prefix="# ",
        )
        self.headers: dict[str, None] = {}
