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
#include <ctime>
#include <unistd.h>
#include "nn_sdk.h"
#include "nn_demo.h"
#include "nn_util.h"
#include <pthread.h>
#include "jpeglib.h"
#include <vector>
#include <iostream>

#define USE_OPENCV
#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
using namespace cv;
#endif
using namespace std;
#define NBG_FROM_MEMORY

static const char *coco_names[] = {
    "person",         "bicycle",    "car",           "motorbike",     "aeroplane",   "bus",           "train",
    "truck",          "boat",       "traffic light", "fire hydrant",  "stop sign",   "parking meter", "bench",
    "bird",           "cat",        "dog",           "horse",         "sheep",       "cow",           "elephant",
    "bear",           "zebra",      "giraffe",       "backpack",      "umbrella",    "handbag",       "tie",
    "suitcase",       "frisbee",    "skis",          "snowboard",     "sports ball", "kite",          "baseball bat",
    "baseball glove", "skateboard", "surfboard",     "tennis racket", "bottle",      "wine glass",    "cup",
    "fork",           "knife",      "spoon",         "bowl",          "banana",      "apple",         "sandwich",
    "orange",         "broccoli",   "carrot",        "hot dog",       "pizza",       "donut",         "cake",
    "chair",          "sofa",       "pottedplant",   "bed",           "diningtable", "toilet",        "tvmonitor",
    "laptop",         "mouse",      "remote",        "keyboard",      "cell phone",  "microwave",     "oven",
    "toaster",        "sink",       "refrigerator",  "book",          "clock",       "vase",          "scissors",
    "teddy bear",     "hair drier", "toothbrush"};

static void *context = nullptr;
char *jpath = nullptr;
static unsigned char *rawdata = nullptr;
static unsigned char *data_carplate = nullptr;
static unsigned char *outbuf = nullptr;
static unsigned char *inbuf = nullptr;
static int use_dma = 0;

static int input_width = 0, input_high = 0;
static int display_width = 640, display_high = 480;
nn_input inData;
static std::string changeFileName(const std::string& filePath, const std::string& newFileName)
{
    size_t lastSlashPos = filePath.find_last_of('/');
    if (lastSlashPos != std::string::npos) {
        std::string pathBeforeFileName = filePath.substr(0, lastSlashPos + 1);
        std::string newPath = pathBeforeFileName + newFileName;
        return newPath;
    } else {
        std::cerr << "Invalid file path format." << std::endl;
        return "erro";
    }
}

int run_network(void *qcontext, unsigned char *qrawdata, int fbmode, unsigned char *fbbuf)
{
    img_classify_out_t *cls_out = nullptr;
    obj_detect_out_t *obj_detect_out = nullptr;
    nn_output *outdata = nullptr;
    aml_module_t modelType;
    int sz = 1;
    int j;
    unsigned int i = 0;
    int ret = 0;
    float *buffer = nullptr;

    int cols = WIDTH;
    int rows = HIGH;

    FILE *fp, *File;
    aml_output_config_t outconfig;
    memset(&outconfig, 0, sizeof(aml_output_config_t));

    if (use_dma == 1) {
        if (qrawdata != inbuf) {
            memcpy(inbuf, qrawdata, input_high * input_width * CHANNEL);
            aml_util_flushTensorHandle(qcontext, AML_INPUT_TENSOR); // !!note,flush the input buffer in dma mode
        }
    } else {
        inData.input = qrawdata;
    }

    ret = aml_module_input_set(qcontext, &inData);
    if (ret != 0) {
        printf("aml_module_input_set error\n");
    }

#ifndef USE_OPENCV
    if (rawdata != nullptr) {
        free(rawdata);
        rawdata = nullptr;
    }
#endif

#ifdef USE_OPENCV
    cv::Mat img;
    cv::Mat img2;
#endif

    // start to record usage time
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    outdata = (nn_output *)aml_module_output_get_simple(qcontext);

    // end to record usage time
    end = clock();
    cpu_time_used = (static_cast<double>(end - start)) / CLOCKS_PER_SEC;
    double fps = 1 / cpu_time_used;
    printf("used time: %f s; fps: %f\n", cpu_time_used, fps);
#ifdef YOLOV2
    obj_detect_out = (obj_detect_out_t *)postprocess_object_detect(outdata);
#else
    obj_detect_out = (obj_detect_out_t *)postprocess_yolov5(outdata);
#endif

#ifdef USE_OPENCV
    img = cv::Mat(cols, rows, CV_8UC3);
    img.data = rawdata;
    if (fbmode != 0) {
        img2 = imread((char *)fbbuf);
        cols = img2.cols;
        rows = img2.rows;
    }
#endif

    if (obj_detect_out == nullptr) {
        printf("aml_module_output_get error\n");
        return -1;
    }

    {
        int classid;
        float prob;
        printf("object_num:%d\n", obj_detect_out->detNum);
        for (i = 0; i < obj_detect_out->detNum; i++) {
            classid = (int)obj_detect_out->pBox[i].objectClass;
            prob = obj_detect_out->pBox[i].score;

            int left = 0, right = 0, top = 0, bot = 0;
#ifdef YOLOV2
            left = (obj_detect_out->pBox[i].x - obj_detect_out->pBox[i].w / 2L) * cols;
            right = (obj_detect_out->pBox[i].x + obj_detect_out->pBox[i].w / 2L) * cols;
            top = (obj_detect_out->pBox[i].y - obj_detect_out->pBox[i].h / 2L) * rows;
            bot = (obj_detect_out->pBox[i].y + obj_detect_out->pBox[i].h / 2L) * rows;
#else
            left = (obj_detect_out->pBox[i].x - obj_detect_out->pBox[i].w / 2L);
            right = (obj_detect_out->pBox[i].x + obj_detect_out->pBox[i].w / 2L);
            top = (obj_detect_out->pBox[i].y - obj_detect_out->pBox[i].h / 2L);
            bot = (obj_detect_out->pBox[i].y + obj_detect_out->pBox[i].h / 2L);
#endif
            int border = 2.f;
            if (left < border) {
                left = border;
            }
            if (right > cols - border) {
                right = cols - border;
            }
            if (top < border) {
                top = border;
            }
            if (bot > rows - border) {
                bot = rows - border;
            }
            printf("class:%s,prob:%f,[left = %d,right = %d,top = %d,bot = %d]\n", coco_names[classid], prob, left,
                   right, top, bot);
#ifdef USE_OPENCV
            {
                cv::rectangle(img, cv::Point(left, top), cv::Point(right, bot), cv::Scalar(0L, 0L, 255L), 2L, 8L);
                cv::putText(img, cv::format("%s %.2f", coco_names[classid], prob), cv::Point(left, top - 10L),
                            cv::FONT_HERSHEY_SIMPLEX, 0.5f, cv::Scalar(0L, 255L, 0L));
            }
#endif
        }
        memset(obj_detect_out, 0, sizeof(obj_detect_out_t));
    }
#ifdef USE_OPENCV

    std::string modifiedPath = changeFileName(jpath, "out.bmp");
    std::strcpy(jpath, modifiedPath.c_str());
    cv::imwrite(jpath, img);

    img2.release();
#endif

#ifdef USE_OPENCV
    if (rawdata != nullptr) {
        free(rawdata);
        rawdata = nullptr;
    }
#endif
    return ret;
}

void *init_network(int argc, char **argv)
{
    const char *jpeg_path = nullptr;
    int size = 0;
    aml_config config;
    tensor_info *inptr;
    tensor_info *outptr;
    void *qcontext = nullptr;

    memset(&config, 0, sizeof(aml_config));
    memset(&inData, 0, sizeof(nn_input));
    FILE *fp, *File;

#ifdef NBG_FROM_MEMORY
    fp = fopen(argv[1], "rb");
    if (fp == nullptr) {
        printf("open %s fail\n", argv[1]);
        return nullptr;
    }
    fseek(fp, 0, SEEK_END);
    size = static_cast<int>(ftell(fp));
    rewind(fp);
    config.pdata = (char *)calloc(1, size);
    if (config.pdata == nullptr) {
        printf("malloc nbg memory fail\n");
        return nullptr;
    }
    fread((void *)config.pdata, 1, size, fp);
    config.nbgType = NN_NBG_MEMORY; // load nbg from memory
    config.length = size;
    fclose(fp);

#else
    config.path = (const char *)argv[1];
    config.nbgType = NN_NBG_FILE; // load nbg from file
#endif

    printf("the input type should be %d*%d*%d\n", WIDTH, HIGH, CHANNEL);
    input_width = WIDTH;
    input_high = HIGH;

    config.modelType = DARKNET;
    qcontext = aml_module_create(&config);
    if (qcontext == nullptr) {
        printf("amlnn_init is fail\n");
        return nullptr;
    }
    inData.input_index = 0; // this value is index of input,begin from 0

    inData.size = WIDTH * HIGH * CHANNEL;

    inData.input_type = RGB24_RAW_DATA;

    if (config.nbgType == NN_NBG_MEMORY && config.pdata != nullptr) {
        free((void *)config.pdata);
    }
    return qcontext;
}

int destroy_network(void *qcontext)
{
    if (outbuf)
        aml_util_freeAlignedBuffer(outbuf);
    if (inbuf)
        aml_util_freeAlignedBuffer(inbuf);

    int ret = aml_module_destroy(qcontext);
    return ret;
}
#ifdef USE_OPENCV
int resize_input_data(unsigned char *indata, unsigned char *outdata)
{
    cv::Mat inImage = cv::Mat(display_high, display_width, CV_8UC3);
    cv::Mat dstImage = cv::Mat(input_high, input_width, CV_8UC3);
    inImage.data = indata;
    dstImage.data = outdata;
    cv::resize(inImage, dstImage, dstImage.size());
    return 0;
}
#endif
void *net_thread_func(void *args)
{
    jpath = (char *)args;
    int ret = 0;
#ifdef USE_OPENCV
    unsigned char *dup_rgbbuf = nullptr;
    unsigned char *input_data = nullptr;
    char cmd[128];
    char img_name[64];
    char *ptr;
#endif
    int newWidth = 640;
    int newHeight = 640;
    // 调用函数，不需要获取返回值
    rawdata = get_jpeg_rawData(jpath, input_width, input_high);
    ret = run_network(context, rawdata, AML_IN_PICTURE, nullptr);

    ret = destroy_network(context);
    if (ret != 0) {
        printf("aml_module_destroy error\n");
    }
    return nullptr;
}

int main(int argc, char **argv)
{
    int ret = 0;
    pthread_t tid;
    void *pic_path = argv[2];

    context = init_network(argc, argv);
    if (0 != pthread_create(&tid, nullptr, net_thread_func, pic_path)) {
        fprintf(stderr, "Couldn't create thread func\n");
        return -1;
    }

    ret = pthread_join(tid, nullptr);

    return ret;
}
