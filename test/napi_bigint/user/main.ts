import * as lib from "bigint_test";

function main() {
    let num1: bigint = lib.processBigInt(18446744073709556846815135465465564525825546451551616n);
    if ( num1 !== 340282366920938559954882708249570542425151457938797744320353482840211456n) throw new Error(`Unexpected result`);
    console.log(num1);
    let num2: bigint = lib.processBigInt(-65535n);
    if ( num2 !== -1208907372870555465154560n) throw new Error(`Unexpected result`);
    console.log(num2);
}

main()
