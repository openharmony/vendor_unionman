/**
 * @author: Mr.Hong
 */
import CommonConstants from '../../constants/CommonConstants'
import Rdb from '../../database/Rdb'
import AccountData from '../../../viewmodel/AccountData';
import relationalStore from '@ohos.data.relationalStore';

//存储账号信息表
export default class AccountTable {
  //通过Rdb类实例化AccountTable对象
  private accountTable = new Rdb(
    CommonConstants.ACCOUNT_TABLE.tableName,
    CommonConstants.ACCOUNT_TABLE.sqlCreate,
    CommonConstants.ACCOUNT_TABLE.columns);

  //构造函数，执行getRS
  constructor(callback: Function = () => {
  }) {
    this.accountTable.getRdbStore(callback)
  }

  //通过accountTable执行RS中的getRS，获取RS对象
  getRdbStore(callback: Function = () => {
  }) {
    this.accountTable.getRdbStore(callback)
  }

  //插入数据
  insertData(user: AccountData, callback: Function) {
    //通过GB转化为键值对存储
    const valueBucket: relationalStore.ValuesBucket = generateBucket(user)
    //执行AT插入方法
    this.accountTable.insertData(valueBucket, callback)
  }

  //删除数据
  deleteData(user: AccountData, callback: Function) {
    //初始化对象
    let predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName);
    //匹配数据库的account
    predicates.equalTo('account', user.account)
    //执行AT删除方法
    this.accountTable.deleteData(predicates, callback)
  }

  //更新数据
  updateData(user: AccountData, callback: Function) {
    //通过GB转化为键值对存储
    const valueBucket: relationalStore.ValuesBucket = generateBucket(user)
    //初始化对象
    let predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName)
    //匹配account
    predicates.equalTo('account', user.account)
    //执行AT更新方法
    this.accountTable.updateData(predicates, valueBucket, callback)
  }

  //查询全部user（在WEILIAO项目的注册方面不需要使用该方法，作为参考用于其他功能）
  query(user: AccountData, callback: Function) {
    //初始化对象
    let predicates = new relationalStore.RdbPredicates(CommonConstants.ACCOUNT_TABLE.tableName)
    //执行查询方法
    this.accountTable.query(predicates, (resultSet: relationalStore.ResultSet) => {
      //创建变量，存储结果行数
      let count: number = resultSet.rowCount
      //如果结果为0，执行回调，传入null array
      if (count === 0 || typeof count === 'string') {
        console.log(`${CommonConstants.TABLE_TAG}` + 'Query no results!')
        callback([])
      } else {
        //如果有结果，则从头开始
        resultSet.goToFirstRow();
        //创建变量存储是否存在该账号
        let result: AccountData = {
          id: 0, account: '', password: ''
        }
        //使用循环对结果赋值
        for (let i = 0; i < count; i++) {
          let tmp: AccountData = {
            id: 0, account: '', password: ''
          };
          tmp.id = resultSet.getDouble(resultSet.getColumnIndex('id'))
          tmp.account = resultSet.getString(resultSet.getColumnIndex('account'))
          tmp.password = resultSet.getString(resultSet.getColumnIndex('password'))
          if (tmp.account == user.account && tmp.password == user.password) {
            result = tmp
            break
          }
          //换行
          resultSet.goToNextRow()
        }
        //执行回调，传入结果
        callback(result)
      }
    })
  }
}

function generateBucket(user: AccountData): relationalStore.ValuesBucket {
  let obj: relationalStore.ValuesBucket = {};
  obj.account = user.account
  obj.password = user.password
  return obj;
}