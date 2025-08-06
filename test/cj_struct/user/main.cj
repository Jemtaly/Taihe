import test_struct

main() {
    let color = test_struct.Color(1,2,3)
    let newColor = test_struct.getNewColor(color)
    println(newColor.r)
    println(newColor.g)
    println(newColor.b)
}