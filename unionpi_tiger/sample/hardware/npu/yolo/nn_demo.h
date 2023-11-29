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

#ifndef _AMLOGIC_NN_DEMO_H
#define _AMLOGIC_NN_DEMO_H

#include "nn_sdk.h"
#include "nn_util.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef YOLOV2
const int WIDTH = 416;
const int HIGH = 416;
#else
const int WIDTH = 640;
const int HIGH = 640;
#endif
const int CHANNEL = 3;

typedef struct __nn_image_classify {
    float score[5];
    unsigned int topClass[5];
} img_classify_out_t;

typedef struct __nn_obj_detect {
    unsigned int detNum;
    detBox *pBox;
} obj_detect_out_t;

void activate_array(float *start, int num);
int entry_index(int lw, int lh, int lclasses, int loutputs, int batch, int location, int entry);
void softmax(float *input, int n, float temp, float *output);
void flatten(float *x, int size, int layers, int batch, int forward);
void do_nms_sort(box *boxes, float **probs, int total, int classes, float thresh);
int nms_comparator(const void *pa, const void *pb);
float box_iou(box a, box b);
float box_union(box a, box b);
float box_intersection(box a, box b);
float overlap(float x1, float w1, float x2, float w2);
float logistic_activate(float x);
float sigmod(float x);
unsigned char *transpose(const unsigned char *src, int width, int height);
void *post_process_all_module(aml_module_t type, nn_output *pOut);
int max_index(float *a, int n);
box get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h);
void process_top5(float *buf, unsigned int num, img_classify_out_t *clsout);
void *postprocess_object_detect(nn_output *pout);
unsigned char *get_jpeg_rawData(const char *name, unsigned int width, unsigned int height);
void *postprocess_yolov5(nn_output *pout);

#ifdef __cplusplus
} // extern "C"
#endif
#endif