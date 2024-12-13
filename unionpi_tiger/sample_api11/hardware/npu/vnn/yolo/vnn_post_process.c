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
#include "vnn_post_process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vnn_global.h"
#include "vsi_nn_pub.h"

#define _BASETSD_H

/*-------------------------------------------
                  Variable definitions
-------------------------------------------*/

/*{graph_output_idx, postprocess}*/
const static vsi_nn_postprocess_map_element_t* postprocess_map = NULL;

#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

struct box {
    float x;
    float y;
    float w;
    float h;
};

static string classes[80] = { "person", "bicycle", "car", "motorbike ", "aeroplane ", "bus ", "train", "truck ", "boat",
    "traffic light", "fire hydrant", "stop sign ", "parking meter", "bench", "bird", "cat", "dog ", "horse ", "sheep",
    "cow", "elephant", "bear", "zebra ", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    "tennis racket", "bottle", "wine glass", "cup", "fork", "knife ", "spoon", "bowl", "banana", "apple", "sandwich",
    "orange", "broccoli", "carrot", "hot dog", "pizza ", "donut", "cake", "chair", "sofa", "pottedplant", "bed",
    "diningtable", "toilet ", "tvmonitor", "laptop	", "mouse	", "remote ", "keyboard ", "cell phone", "microwave ",
    "oven ", "toaster", "sink", "refrigerator ", "book", "clock", "vase", "scissors ", "teddy bear ", "hair drier",
    "toothbrush " };

typedef struct Bbox {
    int x;
    int y;
    int w;
    int h;
    float score;
    int classes;
} Bbox;

static bool sort_score(Bbox box1, Bbox box2)
{
    return box1.score > box2.score ? true : false;
}

float iou(Bbox box1, Bbox box2)
{
    int x1 = max(box1.x, box2.x);
    int y1 = max(box1.y, box2.y);
    int x2 = min(box1.x + box1.w, box2.x + box2.w);
    int y2 = min(box1.y + box1.h, box2.y + box2.h);
    int w = max(0, x2 - x1);
    int h = max(0, y2 - y1);
    float over_area = w * h;
    return over_area / (box1.w * box1.h + box2.w * box2.h - over_area);
}

vector<Bbox> nms(std::vector<Bbox>& boxes, float threshold)
{
    vector<Bbox> resluts;
    std::sort(boxes.begin(), boxes.end(), sort_score);
    while (boxes.size() > 0) {
        resluts.push_back(boxes[0]);
        int index = 1;
        while (index < boxes.size()) {
            float iou_value = iou(boxes[0], boxes[index]);

            if (iou_value > threshold) {
                boxes.erase(boxes.begin() + index);

            } else {
                index++;
            }
        }
        boxes.erase(boxes.begin());
    }

    return resluts;
}

void flatten(float* x, int size, int layers, int batch, int forward)
{
    if (size <= 0 || layers <= 0 || batch <= 0) {
        // error
        return;
    }
    float* swap = (float*)calloc(size * layers * batch, sizeof(float));
    int i;
    int c;
    int b;
    for (b = 0; b < batch; ++b) {
        for (c = 0; c < layers; ++c) {
            for (i = 0; i < size; ++i) {
                int i1 = b * layers * size + c * size + i;
                int i2 = b * layers * size + i * layers + c;
                if (forward) {
                    swap[i2] = x[i1];
                } else {
                    swap[i1] = x[i2];
                }
            }
        }
    }
    memcpy(x, swap, size * layers * batch * sizeof(float));
    free(swap);
}

box get_region_box(float* x, float* biases, int n, int index, int i, int j, int w, int h, int stride)
{
    float xx = (x[index + 0L]);
    float xy = (x[index + 1L]);
    float xw = (x[index + 2L]);
    float xh = (x[index + 3L]);

    box b;

    b.x = (i + xx * 2L - 0.5f) * stride;
    b.y = (j + xy * 2L - 0.5f) * stride;
    b.w = (xw * 2L) * (xw * 2L) * biases[2L * n];
    b.h = (xh * 2L) * (xh * 2L) * biases[2L * n + 1L];

    return b;
}

int yolo_v5_post_process_onescale(
    float* predictions, float* biases, float threshold_in, int stride, vector<Bbox>& input)
{
    int i, j;
    int num_class = 80L;
    int coords = 4L;
    int bb_size = coords + num_class + 1L;

    float threshold = threshold_in;

    int nn_width = 640L;
    int nn_height = 640L;

    int num_box = 3L;

    if (stride == 0) {
        return NULL;
    }
    int modelWidth = nn_width / stride;
    int modelHeight = nn_height / stride;

    for (i = 0; i < modelWidth * modelHeight; ++i) {
        int row = i / modelWidth;
        int col = i % modelWidth;
        int n = 0;
        for (n = 0; n < num_box; ++n) {
            int index = i * num_box + n;
            int p_index = index * bb_size + 4;
            float scale = predictions[p_index];
            int box_index = index * bb_size;
            int class_index = 0;
            class_index = index * bb_size + 5;

            if (scale > threshold) {
                int max_class = 0;
                float probs[num_class];
                for (j = 0; j < num_class; ++j) {
                    float prob = scale * predictions[class_index + j];
                    probs[j] = prob;
                    if (probs[j] > probs[max_class]) {
                        max_class = j;
                    }
                }

                if (probs[max_class] > threshold) {
                    box getbox =
                        get_region_box(predictions, biases, n, box_index, col, row, modelWidth, modelHeight, stride);

                    float l = getbox.x - getbox.w / 2.f;
                    float t = getbox.y - getbox.h / 2.f;
                    float r = getbox.x + getbox.w / 2.f;
                    float d = getbox.y + getbox.h / 2.f;

                    input.push_back({ l, t, r, d, probs[max_class], max_class });
                }
            }
        }
    }

    return 0;
}

void file2buffer(char* file, float* buffer)
{
    FILE* fp = NULL;
    char buff[255L];
    int i = 0;
    fp = fopen(file, "r");
    while (fgets(buff, 255L, (FILE*)fp)) {
        buffer[i] = atof(buff);
        i++;
    }
    fclose(fp);
}

void file_process(char* file, float* biases, float threshold, int stride, vector<Bbox>& input)
{
    if (stride == 0) {
        return;
    }
    float* yolo_buff = (float*)calloc(640L / stride * 640L / stride * 85L * 3L, sizeof(float*));
    file2buffer(file, yolo_buff);
    flatten(yolo_buff, 640L / stride * 640L / stride, 85L * 3L, 1L, 1L);
    yolo_v5_post_process_onescale(yolo_buff, biases, threshold, stride, input);
    free(yolo_buff);
}

void yolo_process(void)
{
    float biases[18] = { 10, 13, 16, 30, 33, 23, 30, 61, 62, 45, 59, 119, 116, 90, 156, 198, 373, 326 };

    float threshold = 0.25f;
    float iou_threshold = 0.45f;
    vector<Bbox> input;
    vector<Bbox> res;
    file_process("./network_dump/NBG_uid_0_t_0_s_80_80_255_1.txt", &biases[0L], threshold, 8L, input);
    file_process("./network_dump/NBG_uid_0_t_1_s_40_40_255_1.txt", &biases[6L], threshold, 16L, input);
    file_process("./network_dump/NBG_uid_0_t_2_s_20_20_255_1.txt", &biases[12L], threshold, 32L, input);

    res = nms(input, iou_threshold);
    for (int i = 0; i < res.size(); i++) {
        printf("class:[%s],[%d %d %d %d] score:%f\n", classes[res[i].classes].c_str(), res[i].x, res[i].y, res[i].w,
            res[i].h, res[i].score);
    }
}
/*-------------------------------------------
                  Functions
-------------------------------------------*/
static void save_output_data(vsi_nn_graph_t* graph)
{
    uint32_t i;
#define _DUMP_FILE_LENGTH 1028
#define _DUMP_SHAPE_LENGTH 128
    char filename[_DUMP_FILE_LENGTH] = { 0 };
    char shape[_DUMP_SHAPE_LENGTH] = { 0 };
    vsi_nn_tensor_t* tensor;

    for (i = 0; i < graph->output.num; i++) {
        tensor = vsi_nn_GetTensor(graph, graph->output.tensors[i]);
        vsi_nn_ShapeToString(tensor->attr.size, tensor->attr.dim_num, shape, _DUMP_SHAPE_LENGTH, FALSE);
        snprintf(filename, _DUMP_FILE_LENGTH, "output%u_%s.dat", i, shape);
        vsi_nn_SaveTensorToBinary(graph, tensor, filename);
    }
}

static vsi_bool get_top(
    float* pfProb, float* pfMaxProb, vsi_size_t* pMaxClass, vsi_size_t outputCount, vsi_size_t topNum)
{
    vsi_size_t i;
    vsi_size_t j;
    vsi_size_t k;

#define MAX_TOP_NUM 20
    if (topNum > MAX_TOP_NUM)
        return FALSE;

    memset(pfMaxProb, 0xfe, sizeof(float) * topNum);
    memset(pMaxClass, 0xff, sizeof(float) * topNum);

    for (j = 0; j < topNum; j++) {
        for (i = 0; i < outputCount; i++) {
            for (k = 0; k < topNum; k++) {
                if (i == pMaxClass[k]) {
                    break;
                }
            }

            if (k != topNum) {
                continue;
            }

            if (pfProb[i] > *(pfMaxProb + j)) {
                *(pfMaxProb + j) = pfProb[i];
                *(pMaxClass + j) = i;
            }
        }
    }

    return TRUE;
}

static vsi_status show_top5(vsi_nn_graph_t* graph, vsi_nn_tensor_t* tensor)
{
    vsi_status status = VSI_FAILURE;
    vsi_size_t i, sz, stride;
    float* buffer = NULL;
    uint8_t* tensor_data = NULL;
    vsi_size_t MaxClass[5];
    float fMaxProb[5];
    vsi_size_t topk = 5;

    sz = 1;
    for (i = 0; i < tensor->attr.dim_num; i++) {
        sz *= tensor->attr.size[i];
    }

    if (topk > sz) {
        topk = sz;
    }

    stride = (vsi_size_t)vsi_nn_TypeGetBytes(tensor->attr.dtype.vx_type);
    tensor_data = (uint8_t*)vsi_nn_ConvertTensorToData(graph, tensor);
    buffer = (float*)malloc(sizeof(float) * sz);

    for (i = 0; i < sz; i++) {
        status = vsi_nn_DtypeToFloat32(&tensor_data[stride * i], &buffer[i], &tensor->attr.dtype);
    }

    if (!get_top(buffer, fMaxProb, MaxClass, sz, topk)) {
        printf("Fail to show result.\n");
        goto final;
    }

    printf(" --- Top%d ---\n", topk);
    for (i = 0; i < topk; i++) {
        printf("%3d: %8.6f\n", MaxClass[i], fMaxProb[i]);
    }
    status = VSI_SUCCESS;

final:
    if (tensor_data) {
        vsi_nn_Free(tensor_data);
    }

    if (buffer) {
        free(buffer);
    }
    return status;
}

vsi_status vnn_PostProcessYolov5s(vsi_nn_graph_t* graph)
{
    vsi_status status = VSI_FAILURE;
    /* Show the top5 result */
    status = show_top5(graph, vsi_nn_GetTensor(graph, graph->output.tensors[0]));
    TEST_CHECK_STATUS(status, final);

    /* Save all output tensor data to txt file */
    save_output_data(graph);

final:
    return VSI_SUCCESS;
}

const vsi_nn_postprocess_map_element_t* vnn_GetPostProcessMap(void)
{
    return postprocess_map;
}

uint32_t vnn_GetPostProcessMapCount()
{
    if (postprocess_map == NULL) {
        return 0;
    } else {
        return sizeof(postprocess_map) / sizeof(vsi_nn_postprocess_map_element_t);
    }
}
