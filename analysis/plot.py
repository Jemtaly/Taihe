#!/usr/bin/env python
import pandas as pd


def read_data():
    LOOKUP = "tbench:"
    SKIP = len(LOOKUP)

    rows = []
    with open("./data.txt") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            pos = line.find("tbench:")
            if pos == -1:
                print("ERR:", line)
                assert False
            xs = line[pos + SKIP :].split(",")
            rows.append([xs[0], int(xs[1]), int(xs[2]), int(xs[3])])
    df = pd.DataFrame.from_records(rows, columns=("Kind", "NItem", "SLen", "Duration"))
    df = df.pivot(columns=["Kind"], index=["SLen", "NItem"])
    df.columns = df.columns.droplevel(0)
    return df


C_INNER = "C++ → Inner API"
C_C = "C++ → C API"
C_ARKVM = "C++ → NAPI (ArkVM)"
C_NODEJS = "C++ → NAPI (NodeJS)"
F_NAPI = "Flutter → N API"
F_CAPI = "Flutter → C API"
F_TAIHE = "Flutter → Taihe"


def normalize(df: pd.DataFrame):
    # Cast all x86 to aarch64 based on C-API.
    x86_to_aarch64_factor = df["c-capi"] / df["x86-c-capi"]
    for row_name in df.columns.values:
        assert isinstance(row_name, str)
        if row_name.startswith("x86-"):
            df[row_name] *= x86_to_aarch64_factor

    # Rename and reorder columns.
    del df["c-capi"]
    new_columns = {
        "x86-cpp-cpp": C_INNER,
        "x86-c-capi": C_C,
        "arkts-napi": C_ARKVM,
        "x86-nodejs-napi": C_NODEJS,
        "dart-napi": F_NAPI,
        "x86-dart-capi": F_CAPI,
        "x86-dart-taihe": F_TAIHE,
    }
    df = df[new_columns.keys()].rename(new_columns, axis=1)  # type:ignore

    # Separate the benchmark for context switching.
    r = df.loc[0, 1500]
    df.drop(index=r.name, inplace=True)
    r = df.loc[1, 1]
    df.drop(index=r.name, inplace=True)

    # Drop the boring "NItem", which always equals to 1500.
    df.index = df.index.droplevel(1)
    return pd.DataFrame(r), df


def plot(df: pd.DataFrame):
    import seaborn as sns
    import matplotlib.pyplot as plt

    # Boxplot
    x = df[[F_CAPI, F_TAIHE]].apply(lambda x: x / df[C_INNER])

    fig, ax = plt.subplots(1, 1)
    sns.boxplot(x, ax=ax)
    print(x.describe())

    # plt.yscale('log')
    fig.tight_layout()
    plt.show()


def main():
    df = read_data()
    df_startup, df_main = normalize(df)
    # print(df_startup)

    plot(df_main)


if __name__ == "__main__":
    main()
