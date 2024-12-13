/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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
#ifndef _VNN_POST_PROCESS_H_
#define _VNN_POST_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

vsi_status vnn_PostProcessLenet(vsi_nn_graph_t* graph);

const vsi_nn_postprocess_map_element_t* vnn_GetPostProcessMap();

uint32_t vnn_GetPostProcessMapCount();
void yolo_process(void);

#ifdef __cplusplus
}
#endif

#endif
