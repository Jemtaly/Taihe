
// kn_all_params function declaration
export function kn_all_params(
    a: number,         // i32 in TypeScript is number
    b: bigint,         // i64 in TypeScript is bigint
    c: number,         // f64 in TypeScript is number
    d: boolean,        // bool in TypeScript is boolean
    e: string          // String in TypeScript is string
  ): void;
  
  // kn_boolean_params function declaration
  export function kn_boolean_params(a: boolean): void;
  
  // kn_boolean_result function declaration
  export function kn_boolean_result(): boolean;
  
  // kn_double_params function declaration
  export function kn_double_params(a: number): void;  // f64 in TypeScript is number
  
  // kn_double_result function declaration
  export function kn_double_result(): number;  // f64 in TypeScript is number
  
  // kn_empty_func function declaration
  export function kn_empty_func(): void;
  
  // kn_int_params function declaration
  export function kn_int_params(a: number): void;  // i32 in TypeScript is number
  
  // kn_int_result function declaration
  export function kn_int_result(): number;  // i32 in TypeScript is number
  
  // kn_long_params function declaration
  export function kn_long_params(a: bigint): void;  // i64 in TypeScript is bigint
  
  // kn_long_result function declaration
  export function kn_long_result(): bigint;  // i64 in TypeScript is bigint
  
  // kn_string_params function declaration
  export function kn_string_params(a: string): void;
  
  // kn_string_result function declaration
  export function kn_string_result(): string;
