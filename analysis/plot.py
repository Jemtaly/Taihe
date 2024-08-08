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


def normalize(df: pd.DataFrame):
    # Use x86-cpp as "100%", or the standard to normalize.
    # Also ensure that x86-capi and aarch64-capi are equal.
    x86_cpp = df["x86-cpp-cpp"]
    aarch64_cpp = df["x86-cpp-cpp"] * (df["c-capi"] / df["x86-c-capi"])

    for row_name in df.columns.values:
        assert isinstance(row_name, str)
        std = x86_cpp if row_name.startswith("x86-") else aarch64_cpp
        df[row_name] /= std

    # Rename and reorder columns.
    del df["c-capi"]
    del df["x86-cpp-cpp"]
    new_columns = {
        "x86-c-capi": "C API",
        "arkts-napi": "NAPI",
        "x86-nodejs-napi": "NAPI (NodeJS)",
        "dart-napi": "Flutter (NAPI)",
        "x86-dart-capi": "Flutter (C API)",
        "x86-dart-taihe": "Flutter (Taihe)",
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

    fig, ax = plt.subplots(1, 1)
    del fig
    sns.lineplot(df, ax=ax)
    plt.yscale('log')
    plt.show()
    print(df)


def main():
    df = read_data()
    df_startup, df_main = normalize(df)
    print(df_startup)

    plot(df_main)


if __name__ == "__main__":
    main()
