"""Manages diagnostics messages such as semantic errors."""

from collections.abc import Callable, Iterable
from contextlib import contextmanager
from dataclasses import dataclass, field
from enum import Enum
from sys import stderr
from typing import (
    TYPE_CHECKING,
    ClassVar,
    Optional,
    TypeVar,
)

if TYPE_CHECKING:
    from taihe.utils.sources import SourceLocation


T = TypeVar("T")


class AnsiStyle:
    RED = "\033[31m"
    GREEN = "\033[32m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"

    RESET = "\033[39m"
    BRIGHT = "\033[1m"
    RESET_ALL = "\033[0m"


def _passthrough(x):
    return x


def _discard(x):
    del x
    return ""


FilterT = Callable[[str], str]


###################
# The Basic Types #
###################


class Level(Enum):
    NOTE = 0
    WARN = 1
    ERROR = 2
    FATAL = 3


@dataclass
class DiagBase:
    """The base class for diagnostic messages."""

    LEVEL: ClassVar[Level] = Level.ERROR
    DESC: ClassVar[str] = "<todo-diagbase-desc>"
    STYLE = AnsiStyle.CYAN

    MSG: ClassVar[str] = "<todo-diagbase-msg>"
    """The template for generating diagnostic message."""

    loc: Optional["SourceLocation"] = field(default=None, kw_only=True)
    """The source location where the diagnostic refers to."""

    def format_msg(self) -> str:
        """Returns the rendered diagnostic mesasge."""
        return self.MSG.format(**self.__dict__)

    def notes(self) -> Iterable["DiagNote"]:
        """Returns the associated notes."""
        return ()

    def _format(self, f: FilterT):
        return f"{f(AnsiStyle.BRIGHT)}{self.loc or '???'}: {f(self.STYLE)}{self.DESC}{f(AnsiStyle.RESET)}: {self.format_msg()}{f(AnsiStyle.RESET_ALL)}"

    def __str__(self) -> str:
        return self._format(_discard)


######################################
# Base classes with different levels #
######################################


@dataclass
class DiagNote(DiagBase):
    LEVEL = Level.NOTE
    DESC = "note"
    STYLE = AnsiStyle.CYAN


@dataclass
class DiagWarn(DiagBase):
    LEVEL = Level.WARN
    DESC = "warning"
    STYLE = AnsiStyle.MAGENTA


@dataclass
class DiagError(DiagBase, Exception):
    LEVEL = Level.ERROR
    DESC = "error"
    STYLE = AnsiStyle.RED


@dataclass
class DiagFatalError(DiagError):
    LEVEL = Level.FATAL
    DESC = "fatal"


########################


@dataclass
class AdhocDiagNote(DiagNote):
    """Helper for constructing an ad-hoc DiagNote."""

    msg: str

    def format_msg(self) -> str:
        return self.msg


class DiagnosticsManager:
    """Manages diagnostic messages."""

    def __init__(self):
        if stderr.isatty():
            self._color_filter_fn = _passthrough
        else:
            self._color_filter_fn = _discard

    # TODO: could be slow.
    def _render_lines(self, loc: "SourceLocation"):
        MAX_LINE_NO_SPACE = 5
        if loc.line == 0:
            return

        line_contents = loc.file.read()
        if loc.line - 1 > len(line_contents):
            return

        line_content = line_contents[loc.line - 1].rstrip("\n")
        print(f"{loc.line:{MAX_LINE_NO_SPACE}} | {line_content}")

        if loc.column == 0 or len(line_content) == 0:
            return

        col_begin = min(loc.column - 1, len(line_content) - 1)
        col_end = min(loc.column - 1 + max(loc.span, 1), len(line_content))
        markers = "^" * (col_end - col_begin)
        f = self._color_filter_fn
        print(
            f"{'':{MAX_LINE_NO_SPACE}} | {f(AnsiStyle.GREEN + AnsiStyle.BRIGHT)}{'':{col_begin}}{markers}{f(AnsiStyle.RESET_ALL)}"
        )

    def _render(self, d: DiagBase):
        print(f"{d._format(self._color_filter_fn)}")
        if d.loc:
            self._render_lines(d.loc)

    def emit(self, diag: DiagBase):
        """Emits a new diagnostic message."""
        self._render(diag)
        for n in diag.notes():
            self._render(n)

    @contextmanager
    def capture_error(self):
        """Captures "error" and "fatal" diagnostics using context manager.

        Example:
        ```
        # Emit the error and prevent its propogation
        with diag_mgr.capture_error():
          foo();
          raise DiagError(...)
          bar();

        # Equivalent to:
        try:
          foo();
          raise DiagError(...)
          bar();
        except DiagError as e:
            diag_mgr.emit(e)
        ```
        """
        try:
            yield None
        except DiagError as e:
            self.emit(e)

    def for_each(self, xs: Iterable[T], cb: Callable[[T], bool | None]) -> bool:
        """Calls `cb` for each element. Records and recovers from `DiagError`s.

        Returns `True` if no errors are encountered.
        """
        no_error = True
        for x in xs:
            try:
                if cb(x):
                    return True
            except DiagError as e:
                self.emit(e)
                no_error = False
        return no_error
