import {IKmpTestArktsService, IKmpTestKotlinService, KmpTestArktsService, KmpTestKotlinService, 
    makeKmpTestArktsService, makeKmpTestKotlinService, as_IKmpTestArktsService, as_IKmpTestKotlinService
} from './libtest'

function main() {
    let service1 = makeKmpTestArktsService()
    let service2 = makeKmpTestKotlinService()

    console.log("funFromArkts: ", as_IKmpTestArktsService(service1).funFromArkts(1))
    console.log("funFromArkts2: ", as_IKmpTestArktsService(service1).funFromArkts2(1.5))

    console.log("funFromKotlin: ", as_IKmpTestKotlinService(service2).funFromKotlin())
}

main()