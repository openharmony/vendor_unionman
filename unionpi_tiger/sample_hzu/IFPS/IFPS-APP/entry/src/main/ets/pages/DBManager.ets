import data_rdb from '@ohos.data.rdb'

const STORE_CONFIG = {name: "test.db"}
const TAB_LOG = "log"

const CREATE_TABLE_LOG_CODE = `
    CREATE TABLE IF NOT EXISTS ${TAB_LOG} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        date TEXT,
        level TEXT,
        info TEXT
    )
`
type logItem = {
  time: string;
  level: string;
  info :string;
}

export function createTable(context: Context) {
  data_rdb.getRdbStore(context, STORE_CONFIG, 1, function (err, rdbStore) {
    if(err){
      console.log("ERROR: Get RdbStore failed, err: " + err)
      return
    }
    rdbStore.executeSql(CREATE_TABLE_LOG_CODE, [], function(err) {
      if (err) {
        console.log("ERROR: ExecuteSql failed, err: " + err)
        return
      }
      console.log('SUCCESS: create table done.')
    })
  })
}

export function insertData(context: Context, item: logItem): Promise<void> {
  const promise = data_rdb.getRdbStore(context, STORE_CONFIG, 1)
  return promise.then(async (rdbStore) => {
    try{
      console.log(`insert : ${item.time}`)
      rdbStore.insert(TAB_LOG, item)
    }catch (err){
      console.log("ERROR: insert failed: " + err)
    }
  }).catch((err) => {
    console.log("ERROR: Get RdbStore failed, err: " + err)
  })
}

export function insertDataArr(context: Context, list: logItem[]): Promise<void> {
  const promise = data_rdb.getRdbStore(context, STORE_CONFIG, 1);
  return promise.then(async (rdbStore) => {
    for (let i: number = 0; i < list.length; i++) {
      const value = {
        date: list[i].time,
        level: list[i].level,
        info: list[i].info,
      };
      try {
        console.log(`insert ${i}: ${list[i].time}`);
        await rdbStore.insert(TAB_LOG, value);
      } catch (err) {
        console.log("ERROR: insert failed: " + err);
      }
    }
  }).catch((err) => {
    console.log("ERROR: Get RdbStore failed, err: " + err);
  });
}

export function queryData(context: Context, page: number, indexOffset: number, pageSize: number): Promise<logItem[] | false> {
  let promise = data_rdb.getRdbStore(context, STORE_CONFIG, 1)
  return promise.then(async (rdbStore) => {
    // 查询操作
    const offset: number = (page - 1) * pageSize + indexOffset;
    const querySqlString =  `SELECT * FROM ${TAB_LOG} ORDER BY date DESC LIMIT ${pageSize} OFFSET ${offset}`

    try {
      let resultSet = await rdbStore.querySql(querySqlString, [])
      if (resultSet.rowCount == 0) return false
      let list: logItem[] = []
      resultSet.goToFirstRow()
      for (let i = 0; i < resultSet.rowCount; i++) {
        const time = resultSet.getString(resultSet.getColumnIndex("date"))
        const level = resultSet.getString(resultSet.getColumnIndex("level"))
        const info = resultSet.getString(resultSet.getColumnIndex("info"))
        resultSet.goToNextRow()
        const data: logItem = { time, level, info }
        list.push(data)
        console.log(`query success ${i} : ${time}`)
      }
      resultSet.close()
      return list
    } catch (err) {
      console.log("ERROR: query failed, err: " + err)
      return false
    }
  }).catch((err) => {
    console.error("ERROR: Get RdbStore failed, err: " + err)
    return false
  })
}