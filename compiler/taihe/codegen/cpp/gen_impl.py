# -*- coding: utf-8 -*-
#
# Copyright (c) 2025 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import re

from taihe.codegen.abi.analyses import (
    GlobFuncAbiInfo,
    IfaceAbiInfo,
    IfaceMethodAbiInfo,
    PackageAbiInfo,
    TypeAbiInfo,
)
from taihe.codegen.abi.writer import CHeaderWriter, CSourceWriter
from taihe.codegen.cpp.analyses import (
    GlobFuncCppImplInfo,
    IfaceCppImplInfo,
    IfaceCppInfo,
    IfaceMethodCppImplInfo,
    IfaceMethodCppInfo,
    PackageCppImplInfo,
    TypeCppInfo,
    from_abi,
    into_abi,
)
from taihe.semantics.declarations import (
    GlobFuncDecl,
    IfaceDecl,
    IfaceMethodDecl,
    PackageDecl,
    PackageGroup,
)
from taihe.semantics.types import IfaceType, NonVoidType
from taihe.utils.analyses import AnalysisManager
from taihe.utils.outputs import FileKind, OutputManager


class CppImplHeadersGenerator:
    def __init__(self, am: AnalysisManager, pg: PackageGroup):
        self.am = am
        self.pg = pg

    def generate(self, om: OutputManager):
        for pkg in self.pg.packages:
            CppMacroPackageGenerator(self.am, pkg).gen_package_file(om)
            # for iface in pkg.interfaces:
            #     CppMacroIfaceGenerator(self.am, iface).gen_iface_file(om)


class CppMacroPackageGenerator:
    def __init__(self, am: AnalysisManager, pkg: PackageDecl):
        self.am = am
        self.pkg = pkg

    def gen_package_file(self, om: OutputManager):
        pkg_cpp_impl_info = PackageCppImplInfo.get(self.am, self.pkg)
        target = CHeaderWriter(
            om,
            f"include/{pkg_cpp_impl_info.header}",
            FileKind.CPP_HEADER,
        )
        pkg_abi_info = PackageAbiInfo.get(self.am, self.pkg)
        with target:
            target.add_include("taihe/common.hpp")
            target.add_include(pkg_abi_info.header)
            for func in self.pkg.functions:
                for param in func.params:
                    param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
                    target.add_include(*param_ty_cpp_info.impl_headers)
                if isinstance(return_ty := func.return_ty, NonVoidType):
                    return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
                    target.add_include(*return_ty_cpp_info.impl_headers)
                self.gen_func(target, func)

    def gen_func(self, target: CHeaderWriter, func: GlobFuncDecl):
        func_abi_info = GlobFuncAbiInfo.get(self.am, func)
        func_cpp_impl_info = GlobFuncCppImplInfo.get(self.am, func)
        func_impl = "CPP_FUNC_IMPL"
        params_abi = []
        args_cpp = []
        for param in func.params:
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            param_ty_abi_info = TypeAbiInfo.get(self.am, param.ty)
            params_abi.append(f"{param_ty_abi_info.as_param} {param.name}")
            args_cpp.append(from_abi(param_ty_cpp_info.as_param, param.name))
        params_abi_str = ", ".join(params_abi)
        args_cpp_str = ", ".join(args_cpp)
        result_cpp = f"{func_impl}({args_cpp_str})"
        if isinstance(return_ty := func.return_ty, NonVoidType):
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_abi_info = TypeAbiInfo.get(self.am, return_ty)
            return_ty_abi_name = return_ty_abi_info.as_owner
            result_abi = into_abi(return_ty_cpp_info.as_owner, result_cpp)
        else:
            return_ty_abi_name = "void"
            result_abi = result_cpp
        target.writelns(
            f"#define {func_cpp_impl_info.macro}({func_impl}) \\",
            f"    {return_ty_abi_name} {func_abi_info.impl_name}({params_abi_str}) {{ \\",
            f"        return {result_abi}; \\",
            f"    }}",
        )


class CppMacroIfaceGenerator:
    def __init__(self, am: AnalysisManager, iface: IfaceDecl):
        self.am = am
        self.iface = iface

    def gen_iface_file(self, om: OutputManager):
        iface_cpp_impl_info = IfaceCppImplInfo.get(self.am, self.iface)
        target = CHeaderWriter(
            om,
            f"include/{iface_cpp_impl_info.header}",
            FileKind.CPP_HEADER,
        )
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        with target:
            target.add_include("taihe/common.hpp")
            target.add_include(iface_cpp_info.impl_header)
            for method in self.iface.methods:
                for param in method.params:
                    param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
                    target.add_include(*param_ty_cpp_info.impl_headers)
                if isinstance(return_ty := method.return_ty, NonVoidType):
                    return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
                    target.add_include(*return_ty_cpp_info.impl_headers)
                self.gen_method(target, method)

    def gen_method(self, target: CHeaderWriter, method: IfaceMethodDecl):
        method_abi_info = IfaceMethodAbiInfo.get(self.am, method)
        method_cpp_impl_info = IfaceMethodCppImplInfo.get(self.am, method)
        method_impl = "CPP_METHOD_IMPL"
        params_abi = []
        args_cpp = []
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        params_abi.append(f"{iface_abi_info.as_param} tobj")
        args_cpp.append(from_abi(iface_cpp_info.as_param, "tobj"))
        for param in method.params:
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            param_ty_abi_info = TypeAbiInfo.get(self.am, param.ty)
            params_abi.append(f"{param_ty_abi_info.as_param} {param.name}")
            args_cpp.append(from_abi(param_ty_cpp_info.as_param, param.name))
        params_abi_str = ", ".join(params_abi)
        args_cpp_str = ", ".join(args_cpp)
        result_cpp = f"{method_impl}({args_cpp_str})"
        if isinstance(return_ty := method.return_ty, NonVoidType):
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_abi_info = TypeAbiInfo.get(self.am, return_ty)
            return_ty_abi_name = return_ty_abi_info.as_owner
            result_abi = into_abi(return_ty_cpp_info.as_owner, result_cpp)
        else:
            return_ty_abi_name = "void"
            result_abi = result_cpp
        target.writelns(
            f"#define {method_cpp_impl_info.macro}({method_impl}) \\",
            f"    {return_ty_abi_name} {method_abi_info.impl_name}({params_abi_str}) {{ \\",
            f"        return {result_abi}; \\",
            f"    }}",
        )


class CppImplSourcesGenerator:
    def __init__(self, am: AnalysisManager, pg: PackageGroup):
        self.am = am
        self.pg = pg
        self.using_namespaces: list[str] = []

    def generate(self, om: OutputManager):
        for pkg in self.pg.packages:
            CppTemplatePackageGenerator(self.am, pkg).gen_package_file(om)
            # for iface in pkg.interfaces:
            #     CppTemplateIfaceGenerator(self.am, iface).gen_iface_file(om)
        for pkg in self.pg.packages:
            for iface in pkg.interfaces:
                CppTemplateClassHeaderGenerator(self.am, iface).gen_file(om)
                CppTemplateClassSourceGenerator(self.am, iface).gen_file(om)


class CppTemplateBaseWriterGenerator:
    def __init__(self, using_namespaces: tuple[str, ...] = ()):
        self.using_namespaces = using_namespaces

    @property
    def make_holder(self):
        return self.mask("taihe::make_holder")

    @property
    def runtime_error(self):
        return self.mask("std::runtime_error")

    def mask(self, cpp_type: str):
        pattern = r"(::)?([A-Za-z_][A-Za-z_0-9]*::)*[A-Za-z_][A-Za-z_0-9]*"

        def replace_ns(match):
            matched = match.group(0)
            for ns in self.using_namespaces:
                ns = ns + "::"
                if matched.startswith(ns):
                    return matched[len(ns) :]
                ns = "::" + ns
                if matched.startswith(ns):
                    return matched[len(ns) :]
            return matched

        return re.sub(pattern, replace_ns, cpp_type)

    def gen_using_namespaces(self, target: CSourceWriter):
        if not self.using_namespaces:
            target.writelns(
                "// You can add using namespace statements here if needed.",
            )
        for namespace in self.using_namespaces:
            target.writelns(
                f"using namespace {namespace};",
            )


class CppTemplatePackageGenerator(CppTemplateBaseWriterGenerator):
    def __init__(self, am: AnalysisManager, pkg: PackageDecl):
        super().__init__()
        self.am = am
        self.pkg = pkg

    def gen_package_file(self, om: OutputManager):
        pkg_cpp_impl_info = PackageCppImplInfo.get(self.am, self.pkg)
        target = CSourceWriter(
            om,
            f"temp/{pkg_cpp_impl_info.source}",
            FileKind.C_TEMPLATE,
        )
        with target:
            target.add_include(pkg_cpp_impl_info.header)
            target.add_include("stdexcept")
            with target.indented(
                f"namespace {{",
                f"}}  // namespace",
                indent="",
            ):
                self.gen_using_namespaces(target)
                for func in self.pkg.functions:
                    target.newline()
                    self.gen_func_impl(target, func)
            target.newline()
            target.writelns(
                "// Since these macros are auto-generate, lint will cause false positive.",
                "// NOLINTBEGIN",
            )
            for func in self.pkg.functions:
                self.gen_func_macro(target, func)
            target.writelns(
                "// NOLINTEND",
            )

    def gen_func_impl(self, target: CSourceWriter, func: GlobFuncDecl):
        func_cpp_impl_info = GlobFuncCppImplInfo.get(self.am, func)
        params_cpp = []
        for param in func.params:
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            params_cpp.append(f"{self.mask(param_ty_cpp_info.as_param)} {param.name}")
        params_cpp_str = ", ".join(params_cpp)
        if isinstance(return_ty := func.return_ty, NonVoidType):
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_cpp_name = self.mask(return_ty_cpp_info.as_owner)
        else:
            return_ty_cpp_name = "void"
        with target.indented(
            f"{return_ty_cpp_name} {func_cpp_impl_info.function}({params_cpp_str}) {{",
            f"}}",
        ):
            if isinstance(return_ty := func.return_ty, IfaceType):
                ret_cpp_impl_info = IfaceCppImplInfo.get(self.am, return_ty.decl)
                target.add_include(ret_cpp_impl_info.template_header)
                target.writelns(
                    f"// The parameters in the make_holder function should be of the same type",
                    f"// as the parameters in the constructor of the actual implementation class.",
                    f"return {self.make_holder}<{ret_cpp_impl_info.template_class}, {return_ty_cpp_name}>();",
                )
            else:
                target.writelns(
                    f'TH_THROW({self.runtime_error}, "not implemented");',
                )

    def gen_func_macro(self, target: CSourceWriter, func: GlobFuncDecl):
        func_cpp_impl_info = GlobFuncCppImplInfo.get(self.am, func)
        target.writelns(
            f"{func_cpp_impl_info.macro}({func_cpp_impl_info.function});",
        )


class CppTemplateIfaceGenerator(CppTemplateBaseWriterGenerator):
    def __init__(self, am: AnalysisManager, iface: IfaceDecl):
        super().__init__()
        self.iface = iface
        self.am = am

    def gen_iface_file(self, om: OutputManager):
        iface_cpp_impl_info = IfaceCppImplInfo.get(self.am, self.iface)
        target = CSourceWriter(
            om,
            f"temp/{iface_cpp_impl_info.source}",
            FileKind.C_TEMPLATE,
        )
        with target:
            target.add_include(iface_cpp_impl_info.header)
            target.add_include("stdexcept")
            with target.indented(
                f"namespace {{",
                f"}}  // namespace",
                indent="",
            ):
                self.gen_using_namespaces(target)
                for method in self.iface.methods:
                    target.newline()
                    self.gen_method_impl(target, method)
            target.newline()
            target.writelns(
                "// Since these macros are auto-generate, lint will cause false positive.",
                "// NOLINTBEGIN",
            )
            for method in self.iface.methods:
                self.gen_method_macro(target, method)
            target.writelns(
                "// NOLINTEND",
            )

    def gen_method_impl(self, target: CSourceWriter, method: IfaceMethodDecl):
        method_cpp_impl_info = IfaceMethodCppImplInfo.get(self.am, method)
        params_cpp = []
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        params_cpp.append(f"{self.mask(iface_cpp_info.as_param)} tobj")
        for param in method.params:
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            params_cpp.append(f"{self.mask(param_ty_cpp_info.as_param)} {param.name}")
        params_cpp_str = ", ".join(params_cpp)
        if isinstance(return_ty := method.return_ty, NonVoidType):
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_cpp_name = self.mask(return_ty_cpp_info.as_owner)
        else:
            return_ty_cpp_name = "void"
        with target.indented(
            f"{return_ty_cpp_name} {method_cpp_impl_info.function}({params_cpp_str}) {{",
            f"}}",
        ):
            if isinstance(return_ty := method.return_ty, IfaceType):
                ret_cpp_impl_info = IfaceCppImplInfo.get(self.am, return_ty.decl)
                target.add_include(ret_cpp_impl_info.template_header)
                target.writelns(
                    f"// The parameters in the make_holder function should be of the same type",
                    f"// as the parameters in the constructor of the actual implementation class.",
                    f"return {self.make_holder}<{ret_cpp_impl_info.template_class}, {return_ty_cpp_name}>();",
                )
            else:
                target.writelns(
                    f'TH_THROW({self.runtime_error}, "not implemented");',
                )

    def gen_method_macro(self, target: CSourceWriter, method: IfaceMethodDecl):
        method_cpp_impl_info = IfaceMethodCppImplInfo.get(self.am, method)
        target.writelns(
            f"{method_cpp_impl_info.macro}({method_cpp_impl_info.function});",
        )


class CppTemplateClassHeaderGenerator:
    def __init__(self, am: AnalysisManager, iface: IfaceDecl):
        self.am = am
        self.iface = iface

    def gen_file(self, om: OutputManager):
        iface_cpp_impl_info = IfaceCppImplInfo.get(self.am, self.iface)
        target = CHeaderWriter(
            om,
            f"temp/{iface_cpp_impl_info.template_header}",
            FileKind.C_TEMPLATE,
        )
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        with target:
            target.add_include("taihe/common.hpp")
            for ancestor in iface_abi_info.ancestor_dict:
                for method in ancestor.methods:
                    for param in method.params:
                        param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
                        target.add_include(*param_ty_cpp_info.impl_headers)
                    if isinstance(return_ty := method.return_ty, NonVoidType):
                        return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
                        target.add_include(*return_ty_cpp_info.impl_headers)
            self.gen_iface_template_class(target)

    def gen_iface_template_class(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_impl_info = IfaceCppImplInfo.get(self.am, self.iface)
        with target.indented(
            f"class {iface_cpp_impl_info.template_class} {{",
            f"}};",
        ):
            target.writelns(
                f"public:",
            )
            target.writelns(
                f"// You can add member variables and constructor here.",
            )
            for ancestor in iface_abi_info.ancestor_dict:
                for method in ancestor.methods:
                    self.gen_iface_method_decl(target, method)

    def gen_iface_method_decl(self, target: CHeaderWriter, method: IfaceMethodDecl):
        method_cpp_info = IfaceMethodCppInfo.get(self.am, method)
        params_cpp = []
        for param in method.params:
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            params_cpp.append(f"{param_ty_cpp_info.as_param} {param.name}")
        params_cpp_str = ", ".join(params_cpp)
        if isinstance(return_ty := method.return_ty, NonVoidType):
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_cpp_name = return_ty_cpp_info.as_owner
        else:
            return_ty_cpp_name = "void"
        target.writelns(
            f"{return_ty_cpp_name} {method_cpp_info.call_name}({params_cpp_str});",
        )


class CppTemplateClassSourceGenerator(CppTemplateBaseWriterGenerator):
    def __init__(self, am: AnalysisManager, iface: IfaceDecl):
        super().__init__()
        self.iface = iface
        self.am = am

    def gen_file(self, om: OutputManager):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_impl_info = IfaceCppImplInfo.get(self.am, self.iface)
        target = CSourceWriter(
            om,
            f"temp/{iface_cpp_impl_info.template_source}",
            FileKind.C_TEMPLATE,
        )
        with target:
            target.add_include(iface_cpp_impl_info.template_header)
            target.add_include("stdexcept")
            self.gen_using_namespaces(target)
            for ancestor in iface_abi_info.ancestor_dict:
                for method in ancestor.methods:
                    target.newline()
                    self.gen_iface_method_impl(target, method)

    def gen_iface_method_impl(self, target: CSourceWriter, method: IfaceMethodDecl):
        iface_cpp_impl_info = IfaceCppImplInfo.get(self.am, self.iface)
        method_cpp_info = IfaceMethodCppInfo.get(self.am, method)
        params_cpp = []
        for param in method.params:
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            params_cpp.append(f"{self.mask(param_ty_cpp_info.as_param)} {param.name}")
        params_cpp_str = ", ".join(params_cpp)
        if isinstance(return_ty := method.return_ty, NonVoidType):
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_cpp_name = self.mask(return_ty_cpp_info.as_owner)
        else:
            return_ty_cpp_name = "void"
        with target.indented(
            f"{return_ty_cpp_name} {iface_cpp_impl_info.template_class}::{method_cpp_info.impl_name}({params_cpp_str}) {{",
            f"}}",
        ):
            if isinstance(return_ty := method.return_ty, IfaceType):
                ret_cpp_impl_info = IfaceCppImplInfo.get(self.am, return_ty.decl)
                target.add_include(ret_cpp_impl_info.template_header)
                target.writelns(
                    f"// The parameters in the make_holder function should be of the same type",
                    f"// as the parameters in the constructor of the actual implementation class.",
                    f"return {self.make_holder}<{ret_cpp_impl_info.template_class}, {return_ty_cpp_name}>();",
                )
            else:
                target.writelns(
                    f'TH_THROW({self.runtime_error}, "not implemented");',
                )
