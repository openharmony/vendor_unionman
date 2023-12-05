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
#ifndef _VNN_GLOBAL_H_
#define _VNN_GLOBAL_H_

typedef struct {
    uint32_t graph_input_idx;
    vsi_nn_preprocess_base_t *preprocesses;
    uint32_t preprocess_count;
} vsi_nn_preprocess_map_element_t;

typedef struct {
    uint32_t graph_output_idx;
    vsi_nn_postprocess_base_t *postprocesses;
    uint32_t postprocess_count;
} vsi_nn_postprocess_map_element_t;

/*
 * This file will be deprecated in the future
 */

#endif
