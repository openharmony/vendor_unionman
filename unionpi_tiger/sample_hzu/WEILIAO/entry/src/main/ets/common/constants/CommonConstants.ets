/**
 * @author: Mr.Hong
 */
/**
 * Common constants for all features.
 */

import relationalStore from '@ohos.data.relationalStore';
import { AccountTable } from '../../viewmodel/ConstantsInterface';
import { ChatTable } from '../../viewmodel/ConstantsInterface'

export default class CommonConstants {
  /**
   * The local port number bound when making a socket connection
   */
  public static readonly LOCAL_POST = 6666;
  /**
   * Title text of the mine page
   */
  public static readonly ERROR_MESSAGE = '请输入有效的房间信息';
  /**
   * Title text of the mine page
   */
  public static readonly LIKE_SUCCESS = '点赞成功';
  /**
   * Title text of the home page
   */
  public static readonly HOME_TITLE = '微聊';
  /**
   * Title text of the address_book page
   */
  public static readonly ADDRESS_BOOK_TITLE = '通讯录';
  /**
   * Title text of the Fine page
   */
  public static readonly FIND_TITLE = '发现';
  /**
   * Title text of the mine page
   */
  public static readonly MINE_TITLE = '我的';
  /**
   * The width or height of the component is spread across the parent component.
   */
  public static readonly FULL_PARENT = '100%';
  /**
   * The width or height of the component is spread across the parent component.
   */
  public static readonly TRIPLE_PARENT = '300%';
  /**
   * The width or height of the component is spread across the parent component.
   */
  public static readonly HALF_PARENT = '50%';
  /**
   * The width of textInput box
   */
  public static readonly TEXT_INPUT_BOX_WIDTH = '75%'
  /**
   * The width of send button
   */
  public static readonly SEND_BUTTON_WIDTH = '20%'
  /**
   * Home tab index
   */
  public static readonly HOME_TAB_INDEX = 0;
  /**
   * stroke_Width string number
   */
  public static readonly STROKE_WIDTH = 1
  /**
   * flexGrow string number
   */
  public static readonly FLEX_GROW = 1
  /**
   * Address_book tab index
   */
  public static readonly ADDRESS_BOOK_TAB_INDEX = 1;
  /**
   * Address_book tab index
   */
  public static readonly MINE_BUTTON_BORDER_WIDTH = 1;
  /**
   * Find tab index
   */
  public static readonly FIND_TAB_INDEX = 2
  /**
   * Mine tab index
   */
  public static readonly MINE_TAB_INDEX = 3;
  /**
   * rowCustomComponent_MARGIN_L of DISTANCE
   */
  public static readonly ROW_CUSTOM_COMPONENT_MARGIN_L = 10
  /**
   * MINE of WxListPage's list space
   */
  public static readonly WXLIST_PAGE_LIST_SPACE = 10
  /**
   * QRCode of Text
   */
  public static readonly QRCODE_TEXT = 'yingle'
  /**
   * MINE_ListItem_HEIGHT of value
   */
  public static readonly MINE_LIST_ITEM_HEIGHT = 180
  /**
   * Mark Unread of Name
   */
  public static readonly MARK_UNREAD = '标记未读'
  /**
   * Not Shown of Name
   */
  public static readonly NOT_SHOWN = '不显示'
  /**
   * Delete of Name
   */
  public static readonly DELETE = '删除'
  /**
   * Search of Name
   */
  public static readonly SEARCH = '搜索'
  /**
   * NewFriends of Name
   */
  public static readonly NEW_FRIENDS = '新的朋友'
  /**
   * Chat Only With friends of Name
   */
  public static readonly CHAT_ONLY_WITH_FRIENDS = '仅聊天的朋友'
  /**
   * Group Chats of Name
   */
  public static readonly GROUP_CHATS = '群聊'
  /**
   * Label of Name
   */
  public static readonly LABEL = '标签'
  /**
   * Official Account of Name
   */
  public static readonly OFFICIAL_ACCOUNT = '公众号'
  /**
   * server of name
   */
  public static readonly SERVER = '服务'
  /**
   * collect of name
   */
  public static readonly COLLECT = '收藏'
  /**
   * cardCase of name
   */
  public static readonly CARD_CASE = '卡包'
  /**
   * phiz of name
   */
  public static readonly PHIZ = '表情'
  /**
   * set of name
   */
  public static readonly SET = '设置'
  //------------------------------------------------------------------------------
  /**
   * Rdb database config.
   */
  public static readonly STORE_CONFIG: relationalStore.StoreConfig = {
    name: 'database.db',
    securityLevel: relationalStore.SecurityLevel.S1
  };
  /**
   * Account table config.
   */
  public static readonly ACCOUNT_TABLE: AccountTable = {
    tableName: 'accountTable',
    sqlCreate: 'CREATE TABLE IF NOT EXISTS accountTable(id INTEGER PRIMARY KEY AUTOINCREMENT,' +
      'ACCOUNT TEXT, PASSWORD TEXT)',
    columns: ['id', 'account', 'password']
  };
  //聊天内容表
  public static readonly CHAT_TABLE: ChatTable = {
    tableName: 'chatTable',
    sqlCreate: 'CREATE TABLE IF NOT EXISTS chatTable(id INTEGER PRIMARY KEY AUTOINCREMENT,' +
      'who TEXT, chatData TEXT, isMe INTEGER)',
    columns: ['id', 'who', 'chatData', 'isMe']
  };
  /**
   * Log tag.
   */
  public static readonly RDB_TAG = '[Debug.Rdb]';
  public static readonly TABLE_TAG = '[Debug.AccountTable]';
  public static readonly INDEX_TAG = '[Debug.Index]';
  /**
   * agreement_title in agreement.ets.
   */
  public static readonly AGREEMENT_TITLE = '用户需知'
  /**
   * agreement_massage in agreement.ets.
   */
  public static readonly AGREEMENT_MASSAGE = '       本文档旨在向用户介绍使用我们的聊天软件时需要注意的事项、规则和协议。我们致力于提供一个安全、私密且高效的通信平台，同时确保用户在使用过程中遵守法律法规和社区准则。' +
    '\n1. 软件概述' +
    '\n我们的聊天软件是一款多功能的即时通讯工具，提供文字、图片、视频和语音通话服务。用户可以通过本软件与亲朋好友保持联系，分享生活点滴，同时也适用于商务沟通和团队协作。' +
    '\n2. 用户注册与账户安全' +
    '\n账户注册：用户需提供真实有效的个人信息进行注册。' +
    '\n账户安全：用户有责任保管好自己的账户信息，不得将账户密码透露给他人。' +
    '\n账户验证：为保证通信安全，用户需通过邮箱验证。' +
    '\n3. 用户隐私保护' +
    '\n隐私政策：我们严格遵守隐私政策，不会无故泄露用户个人信息。' +
    '\n数据加密：所有传输的数据都经过加密处理，确保信息传输的安全性。' +
    '\n隐私设置：用户可自行设置隐私选项，如谁可以添加我为好友，谁可以看到我的动态等。' +
    '\n4. 用户行为准则' +
    '\n合法使用：用户应遵守法律法规，不得利用本软件进行违法活动。' +
    '\n尊重他人：用户应尊重其他用户，禁止发布侮辱、歧视、骚扰等不当言论。' +
    '\n版权尊重：用户应尊重知识产权，不得上传或分享侵权内容。' +
    '\n5. 信息发布规范' +
    '\n内容规范：用户发布的内容应健康、积极，不得包含色情、暴力、恐怖等违法违规信息。' +
    '\n广告与垃圾信息：禁止发布广告、垃圾信息或进行诈骗活动。' +
    '\n信息审核：我们将对用户发布的内容进行审核，如有违规，将采取相应措施。' +
    '\n6. 软件使用限制' +
    '\n使用限制：用户应合理使用软件功能，不得利用软件进行网络攻击或数据爬取。' +
    '\n功能滥用：禁止滥用软件功能，如频繁发送相同信息等。' +
    '\n7. 知识产权' +
    '\n版权归属：软件的所有知识产权，包括但不限于版权、商标、专利等，均归我们所有。' +
    '\n用户内容：用户在软件中创造的内容，我们享有合理使用权。' +
    '\n8. 服务可用性' +
    '\n服务维护：我们有权在必要时对软件进行维护或升级，可能会导致服务暂时中断。' +
    '\n服务变更：我们保留随时更新、改进或停用软件部分服务的权利。' +
    '\n9. 用户反馈与支持' +
    '\n反馈渠道：我们鼓励用户提供反馈，帮助我们改进产品和服务。' +
    '\n技术支持：用户在使用过程中遇到问题，可以通过我们的客服渠道获得帮助。' +
    '\n10. 免责声明' +
    '\n第三方内容：对于用户发布或分享的第三方内容，我们不承担任何责任。' +
    '\n服务风险：用户应意识到使用互联网服务存在一定风险，我们不承担因用户自身原因导致的任何损失。' +
    '\n11. 争议解决' +
    '\n法律适用：本协议的解释、适用及争议解决均服从中华人民共和国法律。' +
    '\n争议解决方式：如发生争议，双方应首先协商解决；协商不成时，可提交至有管辖权的人民法院通过诉讼方式解决。' +
    '\n12. 协议修改' +
    '\n协议更新：我们保留对本用户需知进行修改的权利。任何修改将通过软件更新或网站公告的形式通知用户。' +
    '\n继续使用：用户在协议修改后继续使用软件，即表示接受修改后的协议。' +
    '\n结语' +
    '\n我们致力于打造一个健康、积极的在线社区，希望所有用户都能遵守本需知，共同维护良好的网络环境。感谢您选择我们的聊天软件，期待为您提供更优质的服务。' +
    '\n\n\n\n\n\n\n\n\n\n\n';
}

