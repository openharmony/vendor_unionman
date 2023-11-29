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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "jpeglib.h"
#include "nn_util.h"
#include "nn_sdk.h"
#include "nn_demo.h"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
const float FLT_MAX = 3.402823466e+38F;
static int yolov5_box_num_after_filter = 0;
int g_detect_number = 50; // max detect num
obj_detect_out_t dectout;
/** Status enum */
typedef enum {
    UTIL_FAILURE = -1,
    UTIL_SUCCESS = 0,
} nn_status_e;

/*-------------------------------------------
                  Functions
-------------------------------------------*/
void process_top5(float *buf, unsigned int num, img_classify_out_t *clsout)
{
    int j = 0;
    int nums = 5;
    unsigned int MaxClass[5] = {0};
    float fMaxProb[5] = {0.0};

    float *pfMaxProb = fMaxProb;
    unsigned int *pMaxClass = MaxClass, i = 0;

    for (j = 0; j < nums; j++) {
        for (i = 0; i < num; i++) {
            if ((i == *(pMaxClass + 0L)) || (i == *(pMaxClass + 1L)) || (i == *(pMaxClass + 2L)) ||
                (i == *(pMaxClass + 3L)) || (i == *(pMaxClass + 4L))) {
                continue;
            }

            if (buf[i] > *(pfMaxProb + j)) {
                *(pfMaxProb + j) = buf[i];
                *(pMaxClass + j) = i;
            }
        }
    }
    for (i = 0; i < nums; i++) {
        if (clsout == nullptr) {
            printf("%3d: %8.6f\n", MaxClass[i], fMaxProb[i]);
        } else {
            clsout->score[i] = fMaxProb[i];
            clsout->topClass[i] = MaxClass[i];
        }
    }
}

float logistic_activate(float x)
{
    return 1.f / (1.f + exp(-x));
}

box get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h, int stride)
{
    float xx = (x[index + 0]);
    float xy = (x[index + 1]);
    float xw = (x[index + 2]);
    float xh = (x[index + 3]);

    box b;

    b.x = (i + xx * 2L - 0.5f) * stride;
    b.y = (j + xy * 2L - 0.5f) * stride;
    b.w = pow((xw * 2L), 2L) * biases[2L * n];
    b.h = pow((xh * 2L), 2L) * biases[2L * n + 1];

    return b;
}

box get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h)
{
    box b;
    if (w == 0 || h == 0) {
        return b;
    }
    b.x = (i + logistic_activate(x[index + 0])) / w;
    b.y = (j + logistic_activate(x[index + 1])) / h;
    b.w = exp(x[index + 2L]) * biases[2L * n] / w;
    b.h = exp(x[index + 3L]) * biases[2L * n + 1] / h;
    return b;
}

int max_index(float *a, int n)
{
    int i, max_i = 0;
    float max = a[0];

    if (n <= 0) {
        return -1;
    }

    for (i = 1; i < n; ++i) {
        if (a[i] > max) {
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

int max(int x, int y)
{
    if(x >= y){
        return x;
    }
    return y;
}

int min(int x, int y)
{
    if(x > y){
        return y;
    }
    return x;
}

float box_iou(box box1, box box2)
{
    int x1 = max(box1.x, box2.x);
	int y1 = max(box1.y, box2.y);
	int x2 = min(box1.x + box1.w, box2.x + box2.w);
	int y2 = min(box1.y + box1.h, box2.y + box2.h);
	int w = max(0, x2 - x1);
	int h = max(0, y2 - y1);
	float over_area = w * h;
	return over_area / (box1.w*box1.h + box2.w*box2.h - over_area);
}

int nms_comparator(const void *pa, const void *pb)
{
    sortable_bbox a = *(sortable_bbox *)pa;
    sortable_bbox b = *(sortable_bbox *)pb;
    return a.probs[a.index][b.classId] - b.probs[b.index][b.classId] < 0 ? 1 : -1;
}

void do_nms_sort(box *boxes, float **probs, int total, int classes, float thresh)
{
    if (total <= 0) {
        return;
    }
    sortable_bbox *s = (sortable_bbox *)calloc(total, sizeof(sortable_bbox));

    for (int i = 0; i < total; ++i) {
        s[i].index = i;
        s[i].classId = 0;
        s[i].probs = probs;
    }
    for (int k = 0; k < classes; ++k) {
        for (int i = 0; i < total; ++i) {
            s[i].classId = k;
        }
        qsort(s, total, sizeof(sortable_bbox), nms_comparator);
        for (int i = 0; i < total; ++i) {
            if (probs[s[i].index][k] == 0)
                continue;
            for (int j = i + 1; j < total; ++j) {
                box b = boxes[s[j].index];
                if (probs[s[j].index][k] > 0) {
                    if (box_iou(boxes[s[i].index], b) > thresh) {
                        probs[s[j].index][k] = 0;
                    }
                }
            }
        }
    }
    free(s);
}

void flatten(float *x, int size, int layers, int batch, int forward)
{
    if (size <= 0 || layers <= 0 || batch <= 0) {
        return;
    }
    float *swap = (float *)calloc(size * layers * batch, sizeof(float));
    for (int b = 0; b < batch; ++b) {
        for (int c = 0; c < layers; ++c) {
            for (int i = 0; i < size; ++i) {
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

void softmax(float *input, int n, float temp, float *output)
{
    int i;
    float sum = 0;
    float largest = -FLT_MAX;
    for (i = 0; i < n; ++i) {
        if (input[i] > largest) {
            largest = input[i];
        }
    }
    for (i = 0; i < n; ++i) {
        if (temp == 0) {
            return;
        }
        float e = exp(input[i] / temp - largest / temp);
        sum += e;
        output[i] = e;
    }
    for (i = 0; i < n; ++i) {
        output[i] /= sum;
    }
}

static int _jpeg_to_bmp(FILE *inputFile, unsigned char *bmpData, unsigned int bmpWidth, unsigned int bmpHeight,
                        unsigned int channel)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    unsigned char *point = nullptr;
    unsigned long width, height;
    unsigned short depth = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, inputFile);
    jpeg_read_header(&cinfo, TRUE);

    cinfo.dct_method = JDCT_IFAST;

    if (bmpData == nullptr) {
        return -1;
    } else {
        jpeg_start_decompress(&cinfo);

        width = cinfo.output_width;
        height = cinfo.output_height;
        depth = cinfo.output_components;
        if (width * height * depth != bmpWidth * bmpHeight * channel) {
            printf("wrong jpg file , the jpg file size should be %u %u %u\n", bmpWidth, bmpHeight, channel);
            return -1;
        }

        buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, width * depth, 1);

        point = bmpData;

        while (cinfo.output_scanline < height) {
            jpeg_read_scanlines(&cinfo, buffer, 1);
            memcpy(point, *buffer, width * depth);
            point += width * depth;
        }

        jpeg_finish_decompress(&cinfo);
    }

    jpeg_destroy_decompress(&cinfo);

    return 0;
}

unsigned char *get_jpeg_rawData(const char *name, unsigned int width, unsigned int height)
{
    FILE *bmpFile;
    unsigned char *bmpData;
    unsigned int sz, w, h, c;
    int status;

    bmpFile = nullptr;
    bmpData = nullptr;
    w = width;
    h = height;
    c = CHANNEL;
    sz = w * h * CHANNEL;

    bmpFile = fopen(name, "rb");
    if (bmpFile == nullptr) {
        printf("nullptr returned\n");
        goto final;
    }

    bmpData = (unsigned char *)malloc(sz * sizeof(char));
    if (bmpData == nullptr) {
        printf("nullptr returned\n");
        goto final;
    }
    memset(bmpData, 0, sz * sizeof(char));

    status = _jpeg_to_bmp(bmpFile, bmpData, w, h, c);
    if (status == -1) {
        free(bmpData);
        fclose(bmpFile);
        return nullptr;
    }

final:
    if (bmpFile) {
        fclose(bmpFile);
    }
    return bmpData;
}

float Float16ToFloat32(const signed short *src, float *dst, int lenth)
{
    signed int t1;
    signed int t2;
    signed int t3;
    float out;
    int i;
    for (i = 0; i < lenth; i++) {
        t1 = src[i] & 0x7fff; // Non-sign bits
        t2 = src[i] & 0x8000; // Sign bit
        t3 = src[i] & 0x7c00; // Exponent

        t1 <<= 13L; // Align mantissa on MSB
        t2 <<= 16L; // Shift sign bit into position

        t1 += 0x38000000; // Adjust bias

        t1 = (t3 == 0 ? 0 : t1); // Denormals-as-zero

        t1 |= t2;
        out = t1; // Re-insert sign bit
        dst[i] = out;
    }
    return out;
}

float *dtype_To_F32(nn_output *outdata, int sz)
{
    int stride, fl, i, zeropoint;
    float scale;
    unsigned char *buffer_u8 = nullptr;
    signed char *buffer_int8 = nullptr;
    signed short *buffer_int16 = nullptr;
    float *buffer_f32 = nullptr;

    if (sz <= 0) {
        return nullptr;
    }
    buffer_f32 = (float *)malloc(sizeof(float) * sz);

    if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_UINT8) {
        if (sz == 0) {
            return nullptr;
        }
        stride = (outdata->out[0].size) / sz;
        scale = outdata->out[0].param->quant_data.affine.scale;
        zeropoint = outdata->out[0].param->quant_data.affine.zeroPoint;

        buffer_u8 = (unsigned char *)outdata->out[0].buf;
        for (i = 0; i < sz; i++) {
            buffer_f32[i] = static_cast<float>(buffer_u8[stride * i] - zeropoint) * scale;
        }
    }

    else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_INT8) {
        buffer_int8 = (signed char *)outdata->out[0].buf;
        if (outdata->out[0].param->quant_data.dfp.fixed_point_pos >= 0) {
            fl = 1 << (outdata->out[0].param->quant_data.dfp.fixed_point_pos);
            for (i = 0; i < sz; i++) {
                buffer_f32[i] = static_cast<float>(buffer_int8[i]) * (1.0 / static_cast<float>(fl));
            }
        } else {
            fl = 1 << (-outdata->out[0].param->quant_data.dfp.fixed_point_pos);
            for (i = 0; i < sz; i++) {
                buffer_f32[i] = static_cast<float>(buffer_int8[i]) * (static_cast<float>(fl));
            }
        }
    }

    else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_INT16) {
        buffer_int16 = (signed short *)outdata->out[0].buf;
        if (outdata->out[0].param->quant_data.dfp.fixed_point_pos >= 0) {
            fl = 1 << (outdata->out[0].param->quant_data.dfp.fixed_point_pos);
            for (i = 0; i < sz; i++) {
                buffer_f32[i] = static_cast<float>((buffer_int16[i]) * (1.0 / static_cast<float>(fl)));
            }
        } else {
            fl = 1 << (-outdata->out[0].param->quant_data.dfp.fixed_point_pos);
            for (i = 0; i < sz; i++) {
                buffer_f32[i] = static_cast<float>((buffer_int16[i]) * (static_cast<float>(fl)));
            }
        }
    } else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_FP16) {
        buffer_int16 = (signed short *)outdata->out[0].buf;

        Float16ToFloat32(buffer_int16, buffer_f32, sz);
    }

    else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_FP32) {
        memcpy(buffer_f32, outdata->out[0].buf, sz);
    } else {
        printf("Error: currently not support type, type = %d\n", outdata->out[0].param->data_format);
    }
    return buffer_f32;
}

void *yolov2_result(int num, float thresh, box *boxes, float **probs, int classes)
{
    int i = 0, detect_num = 0;

    if (dectout.pBox == nullptr) {
        dectout.pBox = (detBox *)malloc(MAX_DETECT_NUM * sizeof(detBox));
    }
    for (i = 0; i < num; ++i) {
        int classId = max_index(probs[i], classes);
        float prob = probs[i][classId];
        if (prob > thresh) {
            if (detect_num >= g_detect_number) {
                break;
            }
            dectout.pBox[detect_num].x = boxes[i].x;
            dectout.pBox[detect_num].y = boxes[i].y;
            dectout.pBox[detect_num].w = boxes[i].w;
            dectout.pBox[detect_num].h = boxes[i].h;
            dectout.pBox[detect_num].score = prob;
            dectout.pBox[detect_num].objectClass = (float)classId;
            detect_num++;
        }
    }
    dectout.detNum = detect_num;
    return &dectout;
}

void *object_detect_postprocess(float *predictions, int width, int height, int modelWidth, int modelHeight,
                                int input_num)
{
    int i, j, n;
    float threshold = 0.24;
    float iou_threshold = 0.4;
    int num_class = 80;
    int num_box = 5;
    int coordinate_size = 4;
    int conf_size = 1;
    int pre_size = coordinate_size + conf_size;
    int grid_size = 13;
    float biases[10] = {0.738768, 0.874946,  2.422040, 2.657040,  4.309710,
                        7.044930, 10.246000, 4.594280, 12.686800, 11.874100};
    void *objout = nullptr;

    box *boxes = (box *)calloc(modelWidth * modelHeight * num_box, sizeof(box));
    float **probs = (float **)calloc(modelWidth * modelHeight * num_box, sizeof(float *));

    for (j = 0; j < modelWidth * modelHeight * num_box; ++j) {
        probs[j] = (float *)calloc(num_class + 1, sizeof(float *)); // calloc "num_class+1" float for every W*H*num_box
    }

    {
        int b;
        int coords = 4, classes = 80;
        int size = coords + classes + 1;
        int w = 13;
        int h = 13;
        int batch = 1;
        n = 425L / size;
        flatten(predictions, w * h, size * n, batch, 1);

        for (b = 0; b < batch; ++b) {
            for (i = 0; i < h * w * n; ++i) {
                int index = size * i + b * input_num;
                predictions[index + 4L] = logistic_activate(predictions[index + 4L]);
            }
        }

        for (b = 0; b < batch; ++b) {
            for (i = 0; i < h * w * n; ++i) {
                int index = size * i + b * input_num;
                softmax(predictions + index + pre_size, classes, 1, predictions + index + pre_size);
            }
        }
    }

    for (i = 0; i < modelWidth * modelHeight; ++i) {
        if (modelWidth == 0) {
            return objout;
        }
        int row = i / modelWidth;
        int col = i % modelWidth;
        for (n = 0; n < num_box; ++n) {
            int index = i * num_box + n;
            int p_index = index * (num_class + pre_size) + 4;
            float scale = predictions[p_index];
            int box_index = index * (num_class + pre_size);
            int class_index = 0;
            boxes[index] = get_region_box(predictions, biases, n, box_index, col, row, modelWidth, modelHeight);
            class_index = index * (num_class + pre_size) + pre_size;
            for (j = 0; j < num_class; ++j) {
                float prob = scale * predictions[class_index + j];
                probs[index][j] = (prob > threshold) ? prob : 0;
            }
        }
    }

    do_nms_sort(boxes, probs, grid_size * grid_size * num_box, num_class, iou_threshold);
    objout = yolov2_result(grid_size * grid_size * num_box, threshold, boxes, probs, num_class);

    free(boxes);
    boxes = nullptr;

    for (j = 0; j < grid_size * grid_size * num_box; ++j) {
        free(probs[j]);
        probs[j] = nullptr;
    }

    free(probs);
    probs = nullptr;
    return objout;
}

int yolo_v5_post_process_onescale(float *predictions, int input_size[3], float *biases, box *boxes, float **pprobs,
                                  float threshold_in, int stride)
{
    int i, j;
    int num_class = 80;
    int coords = 4;
    int bb_size = coords + num_class + 1;
    int num_box = input_size[2] / bb_size;
    int modelWidth = input_size[0];
    int modelHeight = input_size[1];
    float threshold = threshold_in;

    for (j = 0; j < modelWidth * modelHeight * num_box; ++j) {
        pprobs[j] = (float *)calloc(num_class + 1, sizeof(float *));
    }

    int ck0, batch = 1;
    flatten(predictions, modelWidth * modelHeight, bb_size * num_box, batch, 1);

    for (i = 0; i < modelHeight * modelWidth * num_box; ++i) {
        for (ck0 = coords; ck0 < bb_size; ck0++) {
            int index = bb_size * i;
            if (ck0 == coords) {
                if (predictions[index + ck0] <= threshold) {
                    break;
                }
            }
        }
    }

    for (i = 0; i < modelWidth * modelHeight; ++i) {
        int row = i / modelWidth;
        int col = i % modelWidth;
        int n = 0;
        for (n = 0; n < num_box; ++n) {
            int index = i * num_box + n;
            int p_index = index * bb_size + 4L;
            float scale = predictions[p_index];
            int box_index = index * bb_size;
            int class_index = 0;
            class_index = index * bb_size + 5L;

            if (scale > threshold) {
                yolov5_box_num_after_filter++;
                for (j = 0; j < num_class; ++j) {
                    float prob = scale * predictions[class_index + j];
                    pprobs[index][j] = (prob > threshold) ? prob : 0;
                }
                boxes[index] =
                    get_region_box(predictions, biases, n, box_index, col, row, modelWidth, modelHeight, stride);
            }
            boxes[index].prob_obj = (scale > threshold) ? scale : 0;
        }
    }
    return 0;
}

void yolov5_result(int num, float thresh, box *boxes, float **probs, int classes, obj_detect_out_t *dect_out)
{
    int i = 0, detect_num = 0;
    dect_out->pBox = (detBox *)malloc(MAX_DETECT_NUM * sizeof(detBox));
    for (i = 0; i < num; ++i) {
        int classId = max_index(probs[i], classes);
        float prob = probs[i][classId];
        if (prob > thresh) {
            if (detect_num >= g_detect_number) {
                break;
            }
            dect_out->pBox[detect_num].x = boxes[i].x;
            dect_out->pBox[detect_num].y = boxes[i].y;
            dect_out->pBox[detect_num].w = boxes[i].w;
            dect_out->pBox[detect_num].h = boxes[i].h;
            dect_out->pBox[detect_num].score = prob;
            dect_out->pBox[detect_num].objectClass = (float)classId;
            detect_num++;
        }
    }
    dect_out->detNum = detect_num;
}

void *yolov5_postprocess(float **predictions, int width, int height, int modelWidth, int modelHeight, int input_num)
{
    yolov5_box_num_after_filter = 0;
    int nn_width, nn_height, nn_channel;
    void *objout = nullptr;
    nn_width = width;
    nn_height = height;
    nn_channel = CHANNEL;
    (void)nn_channel;
    int size[3] = {nn_width / 32, nn_height / 32, 85 * 3};

    int j, k, index;
    int num_class = 80;
    float threshold = 0.25;
    float iou_threshold = 0.45;

    float biases[18] = {10, 13., 16, 30, 33, 23, 30, 61, 62, 45, 59, 119, 116, 90, 156, 198, 373, 326};
    int size2[3] = {size[0] * 2, size[1] * 2, size[2]};
    int size4[3] = {size[0] * 4, size[1] * 4, size[2]};
    int len1 = size[0] * size[1] * size[2];
    int box1 = len1 / (num_class + 5);

    box *boxes = (box *)calloc(box1 * (1L + 4L + 16L), sizeof(box));
    float **probs = (float **)calloc(box1 * (1L + 4L + 16L), sizeof(float *));

    yolo_v5_post_process_onescale(predictions[0L], size4, &biases[0L], &boxes[box1 * (1L + 4L)],
                                  &probs[box1 * (1L + 4L)], threshold, 8L);
    yolo_v5_post_process_onescale(predictions[1L], size2, &biases[6L], &boxes[box1], &probs[box1], threshold, 16L);
    yolo_v5_post_process_onescale(predictions[2L], size, &biases[12L], boxes, &probs[0L], threshold, 32L);

    box *tmp_boxes = (box *)calloc(yolov5_box_num_after_filter, sizeof(box));
    float **tmp_probs = (float **)calloc(yolov5_box_num_after_filter, sizeof(float *));

    for (index = 0, k = 0; index < box1 * (1L + 4L + 16L); index++) {
        if ((fabs(boxes[index].prob_obj) - 0) > 0.000001f) {
            tmp_probs[k] = probs[index];
            tmp_boxes[k] = boxes[index];
            k++;
        }
    }
    do_nms_sort(tmp_boxes, tmp_probs, yolov5_box_num_after_filter, num_class, iou_threshold);
    objout = yolov2_result(yolov5_box_num_after_filter, threshold, tmp_boxes, tmp_probs, num_class);

    free(tmp_boxes);
    tmp_boxes = nullptr;
    free(tmp_probs);
    tmp_probs = nullptr;

    for (j = 0; j < box1 * (1L + 4L + 16L); ++j) {
        free(probs[j]);
        probs[j] = nullptr;
    }
    free(boxes);
    boxes = nullptr;
    free(probs);
    probs = nullptr;
    return objout;
}

void *postprocess_object_detect(nn_output *pout)
{
    float *obj_out;
    int output_num;

    obj_out = (float *)pout->out[0].buf;
    output_num = pout->out[0].size / sizeof(float);

    return object_detect_postprocess(obj_out, 416L, 416L, 13L, 13L, output_num);
}

void *postprocess_yolov5(nn_output *pout)
{
    float *yolov3_buffer[3] = {nullptr};
    yolov3_buffer[0L] = (float *)pout->out[0L].buf;
    yolov3_buffer[1L] = (float *)pout->out[1L].buf;
    yolov3_buffer[2L] = (float *)pout->out[2L].buf;
    return yolov5_postprocess(yolov3_buffer, 640L, 640L, 13L, 13L, 0L);
}