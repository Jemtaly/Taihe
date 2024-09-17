"""Common runtime for inspecting and iterating from the root node of a grammar."""

from typing import (
    Any,
    Callable,
    ClassVar,
    Dict,
    Iterable,
    Optional,
    Self,
    TextIO,
    Type,
    List,
    TYPE_CHECKING,
)
import inspect
from abc import ABC, abstractmethod

if TYPE_CHECKING:
    from antlr4 import ParserRuleContext, Parser, TokenStream


NodeT = Type["Node"]
RootNodeT = Type["RootNode"]
ParseFnT = Callable[["TokenStream"], "RootNode"]
RuleT = str | Iterable[str]


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
            return ret

        return wrapper

    @classmethod
    def _on_compile(cls, b: "AntlrBuilder"):
        b.add_rule(cls.__name__, cls.RULE)


class TokenNode(Node):
    @classmethod
    def _on_wrap(cls, base_fn: Callable):
        def wrapper(*args, **kwargs):
            ctx: ParserRuleContext = base_fn(*args, **kwargs)
            symbol = ctx.children[0].symbol
            ret = cls.from_antlr(symbol)
            ret._ctx = symbol
            return ret

        return wrapper

    @classmethod
    def _on_compile(cls, b: "AntlrBuilder"):
        # Generate two rules.
        #
        # For instance, for class Num:
        #   class Num:
        #     RULE = '[0-9]+'"
        #
        # We generate:
        rule_name = cls.__name__
        lex_name = rule_name.upper()
        b.add_rule(lex_name, cls.RULE)  # NUM: [0-9]+
        b.add_rule(rule_name, lex_name)  # Num: NUM


class RootNode(Node):
    GRAMMAR_NAME: ClassVar[str] = "<forgot-to-override-root-node-name?>"
    GRAMMAR_LEX_RULES: ClassVar[str] = ""

    @classmethod
    def _iter_nodes(cls) -> Iterable[NodeT]:
        mod = inspect.getmodule(cls)
        assert mod
        for c in mod.__dict__.values():
            if not inspect.isclass(c):
                continue
            if not issubclass(c, Node):
                continue
            if inspect.isabstract(c):
                continue
            yield c

    @classmethod
    def _compile_to(cls, out_base_dir: str):
        """Generates the parser with ANTLR before running."""
        from subprocess import check_call
        from pathlib import Path

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


class AntlrBuilder:
    """Writes ANTLR grammar file to a buffer."""

    _rules: Dict[str, RuleT]

    def __init__(self):
        self._rules = {}
        self._header = []

    def add_rule(self, name: str, rule: RuleT):
        """Adds a parser or lexer rule.

        Names of parser rules must be PascalCase.
        Names of lexer rules must be SCREAM_CASE.

        Example:
        - add_rule("ParserRule", "ParserRuleX '+' ParserRuleY")
        - add_rule("LEX_RULE", "0..9")
        """
        self._rules[name] = rule

    def _write_rule(self, name: str, rule: RuleT):
        if isinstance(rule, str):
            self._writeln(f"{name}: {rule};")
        else:
            self._writeln(name)
            is_first = True
            for r in rule:
                prefix = "\t:" if is_first else "\t|"
                self._writeln(prefix, r)
                is_first = False
            self._writeln("\t;")

        self._writeln()

    def flush(self, buf: TextIO):
        import re

        name_to_antlr = {s: to_antlr_name(s) for s in self._rules if not is_lex_name(s)}
        # Compile a regex pattern to match whole identifiers.
        names_escaped = (re.escape(name) for name in name_to_antlr.keys())
        pattern = re.compile(r"\b(" + "|".join(names_escaped) + r")\b")

        def convert_once(s: str):
            return pattern.sub(lambda m: name_to_antlr[m.group(0)], s)

        def convert_rule(snippet: str | Iterable[str]):
            if isinstance(snippet, str):
                return convert_once(snippet)
            else:
                return [convert_once(s) for s in snippet]

        # Write to the buffer.
        self._buf = buf
        for l in self._header:
            self._writeln(l)
        for name, rule in self._rules.items():
            if is_lex_name(name):
                self._write_rule(name, rule)
            else:
                self._write_rule(name_to_antlr[name], convert_rule(rule))
        self._buf = None

    def _writeln(self, *args, end=None):
        print(*args, file=self._buf, end=end)

    def add_to_header(self, x: str):
        self._header.append(x)
