from pathlib import Path
from uu import Error
from taihe.parse import ast
from taihe.semantics import types

ErrorManager = list


def get_int_val(node: ast.IntExpr) -> int:
    if isinstance(node, ast.IntLiteralExpr):
        text = node.val.text
        if text.startswith("0b"):
            return int(text, 2)
        if text.startswith("0o"):
            return int(text, 8)
        if text.startswith("0x"):
            return int(text, 16)
        return int(text)

    if isinstance(node, ast.IntParenthesisExpr):
        return get_int_val(node.expr)

    if isinstance(node, ast.IntConditionalExpr):
        return get_int_val(node.then_expr) if get_bool_val(node.cond) else get_int_val(node.else_expr)

    if isinstance(node, ast.IntUnaryExpr):
        return {
            "-": int.__neg__,
            "+": int.__pos__,
            "~": int.__invert__,
        }[node.op.text](
            get_int_val(node.expr),
        )

    if isinstance(node, ast.IntBinaryExpr):
        return {
            "+": int.__add__,
            "-": int.__sub__,
            "*": int.__mul__,
            "/": int.__floordiv__,
            "%": int.__mod__,
            "<<": int.__lshift__,
            ">>": int.__rshift__,
            "&": int.__and__,
            "|": int.__or__,
            "^": int.__xor__,
        }[node.op.text](
            get_int_val(node.left),
            get_int_val(node.right),
        )

    raise NotImplementedError


def get_bool_val(node: ast.BoolExpr) -> bool:
    if isinstance(node, ast.IntComparisonExpr):
        return {
            ">": int.__gt__,
            "<": int.__lt__,
            ">=": int.__ge__,
            "<=": int.__le__,
            "==": int.__eq__,
            "!=": int.__ne__,
        }[node.op.text](
            get_int_val(node.left),
            get_int_val(node.right),
        )

    if isinstance(node, ast.BoolUnaryExpr):
        assert node.op.text == "!"
        return not get_bool_val(node.expr)

    if isinstance(node, ast.BoolBinaryExpr):
        return {
            "&&": bool.__and__,
            "||": bool.__or__,
        }[node.op.text](
            get_bool_val(node.left),
            get_bool_val(node.right),
        )

    if isinstance(node, ast.BoolParenthesisExpr):
        return get_bool_val(node.expr)

    if isinstance(node, ast.BoolConditionalExpr):
        return get_bool_val(node.then_expr) if get_bool_val(node.cond) else get_bool_val(node.else_expr)

    raise NotImplementedError


def get_package_group(files: dict[Path, ast.Spec], error_manager: ErrorManager) -> types.PackageGroup:
    package_group = types.PackageGroup()
    packages: list[tuple[Path, ast.Spec, types.Package]] = []
    for filename, spec in files.items():
        name = tuple(filename.stem.split("."))
        package = package_group.add_package(name)
        packages.append((filename, spec, package))

    return package_group
    

