import test_string

main() { 
    let cjs="hello仓颉"
    let in_s=test_string.in_string(test_string.TString(cjs))
    let newString = test_string.getNewString(in_s)
    println(newString)
}