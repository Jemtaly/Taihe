import testNapi from 'libentry.so';
import router from '@ohos.router';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  @State concat: string = 'benchmark arkts';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
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
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
