"""Generates the parser with ANTLR before running."""

from typing import Dict, Iterable, TextIO
from ast_base import is_lex_name, to_antlr_name
import re


RuleT = str | Iterable[str]


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


def main():
    from myast import Prog

    Prog._compile_to("antlr_gen")


if __name__ == "__main__":
    main()
