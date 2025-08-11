import test_string
import std.time.*
main() { 
    let cjs = StringBuilder(r'a', Int64(Int16.Max) *128).toString()
    let in_s = test_string.in_string(8, cjs)
    
    let iterations = 100  // 执行100次
    let start =  MonoTime.now()
    var newString =""
    // 多次执行目标函数
    for (i in 0..iterations) {
         newString = test_string.getNewString(in_s)
    }
    
    let end =  MonoTime.now()
    let result = end - start
    println("length:${cjs.size}")
    println("length:${newString.size}")
    println("total cost: ${result.toMilliseconds()/iterations}ms")
}