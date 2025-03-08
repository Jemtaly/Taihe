export interface KNObject {
    kn_all_params(
        a: number, 
        b: number, 
        c: number, 
        d: boolean, 
        e: string
    ): void;
    
    kn_boolean_params(a: boolean): void;
    kn_boolean_result(): boolean;
    
    kn_double_params(a: number): void;
    kn_double_result(): number;
    
    kn_empty_func(): void;
    
    kn_int_params(a: number): void;
    kn_int_result(): number;
    
    kn_long_params(a: bigint): void;
    kn_long_result(): bigint;
    
    kn_string_params(a: string): void;
    kn_string_result(): string;
}

export function getKNObject(): KNObject;
export function makeKNObject(): KNObject;
