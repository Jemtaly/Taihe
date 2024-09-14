from typing import Type
from antlr4 import InputStream, CommonTokenStream, Parser, Lexer
from ast_base import RootNodeT, NodeInspector

ParserT = Type[Parser]
LexerT = Type[Lexer]


def parse(
    input_stream: InputStream,
    root_node: RootNodeT,
    lexer_ty: LexerT,
    parser_ty: ParserT,
):
    lexer = lexer_ty(input_stream)
    token_stream = CommonTokenStream(lexer)
    parser = parser_ty(token_stream)

    ni = NodeInspector(root_node)
    root_fn_name = ni.node_to_name[root_node]
    root_fn = getattr(parser, root_fn_name)

    global e
    e = root_fn()
    print(e.toStringTree())


def main():
    from antlr_gen.DemoParser import DemoParser
    from antlr_gen.DemoLexer import DemoLexer
    from myast import Prog as TheRoot

    s = InputStream("2 + 3")
    parse(s, TheRoot, DemoLexer, DemoParser)


if __name__ == "__main__":
    main()
