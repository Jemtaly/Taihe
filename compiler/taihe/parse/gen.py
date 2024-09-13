"""Generates the parser with ANTLR before running."""

from typing import Iterable, TextIO
from pathlib import Path
from subprocess import check_call
from visitor import NodeT, class_name_to_antlr_name, iter_nodes
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
            is_first = False
            for r in rule:
                prefix = "\t|" if is_first else "\t:"
                print(prefix, r, file=self.buf)
                is_first = True
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


def gen(root_node: NodeT, out_dir: str):
    grammar_name = getattr(root_node, "GRAMMAR_NAME")
    grammar_lexer = getattr(root_node, "GRAMMAR_LEXER")
    assert isinstance(grammar_name, str)
    assert isinstance(grammar_lexer, str)

    nodes = list(iter_nodes(root_node))
    node_names = [n.node_name() for n in nodes]
    # Compile a regex pattern to match whole identifiers.
    pattern = re.compile(r"\b(" + "|".join(re.escape(s) for s in node_names) + r")\b")
    # Python class style 'FooBarBaz' to ANTLR rule style 'fooBarBaz'.
    to_antlr_style = {s: class_name_to_antlr_name(s) for s in node_names}

    def convert_snippet(s: str):
        return pattern.sub(lambda m: to_antlr_style[m.group(0)], s)

    def convert_all(snippet: str | Iterable[str]):
        if isinstance(snippet, str):
            return convert_snippet(snippet)
        else:
            return [convert_snippet(s) for s in snippet]

    p = Path(out_dir)
    p.mkdir(exist_ok=True)
    b = AntlrCompiler(p, grammar_name)

    for n in nodes:
        b.rule(to_antlr_style[n.node_name()], convert_all(n.RULE))

    b.raw(grammar_lexer)
    b.compile()


def main():
    from myast import Prog as RootNode

    gen(RootNode, "antlr_gen")


if __name__ == "__main__":
    main()
