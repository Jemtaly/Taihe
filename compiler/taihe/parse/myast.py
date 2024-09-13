from dataclasses import dataclass
from typing import List

_LEXER = r"""
NEWLINE : [\r\n]+ -> skip;
INT     : [0-9]+;
WS      : ' '+ -> channel(HIDDEN);
"""


class Node:
    RULE: str | List[str] = "<todo>"

    @classmethod
    def node_name(cls) -> str:
        return cls.__name__


@dataclass
class Prog(Node):
    GRAMMAR_NAME = "Demo"
    GRAMMAR_LEXER = _LEXER
    RULE = [
        "Array EOF",
        "Expr EOF",
    ]


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
