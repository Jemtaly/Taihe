import 'dart:ffi' as ffi;

typedef ConcatNative = ffi.Handle Function(ffi.Handle);
typedef ConcatDart = Object Function(Object);

ffi.DynamicLibrary _lib = ffi.DynamicLibrary.open("dart_api.so");

final ConcatDart concatNative =
    _lib.lookupFunction<ConcatNative, ConcatDart>('dart_concat');

String concat(List<String> args) {
  return concatNative(args) as String;
}
