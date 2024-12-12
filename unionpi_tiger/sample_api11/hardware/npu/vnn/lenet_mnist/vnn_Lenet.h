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

#ifndef _VNN_Lenet_H
#define _VNN_Lenet_H

#ifdef __cplusplus

#include "vsi_nn_pub.h"

extern "C" {
#endif

#define VNN_APP_DEBUG (FALSE)
#define VNN_VERSION_MAJOR 1
#define VNN_VERSION_MINOR 1
#define VNN_VERSION_PATCH 34
#define VNN_RUNTIME_VERSION (VNN_VERSION_MAJOR * 10000 + VNN_VERSION_MINOR * 100 + VNN_VERSION_PATCH)

_version_assert(VNN_RUNTIME_VERSION <= VSI_NN_VERSION, CASE_VERSION_is_higher_than_OVXLIB_VERSION)

    void vnn_ReleaseLenet(vsi_nn_graph_t* graph, vsi_bool release_ctx);

vsi_nn_graph_t* vnn_CreateLenet(const char* data_file_name, vsi_nn_context_t in_ctx,
    const vsi_nn_preprocess_map_element_t* pre_process_map, uint32_t pre_process_map_count,
    const vsi_nn_postprocess_map_element_t* post_process_map, uint32_t post_process_map_count);

#ifdef __cplusplus
}
#endif

#endif
