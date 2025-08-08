import test_string

main() { 
    let cjs="hello仓颉"
    unsafe{let cjcs=LibC.mallocCString(cjs)
    let in_s=test_string.in_string(test_string.TString(0,UInt32(cjcs.size()),cjcs.getChars()))
    let newString = test_string.getNewString(in_s)
    println(newString)
    }
}