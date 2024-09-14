"""Common runtime for inspecting and iterating from the root node of a grammar."""

from typing import ClassVar, Dict, Iterable, Optional, Tuple, Type, List
import inspect
from antlr4 import ParserRuleContext


NodeT = Type["Node"]
RootNodeT = Type["RootNode"]


class Node:
    RULE: ClassVar[str | List[str]] = "<todo-rule>"
    _ctx: Optional[ParserRuleContext]

    @classmethod
    def from_antlr(cls, ctx) -> "Node":
        del ctx
        raise NotImplementedError(f"remember to override in {cls}")


class RootNode(Node):
    GRAMMAR_NAME: ClassVar[str] = "<forgot-to-override-root-node-name?>"
    GRAMMAR_LEXER: ClassVar[str] = ""

    @classmethod
    def _iter_nodes(cls) -> Iterable[NodeT]:
        mod = inspect.getmodule(cls)
        assert mod
        for c in mod.__dict__.values():
            if inspect.isclass(c) and issubclass(c, Node) and c != Node:
                yield c


class NodeInspector:
    root: RootNodeT
    nodes: Dict[str, NodeT]
    node_to_name: Dict[NodeT, str]

    def __init__(self, root: RootNodeT):
        name_to_node = {}
        node_to_name = {}

        for name, node in NodeInspector._iter_nodes(root):
            # Python class style 'FooBarBaz' to ANTLR rule style 'fooBarBaz'.
            antlr_name = name[0].lower() + name[1:]
            name_to_node[antlr_name] = node
            node_to_name[node] = antlr_name

        self.root = root
        self.nodes = name_to_node
        self.node_to_name = node_to_name

    @staticmethod
    def _iter_nodes(root: RootNodeT) -> Iterable[Tuple[str, NodeT]]:
        mod = inspect.getmodule(root)
        assert mod
        for name, c in mod.__dict__.items():
            if c is not Node and c is not RootNode:
                if inspect.isclass(c) and issubclass(c, Node):
                    yield name, c
