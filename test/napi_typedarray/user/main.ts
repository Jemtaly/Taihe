import * as lib from "typedarray_test";

function main() {
  let arr = lib.NewUint8Array(5, 10);
  if ( arr[0] !== 10) throw new Error(`Unexpected result`);
  console.log("NewUint8Array:", arr);
  let sum = lib.SumUint8Array(arr);
  if ( sum !== 50) throw new Error(`Unexpected result`);
  console.log("SumUint8Array:", sum);
  let floatArr = lib.NewFloat32Array(5, 3.14);
  console.log("NewFloat32Array:", floatArr);
  let floatSum = lib.SumFloat32Array(floatArr);
  console.log("SumFloat32Array:", floatSum);
}

main();
