import testNapi from 'libentry.so';
import router from '@ohos.router';
import systemDateTime from '@ohos.systemDateTime';

@Entry
@Component
struct Index {
  @State message: string = 'benchmark flutter\n';
  @State concat: string = 'benchmark arkts';

  benchmark(numStrs: number, lengthPerStr: number, numTrials: number): number {
    let args: string[] = [];
    for (let i = 0; i < numStrs; i++) {
      args.push('x'.repeat(lengthPerStr));
    }

    const start = systemDateTime.getTime(true);
    for (let i = 0; i < numTrials; i++) {
      testNapi.concat(args);
    }
    const end = systemDateTime.getTime(true);
    return (end - start) / 1000 / numTrials;
  }

  runOnce(numStrs: number, lengthPerStr: number, numTrials: number) {
    const dur = Math.round(this.benchmark(numStrs, lengthPerStr, numTrials));
    const s = `tbench:arkts-napi,${numStrs},${lengthPerStr},${dur}`;
    this.message += `${s}\n`;
    console.log(`${s}`);
  }

  dobench() {
    this.runOnce(1, 1, 1);
    for (let i = 0; i < 5000; i += 200) {
      this.runOnce(1500, i, 3);
    }
  }

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(10)
          .onClick(() => {
            router.pushUrl({ url: 'pages/IndexFlutter' }, (err) => {
              if (err) {
                this.message += `replaceUrl failed, code is ${err.code}, message is ${err.message}`;
                return;
              }
            })
          })
        Button(this.concat)
          .onClick(() => {
            this.concat = testNapi.concat(['xy', 'z']);
            this.dobench();
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
