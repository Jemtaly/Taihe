from io import StringIO
from typing import Iterable, TextIO


class AntlrBuilder:
    """Builds ANTLR grammar file with ease."""

    def __init__(self, buf: TextIO):
        self.buf = buf

    def grammar(self, name: str):
        print(f"grammar {name};", file=self.buf)
        print(file=self.buf)

    def rule(self, name: str, rule: str | Iterable[str]):
        print(name, file=self.buf)
        if isinstance(rule, str):
            r = (rule,)

        is_first = False
        for r in rule:
            prefix = "\t:" if is_first else "\t|"
            print(prefix, r, file=self.buf)
            is_first = True
        print("\t;", file=self.buf)
        print(file=self.buf)


def main():
    buf = StringIO()
    b = AntlrBuilder(buf)
    b.grammar("Taihe")
    b.rule("fragment LETTER", ["_", "'A' .. 'Z'"])
    print(buf.getvalue())


if __name__ == "__main__":
    main()
