"""Common runtime for inspecting and iterating from the root node of a grammar."""

from typing import (
    Any,
    Callable,
    ClassVar,
    Iterable,
    Optional,
    Self,
    Type,
    List,
    TYPE_CHECKING,
)
import inspect
from abc import ABC, abstractmethod

if TYPE_CHECKING:
    from antlr4 import ParserRuleContext, Parser, TokenStream
    from .gen import AntlrBuilder


NodeT = Type["Node"]
RootNodeT = Type["RootNode"]
ParseFnT = Callable[["TokenStream"], "RootNode"]


class Node(ABC):
    RULE: ClassVar[str | List[str]] = "<todo-rule>"

    _ctx: Optional[Any]

    @classmethod
    @abstractmethod
    def from_antlr(cls, ctx) -> Self:
        pass

    @classmethod
    def _on_wrap(cls, base_fn: Callable):
        def wrapper(*args, **kwargs):
            ctx: ParserRuleContext = base_fn(*args, **kwargs)
            ret = cls.from_antlr(ctx)
            ret._ctx = ctx
            print(f"{cls=} {base_fn=} {ret=}")
            return ret

        return wrapper

    @classmethod
    def _on_compile(cls, b: "AntlrBuilder"):
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
            node._on_compile(builder)

        out_dir = Path(out_base_dir)
        out_dir.mkdir(exist_ok=True)
        out_file = out_dir / f"{name}.g4"
        with open(out_file, "w") as f:
            builder.flush(f)
        check_call(["antlr4", "-Dlanguage=Python3", "-no-listener", out_file])

    @classmethod
    def _build_parser(cls, base_parser: Type["Parser"]) -> ParseFnT:
        class_members = {}
        root_fn = None
        for node_cls in cls._iter_nodes():
            fn_name = to_antlr_name(node_cls.__name__)
            fn = getattr(base_parser, fn_name)
            fn_wrapped = node_cls._on_wrap(fn)
            class_members[fn_name] = fn_wrapped
            if node_cls == cls:
                root_fn = fn_wrapped
        assert root_fn is not None

        proxy_name = f"Wrapped{base_parser.__name__}"
        proxy_cls = type(proxy_name, (base_parser,), class_members)

        def parse(tokens: "TokenStream") -> Self:
            instance = proxy_cls(tokens)
            return root_fn(instance)

        return parse


def is_lex_name(s: str):
    return s.isupper()


def to_antlr_name(s: str):
    return s[0].lower() + s[1:]
