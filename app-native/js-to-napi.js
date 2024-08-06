api = require('../bin/node_api')
process = require('process')

function benchmark(numStrs, lengthPerStr, numTrials) {
    const args = Array.from({
        length: numStrs
    }, () => new Array(lengthPerStr).join('x'));

    const start = process.hrtime();
    for (let i = 0; i < numTrials; i++) api.concat(args);
    const end = process.hrtime();
    return (end[0] - start[0]) * 1e6 + (end[1] - start[1]) / 1e3;
}

function main() {
    benchmark(1, 1, 1);
    for (let i = 0; i < 5000; i += 200) {
        const dur = Math.round(benchmark(1500, i, 3));
        console.log(`${i},${dur}`);
    }
}

main();
