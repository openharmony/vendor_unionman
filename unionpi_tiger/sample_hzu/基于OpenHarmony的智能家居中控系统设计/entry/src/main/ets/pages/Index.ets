import router from '@ohos.router';

@Entry
@Component
struct Index {
  build() {
    Stack() {
      // 背景图片
      Image($r("app.media.bj1"));

      Row() {
        Text('智能家居系统')
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .margin({top:'-30%'});
      }

      Column() {
        Row() {
          Button() {
            Image($r('app.media.z3'));
          }
          .width('30%')
          .height('66%')
          .margin({ top: 80 })
          .backgroundColor("rgba(0, 0, 0, 0)")
          .type(ButtonType.Capsule)
          .onClick(() => {
            router.pushUrl({ url: 'pages/Madaish' });
          });

          Blank();

          Button() {
            Image($r('app.media.2'));
          }
          .width('30%')
          .height('66%')
          .margin({ top: 80 })
          .backgroundColor("rgba(0, 0, 0, 0)")
          .type(ButtonType.Capsule)
          .onClick(() => {
            router.pushUrl({ url: 'pages/Humiture' });
          });

          Blank();

          Button() {
            Image($r('app.media.3'));
          }
          .width('30%')
          .height('66%')
          .margin({ top: 80 })
          .backgroundColor("rgba(0, 0, 0, 0)")
          .type(ButtonType.Capsule)
          .onClick(() => {
            router.pushUrl({ url: 'pages/Servo' });
          });
        }
        .width('90%')
        .height('100%');
      }
    }
    .width('100%')
    .height('100%');
  }
}