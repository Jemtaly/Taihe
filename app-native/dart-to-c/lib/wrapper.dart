import 'dart:ffi' as ffi;
import 'package:ffi/ffi.dart';

// Define the function type
typedef ConcatNative = ffi.Pointer<Utf8> Function(ffi.Pointer<ffi.Pointer<Utf8>> args);
typedef ConcatDart = ffi.Pointer<Utf8> Function(ffi.Pointer<ffi.Pointer<Utf8>> args);

ffi.DynamicLibrary _lib = ffi.DynamicLibrary.open("impl.so");
ConcatDart _ohConcat = _lib.lookupFunction<ConcatNative, ConcatDart>('OH_Concat');

String concat(List<String> args) {
  final argPointers = args.map((s) => s.toNativeUtf8()).toList();
  final pointerPointer = calloc<ffi.Pointer<Utf8>>(argPointers.length + 1);

  for (var i = 0; i < argPointers.length; i++) {
    pointerPointer[i] = argPointers[i];
  }
  pointerPointer[argPointers.length] = ffi.nullptr;

  final result = _ohConcat(pointerPointer);
  final dartString = result.toDartString();

  // Clean up
  for (var ptr in argPointers) {
    calloc.free(ptr);
  }
  calloc.free(pointerPointer);
  calloc.free(result);

  return dartString;
}
