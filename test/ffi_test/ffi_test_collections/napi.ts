import { CollectionParam1, CollectionParam1Impl, CollectionParam2, CollectionParam3, CollectionParam3Impl,
    CollectionResult, KotlinClass, makeCollectionParam1Impl, makeCollectionParam2, makeCollectionParam3Impl,
    makeCollectionResult, makeKotlinClass, get_globalCollectionParam1Array, get_globalCollectionParam1List,
    get_globalCollectionParam1Map, get_globalCollectionParam2Array, get_globalCollectionParam2List,
    get_globalCollectionParam2Map, printCollectionParam3, printTest1Result, printTest2Result, printTest3Result, 
    as_CollectionParam1, as_CollectionParam3
} from './libtest'

function main() {
    let factory = makeKotlinClass()
    let param1 = get_globalCollectionParam1List()
    let param2 = get_globalCollectionParam2List()
    let param3 = get_globalCollectionParam1Array()
    let param4 = get_globalCollectionParam2Array()
    let param5 = get_globalCollectionParam1Map()
    let param6 = get_globalCollectionParam2Map()

    console.log("run test1: ", factory.test1(param1, param2))
    console.log("run test2: ", factory.test2(param3, param4))
    console.log("run test3: ", factory.test3(param5, param6))

    let result1 = factory.test1(param1, param2)
    let result2 = factory.test2(param3, param4)
    let result3 = factory.test3(param5, param6)

    console.log("result1: ", printTest1Result(result1))
    console.log("result2: ", printTest2Result(result2))
    console.log("result3: ", printTest3Result(result3))
}

main()