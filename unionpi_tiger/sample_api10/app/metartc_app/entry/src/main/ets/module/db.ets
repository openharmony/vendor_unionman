import dataPreferences from '@ohos.data.preferences';
import promptAction from '@ohos.promptAction';

let context = getContext(this);
let preference: dataPreferences.Preferences | null = null;

// 自定义用户首选项类
export class PreferenceModel {

  // 创建用户首选项实例preference
  async getPreferencesFromStorage() {
    try {
      preference = await dataPreferences.getPreferences(context, 'setting.db');
    } catch (err) {
      console.error('[PreferenceModel]', `Failed to get preferences, Cause: ${err}`);
    }
  }

  // 删除数据，调用dataPreferences的deletePreferences接口
  async deletePreferences() {
    try {
      await dataPreferences.deletePreferences(context, 'setting.db');
    } catch(err) {
      console.error('[PreferenceModel]', `Failed to delete preferences, Cause: ${err}`);
    };
    preference = null;
    this.showToastMessage($r('app.string.delete_success_msg'));
  }

  // 保存数据
  async putPreference(key:string, value:string) {
    if (preference === null) {
      await this.getPreferencesFromStorage();
    }
    if (preference === null) {
      return
    }
    // 将用户输入的亮度数据保存到preference，调用用户首选项实例的put接口
    try {
      await preference.put(key, value);
    } catch (err) {
      console.error('[PreferenceModel]', `Failed to put value, Cause: ${err}`);
    }
    // 使用flush方法将preferences实例的数据存储到持久化文件，调用用户首选项实例的flush接口
    await preference.flush();
  }

  // 获取数据，调用用户首选项实例的get接口
  async getPreference(key:string, def:string) {
    let value = '';
    if (preference === null) {
      await this.getPreferencesFromStorage();
    }
    if (preference === null) {
      return def
    }
    try {
      value = await preference.get(key, def) as string;
    } catch (err) {
    }
    console.error('[PreferenceModel]', `get value, Cause: ${value}`);

    // 如果判断数据为空则提示用户先输入数据
    if (value === '') {
      this.showToastMessage($r('app.string.data_is_null_msg'));
      return;
    }
    // this.showToastMessage($r('app.string.read_success_msg'));
    return value;
  }

  // 点击保存按钮保存数据
  writeData(key: string, value: string) {
    // The data is inserted into the preferences database if it is not empty.
    this.putPreference(key, value);
    // this.showToastMessage($r('app.string.write_success_msg'));
  }

  // 消息弹框
  showToastMessage(message: Resource) {
    promptAction.showToast({
      message: message,
      duration: 3000
    });
  };
}