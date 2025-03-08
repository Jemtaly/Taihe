export interface CollectionParam1 {
  names(): any;
}

export interface CollectionParam3 {
  name(): any;
}

export interface CollectionParam2  {
  get_name(): String;
  name(): String;
}

export interface CollectionResult {
  get_name(): String;
  name(): String;
}

export interface KotlinClass {
  test1(a: any, b: any): any;
  test2(a: any, b: any): any;
  test3(a: any, b: any): any;
}

export interface CollectionParam1Impl extends CollectionParam1 {
}

export interface CollectionParam3Impl extends CollectionParam3 {
}

export function makeCollectionParam2(name: String): CollectionParam2;

export function makeCollectionResult(name: String): CollectionResult;

export function makeKotlinClass(): KotlinClass;

export function makeCollectionParam1Impl(namesList: any): CollectionParam1Impl;

export function makeCollectionParam3Impl(nameValue: String): CollectionParam3Impl;

export function get_globalCollectionParam1Array(): any;

export function get_globalCollectionParam1List(): any;

export function get_globalCollectionParam1Map(): any;

export function get_globalCollectionParam2Array(): any;

export function get_globalCollectionParam2List(): any;

export function get_globalCollectionParam2Map(): any;

export function printCollectionParam3(result: CollectionParam3): void;

export function printTest1Result(result: any): void;

export function printTest2Result(result: any): void;

export function printTest3Result(result: any): void;

export function as_CollectionParam1(a: any): CollectionParam1;

export function as_CollectionParam3(a: any): CollectionParam3;