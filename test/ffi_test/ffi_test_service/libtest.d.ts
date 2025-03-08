export interface IKmpTestArktsService {
    funFromArkts(v: number): string;
    funFromArkts2(v: number): number;
}

export interface IKmpTestKotlinService  {
    funFromKotlin(): String;
}

export interface KmpTestKotlinService extends IKmpTestKotlinService {

}

export interface KmpTestArktsService extends IKmpTestArktsService {

}

export function makeKmpTestKotlinService(): KmpTestKotlinService;

export function makeKmpTestArktsService(): KmpTestArktsService;

export function as_IKmpTestKotlinService(a: any): IKmpTestKotlinService;

export function as_IKmpTestArktsService(a: any): IKmpTestArktsService;
