import promptAction from '@ohos.promptAction';
import Constant from '../../common/Constant'
import router from '@ohos.router';
import TextPro from './TextPro';
import TextPlus from './TextPlus';
import TextPro2 from './TextPro2';

// 获取路由传递的参数。这里用as进行类型转换
let record = router.getParams() as Record<string, string>

@Preview
@Component
export default struct person {
  @State username: string = record['username']
  @State msg: string = '';
  @State sex: string[] = ['男', '女'];
  private select: number = 0

  build() {
    Column() {
      // 个人信息头像
      Image($r('app.media.img_avatar'))
        .width(Constant.AVATAR_SIZE)
        .height(Constant.AVATAR_SIZE)
        .alignSelf(ItemAlign.Center)
      Text('个人信息')
        .fontSize(16)
        .margin({ top: 10 })
        .alignSelf(ItemAlign.Center)

      // 电子邮箱
      TextPro2({
        inputImage: $r('app.media.email'),
        email: this.username
      })

      // 昵称框
      TextPro({
        inputImage: $r('app.media.hintText'),
        hintText: '昵称'
      })

      // 性别框
      TextPlus({
        inputImage: $r('app.media.sex'),
        title: '性别',
        content: this.msg,
        onItemClick: () => {
          TextPickerDialog.show({
            range: this.sex,
            selected: this.select,
            onAccept: (value: TextPickerResult) => {
              this.msg = value.value
              this.select = value.index
            },
            onCancel: () => {
              console.info("取消")
            },
            onChange: () => {
              console.info("更改")
            }
          })
        }
      })

      // 个性签名框
      TextPro({
        inputImage: $r('app.media.signature'),
        hintText: '个性签名'
      })
    }
    .width('100%')
    .height('100%')
    .padding(20)
    .backgroundColor('#f5f5f5')
    .borderRadius(15)
  }
}
