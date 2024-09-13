from typing import Iterable, TextIO
from pathlib import Path
from subprocess import check_call


class AntlrBuilder:
    """Writes ANTLR grammar file to a buffer."""

    def __init__(self, buf: TextIO, name: str):
        self.buf = buf
        print(f"grammar {name};", file=self.buf)
        print(file=self.buf)

    def rule(self, name: str, rule: str | Iterable[str]):
        print(name, file=self.buf)
        if isinstance(rule, str):
            rs = (rule,)
        else:
            rs = rule

        is_first = False
        for r in rs:
            prefix = "\t|" if is_first else "\t:"
            print(prefix, r, file=self.buf)
            is_first = True
        print("\t;", file=self.buf)
        print(file=self.buf)


class AntlrCompiler(AntlrBuilder):
    """Writes ANTLR grammar file to a directory and complies it."""

    def __init__(self, out_dir: Path, name: str):
        self._out_dir = out_dir
        self._g4_path = out_dir / f"{name}.g4"
        f = open(self._g4_path, "w")
        super().__init__(f, name)

    def compile(self):
        self.buf.close()
        args = [
            "antlr4",
            "-Dlanguage=Python3",
            "-no-listener",
            self._g4_path,
        ]
        check_call(args)


def main():
    p = Path("./antlr-gen")
    p.mkdir(exist_ok=True)
    b = AntlrCompiler(p, "Taihe")
    b.rule("specification", "SEMI")
    b.rule("fragment LETTER", ["'_'", "'A' .. 'Z'"])
    b.rule("SEMI", "';'")
    b.compile()


if __name__ == "__main__":
    main()
