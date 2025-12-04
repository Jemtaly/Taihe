# String 设计文档

## 设计目标

修改现有 string 直接原因：1 新增 external 模式实现 0 拷贝提升 string 性能; 2 解决目前子系统需要使用 Opaque 类型来传递 UTF-16 编码字符串来避免编码转换的问题。

设计目标如下：

1. 支持 external 模式来提升某些场景下的性能；
2. taihe::string 的对外表现为 UTF-8 编码（类似 Objective C），但内部支持多编码存储；
3. 提供少量函数用于支持对于std::u16string的转换功能等。

## 实现

### C ABI

TStringFlags 位定义
```c++
enum TStringFlags {
  TSTRING_MODE_MASK      = 0xFFFF,
  TSTRING_ENCODING_MASK  = 0xFFFF0000,

  // 内存模式
  TSTRING_REF            = 1u,          // 引用外部 buffer，无 ownership
  TSTRING_EXT            = 1u << 1,     // 托管外部对象（带 destructor）

  // 缓存
  TSTRING_HAS_CACHE      = 1u << 2,     // cache 字段有效

  // 编码类型
  TSTRING_UTF8           = 1u << 16,
  TSTRING_UTF16          = 1u << 17,
};
```

TStringInfo — 主存储头（shared header）
```c++
struct TStringInfo {
  TRefCount count;         // shared refcount
  void *external_obj;      // 外部对象（可选）
  void (*dest)(void *);    // 外部对象析构函数（可选）
};
```
Owned 模式下 TStringInfo 代表一个共享字符串的“主体”；其后紧跟实际字符 buffer。

TStringCache — UTF8←→UTF16 缓存
```c++
struct TStringCache {
  TRefCount count;
  uint32_t length;
  char buffer[];
};
```
缓存由转换时 lazy 创建，依附于字符串实例，但具有自己独立 refcount。

TString — ABI 字符串对象
```c++
struct TString {
  uint32_t flags;
  uint32_t length;        // 单位：字节数（非 code unit 数）
  struct TStringInfo *pstrinfo;
  char const *ptr;        // 指向 UTF-8 或 UTF-16 数据
  char const *cache;      // 指向 UTF-8/UTF-16 “另一种编码形式”的缓存
};
```
TString 本身是 按值传递的小对象（类似 C++ 的 std::string_view + 句柄组合）

### C++ 投影层

#### 总体设计

C++ 投影层（Projection Layer）是 C 侧 ABI（TString 及其相关函数）的类型安全包装，用于为 C++ 开发者提供符合 C++ 语义的字符串类型，包括：

- 面向 UTF-8/UTF-16 双编码的透明访问与缓存管理

- RAII 语义的自动资源管理

- std::string_view 风格的“借用型”视图类型 taihe::string_view

- std::string 风格的“拥有型”动态字符串类型 taihe::string

- 与 C ABI 的零拷贝互操作能力

- 与标准库算法、比较、to_chars 等生态的自动适配

C++ 投影层通过一个轻量的 class 封装来隐藏底层的 TString 实现细节，使得上层逻辑能够像使用 std::string / std::string_view 那样自然地处理 UTF 字符串，同时保持 ABI 层的性能优势。

#### `taihe::string_view`

`string_view` 是一个轻量级的、不可变的字符串视图。它持有一个 `TString` 句柄但不负责释放内存：
```c++
struct string_view {
    explicit string_view(struct TString handle);
    // ...
protected:
    mutable struct TString m_handle;
};
```

其语义特点：

- 类似 std::string_view

- 常量只读

- 拷贝代价小（只复制句柄）

- 不会释放 `TString`（引用计数策略由 C 侧完成）

- 若字符串为 UTF-16 编码，则访问 UTF-8 相关接口时自动生成对应缓存

支持的构造方式包含：

- `char*` / `char16_t*` 零拷贝借用

- `std::string` / `std::string_view`

- `initializer_list<char>`

提供向 `std::string_view` 的隐式转换：
```c++
operator std::string_view() const noexcept;
```
注：该接口保留原因为易用性。但由于 `taihe::string` 支持多编码，所以使用缓存时会有性能开销，并非 0 开销接口

#### `taihe::string`

`string` 是拥有语义的字符串类型，它负责管理 TString 的生命周期。

`string : public string_view` 采用 视图类型的增强与拥有类型的继承合成 设计，使得 API 完整复用。

支持：

- UTF-8/UTF-16 构造

- 拷贝赋值、移动赋值（swap 优化）

- `operator+=`

- 拼接 `operator+`

此类型完全遵循 RAII，确保所有 ABI 层资源得到自动释放。

#### 关键 API 新增行为规范

1. 自动生成 UTF-8 缓存

    懒加载缓存机制, `string_view` 所有访问行为（如 `operator[]`, `size()`, `begin()` 等）都遵循以下规则:

    1 如果原字符串编码 == UTF-8, 直接访问 buffer

    2 否则自动生成 UTF-8 缓存, 返回缓存视图

    示例：
    ```c++
    operator std::string_view() const noexcept {
        if (tstr_encoding(m_handle) == TSTRING_UTF8) {
            return {tstr_buf(m_handle), tstr_len(m_handle)};
        }
        tstr_generate_utf8_cache(&m_handle);
        return {tstr_cache(m_handle), tstr_cache_len(m_handle)};
    }
    ```

2. 多编码能力支持

    不提供 `operator std::u16string_view` 以避免 API 污染

    对外 “看起来永远是 UTF-8” 类似 Objective C NSString

    `taihe::string_view/string` s内部可存 UTF-16

    提供显式的转换函数 `std::string u8string()` 和 `std::u16string u16string()` 用于转换为 std 类型

    提供 `is_utf8()` 和 `is_utf16()` 判断 `taihe::string` 是否为某种编码

    提供 `encoding()` 返回具体编码
    ```c++
    enum class StringEncoding { utf8, utf16, unknown };
    ```

    提供 `concat_utf16()` `substr_utf16()` 方法支持对 UTF-16 编码字符串进行操作（注：这两个函数感觉不太有提供的必要）
