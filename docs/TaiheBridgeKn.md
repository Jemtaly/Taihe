# Taihe 桥接 Kotlin 与 Arkts 方案

## 整体方案概述

Kotlin跨平台使用Kotlin Native技术，将Kotlin代码编译为无需虚拟机的二进制技术，但是目前的Kotlin的CExport在与Arkts桥接时遇到了许多需要优化的点。

以Arkts为例

Kotlin Native原流程如下：

Kotlin文件编译为二进制，用户通过一同导出的.h与.a/.so来使用Kotlin里的方法与变量

1. kotlin compiler: .kt -> kotlin.h + kotlin.a/.so

2. 手写napi.cpp + .d.ts对kotlin.h的方法进行绑定

3. ets调用napi方法

使用Taihe桥接时，流程如下：

1. kotlin compiler: .kt -> .a + .taihe

2. taihe compiler: .taihe -> .so

3. 手写.d.ts(待实现自动生成)

4. ets调用napi方法

其中第2步为关键流程，该流程完成了kotlin导出符号与taihe的绑定，taihe与napi的绑定

## Kotlin Native

Kotlin Native会把Kotlin文件的内容以函数方式导出

func 导出为 func
member 导出为 get / set func

假设有如下kotlin文件
```Kotlin
@OptIn(kotlinx.cinterop.ExperimentalForeignApi::class)
fun createMyStr1(): CPointer<out CPointed> {
    val MyStr : String = "Hello world"
    return StableRef.create(MyStr).asCPointer()
}
```

使用Kotlin Native导出, 则在IR中我们可以看到
```ll
define ptr @"kfun:#createMyStr1(){}kotlinx.cinterop.CPointer<out|kotlinx.cinterop.CPointed>"() #4 {
 xxx
}

...

define ptr @_konan_function_3() #4 {
prologue:
  br label %locals_init

locals_init:                                      ; preds = %prologue
  %0 = phi ptr [ null, %prologue ]
  br label %stack_locals_init

stack_locals_init:                                ; preds = %locals_init
  call void @Kotlin_mm_safePointFunctionPrologue() #113
  br label %entry

entry:                                            ; preds = %stack_locals_init
  %1 = call ptr @"kfun:#createMyStr1(){}kotlinx.cinterop.CPointer<out|kotlinx.cinterop.CPointed>"()
  br label %call_success

call_success:                                     ; preds = %entry
  br label %epilogue

epilogue:                                         ; preds = %call_success
  %2 = phi ptr [ %1, %call_success ]
  ret ptr %2
}
```

然后在Cexport，有
```C++
extern "C" void* _konan_function_3();
static void* _konan_function_3_impl() {
  Kotlin_initRuntimeIfNeeded();
  ScopedRunnableState stateGuard;
  FrameOverlay* frame = getCurrentFrame();   try {
  auto result =   _konan_function_3();
  return result;
   } catch (...) {       SetCurrentFrame(reinterpret_cast<KObjHeader**>(frame));
       HandleCurrentExceptionWhenLeavingKotlinCode();
   } 
}

...

static static_ExportedSymbols __konan_symbols = {
  .kotlin = {
    .root = {
        ...
      /* createMyStr1 = */ _konan_function_3_impl, 
        ...
    },
  },
};
```

从C/C++视角看，调用流程为 __konan_symbols.createMyStr1 -> _konan_function_3_impl -> _konan_function_3 -> kfun:#createMyStr1()

## Taihe方案

在Taihe方案中，仍然使用 _konan_function_3 -> kfun:#createMyStr1() 这部分的流程

但在api.cpp中，如_konan_function_3_impl的参数类型则为taihe的参数类型

如：
```C++
extern "C" void _konan_function_13 (KObjHeader*, KObjHeader*);
static void _konan_function_13_impl(Kref_KNObject thiz, ::taihe::core::string_view a) {
  Kotlin_initRuntimeIfNeeded();
  ScopedRunnableState stateGuard;
  FrameOverlay* frame = getCurrentFrame();
  KObjHolder a_holder;
  KObjHolder thiz_holder;
  try {
    _konan_function_13(DerefStablePointer(thiz, thiz_holder.slot()), CreateStringFromTHString(a, a_holder.slot()));
  } catch (...) {
    SetCurrentFrame(reinterpret_cast<KObjHeader**>(frame));
    HandleCurrentExceptionWhenLeavingKotlinCode();
  }
}
```
然后再使用taihe类型与napi对接

## 支持类型

目前Kotlin类型支持如下：

✅ Basic Type

✅ String

✅ Array

✅ Bytearray

✅ List

✅ Map

✅ Interface

✅ Class

❌ Enum

Doing:

object

## 类型转换对照

| kotlin types  | taihe type          |
|---------------|---------------------|
| Int           | int32_t             |
| long          | int64_t             |
| float         | float               |
| double        | double              |
| Boolean       | bool                |
| String        | taihe::core::string |
| interface     | taihe interface     |
| class         | taihe interface     |
| Array         | sturct KArray       |
| List          | struct KList        |
| Map           | struct Map          |
| ByteArray     | struct KByteArray   |

## 类型转换详情

### 基础类型

基础类型本质一样，可以直接使用

### String

kotlin的string使用utf16编码，目前taihe采用的是char*存储字符串，需要有一个转换函数

maybe to do: 增加utf16的taihe string

目前该转换函数在项目kt_node_rt的boxing-kt-runtime分支上

```C++
OBJ_GETTER(CreateStringFromTHString, ::taihe::core::string_view thstring) {
    const char* cstring = thstring.c_str();
    RETURN_RESULT_OF(utf8ToUtf16, cstring, cstring ? strlen(cstring) : 0);
}

::taihe::core::string CreateTHStringFromString(KConstRef kref) {
    if (kref == nullptr) return nullptr;
      std::string utf8 = to_string(kref->array());
    TString tstr;
    ::memcpy(tstr_initialize(&tstr, utf8.size() + 1), utf8.c_str(), utf8.size());
    tstr.length = utf8.size();
    return ::taihe::core::string(tstr);
}
```

### Class
kotlin目前创建对象时，返回的是一个指针，在C语言侧可以理解为一个void*指针

在taihe侧，一个kotlin class 对应一个taihe interface + class implement

```C++
class KNObject {
protected:
    void* m_handle;
public:
    KNObject(void* ptr): m_handle(ptr) {}
    ~KNObject() {
        DisposeStablePointer(m_handle);
        m_handle = nullptr;
    }
    // member func
}
```

桥接两个复杂类型自然也涉及转换函数，class转换函数如下：

```C++
// as param
KObjHeader* thobj_tokotlin(void* thiface, KObjHeader** kobj_slot) {
  void* data_ptr = *(void**)((char*)thiface + 8);
  void* kn_obj = *(void**)((char*)data_ptr + 16);
  KObjHeader* result = DerefStablePointer(kn_obj, kobj_slot);
  return result;
}

#define THOBJ_toKotlin(obj, obj_slot) thobj_tokotlin((void*)&obj, obj_slot)

// as retval
taihe::core::make_holder<class_impl, interface>(CreateStablePointer(result))
```

### Interface

kotlin目前创建对象时，返回的是一个指针，在C语言侧可以理解为一个void*指针

在taihe侧，一个对象实际是一个胖指针，一个指针指向虚表，一个指针指向数据

使用一个class implement作为绑定类型

```C++
class KNObject {
protected:
    void* m_handle;
public:
    KNObject(void* ptr): m_handle(ptr) {}
    ~KNObject() {
        DisposeStablePointer(m_handle);
        m_handle = nullptr;
    }
    // member func
}
```

理论上interface不应该有class implement，因为interface为接口，但是为了考虑kotlin的一些特殊用法，interface也需要一个默认的class implement

类型转换，同class

```kotlin
interface A {
    fun foo()
}

fun bar(): A {
    return object: A {
        override fun foo() {
            println("new foo")
        }
    }
}
```

### Object


### Container

Kotlin container在传到C语言侧已失去泛型类型信息
```C++
typedef void* container_KNativePtr;

typedef struct {
  container_KNativePtr pinned;
} container_kref_kotlin_collections_List;
typedef struct {
  container_KNativePtr pinned;
} container_kref_kotlin_collections_Map;
typedef struct {
  container_KNativePtr pinned;
} container_kref_kotlin_Array;
```

如果用户需要在其他语言使用容器信息，如随机存取等能力，则理论上需要在Kotlin本身运行时能力上修改

目前在taihe，这些container 都以一个包含一个pointer的struct存在
```C++
struct KArray{
    uint64_t ptr;
}

struct KList{
    uint64_t ptr;
}

struct KMap{
    uint64_t ptr;
}
```
