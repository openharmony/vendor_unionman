/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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

#ifndef __SERVER_PROCESS_H__
#define __SERVER_PROCESS_H__

/*
Init函数：用于创建socket环境，并预设置一些属性
*/
int Init(void);

/*
Open函数：用于服务打开的接口
*/
int Open(void);

/*
MainLoop函数：每隔100ms调用一次
*/
int MainLoop(void);

/*
Close函数，用于关闭服务并释放资源
*/
int Close(void);

#endif // __SERVER_PROCESS_H__