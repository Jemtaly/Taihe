# 测试样例用户侧调用代码

## 头文件
导入头文件 `packagename.proj.hpp`。例如：
```cpp
#include "integer.arithmetic.proj.hpp"
#include "integer.io.proj.hpp"
```

## 函数
根据 IDL 文件中定义的函数名调用 `packagename::funcname()`。例如：
```cpp
auto a = integer::io::input_i32();
auto b = integer::io::input_i32();
auto sum = integer::arithmetic::add_i32(a, b);
auto [quo, rem] = integer::arithmetic::divmod_i32(a, b);
integer::io::output_i32(sum);
```

## 结构体
使用 IDL 文件中定义的结构体名 `packagename::structname`，用花括号初始化结构体的成员变量。以 test/rgb 中 rgb.base.taihe 文件的定义为例：
```cpp
rgb::base::RGB color_rgb;
rgb::base::RGB color_rgb = rgb::base::RGB{0x11, 0x45, 0x14};
```

## 枚举类
使用 IDL 文件中定义的枚举类名及元素名。以 test/rgb 中 rgb.base.taihe 文件的定义为例：

1. 使用 `packagename::enumname::make_itemname(item_ctor_args, ...)` 构造一个 item 类型的 enum 对象：
    ```cpp
    rgb::base::Color yellow = rgb::base::Color::make_yellow();
    rgb::base::ColorOrRGBOrName color_114514 = rgb::base::ColorOrRGBOrName::make_rgb(RGB{0x11, 0x45, 0x14});
    rgb::base::ColorOrRGBOrName color_yellow = rgb::base::ColorOrRGBOrName::make_color(yellow);
    rgb::base::ColorOrRGBOrName color_my_color = rgb::base::ColorOrRGBOrName::make_name("My Color");
    rgb::base::ColorOrRGBOrName color_unknown = rgb::base::ColorOrRGBOrName::make_undefined();
    ```

2. 使用 `enum_obj.emplace_itemname(item_ctor_args, ...)` 在一个已有的 enum 对象上重新构造一个 item 类型对象：
    ```cpp
    color_my_color.emplace_name("emplace color");
    ```

3. 使用 `enum_obj.get_itemname_ptr()`，检查 enum 对象中实际包含的数据类型是否是 item 类型，是则得到一个指向 item 类型对象数据的指针，否则返回一个空指针：
    ```cpp
    auto* color_ptr_rgb_ptr = color_114514.get_rgb_ptr();
    ```

4. 使用 `enum_obj.unsafe_get_itemname_ptr()`，与 `enum_obj.get_itemname_ptr()` 类似但不检查：
    ```cpp
    auto* color_unsafe_ptr_rgb_ptr = color_114514.unsafe_get_rgb_ptr();
    ```

5. 使用 `enum_obj.holds_itemname()` 判断 enum 对象中实际包含的数据类型是否是 item 类型，返回值为 `bool` 类型：
    ```cpp
    bool holds_name = color_my_color.holds_name();
    ```

6. 使用 `enum_obj.get_tag()` 得到当前对象的类型，返回值为 `packagename::enumname::tag_t` 类型：
    ```cpp
    rgb::base::ColorOrRGBOrName::tag_t color_my_color_tag = color_my_color.get_tag();
    ```

## 接口
用户可自定义一个类实现 IDL 中定义的一个或多个接口，`make_holder<classname, ifacename1, ifacename2, ...>(class_init_args, ...)`。以 test/rgb 中 rgb.show.taihe 文件的定义为例：
```cpp
class ColoredCircle {
    // Definition
}
auto circle = make_holder<ColoredCircle, rgb::show::IShowable>("A", 10, color_114514);
```

接口对象的生命周期采用引用计数管理，每个接口对应 `packagename::ifacename` 和 `packagename::weak::ifacename` 两种类型，它们分别对应 C++ 中的 `std::shared_ptr` 和 `std::weak_ptr`。

在传递参数的时候，建议使用`packagename::weak::ifacename`作为参数类型，例如：
```cpp
void copyColorImpl(weak::IColorable dst, weak::IColorable src) {
    dst.setColor(src.getColor());
}
```
