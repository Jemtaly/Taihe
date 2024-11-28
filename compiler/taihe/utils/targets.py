from os import PathLike


class TargetBuffer:
    basename: str
    ext: str

    def __init__(self, basename: str, ext: str):
        self.basename = basename
        self.ext = ext

    def output_to(self, dst_path: PathLike):
        pass

    def show(self):
        pass


class TargetManager:
    def __init__(self):
        self.targets: list[TargetBuffer] = []

    def output_to(self, dst_path: PathLike):
        for target in self.targets:
            target.output_to(dst_path)

    def show(self):
        for target in self.targets:
            target.show()

    def add(self, target: TargetBuffer):
        self.targets.append(target)
