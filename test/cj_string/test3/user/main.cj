import test_string

main() { 
    let cjs="hello仓颉"
    let in_s=test_string.in_string(2,cjs)
    let newString = test_string.getNewString(in_s)
    println(newString)
}