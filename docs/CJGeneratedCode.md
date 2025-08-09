# Taihe Cangjie 用户文档

**需要从官网下载仓颉工具链，[下载链接](https://cangjie-lang.cn/download/1.0.0)，下载之后`source xxx/cangjie/envsetup.sh`，可以尝试在命令行`echo $CANGJIE_HOME`来验证仓颉工具链是否正常配置。之后就可以正常使用taihe-tryit和taihec工具。**

## Cangjie 调用发布方 C++ API

假如我们有一个taihe idl文件，其中定义了一个函数`add_i32`。

```rust
// example/idl/integer.arithmetic.taihe
function add_i32(a: i32, b: i32): i32;
```

当在`user/main.cj`中调用`add_i32`会经历以下步骤：

1. `main.cj`中导入`integer.arithmetic`包并调用该包的`add_i32`函数
```swift
import integer.arithmetic

main() {
    println(arithmetic.add_i32(1,2))
}
```

2. `integer.arithmetic`包中的`add_i32`函数会调用对应的foreign函数`integer_arithmetic_add_i32_f2`

```swift
// generated/cj/integer.arithmetic.cj
package integer.arithmetic
foreign func integer_arithmetic_add_i32_f2(a: Int32, b: Int32): Int32
public func add_i32(a: Int32, b: Int32): Int32 {
    unsafe {
        let res = integer_arithmetic_add_i32_f2(a, b)
        return res
    }
}
```

3. 该foreign函数即是在C ABI层被external "C"导出的函数
```cpp
// generated/include/integer.arithmetic.abi.h
TH_EXPORT int32_t integer_arithmetic_add_i32_f2(int32_t a, int32_t b);
```

4. `integer_arithmetic_add_i32_f2`函数会调用发布方的cpp实现函数`add_i32`
```cpp
int32_t add_i32(int32_t a, int32_t b) {
  return a + b;
}
TH_EXPORT_CPP_API_add_i32(add_i32);
```
