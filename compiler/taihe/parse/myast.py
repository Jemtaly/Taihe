from dataclasses import dataclass
from typing import List

from ast_base import ParserNode, RootNode, LexerNode

_LEXER = r"""
NEWLINE : [\r\n]+ -> skip;
WS      : ' '+ -> channel(HIDDEN);
"""


@dataclass
class Num(LexerNode):
    RULE = "[0-9]+"

    value: int

    @classmethod
    def from_lexer(cls, token):
        return Num(int(token.text))


@dataclass
class Prog(RootNode):
    GRAMMAR_NAME = "Demo"
    GRAMMAR_LEX_RULES = _LEXER
    RULE = [
        "v=Array EOF",
        "u=Expr EOF",
    ]

    data: ParserNode

    @classmethod
    def from_parser(cls, ctx):
        return Prog(ctx.v)


@dataclass
class Expr(ParserNode):
    RULE = [
        "Expr ('*' | '/') Expr",
        "Expr ('+' | '-') Expr",
        "Num",
        "'(' Expr ')'",
    ]

    @classmethod
    def from_parser(cls, ctx):
        return ctx


def csf(field: str, token: str):
    """Comma-separated fields."""
    return f"{field}+={token} (',' {field}+={token})*"


@dataclass
class Array(ParserNode):
    RULE = f"'[' {csf("xs", "Num")} ']'"
    values: List[Num]

    @classmethod
    def from_parser(cls, ctx):
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
    gen()
    parse()
