from typing import Callable, Type
from antlr4 import (
    InputStream,
    CommonTokenStream,
    Parser,
    Lexer,
    TokenStream,
)
from ast_base import RootNode, RootNodeT

ParserT = Type[Parser]
LexerT = Type[Lexer]
ParseFnT = Callable[[TokenStream], RootNode]


def parse(
    input_stream: InputStream,
    root: RootNodeT,
    lex: LexerT,
    base_parser: ParserT,
):
    parse = root._build_parser(base_parser)

    lexer = lex(input_stream)
    token_stream = CommonTokenStream(lexer)
    global e
    e = parse(token_stream)
    print(f"{e=}")


def main():
    from antlr_gen.DemoParser import DemoParser
    from antlr_gen.DemoLexer import DemoLexer
    from myast import Prog as TheRoot

    s = InputStream("[1, 2, 3]")
    parse(s, TheRoot, DemoLexer, DemoParser)


if __name__ == "__main__":
    main()
