from dataclasses import dataclass
from typing import List

_LEXER = r"""
NEWLINE : [\r\n]+ -> skip;
INT     : [0-9]+ ;
"""


class Node:
    RULE: str | List[str] = "<todo>"

    @classmethod
    def node_name(cls) -> str:
        return cls.__name__


@dataclass
class Prog(Node):
    GRAMMAR_NAME = "Taihe"
    GRAMMAR_LEXER = _LEXER
    RULE = "Expr EOF"


@dataclass
class Expr(Node):
    RULE = [
        "Expr ('*' | '/') Expr",
        "Expr ('+' | '-') Expr",
        "INT",
        "'(' Expr ')'",
    ]
