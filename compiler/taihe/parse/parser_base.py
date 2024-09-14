from typing import Callable, Type
from antlr4 import (
    InputStream,
    CommonTokenStream,
    Parser,
    Lexer,
    ParserRuleContext,
    TokenStream,
)
from ast_base import NodeT, RootNode, RootNodeT, NodeInspector

ParserT = Type[Parser]
LexerT = Type[Lexer]
ParseFnT = Callable[[TokenStream], RootNode]


# Pitfall: capture the *value* instead of the *reference* of `base_fn`.
# See also:
# https://stackoverflow.com/questions/2295290/what-do-lambda-function-closures-capture
def make_wrapper(base_fn: Callable, node: NodeT):
    def wrapper(*args, **kwargs):
        ctx: ParserRuleContext = base_fn(*args, **kwargs)
        ret = node.from_antlr(ctx)
        return ret

    return wrapper


def build_parser(base: ParserT, root: RootNodeT) -> ParseFnT:
    ni = NodeInspector(root)

    class_members = {}
    for name, node in ni.nodes.items():
        base_fn = getattr(base, name)
        class_members[name] = make_wrapper(base_fn, node)

    proxy_class = type(f"Proxy{base.__name__}", (base,), class_members)
    root_rule_name = ni.node_to_name[root]
    root_rule_fn = getattr(proxy_class, root_rule_name)

    def parse(tokens: TokenStream) -> RootNode:
        instance = proxy_class(tokens)
        return root_rule_fn(instance)

    return parse


def parse(
    input_stream: InputStream,
    root: RootNodeT,
    lex: LexerT,
    base_parser: ParserT,
):
    parse = build_parser(base_parser, root)

    lexer = lex(input_stream)
    token_stream = CommonTokenStream(lexer)
    global e
    e = parse(token_stream)
    print(e)


def main():
    from antlr_gen.DemoParser import DemoParser
    from antlr_gen.DemoLexer import DemoLexer
    from myast import Prog as TheRoot

    s = InputStream("[1, 2, 3]")
    parse(s, TheRoot, DemoLexer, DemoParser)


if __name__ == "__main__":
    main()
