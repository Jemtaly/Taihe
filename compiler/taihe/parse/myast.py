from dataclasses import dataclass
from typing import List

from ast_base import Node, RootNode

_LEXER = r"""
NEWLINE : [\r\n]+ -> skip;
INT     : [0-9]+;
WS      : ' '+ -> channel(HIDDEN);
"""


@dataclass
class Num(Node):
    RULE = "v=INT"

    value: int

    @classmethod
    def from_antlr(cls, ctx):
        return Num(int(ctx.v.text))


@dataclass
class Prog(RootNode):
    GRAMMAR_NAME = "Demo"
    GRAMMAR_LEX_RULES = _LEXER
    RULE = [
        "v=Array EOF",
        "u=Expr EOF",
    ]

    data: Node

    @classmethod
    def from_antlr(cls, ctx):
        return Prog(ctx.v)


@dataclass
class Expr(Node):
    RULE = [
        "Expr ('*' | '/') Expr",
        "Expr ('+' | '-') Expr",
        "INT",
        "'(' Expr ')'",
    ]


def csf(field: str, token: str):
    """Comma-separated fields."""
    return f"{field}+={token} (',' {field}+={token})*"


@dataclass
class Array(Node):
    RULE = f"'[' {csf("xs", "Num")} ']'"
    values: List[Num]

    @classmethod
    def from_antlr(cls, ctx):
        return Array(ctx.xs)


def gen():
    Prog._compile_to("antlr_gen")


def parse():
    from antlr4 import InputStream, CommonTokenStream
    from antlr_gen.DemoParser import DemoParser
    from antlr_gen.DemoLexer import DemoLexer

    lexer = DemoLexer(InputStream("[1, 2, 3]"))
    token_stream = CommonTokenStream(lexer)
    parse = Prog._build_parser(DemoParser)
    global e
    e = parse(token_stream)
    print(f"{e=}")


if __name__ == "__main__":
    parse()
