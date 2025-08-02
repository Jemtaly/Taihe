from typing import TextIO

from typing_extensions import override

from taihe.utils.outputs import DEFAULT_INDENT, FileKind, FileWriter, OutputManager


class CJSourceWriter(FileWriter):
    """Represents a CJ source file."""

    def __init__(
        self,
        om: OutputManager,
        relative_path: str,
        file_kind: FileKind,
        indent_unit: str = DEFAULT_INDENT,
    ):
        super().__init__(
            om,
            relative_path=relative_path,
            file_kind=file_kind,
            default_indent=indent_unit,
            comment_prefix="// ",
        )
        self.headers: dict[str, None] = {}

    @override
    def write_prologue(self, f: TextIO):
        pass

    @override
    def write_epilogue(self, f: TextIO):
        pass

    def add_include(self, *headers: str):
        pass
