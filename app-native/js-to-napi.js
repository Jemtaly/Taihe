api = require('../bin/node_api')
process = require('process')

function benchmark(numStrs, lengthPerStr, numTrials) {
    const args = Array.from({
        length: numStrs
    }, () => new Array(lengthPerStr).join('x'));

    const start = process.hrtime();
    for (let i = 0; i < numTrials; i++) api.concat(args);
    const end = process.hrtime();
    return (end[0] - start[0]) * 1e9 + (end[1] - start[1]);
}

function main() {
    for (let i = 0; i < 5000; i += 200) {
        const dur = benchmark(1500, i, 3);
        console.log(`${i},${dur}`);
    }
}

main();
