"""Common runtime for inspecting and iterating from the root node of a grammar."""

from typing import (
    ClassVar,
    Dict,
    Iterable,
    Optional,
    Self,
    Tuple,
    Type,
    List,
    TYPE_CHECKING,
)
import inspect

if TYPE_CHECKING:
    from antlr4 import ParserRuleContext
    from .gen import AntlrBuilder


NodeT = Type["Node"]
RootNodeT = Type["RootNode"]


class Node:
    RULE: ClassVar[str | List[str]] = "<todo-rule>"
    _ctx: Optional["ParserRuleContext"]

    @classmethod
    def from_antlr(cls, ctx) -> Self:
        del ctx
        raise NotImplementedError(f"remember to override in {cls}")

    @classmethod
    def _build_grammar(cls, b: "AntlrBuilder"):
        b.add_rule(cls.__name__, cls.RULE)


class RootNode(Node):
    GRAMMAR_NAME: ClassVar[str] = "<forgot-to-override-root-node-name?>"
    GRAMMAR_LEX_RULES: ClassVar[str] = ""

    @classmethod
    def _iter_nodes(cls) -> Iterable[NodeT]:
        mod = inspect.getmodule(cls)
        assert mod
        for c in mod.__dict__.values():
            if c is not Node and c is not RootNode:
                if inspect.isclass(c) and issubclass(c, Node):
                    yield c

    @classmethod
    def _compile_to(cls, out_base_dir: str):
        from subprocess import check_call
        from pathlib import Path
        from gen import AntlrBuilder

        name = cls.GRAMMAR_NAME
        builder = AntlrBuilder()
        builder.add_to_header(f"grammar {name};\n")
        builder.add_to_header(cls.GRAMMAR_LEX_RULES)
        for node in cls._iter_nodes():
            node._build_grammar(builder)

        out_dir = Path(out_base_dir)
        out_dir.mkdir(exist_ok=True)
        out_file = out_dir / f"{name}.g4"
        with open(out_file, "w") as f:
            builder.flush(f)
        check_call(["antlr4", "-Dlanguage=Python3", "-no-listener", out_file])


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


def is_lex_name(s: str):
    return s.isupper()


def to_antlr_name(s: str):
    return s[0].lower() + s[1:]
