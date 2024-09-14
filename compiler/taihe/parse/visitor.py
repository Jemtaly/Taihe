"""Common runtime for inspecting and iterating from the root node of a grammar."""

from antlr4 import InputStream, CommonTokenStream, Parser, Lexer
from typing import ClassVar, Iterable, Type, List
import inspect

NodeT = Type["Node"]
RootNodeT = Type["RootNode"]
ParserT = Type[Parser]
LexerT = Type[Lexer]


class Node:
    RULE: ClassVar[str | List[str]] = "<todo>"

    @classmethod
    def node_name(cls) -> str:
        return cls.__name__


class RootNode(Node):
    GRAMMAR_NAME: ClassVar[str] = "<todo>"
    GRAMMAR_LEXER: ClassVar[str] = ""

    @classmethod
    def _iter_nodes(cls) -> Iterable[NodeT]:
        mod = inspect.getmodule(cls)
        assert mod
        for c in mod.__dict__.values():
            if inspect.isclass(c) and issubclass(c, Node) and c != Node:
                yield c


def class_name_to_antlr_name(s: str):
    return s[0].lower() + s[1:]


def parse(
    input_stream: InputStream,
    root_node: RootNodeT,
    lexer_ty: LexerT,
    parser_ty: ParserT,
):
    lexer = lexer_ty(input_stream)
    token_stream = CommonTokenStream(lexer)
    parser = parser_ty(token_stream)

    root_fn_name = class_name_to_antlr_name(root_node.node_name())
    root_fn = getattr(parser, root_fn_name)

    global e
    e = root_fn()
    print(e.toStringTree())


def main():
    from antlr_gen.DemoParser import DemoParser
    from antlr_gen.DemoLexer import DemoLexer
    from myast import Prog as RootNode

    s = InputStream("2 + 3")
    parse(s, RootNode, DemoLexer, DemoParser)


if __name__ == "__main__":
    main()
