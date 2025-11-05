// import * as lib_people from "people";                    // Use .d.ts
// import * as lib_building from "building";                // Use .d.ts
import * as lib_people from "../generated/proxy/people";           // Use .ts
import * as lib_building from "../generated/proxy/building";       // Use .ts

function main() {
    let g = lib_building.make_group();
    if ( g.member.age !== 20 && g.member.name !== "mary" && g.number !== 23) throw new Error(`Unexpected result`);
    console.log(g.member.age, g.member.name, g.number);
    let p = lib_people.make_student();
    if ( p.age !== 22 && p.name !== "mike") throw new Error(`Unexpected result`);
    console.log(p.age, p.name);
}

main();