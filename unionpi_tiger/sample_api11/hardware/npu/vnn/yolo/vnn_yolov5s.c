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
/*-------------------------------------------
                   Includes
 -------------------------------------------*/
#include "vnn_yolov5s.h"

#include <stdio.h>
#include <stdlib.h>

#include "vnn_global.h"
#include "vsi_nn_pub.h"

/*-------------------------------------------
                   Macros
 -------------------------------------------*/

#define NEW_VXNODE(_node, _type, _in, _out, _uid)              \
    do {                                                       \
        _node = vsi_nn_AddNode(graph, _type, _in, _out, NULL); \
        if (NULL == (_node)) {                                 \
            goto error;                                        \
        }                                                      \
        (_node)->uid = (uint32_t)(_uid);                       \
    } while (0)

#define NEW_VIRTUAL_TENSOR(_id, _attr, _dtype)                                  \
    do {                                                                        \
        memset((_attr).size, 0, VSI_NN_MAX_DIM_NUM * sizeof(vsi_size_t));       \
        (_attr).dim_num = VSI_NN_DIM_AUTO;                                      \
        (_attr).vtl = !VNN_APP_DEBUG;                                           \
        (_attr).is_const = FALSE;                                               \
        (_attr).dtype.vx_type = (_dtype);                                       \
        (_id) = vsi_nn_AddTensor(graph, VSI_NN_TENSOR_ID_AUTO, &(_attr), NULL); \
        if (VSI_NN_TENSOR_ID_NA == (_id)) {                                     \
            goto error;                                                         \
        }                                                                       \
    } while (0)

// Set const tensor dims out of this macro.
#define NEW_CONST_TENSOR(_id, _attr, _dtype, _ofst, _size)                      \
    do {                                                                        \
        data = load_data(fp, (_ofst), (_size));                                 \
        (_attr).vtl = FALSE;                                                    \
        (_attr).is_const = TRUE;                                                \
        (_attr).dtype.vx_type = (_dtype);                                       \
        (_id) = vsi_nn_AddTensor(graph, VSI_NN_TENSOR_ID_AUTO, &(_attr), data); \
        free(data);                                                             \
        if (VSI_NN_TENSOR_ID_NA == (_id)) {                                     \
            goto error;                                                         \
        }                                                                       \
    } while (0)

// Set generic tensor dims out of this macro.
#define NEW_NORM_TENSOR(_id, _attr, _dtype)                                     \
    do {                                                                        \
        (_attr).vtl = FALSE;                                                    \
        (_attr).is_const = FALSE;                                               \
        (_attr).dtype.vx_type = (_dtype);                                       \
        (_id) = vsi_nn_AddTensor(graph, VSI_NN_TENSOR_ID_AUTO, &(_attr), NULL); \
        if (VSI_NN_TENSOR_ID_NA == (_id)) {                                     \
            goto error;                                                         \
        }                                                                       \
    } while (0)

// Set generic tensor dims out of this macro.
#define NEW_NORM_TENSOR_FROM_HANDLE(_id, _attr, _dtype)                                   \
    do {                                                                                  \
        (_attr).vtl = FALSE;                                                              \
        (_attr).is_const = FALSE;                                                         \
        (_attr).dtype.vx_type = (_dtype);                                                 \
        (_id) = vsi_nn_AddTensorFromHandle(graph, VSI_NN_TENSOR_ID_AUTO, &(_attr), NULL); \
        if (VSI_NN_TENSOR_ID_NA == (_id)) {                                               \
            goto error;                                                                   \
        }                                                                                 \
    } while (0)

#define NET_NODE_NUM (1)
#define NET_NORM_TENSOR_NUM (4)
#define NET_CONST_TENSOR_NUM (0)
#define NET_VIRTUAL_TENSOR_NUM (3)
#define NET_TOTAL_TENSOR_NUM (NET_NORM_TENSOR_NUM + NET_CONST_TENSOR_NUM + NET_VIRTUAL_TENSOR_NUM)

/*-------------------------------------------
               Local Variables
 -------------------------------------------*/

/*-------------------------------------------
                  Functions
 -------------------------------------------*/
static uint8_t* load_data(FILE* fp, size_t ofst, size_t sz)
{
    uint8_t* data;
    int32_t ret;
    data = NULL;
    if (NULL == fp) {
        return NULL;
    }

    ret = fseek(fp, ofst, SEEK_SET);
    if (ret != 0) {
        VSILOGE("blob seek failure.");
        return NULL;
    }

    if (sz <= 0) {
        // error
        return NULL;
    }
    data = (uint8_t*)malloc(sz);
    if (data == NULL) {
        VSILOGE("buffer malloc failure.");
        return NULL;
    }
    ret = fread(data, 1, sz, fp);
    return data;
} /* load_data() */

vsi_nn_graph_t* vnn_CreateYolov5s(const char* data_file_name, vsi_nn_context_t in_ctx,
    const vsi_nn_preprocess_map_element_t* pre_process_map, uint32_t pre_process_map_count,
    const vsi_nn_postprocess_map_element_t* post_process_map, uint32_t post_process_map_count)
{
    uint32_t _infinity = VSI_NN_FLOAT32_INF;
    vsi_status status;
    vsi_bool release_ctx;
    vsi_nn_context_t ctx;
    vsi_nn_graph_t* graph;
    vsi_nn_node_t* node[NET_NODE_NUM];
    vsi_nn_tensor_id_t norm_tensor[NET_NORM_TENSOR_NUM];

    vsi_nn_tensor_attr_t attr;
    FILE* fp;
    uint8_t* data;
    uint32_t i = 0L;
    char* use_img_process_s;
    int32_t enable_pre_post_process = 0L;
    vsi_bool sort = FALSE;
    vsi_bool inference_with_nbg = FALSE;
    char* pos = NULL;

    (void)(_infinity);
    ctx = NULL;
    graph = NULL;
    status = VSI_FAILURE;
    memset(&attr, 0, sizeof(attr));
    memset(&node, 0, sizeof(vsi_nn_node_t*) * NET_NODE_NUM);

    fp = fopen(data_file_name, "rb");
    if (NULL == fp) {
        VSILOGE("Open file %s failed.", data_file_name);
        goto error;
    }

    pos = strstr((char*)data_file_name, ".nb");
    if (pos && strcmp(pos, ".nb") == 0) {
        inference_with_nbg = TRUE;
    }

    if (NULL == in_ctx) {
        ctx = vsi_nn_CreateContext();
    } else {
        ctx = in_ctx;
    }

    use_img_process_s = getenv("VSI_USE_IMAGE_PROCESS");
    if (use_img_process_s) {
        enable_pre_post_process = atoi(use_img_process_s);
    }

    graph = vsi_nn_CreateGraph(ctx, NET_TOTAL_TENSOR_NUM, NET_NODE_NUM);
    if (NULL == graph) {
        VSILOGE("Create graph fail.");
        goto error;
    }
    vsi_nn_SetGraphVersion(graph, VNN_VERSION_MAJOR, VNN_VERSION_MINOR, VNN_VERSION_PATCH);
    vsi_nn_SetGraphInputs(graph, NULL, 1L);
    vsi_nn_SetGraphOutputs(graph, NULL, 3L);

    /*-----------------------------------------
      Register client ops
     -----------------------------------------*/

    /*-----------------------------------------
      Node definitions
     -----------------------------------------*/
    if (!inference_with_nbg) {
        /*-----------------------------------------
          lid       - nbg_0
          var       - node[0]
          name      - nbg
          operation - nbg
          input     - [640, 640, 3, 1]
          output    - [80, 80, 255, 1]
                      [40, 40, 255, 1]
                      [20, 20, 255, 1]
        -----------------------------------------*/
        NEW_VXNODE(node[0], VSI_NN_OP_NBG, 1, 3, 0);
        node[0]->nn_param.nbg.type = VSI_NN_NBG_FILE;
        node[0]->nn_param.nbg.url = data_file_name;

    } else {
        NEW_VXNODE(node[0], VSI_NN_OP_NBG, 1, 3, 0);
        node[0]->nn_param.nbg.type = VSI_NN_NBG_FILE;
        node[0]->nn_param.nbg.url = data_file_name;
    }

    /*-----------------------------------------
      Tensor initialize
     -----------------------------------------*/
    attr.dtype.fmt = VSI_NN_DIM_FMT_NCHW;
    /* @attach_Sigmoid_Sigmoid_146/out0_0:out0 */
    attr.size[0L] = 80L;
    attr.size[1L] = 80L;
    attr.size[2L] = 255L;
    attr.size[3L] = 1L;
    attr.dim_num = 4L;
    attr.dtype.scale = 0.0039130267687141895f;
    attr.dtype.zero_point = 0L;
    attr.dtype.qnt_type = VSI_NN_QNT_TYPE_AFFINE_ASYMMETRIC;
    NEW_NORM_TENSOR(norm_tensor[0], attr, VSI_NN_TYPE_UINT8);

    /* @attach_Sigmoid_Sigmoid_148/out0_1:out0 */
    attr.size[0L] = 40L;
    attr.size[1L] = 40L;
    attr.size[2L] = 255L;
    attr.size[3L] = 1L;
    attr.dim_num = 4L;
    attr.dtype.scale = 0.003908432554453611f;
    attr.dtype.zero_point = 0L;
    attr.dtype.qnt_type = VSI_NN_QNT_TYPE_AFFINE_ASYMMETRIC;
    NEW_NORM_TENSOR(norm_tensor[1], attr, VSI_NN_TYPE_UINT8);

    /* @attach_Sigmoid_Sigmoid_150/out0_2:out0 */
    attr.size[0L] = 20L;
    attr.size[1L] = 20L;
    attr.size[2L] = 255L;
    attr.size[3L] = 1L;
    attr.dim_num = 4L;
    attr.dtype.scale = 0.003916851710528135f;
    attr.dtype.zero_point = 0L;
    attr.dtype.qnt_type = VSI_NN_QNT_TYPE_AFFINE_ASYMMETRIC;
    NEW_NORM_TENSOR(norm_tensor[2], attr, VSI_NN_TYPE_UINT8);

    /* @images_152:out0 */
    attr.size[0L] = 640L;
    attr.size[1L] = 640L;
    attr.size[2L] = 3L;
    attr.size[3L] = 1L;
    attr.dim_num = 4L;
    attr.dtype.scale = 0.00390625f;
    attr.dtype.zero_point = 0L;
    attr.dtype.qnt_type = VSI_NN_QNT_TYPE_AFFINE_ASYMMETRIC;
    NEW_NORM_TENSOR(norm_tensor[3], attr, VSI_NN_TYPE_UINT8);

    if (!inference_with_nbg) {
        /*-----------------------------------------
          Connection initialize
         -----------------------------------------*/
        node[0L]->input.tensors[0L] = norm_tensor[3L];
        node[0L]->output.tensors[0L] = norm_tensor[0L];
        node[0L]->output.tensors[1L] = norm_tensor[1L];
        node[0L]->output.tensors[2L] = norm_tensor[2L];
        /* nbg_0L */
    } else {
        node[0L]->output.tensors[0L] = norm_tensor[0L];
        node[0L]->output.tensors[1L] = norm_tensor[1L];
        node[0L]->output.tensors[2L] = norm_tensor[2L];
        node[0L]->input.tensors[0L] = norm_tensor[3L];
    }
    graph->output.tensors[0L] = norm_tensor[0L];
    graph->output.tensors[1L] = norm_tensor[1L];
    graph->output.tensors[2L] = norm_tensor[2L];
    graph->input.tensors[0L] = norm_tensor[3L];

    if (enable_pre_post_process) {
        sort = TRUE;
        if (pre_process_map_count > 0) {
            for (i = 0; i < pre_process_map_count; i++) {
                status = vsi_nn_AddGraphPreProcess(graph, pre_process_map[i].graph_input_idx,
                    pre_process_map[i].preprocesses, pre_process_map[i].preprocess_count);
                TEST_CHECK_STATUS(status, error);
            }
        }

        if (post_process_map_count > 0) {
            for (i = 0; i < post_process_map_count; i++) {
                status = vsi_nn_AddGraphPostProcess(graph, post_process_map[i].graph_output_idx,
                    post_process_map[i].postprocesses, post_process_map[i].postprocess_count);
                TEST_CHECK_STATUS(status, error);
            }
        }
    }

    status = vsi_nn_SetupGraph(graph, sort);
    TEST_CHECK_STATUS(status, error);

    if (VSI_FAILURE == status) {
        goto error;
    }

    fclose(fp);

    return graph;

error:
    if (NULL != fp) {
        fclose(fp);
    }

    release_ctx = (NULL == in_ctx);
    vsi_nn_DumpGraphToJson(graph);
    vnn_ReleaseYolov5s(graph, release_ctx);

    return NULL;
} /* vsi_nn_CreateYolov5s() */

void vnn_ReleaseYolov5s(vsi_nn_graph_t* graph, vsi_bool release_ctx)
{
    vsi_nn_context_t ctx;
    if (NULL != graph) {
        ctx = graph->ctx;
        vsi_nn_ReleaseGraph(&graph);

        /*-----------------------------------------
        Unregister client ops
        -----------------------------------------*/

        if (release_ctx) {
            vsi_nn_ReleaseContext(&ctx);
        }
    }
} /* vsi_nn_ReleaseYolov5s() */
