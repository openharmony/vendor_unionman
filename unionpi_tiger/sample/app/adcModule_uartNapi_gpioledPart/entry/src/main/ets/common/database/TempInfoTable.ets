/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import data_rdb from '@ohos.data.rdb'
import hilog from '@ohos.hilog';
import featureAbility from '@ohos.ability.featureAbility';


const Tag='tempInfo'

export class TempInfo{
  year:number;
  month:number;
  day:number;
  hour:number;
  min:number;
  sec:number;
  temperature:number;
  constructor(year:number,month:number,day:number,hour:number,min:number,sec:number, temperature: number) {
    this.year = year;
    this.month = month;
    this.day = day;
    this.hour = hour;
    this.min = min;
    this.sec = sec;
    this.temperature = temperature;
  }
}

export class TempInfoTable{
  rabStore:data_rdb.RdbStore
  constructor() {
    this.rabStore = null;
  }
  getRdbStore(callback){
    //RdbStore存在
    if(this.rabStore)
    {
      console.log("数据库已存在")
      callback();
      return;
    }
    const STORE_CONFIG = { name: "rdbStore.db" }
    let context = featureAbility.getContext()
    const CREATE_TABLE_TempInfo = "CREATE TABLE IF NOT EXISTS myTempInfotable (" + "id INTEGER PRIMARY KEY AUTOINCREMENT, " + "year INTEGER NOT NULL, " + "month INTEGER NOT NULL, "+"day INTEGER NOT NULL, "+"hour INTEGER NOT NULL, "+"min INTEGER NOT NULL, "+"sec INTEGER NOT NULL, "+"temperature REAL)";
    data_rdb.getRdbStore(context,STORE_CONFIG,1, (err, rdbStore)=> {
      this.rabStore =rdbStore;
      rdbStore.executeSql(CREATE_TABLE_TempInfo )
      console.info('create table done.')
    })
  }

  generateBucket(tempinfo:TempInfo)
  {
    const {year,month,day,hour,min,sec,temperature}=tempinfo;
    return{
      year,
      month,
      day,
      hour,
      min,
      sec,
      temperature,
    };
  }

  public insertdata(year:number,month:number,day:number,hour:number,min:number,sec:number,temperatrue:number,callback) {
    try {
      this.getRdbStore(() => {
        let tempinfo=new TempInfo (year,month,day,hour,min,sec,temperatrue);
        let valueBucket = this.generateBucket(tempinfo);
        console.log("valueBucket为"+valueBucket);
        this.rabStore.insert("myTempInfotable", valueBucket);
        callback(true);
      })
    }catch (err){
      callback(false);
      console.log(err+"ming");
    }
  }
  public queryAll(callback)
  {
    try {
      this.getRdbStore(() =>{
        let predicates = new data_rdb.RdbPredicates("myTempInfotable");
        const columns=["year","month","day","hour","min","sec","temperature"];
        this.rabStore.query(predicates,columns,(err,resultSet)=>{
          if(err){
            console.log("query failed, err:"+err);
            return;
          }
          console.log("我是resultSet"+resultSet);
          let result:Array<TempInfo>=[];
          let count=resultSet.rowCount;
          if(count==0){
            console.log("没有数据");
            callback(result);
          }
          else {
            console.log("读取成功");
            console.log("count的值为"+count);
            resultSet.goToFirstRow();
            for(let i=0;i<count;i++){
              let tem=new TempInfo(-1,-1,-1,-1,-1,-1,-1);
              console.log("第几个数据"+ i);
              //tem.id=resultSet.getColumnIndex('id');
              tem.year=resultSet.getDouble(resultSet.getColumnIndex('year'));
              tem.month=resultSet.getDouble(resultSet.getColumnIndex('month'));
              tem.day=resultSet.getDouble(resultSet.getColumnIndex('day'));
              tem.hour=resultSet.getDouble(resultSet.getColumnIndex('hour'));
              tem.min=resultSet.getDouble(resultSet.getColumnIndex('min'));
              tem.sec=resultSet.getDouble(resultSet.getColumnIndex('sec'));
              tem.temperature=resultSet.getDouble(resultSet.getColumnIndex('temperature'));
              result[i]=tem;
              resultSet.goToNextRow();
            }
            console.log("我是result"+result);
            //关闭结果集
            resultSet.close();
            callback(result);
          }
        })

      })
    }catch (err){
        callback([]);
    }
  }
}

