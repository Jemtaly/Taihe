export interface ACrash  {
  get_list(): any;
}

export interface BCrash  {
  get_strList(): any;
}

export interface CCrash  {
  get_name(): String;
}

export function makeACrash(list: any): ACrash;

export function makeBCrash(list: any): BCrash;

export function makeCCrash(name: any): CCrash;

export function get_bCrashList(): any;

export function get_stringList(): any;