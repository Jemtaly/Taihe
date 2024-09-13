from dataclasses import dataclass
from typing import List

_LEXER = r"""
PLUS
   : '+'
   ;


MINUS
   : '-'
   ;


TIMES
   : '*'
   ;


DIV
   : '/'
   ;


DIGIT
   : ('0' .. '9')
   ;


WS
   : [ \r\n\t] + -> channel (HIDDEN)
   ;
"""


class Node:
    RULE: str | List[str] = "<todo>"

    @classmethod
    def node_name(cls) -> str:
        return cls.__name__


@dataclass
class Expr(Node):
    GRAMMAR_NAME = "Taihe"
    GRAMMAR_LEXER = _LEXER
    RULE = "MulExpr ((PLUS | MINUS) MulExpr)*"


@dataclass
class MulExpr(Node):
    RULE = "Number ((TIMES | DIV) Number)*"


@dataclass
class Number(Node):
    RULE = "MINUS? DIGIT +"
