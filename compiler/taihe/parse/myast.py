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
