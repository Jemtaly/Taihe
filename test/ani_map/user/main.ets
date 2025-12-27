/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {BusinessError} from "@ohos.base";
import * as map_test from "map_test";
import {Color, Data, ICpu, ICpuInfo, ICpuZero, Pair} from "map_test";

loadLibrary("ani_map");

class A {
    constructor() {}
}

function getCpuSize() {
    // test function getCpuSize(r: Map<string, ICpu>): int
    let cpu0: ICpu = map_test.makeCpu();
    let cpu1: ICpu = map_test.makeCpu();
    let cpus: Map<string, ICpu> = {
        "cpu0": cpu0,
        "cpu1": cpu1,
    };
    let ret0: int = map_test.getCpuSize(cpus);
    console.log(ret0);
    arktest.assertEQ(ret0, 2);
}

function getASize() {
    // test function getASize(r: Map<int, Object>): int
    let a = new A();
    let b = new A();
    let c = new A();
    let As: Map<int, Object> = {
        0: a,
        1: b,
        2: c,
    };
    let ret1: int = map_test.getASize(As);
    console.log(ret1);
    arktest.assertEQ(ret1, 3);
}

function getStringIntSize() {
    // test function getStringIntSize(r: Map<string, int>): int
    let map: Map<string, int> = {
        "aaa": 0,
        "bbb": 1,
        "ccc": 2,
        "ddd": 3,
    };
    let ret2: int = map_test.getStringIntSize(map);
    console.log(ret2);
    arktest.assertEQ(ret2, 4);
}

function createStringString() {
    // test function createStringString(a: int): Map<string, string>
    let strMap: Map<string, string> = map_test.createStringString(5);
    console.log(`Key: 0, Value: ` + strMap.get("0"));
    arktest.assertEQ(strMap.get("0"), "abc");
}

function getMapfromArray() {
    // test function getMapfromArray(d: (Data[])): Map<string, int>
    let d0: Data = {a: "a", b: 1};
    let d1: Data = {a: "b", b: 2};
    let d2: Data = {a: "c", b: 3};
    let data: Data[] = [d0, d1, d2];
    let dataMap: Map<string, int> = map_test.getMapfromArray(data);
    console.log(`Key: b, Value: ` + dataMap.get("b"));
    arktest.assertEQ(dataMap.get("b"), 2);
}

function getDatafromMap() {
    // test function getDatafromMap(m: Map<string, Data>, k: string): Data
    let data1: Data = {a: "aaa", b: 1};
    let data2: Data = {a: "bbb", b: 2};
    let data3: Data = {a: "ccc", b: 3};
    let dmap: Map<string, Data> = {
        "aa": data1,
        "bb": data2,
        "cc": data3,
    };
    let d: Data = map_test.getDatafromMap(dmap, "bb");
    console.log(`Data Value: ` + d.a);
    arktest.assertEQ(d.a, "bbb");
}

function foreachMap() {
    let mapStrForeach: Map<string, string> = {
        "key1": "v1",
        "key2": "v2",
        "key3": "v3",
        "key4": "v4",
    };
    map_test.foreachMap(mapStrForeach);
}

function main() {
    console.log("##############map ut start#############")
    const suite = new arktest.ArkTestsuite("map ut")
    suite.addTest("get Cpu Size test", getCpuSize)
    suite.addTest("get A Size test", getASize)
    suite.addTest("get String Int Size test", getStringIntSize)
    suite.addTest("create String String test", createStringString)
    suite.addTest("get Map from Array test", getMapfromArray)
    suite.addTest("get Data from Map test", getDatafromMap)
    suite.addTest("foreach Map test", foreachMap)
    suite.addTest("testMapfunc01", testMapfunc01)
    suite.addTest("testMapfunc02", testMapfunc02)
    suite.addTest("testMapfunc03", testMapfunc03)
    suite.addTest("testMapfunc04", testMapfunc04)
    suite.addTest("testMapfunc05", testMapfunc05)
    suite.addTest("testMapfunc06", testMapfunc06)
    suite.addTest("testMapfunc07", testMapfunc07)
    suite.addTest("testMapfunc08", testMapfunc08)
    suite.addTest("testMapfunc09", testMapfunc09)
    suite.addTest("testMapfunc10", testMapfunc10)
    suite.addTest("testMapfunc11", testMapfunc11)
    suite.addTest("testMapfunc12", testMapfunc12)
    suite.addTest("testMapfunc13", testMapfunc13)
    suite.addTest("testMapfunc14", testMapfunc14)
    suite.addTest("testMapfunc15", testMapfunc15)
    suite.addTest("testMapfunc16", testMapfunc16)
    suite.addTest("testMapfunc17", testMapfunc17)
    suite.addTest("testMapfunc18", testMapfunc18)
    suite.addTest("testMapfunc19", testMapfunc19)
    suite.addTest("testMapfunc20", testMapfunc20)
    suite.addTest("testMapfunc21", testMapfunc21)
    suite.addTest("testMapfunc22", testMapfunc22)
    suite.addTest("testMapfunc23", testMapfunc23)
    suite.addTest("testMapfunc24", testMapfunc24)
    suite.addTest("testMapfunc25", testMapfunc25)
    suite.addTest("testMapfunc26", testMapfunc26)
    suite.addTest("testMapfunc27", testMapfunc27)
    suite.addTest("testMapfunc28", testMapfunc28)
    suite.addTest("testMapfunc29", testMapfunc29)
    suite.addTest("testMapfunc30", testMapfunc30)
    suite.addTest("testMapfunc31", testMapfunc31)
    suite.addTest("testMapfunc32", testMapfunc32)
    suite.addTest("testMapfunc33", testMapfunc33)
    suite.addTest("testMapfunc34", testMapfunc34)
    suite.addTest("testMapfunc35", testMapfunc35)
    suite.addTest("testMapfunc36", testMapfunc36)
    suite.addTest("testMapfunc37", testMapfunc37)
    suite.addTest("testMapfunc38", testMapfunc38)
    suite.addTest("testMapfunc39", testMapfunc39)
    suite.addTest("testMapfunc40", testMapfunc40)
    suite.addTest("testMapfunc41", testMapfunc41)
    suite.addTest("testMapfunc42", testMapfunc42)
    suite.addTest("testMapfunc43", testMapfunc43)
    suite.addTest("testMapfunc44", testMapfunc44)
    suite.addTest("testMapfunc45", testMapfunc45)
    suite.addTest("testMapfunc46", testMapfunc46)
    suite.addTest("testMapfunc47", testMapfunc47)
    suite.addTest("testMapfunc48", testMapfunc48)
    suite.addTest("testMapfunc49", testMapfunc49)
    suite.addTest("testMapfunc50", testMapfunc50)
    suite.addTest("testMapfunc51", testMapfunc51)
    exit(suite.run())
    console.log("##############map ut end#############")
}


// function mapfunc01(m: Map<string, boolean>): boolean
function testMapfunc01() {
    let testData: Map<string, boolean> = {"key1": true, "key2": false};
    let result: boolean = map_test.mapfunc01(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc02(m: Map<string, byte>): boolean
function testMapfunc02() {
    let testData:
        Map<string, byte> = {"key1": (123).toByte(), "key2": (45).toByte()};
    let result: boolean = map_test.mapfunc02(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc03(m: Map<string, short>): boolean
function testMapfunc03() {
    let testData: Map<string, short> = {
        "key1": (1234).toShort(),
        "key2": (5678).toShort()
    };
    let result: boolean = map_test.mapfunc03(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc04(m: Map<string, int>): boolean
function testMapfunc04() {
    let testData:
        Map<string, int> = {"key1": (12345).toInt(), "key2": (67890).toInt()};
    let result: boolean = map_test.mapfunc04(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc05(m: Map<string, long>): boolean
function testMapfunc05() {
    let testData: Map<string, long> = {
        "key1": (123456).toLong(),
        "key2": (789012).toLong()
    };
    let result: boolean = map_test.mapfunc05(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc06(m: Map<string, float>): boolean
function testMapfunc06() {
    let testData: Map<string, float> = {"key1": 123.45f, "key2": 67.89f};
    let result: boolean = map_test.mapfunc06(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc07(m: Map<string, double>): boolean
function testMapfunc07() {
    let testData: Map<string, double> = {
        "key1": (123.456).toDouble(),
        "key2": (789.012).toDouble()
    };
    let result: boolean = map_test.mapfunc07(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc08(m: Map<string, string>): boolean
function testMapfunc08() {
    let testData: Map<string, string> = {"key1": "value1", "key2": "value2"};
    let result: boolean = map_test.mapfunc08(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc09(m: Map<string, (byte[])>): boolean
function testMapfunc09() {
    let testData: Map<string, (byte[])> = {
        "key1": [(1).toByte(), (2).toByte(), (3).toByte()],
        "key2": [(4).toByte(), (5).toByte(), (6).toByte()]
    };
    let result: boolean = map_test.mapfunc09(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc10(m: Map<string, (short[])>): boolean
function testMapfunc10() {
    let testData: Map<string, (short[])> = {
        "key1": [(123).toShort(), (456).toShort()],
        "key2": [(789).toShort(), (1011).toShort()]
    };
    let result: boolean = map_test.mapfunc10(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc11(m: Map<string, (int[])>): boolean
function testMapfunc11() {
    let testData: Map<string, (int[])> = {
        "key1": [(1234).toInt(), (5678).toInt()],
        "key2": [(9012).toInt(), (3456).toInt()]
    };
    let result: boolean = map_test.mapfunc11(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc12(m: Map<string, (long[])>): boolean
function testMapfunc12() {
    let testData: Map<string, (long[])> = {
        "key1": [(12345).toLong(), (67890).toLong()],
        "key2": [(11111).toLong(), (22222).toLong()]
    };
    let result: boolean = map_test.mapfunc12(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc13(m: Map<string, (ArrayBuffer[])>): boolean
function testMapfunc13() {
    let testData: Map<string, (ArrayBuffer[])> = {
        "key1": [new ArrayBuffer(8), new ArrayBuffer(8)],
        "key2": [new ArrayBuffer(8), new ArrayBuffer(8)]
    };
    let result: boolean = map_test.mapfunc13(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc14(m: Map<string, (boolean[])>): boolean
function testMapfunc14() {
    let testData: Map<string, (boolean[])> = {
        "key1": [true, false],
        "key2": [false, true]
    };
    let result: boolean = map_test.mapfunc14(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc15(m: Map<string, (string[])>): boolean
function testMapfunc15() {
    let testData: Map<string, (string[])> = {
        "key1": ["value1", "value2"],
        "key2": ["value3", "value4"]
    };
    let result: boolean = map_test.mapfunc15(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc16(m: Map<string, (int | boolean | string)>): boolean
function testMapfunc16() {
    let testData: Map<string, (int|boolean|string)> = {
        "key1": 123,
        "key2": true,
        "key3": "value"
    };
    let result: boolean = map_test.mapfunc16(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc17(m: Map<string, Color>): boolean
function testMapfunc17() {
    let testData: Map<string, Color> = {"key1": Color.RED, "key2": Color.GREEN};
    let result: boolean = map_test.mapfunc17(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc18(m: Map<string, Pair>): boolean
function testMapfunc18() {
    let pair1: Pair = {a: "one", b: true};
    let pair2: Pair = {a: "two", b: false};
    let testData: Map<string, Pair> = {"key1": pair1, "key2": pair2};
    let result: boolean = map_test.mapfunc18(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc19(m: Map<string, ICpuZero>): boolean
function testMapfunc19() {
    let a: ICpuZero = map_test.makeICpuZero();
    let b: ICpuZero = map_test.makeICpuZero();
    let testData: Map<string, ICpuZero> = {"key1": a, "key2": b};
    let result: boolean = map_test.mapfunc19(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc20(m: Map<string, ICpuInfo>): boolean
function testMapfunc20() {
    let a: ICpuInfo = map_test.makeICpuInfo();
    let b: ICpuInfo = map_test.makeICpuInfo();
    let testData: Map<string, ICpuInfo> = {"key1": a, "key2": a};
    let result: boolean = map_test.mapfunc20(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc21(m: Map<string, Object>): boolean
function testMapfunc21() {
    let testData: Map<string, Object> = {"key1": "val1", "key2": "val2"};
    let result: boolean = map_test.mapfunc21(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc22(m: Map<string, Map<string, boolean>>): boolean
function testMapfunc22() {
    let testData: Map<string, Map<string, boolean>> = {
        "key1": {"subkey1": true, "subkey2": false},
        "key2": {"subkey3": true, "subkey4": false}
    };
    let result: boolean = map_test.mapfunc22(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc23(m: Map<string, Map<string, int>>): boolean
function testMapfunc23() {
    let testData: Map<string, Map<string, int>> = {
        "key1": {"subkey1": 100, "subkey2": 200},
        "key2": {"subkey3": 300, "subkey4": 400}
    };
    let result: boolean = map_test.mapfunc23(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc24(m: Map<string, Map<string, (int[])>>): boolean
function testMapfunc24() {
    let testData: Map<string, Map<string, (int[])>> = {
        "key1": {"subkey1": [1, 2, 3], "subkey2": [4, 5, 6]},
        "key2": {"subkey3": [7, 8, 9], "subkey4": [10, 11, 12]}
    };
    let result: boolean = map_test.mapfunc24(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc25(m: Map<string, Map<string, string>>): boolean
function testMapfunc25() {
    let testData: Map<string, Map<string, string>> = {
        "key1": {"subkey1": "value1", "subkey2": "value2"},
        "key2": {"subkey3": "value3", "subkey4": "value4"}
    };
    let result: boolean = map_test.mapfunc25(testData);
    arktest.assertEQ(result, true);
}

// function mapfunc26(): Map<string, boolean>
function testMapfunc26() {
    let result: Map<string, boolean> = map_test.mapfunc26();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc27(): Map<string, byte>
function testMapfunc27() {
    let result: Map<string, byte> = map_test.mapfunc27();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc28(): Map<string, short>
function testMapfunc28() {
    let result: Map<string, short> = map_test.mapfunc28();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc29(): Map<string, int>
function testMapfunc29() {
    let result: Map<string, int> = map_test.mapfunc29();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc30(): Map<string, long>
function testMapfunc30() {
    let result: Map<string, long> = map_test.mapfunc30();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc31(): Map<string, float>
function testMapfunc31() {
    let result: Map<string, float> = map_test.mapfunc31();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc32(): Map<string, double>
function testMapfunc32() {
    let result: Map<string, double> = map_test.mapfunc32();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc33(): Map<string, string>
function testMapfunc33() {
    let result: Map<string, string> = map_test.mapfunc33();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc34(): Map<string, (byte[])>
function testMapfunc34() {
    let result: Map<string, (byte[])> = map_test.mapfunc34();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc35(): Map<string, (short[])>
function testMapfunc35() {
    let result: Map<string, (short[])> = map_test.mapfunc35();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc36(): Map<string, (int[])>
function testMapfunc36() {
    let result: Map<string, (int[])> = map_test.mapfunc36();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc37(): Map<string, (long[])>
function testMapfunc37() {
    let result: Map<string, (long[])> = map_test.mapfunc37();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc38(): Map<string, ArrayBuffer>
function testMapfunc38() {
    let result: Map<string, ArrayBuffer> = map_test.mapfunc38();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc39(): Map<string, (boolean[])>
function testMapfunc39() {
    let result: Map<string, (boolean[])> = map_test.mapfunc39();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc40(): Map<string, (string[])>
function testMapfunc40() {
    let result: Map<string, (string[])> = map_test.mapfunc40();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc41(): Map<string, (int | boolean | string)>
function testMapfunc41() {
    let result: Map<string, (int|boolean|string)> = map_test.mapfunc41();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc42(): Map<string, Color>
function testMapfunc42() {
    let result: Map<string, Color> = map_test.mapfunc42();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc43(): Map<string, Pair>
function testMapfunc43() {
    let result: Map<string, Pair> = map_test.mapfunc43();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc44(): Map<string, ICpuZero>
function testMapfunc44() {
    let result: Map<string, ICpuZero> = map_test.mapfunc44();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc45(): Map<string, ICpuInfo>
function testMapfunc45() {
    let result: Map<string, ICpuInfo> = map_test.mapfunc45();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc46(): Map<string, Object>
function testMapfunc46() {
    let result: Map<string, Object> = map_test.mapfunc46();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc47(): Map<string, Map<string, boolean>>
function testMapfunc47() {
    let result: Map<string, Map<string, boolean>> = map_test.mapfunc47();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc48(): Map<string, Map<string, int>>
function testMapfunc48() {
    let result: Map<string, Map<string, int>> = map_test.mapfunc48();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc49(): Map<string, Map<string, (int[])>>
function testMapfunc49() {
    let result: Map<string, Map<string, (int[])>> = map_test.mapfunc49();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc50(): Map<string, Map<string, string>>
function testMapfunc50() {
    let result: Map<string, Map<string, string>> = map_test.mapfunc50();
    arktest.assertEQ(result.has("key1"), true);
}

// function mapfunc51(op: (Map<string, string> | undefined)): Map<string,
// Map<string, string>>
function testMapfunc51() {
    let testData: Map<string, string> = {"key1": "value1", "key2": "value2"};
    let result: Map<string, Map<string, string>> = map_test.mapfunc51(testData);
    arktest.assertEQ(result.has("key1"), true);
}
