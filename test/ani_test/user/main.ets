import * as ani_test from "../author_generated/ani_test"
function main() {
    console.log("hello main")
    console.log(ani_test.getNumberArg4(1, 2, 3, 4))

    const option: ani_test.Option = {
        src: "path/to/source",
        dest: 123,
        files: ["file1.txt", "file2.txt", "file3.txt"],
    };
    ani_test.optionArg1("option1", option)

    console.log(ani_test.getOption())

    ani_test.testUnion({tag: 0, value: undefined})
    ani_test.testUnion({tag: 1, value: 1 as int})
    ani_test.testUnion({tag: 2, value: 2 as float})
    ani_test.testUnion({tag: 3, value: "Hello from STS!"})

    console.log(ani_test.getUnion(0))
    console.log(ani_test.getUnion(1))
    console.log(ani_test.getUnion(2))
    console.log(ani_test.getUnion(3))
}
