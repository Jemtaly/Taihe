import { showOptionalInt, makeOptionalInt } from "opt_test";

function main() {
  showOptionalInt(1);
  showOptionalInt(undefined);
  let res1 = makeOptionalInt(true);
  if ( res1 !== 10) throw new Error(`Unexpected result`);
  console.log(res1);
  let res2 = makeOptionalInt(false);
  if ( res2 !== undefined) throw new Error(`Unexpected result`);
  console.log(res2);
}

main();
