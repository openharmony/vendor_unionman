<!--  Copyright (c) 2023 Unionman Technology Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.  -->

<template>
<el-container id="main">
  <el-header>
    <el-page-header content="系统升级管理系统"></el-page-header>
  </el-header>
  <el-main>
    <el-row>
      <el-col :span="12" offset=6>
        <el-form :model="form" ref="form" label-width="120px" size="small" label-position="left">
          <el-upload :limit="1"  :on-change="handleChange" class="upload-demo" drag action="none" multiple  :auto-upload="false" :file-list="form.fileList">
          <i class="el-icon-upload" requried></i>
          <div class="el-upload__text">将文件拖到此处，或<em>点击上传</em></div>
          </el-upload>
          <el-form-item label="版本数:" requried>
            <el-input v-model="form.versionCode"></el-input>
          </el-form-item>
          <el-form-item label="版本名称" requried>
            <el-input v-model="form.versionName"></el-input>
          </el-form-item>
          <el-form-item label="是否为系统升级">
            <el-switch v-model="form.isSystem" active-color="#13ce66" inactive-color="#ff4949"></el-switch>
          </el-form-item>
          <el-form-item label="更新信息">
            <el-input v-model="form.content" :disabled="!form.isSystem"></el-input>
          </el-form-item>
          <el-form-item>
            <el-button type="primary" @click="onSubmit">提交</el-button>
          </el-form-item>
        </el-form>
      </el-col>
    </el-row>
    
  </el-main>
  
</el-container>

</template>

<script>
import axios from 'axios'
import { Loading } from 'element-ui'
export default {
  name: 'UpLoad',
  data(){
    return{
       form:{
        fileList:[],
        versionCode:"",
        versionName:"",
        isSystem:true,
        content:""
      }
    }
  },
  methods:{
    onSubmit(){
      let mutipartFile = new FormData();
      console.log(this.form.fileList);
      mutipartFile.append("versionCode",this.form.versionCode);
      mutipartFile.append("versionName",this.form.versionName);
      mutipartFile.append("content",this.form.isSystem?this.form.content:"");
      mutipartFile.append("file",this.form.fileList[0].raw);
      let loading = Loading.service();
      loading.$nextTick(()=>{
        axios.post("/api/upload",mutipartFile,{
          Headers:{
            'Content-Type': 'multipart/form-data'
          }
        }).then(()=>{
          loading.close();
          this.$message({message: '上传成功！',type: 'success'});
        });
      })
    },
    handleChange(file){
      this.form.fileList[0]=file;
    }
  }
}
</script>
<style>
#info {
  background: #e0f0f0;
  border-radius: .5em;
  padding: 2em;
}
</style>