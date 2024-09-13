from typing import Iterable, TextIO, Type
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


import myast as ast
from myast import Node

NodeT = Type[Node]

import inspect
import re


def iter_nodes(root_node: NodeT) -> Iterable[NodeT]:
    mod = inspect.getmodule(root_node)
    assert mod
    for c in mod.__dict__.values():
        if inspect.isclass(c) and issubclass(c, Node) and c != Node:
            yield c


def main():
    root_node = ast.Expr
    nodes = list(iter_nodes(root_node))

    node_names = [n.node_name() for n in nodes]
    # Compile a regex pattern to match whole identifiers
    pattern = re.compile(r"\b(" + "|".join(re.escape(s) for s in node_names) + r")\b")
    # Python class style 'FooBarBaz' to ANTLR rule style 'fooBarBaz'.
    to_antlr_style = {s: s[0].lower() + s[1:] for s in node_names}

    def convert_snippet(snippet):
        return pattern.sub(lambda m: to_antlr_style[m.group(0)], snippet)

    p = Path("./antlr_gen")
    p.mkdir(exist_ok=True)
    b = AntlrCompiler(p, root_node.GRAMMAR_NAME)

    for n in nodes:
        b.rule(to_antlr_style[n.node_name()], convert_snippet(n.RULE))

    b.raw(root_node.GRAMMAR_LEXER)
    b.compile()


if __name__ == "__main__":
    main()
