/* Copyright 2023 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
const mongoose = require('mongoose');
 
//连接mongodb数据库
//本地数据库
// mongoose.connect("mongodb://localhost:27017/users")
// 云数据库
mongoose.connect("url")
    .then(() => {
        console.log("数据库连接成功！")
    })
    .catch((err) => {
        console.log("数据库连接失败！", err)
    })
 
// 创建表
const usersSchema = new mongoose.Schema({
    account: {
        type: Number
    },
    pwd: {
        type: String
    },
})
 
const Users = mongoose.model("Users", usersSchema,'usersaccount');
module.exports = {
    Users
    
}