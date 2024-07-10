/**
 * @author: Mr.Hong
 */


//Rdb数据库

import relationalStore from '@ohos.data.relationalStore';
import CommonConstants from '../constants/CommonConstants';
import { Logger } from '../utils/Logger';

export default class Rdb {
  private rdbStore: relationalStore.RdbStore | null = null;
  private tableName: string;
  //数据库表名称
  private sqlCreateTable: string;
  //SQL表创建语句
  private columns: Array<string>;

  //列名字段数组

  //实例化Rdb对象时进行表名、创表、列名字段的创建
  constructor(tableName: string, sqlCreateTable: string, columns: Array<string>) {
    this.tableName = tableName;
    this.sqlCreateTable = sqlCreateTable;
    this.columns = columns;
  }

  //获取RS对象并接受回调函数
  getRdbStore(callback: Function = () => {
  }) {
    //如果回调函数返回空或undefined，打印错误日志并退出方法
    if (!callback || typeof callback == 'undefined' || callback == undefined) {
      Logger.verbose(`${CommonConstants.RDB_TAG}`, 'getRdbStore() has no callback!');
      return;
    }
    if (this.rdbStore != null) {
      Logger.verbose(`${CommonConstants.RDB_TAG}`, 'The rdbStore exists.');
      callback();
      return
    }
    //获取context
    let context: Context = getContext(this) as Context;
    //context:上下文
    //config:数据库配置
    //callback:回调
    relationalStore.getRdbStore(context, CommonConstants.STORE_CONFIG, (err, rdb) => {
      if (err) {
        Logger.error(`${CommonConstants.RDB_TAG}`, 'gerRdbStore() failed, err: ' + err);
        return;
      }
      //为RS属性赋值
      this.rdbStore = rdb;
      //执行SQL语句
      this.rdbStore.executeSql(this.sqlCreateTable);
      //打印日志
      Logger.verbose(`${CommonConstants.RDB_TAG}`, 'getRdbStore() finished.');
      //执行回调
      callback();
    });
  }

  //插入数据
  insertData(data: relationalStore.ValuesBucket, callback: Function = () => {
  }) {
    if (!callback || typeof callback == 'undefined' || callback == undefined) {
      Logger.verbose(`${CommonConstants.RDB_TAG}`, 'insertData() has no callback!');
      return;
    }
    //创建FLAG
    let resFlag: boolean = false;
    //创建存储键值对变量
    const valueBucket: relationalStore.ValuesBucket = data;
    if (this.rdbStore) {
      //调用RS插入方法
      this.rdbStore.insert(this.tableName, valueBucket, (err, ret) => {
        if (err) {
          Logger.error(`${CommonConstants.RDB_TAG}`, 'insertData() failed, err: ' + err);
          callback(resFlag);
          return;
        }
        Logger.verbose(`${CommonConstants.RDB_TAG}`, 'insertData() finished: ' + ret);
        callback(ret);
      });
    }
  }

  //删除数据
  deleteData(predicates: relationalStore.RdbPredicates, callback: Function = () => {
  }) {
    if (!callback || typeof callback == 'undefined' || callback == undefined) {
      Logger.verbose(`${CommonConstants.RDB_TAG}`, 'deleteData() has no callback!');
      return;
    }
    let resFlag: boolean = false;
    if (this.rdbStore) {
      this.rdbStore.delete(predicates, (err, ret) => {
        if (err) {
          Logger.error(`${CommonConstants.RDB_TAG}`, 'deleteData() failed, err: ' + err);
          callback(resFlag);
          return;
        }
        Logger.verbose(`${CommonConstants.RDB_TAG}`, 'deleteData() finished: ' + ret);
        callback(!resFlag);
      });
    }
  }

  updateData(predicates: relationalStore.RdbPredicates, data: relationalStore.ValuesBucket, callback: Function = () => {
  }) {
    if (!callback || typeof callback == 'undefined' || callback == undefined) {
      Logger.verbose(`${CommonConstants.RDB_TAG}`, 'updateDate() has no callback!');
      return;
    }
    let resFlag: boolean = false;
    const valueBucket: relationalStore.ValuesBucket = data;
    if (this.rdbStore) {
      this.rdbStore.update(valueBucket, predicates, (err, ret) => {
        if (err) {
          Logger.error(`${CommonConstants.RDB_TAG}`, 'updateData() failed, err: ' + err);
          callback(resFlag);
          return;
        }
        Logger.verbose(`${CommonConstants.RDB_TAG}`, 'updateData() finished: ' + ret);
        callback(!resFlag);
      });
    }
  }

  query(predicates: relationalStore.RdbPredicates, callback: Function = () => {
  }) {
    if (!callback || typeof callback == 'undefined' || callback == undefined) {
      Logger.verbose(`${CommonConstants.RDB_TAG}`, 'query() has no callback!');
      return;
    }
    if (this.rdbStore) {
      this.rdbStore.query(predicates, this.columns, (err, resultSet) => {
        if (err) {
          Logger.error(`${CommonConstants.RDB_TAG}`, 'query() failed, err: ' + err);
          return;
        }
        Logger.verbose(`${CommonConstants.RDB_TAG}`, 'query() finished.');
        callback(resultSet);
        resultSet.close();
      });
    }
  }
}