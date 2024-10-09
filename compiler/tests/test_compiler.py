import os
from exceptiongroup import ExceptionGroup
from taihe.compilation import compile as taihec
from taihe.exceptions import (
    EnumValueCollisionError,
    PackageAliasConflictError,
    PackageNotExistError,
    PackageNotImportedError,
    QualifierError,
    RecursiveInclusionError,
    SymbolConflictError,
    SymbolConflictWithNamespaceError,
    SymbolNotExistError,
    SymbolNotImportedError,
)

def check_exception(test_cases):
    idl_dir = "test_cases/idl"
    dst_dir = "test_cases/test/include"
    for test_case in test_cases:
        for file in test_case[1]:
            with open(f"{idl_dir}/{file[0]}", "w") as f:
                f.writelines(file[1])
        
        try:
            taihec([idl_dir], dst_dir)
        except ExceptionGroup as e:
            for excepetion in e.exceptions:
                assert type(excepetion) == test_case[0]
        os.system("rm -rf test_cases/idl")
        os.system("mkdir test_cases/idl")


test_cases_excepetions = [
    [PackageNotExistError, [["package.taihe", ["use a; \n"]]]],

    [PackageAliasConflictError,
    [["package.taihe", ["use package.example1 as example; \n",
                        "use package.example2 as example; \n"]],
    ["package.example1.taihe", ["\n"]],
    ["package.example2.taihe", ["\n"]]]],

    [PackageNotExistError,
    [["package.taihe", ["from package.example2 use A; \n"]],
    ["package.example1.taihe", ["struct A { \n", "    a: bool; \n", "} \n"]]]],

    [SymbolConflictWithNamespaceError,
    [["package.taihe", ["use package.example1.a; \n"]],
    ["package.example1.taihe", ["struct a { \n", "    A: String; \n", "} \n"]],
    ["package.example1.a.taihe", []]]],

    [QualifierError, [["package.taihe", ["function bad_func(a: mut i32): (); \n"]]]],

    [QualifierError, [["package.taihe", ["enum Enum { \n",
                                              "    A; \n",
                                              "} \n",
                                        "struct Struct { \n",
                                              "    a: Enum; \n",
                                              "} \n",
                                        "function bad_func(a: mut Struct, b: mut Enum): (); \n"]]]],

    [SymbolConflictError, [["package.taihe", ["function bad_func(a: i32, a: i32): (); \n"]]]],

    [SymbolConflictError, [["package.taihe", ["enum BadEnum { \n",
                                              "    A; \n",
                                              "    A; \n",
                                              "} \n"]]]],

    [EnumValueCollisionError, [["package.taihe", ["enum BadEnum { \n",
                                              "    A=if !(if 1+1==2 then 2<1&&3<2 else 1!=1) then -1 else -2; \n",
                                              "    B=-1; \n",
                                              "} \n"]]]],

    [EnumValueCollisionError, [["package.taihe", ["enum BadEnum { \n",
                                              "    A = 0b01 << 0b01; \n",
                                              "    B = if (7 << 1 + 1) + (3 * 3 - 2 & 11) == 31 && 1 + 1 == 2 then 1 else 10; \n",
                                              "    C; \n",
                                              "} \n"]]]],

    [SymbolNotExistError,
    [["package.taihe", ["from package.example1 use A; \n"]],
    ["package.example1.taihe", []]]],

    [SymbolConflictError,
    [["package.taihe", ["from package.example1 use A; \n",
                        "from package.example2 use A; \n"]],
    ["package.example1.taihe", ["struct A { \n", "    a: i32; \n", "} \n"]],
    ["package.example2.taihe", ["struct A { \n", "    a: i32; \n", "} \n"]]]],

    [SymbolNotImportedError, [["package.taihe", ["struct BadStruct { \n",
                                              "    a: UnimportedType; \n",
                                              "} \n"]]]],

    [PackageNotImportedError, [["package.taihe", ["struct BadStruct { \n",
                                              "    a: unimported.package.Type; \n",
                                              "} \n"]]]],

    [SymbolNotExistError, [["package.taihe", ["use package.example1; \n",
                                              "struct BadStruct { \n",
                                              "    a: package.example1.B; \n",
                                              "} \n"]],
                            ["package.example1.taihe", ["struct A { \n", "    a: i32; \n", "} \n"]]]],

    [SymbolConflictError, [["package.taihe", ["struct BadStruct { \n",
                                              "    a: i32; \n",
                                              "    a: i32; \n",
                                              "} \n"]]]],

    [RecursiveInclusionError, [["package.taihe", ["struct BadStructA { \n",
                                              "    a: BadStructB; \n",
                                              "} \n",
                                              "struct BadStructB { \n",
                                              "    a: BadStructC; \n",
                                              "} \n",
                                              "struct BadStructC { \n",
                                              "    a: BadStructA; \n",
                                              "} \n", ]]]],
]


if __name__ == "__main__":
    check_exception(test_cases_excepetions)
