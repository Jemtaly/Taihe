"""Generates the parser with ANTLR before running."""

from typing import Iterable, TextIO
from pathlib import Path
from subprocess import check_call
from ast_base import NodeInspector, RootNodeT
import re


class AntlrBuilder:
    """Writes ANTLR grammar file to a buffer."""

    def __init__(self, buf: TextIO, name: str):
        self.buf = buf
        print(f"grammar {name};", file=self.buf)
        print(file=self.buf)

    def rule(self, name: str, rule: str | Iterable[str]):
        if isinstance(rule, str):
            print(f"{name}: {rule};", file=self.buf)
        else:
            print(name, file=self.buf)
            is_first = True
            for r in rule:
                prefix = "\t:" if is_first else "\t|"
                print(prefix, r, file=self.buf)
                is_first = False
            print("\t;", file=self.buf)

        print(file=self.buf)

    def raw(self, s: str):
        print(s, file=self.buf)


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


def gen(root: RootNodeT, out_dir: str):
    ni = NodeInspector(root)
    name_to_antlr = {n.__name__: antlr_name for antlr_name, n in ni.nodes.items()}
    # Compile a regex pattern to match whole identifiers.
    names_escaped = (re.escape(name) for name in name_to_antlr.keys())
    pattern = re.compile(r"\b(" + "|".join(names_escaped) + r")\b")

    def convert_snippet(s: str):
        return pattern.sub(lambda m: name_to_antlr[m.group(0)], s)

    def convert_all(snippet: str | Iterable[str]):
        if isinstance(snippet, str):
            return convert_snippet(snippet)
        else:
            return [convert_snippet(s) for s in snippet]

    p = Path(out_dir)
    p.mkdir(exist_ok=True)
    b = AntlrCompiler(p, root.GRAMMAR_NAME)

    for name, n in ni.nodes.items():
        b.rule(name, convert_all(n.RULE))

    b.raw(root.GRAMMAR_LEXER)
    b.compile()


def main():
    from myast import Prog as RootNode

    gen(RootNode, "antlr_gen")


if __name__ == "__main__":
    main()
