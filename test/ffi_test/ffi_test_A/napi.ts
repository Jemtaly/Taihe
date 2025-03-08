// import { KList } from "./KContainer";
import {makeACrash, makeBCrash, makeCCrash, get_bCrashList, get_stringList} from './libtest'

function main() {
    let bCrashList1 = get_bCrashList();
    let stringList1 = get_stringList();
    let ACrash1 = makeACrash(bCrashList1);
    let BCrash1 = makeBCrash(stringList1);
    let CCrash1 = makeCCrash("abcde");
    console.log("ACrash1: ", bCrashList1, " != ", ACrash1.get_list())
    console.log("BCrash1: ", stringList1, " != ", BCrash1.get_strList())
    console.log("CCrash1: ", "abcde", " == ", CCrash1.get_name())
}

main()