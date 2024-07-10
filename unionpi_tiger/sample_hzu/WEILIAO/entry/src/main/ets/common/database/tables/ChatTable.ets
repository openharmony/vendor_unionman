import relationalStore from '@ohos.data.relationalStore'
import ChatData from '../../../viewmodel/ChatData'
import CommonConstants from '../../constants/CommonConstants'
import Rdb from '../Rdb'


export default class ChatTable {
  //通过Rdb类实例化ChatTable对象
  private chatTable = new Rdb(
    CommonConstants.CHAT_TABLE.tableName,
    CommonConstants.CHAT_TABLE.sqlCreate,
    CommonConstants.CHAT_TABLE.columns)

  //构造函数，执行getRS
  constructor(callback: Function = () => {
  }) {
    this.chatTable.getRdbStore(callback)
  }

  //通过charTable执行RS中的getRS，获取RS对象
  getRdbStore(callback: Function = () => {
  }) {
    this.chatTable.getRdbStore(callback)
  }

  //插入数据
  insertData(user: ChatData, callback: Function) {
    //通过GB转化为键值对存储
    const valueBucket: relationalStore.ValuesBucket = generateBucket(user)
    //执行AT插入方法
    this.chatTable.insertData(valueBucket, callback)
  }

  //删除数据

  //查询全部user
  query(callback: Function) {
    //初始化对象
    let predicates = new relationalStore.RdbPredicates(CommonConstants.CHAT_TABLE.tableName)
    //执行查询方法
    this.chatTable.query(predicates, (resultSet: relationalStore.ResultSet) => {
      //创建变量，存储结果行数
      let count: number = resultSet.rowCount
      //如果结果为0，执行回调，传入null array
      if (count === 0 || typeof count === 'string') {
        console.log(`${CommonConstants.TABLE_TAG}` + 'Query no results!')
        callback([])
      } else {
        //如果有结果，则从头开始
        resultSet.goToFirstRow();
        //创建变量存储返回的数组
        const result: ChatData[] = []
        //使用循环对结果赋值
        for (let i = 0; i < count; i++) {
          let tmp: ChatData = {
            id: 0, who: '', chatData: '', isMe: 0
          }
          tmp.id = resultSet.getDouble(resultSet.getColumnIndex('id'))
          tmp.who = resultSet.getString(resultSet.getColumnIndex('who'))
          tmp.chatData = resultSet.getString(resultSet.getColumnIndex('chatData'))
          tmp.isMe = resultSet.getDouble(resultSet.getColumnIndex('isMe'))
          result[i] = tmp
          //换行
          resultSet.goToNextRow()
        }
        //执行回调，传入结果
        callback(result)
      }
    })
  }
}

function generateBucket(user: ChatData): relationalStore.ValuesBucket {
  let obj: relationalStore.ValuesBucket = {};
  obj.who = user.who
  obj.chatData = user.chatData
  obj.isMe = user.isMe
  return obj;
}