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
const express = require('express');
const app = express();
const { Users } = require('./db');
 
app.use(express.urlencoded({ extended: true }));
app.use(express.json())
 
// 注册账号
app.post("/publish", async (req, res) => {
    try {
        const { account, pwd } = req.body;
        const results = await Users.find({ account });
        if(!results.length>0){
            await Users.create({
                account, pwd
            });
            res.send("success")
        }
    } catch (error) {
        res.send(error, "error")
    }
})
// 注销账号
app.post("/del", async (req, res) => {
    console.log(req.body.account)
    try {
        const { account } = req.body;
 
        // 使用 deleteOne 删除指定 account 的数据
        const result = await Users.deleteOne({ account });
 
        if (result.deletedCount === 1) {
            res.send("success");
        } else {
            res.send("未找到匹配的记录");
        }
    } catch (error) {
        res.send(error, "error");
    }
})
// 修改账号密码
app.post("/upd", async (req, res) => {
    try {
        const { account,pwd, newpwd } = req.body;
        //验证原密码是否正确
        const results = await Users.find({ account, pwd });
        // 使用 updateOne 更新指定 account 的数据记录的 pwd 字段
        if(results.length > 0) {
            const result = await Users.updateOne({ account }, { $set: { pwd: newpwd } });
            res.json({ message: "密码更新成功！", result });
        }
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});
 
// 账号登录
app.get("/find/:account/:pwd", async (req, res) => {
    try {
        const account = req.params.account;
        const pwd = req.params.pwd;
 
        // 使用 find 查询所有匹配指定 account 的数据记录
        const results = await Users.find({ account, pwd });
 
        if (results.length > 0) {
            // 如果找到匹配的记录，则返回所有匹配的记录
            res.json({ data: results, message: "登录成功！" });
        } else {
            res.status(404).json({ message: "未找到匹配的记录" });
        }
    } catch (error) {
        res.status(500).json({ message: "服务器内部错误" });
    }
});
 
 
app.listen(3000, () => {
    console.log('server running')
})