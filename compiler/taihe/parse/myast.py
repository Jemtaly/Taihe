from dataclasses import dataclass
from visitor import Node

_LEXER = r"""
NEWLINE : [\r\n]+ -> skip;
INT     : [0-9]+;
WS      : ' '+ -> channel(HIDDEN);
"""


@dataclass
class Prog(Node):
    GRAMMAR_NAME = "Demo"
    GRAMMAR_LEXER = _LEXER
    RULE = [
        "Array EOF",
        "Expr EOF",
    ]

    @staticmethod
    def from_ast():
        pass


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
    RULE = f"'[' {csf("xs", "INT")} ']'"
