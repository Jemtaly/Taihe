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
            rows.append([xs[0], float(xs[1]), float(xs[2]), float(xs[3])])
    df = pd.DataFrame.from_records(rows, columns=("Kind", "NItem", "SLen", "Duration"))
    df = df.pivot(columns=["Kind"], index=["SLen", "NItem"])
    df.columns = df.columns.droplevel(0)
    return df


C_INNER = "InnerAPI"
C_C = "C API → InnerAPI"
C_ARKVM = "NAPI (ArkVM) → InnerAPI"
C_NODEJS = "NAPI (NodeJS) → InnerAPI"
C_TAIHE = "Taihe → InnerAPI"
F_NAPI = "Flutter → NAPI"
F_CAPI = "Flutter → C API"
F_TAIHE = "Flutter → Taihe"

C_ALL = [C_INNER, C_TAIHE, C_C, C_ARKVM, C_NODEJS]
F_ALL = [F_NAPI, F_CAPI, F_TAIHE]
NEW_COLUMNS = {
    "x86-cpp-cpp": C_INNER,
    "x86-c-capi": C_C,
    "x86-cpp-taihe": C_TAIHE,
    "arkts-napi": C_ARKVM,
    "x86-nodejs-napi": C_NODEJS,
    "dart-napi": F_NAPI,
    "x86-dart-capi": F_CAPI,
    "x86-dart-taihe": F_TAIHE,
}


def normalize(df: pd.DataFrame):
    # Cast all x86 to aarch64 based on InnerAPI.
    x86_to_aarch64_factor = (df["cpp-cpp"] / df["x86-cpp-cpp"]).mean()
    for row_name in df.columns.values:
        assert isinstance(row_name, str)
        if row_name.startswith("x86-"):
            df[row_name] *= x86_to_aarch64_factor

    # Rename and reorder columns.
    df = df[NEW_COLUMNS.keys()].rename(NEW_COLUMNS, axis=1)  # type:ignore

    # Separate the benchmark for context switching.
    r = df.loc[0, 1500]
    df.drop(index=r.name, inplace=True)
    r = df.loc[1, 0]
    df.drop(index=r.name, inplace=True)
    r.name = 'Call Duration'

    # Drop the boring "NItem", which always equals to 1500.
    df.index = df.index.droplevel(1)
    return pd.DataFrame(r), df


def plot(df: pd.DataFrame):
    import seaborn as sns
    import matplotlib.pyplot as plt

    # Boxplot
    x = df[F_ALL].apply(lambda x: x / df[C_INNER])

    fig, ax = plt.subplots(1, 1)
    sns.barplot(x, ax=ax)
    print(x.describe())

    # plt.yscale('log')
    fig.tight_layout()
    plt.show()


def main():
    df = read_data()
    df_startup, df_main = normalize(df)
    df = df_main
    del df_startup
    print(df)
    plot(df)


if __name__ == "__main__":
    main()
