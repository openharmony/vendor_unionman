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
#include "vnn_pre_process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jpeglib.h"
#include "vnn_global.h"
#include "vsi_nn_pub.h"

#define _BASETSD_H

/*-------------------------------------------
                  Variable definitions
-------------------------------------------*/

/*{graph_input_idx, preprocess}*/
const static vsi_nn_preprocess_map_element_t* preprocess_map = NULL;

/*-------------------------------------------
                  Functions
-------------------------------------------*/
#define INPUT_META_NUM 1
static vnn_input_meta_t input_meta_tab[INPUT_META_NUM];
static void _load_input_meta(void)
{
    uint32_t i;
    for (i = 0; i < INPUT_META_NUM; i++) {
        memset(&input_meta_tab[i].image.preprocess, VNN_PREPRO_NONE, sizeof(int32_t) * VNN_PREPRO_NUM);
    }
    /* lid: images_152 */
    input_meta_tab[0L].image.preprocess[0L] = VNN_PREPRO_REORDER;
    input_meta_tab[0L].image.preprocess[1L] = VNN_PREPRO_MEAN;
    input_meta_tab[0L].image.preprocess[2L] = VNN_PREPRO_SCALE;
    input_meta_tab[0L].image.reorder[0L] = 2L;
    input_meta_tab[0L].image.reorder[1L] = 1L;
    input_meta_tab[0L].image.reorder[2L] = 0L;
    input_meta_tab[0L].image.mean[0L] = 0.0f;
    input_meta_tab[0L].image.mean[1L] = 0.0f;
    input_meta_tab[0L].image.mean[2L] = 0.0f;
    input_meta_tab[0L].image.scale = 0.00390625f;
}

static vsi_enum _get_file_type(const char* file_name)
{
    vsi_enum type = 0L;
    const char* ptr;
    char sep = '.';
    uint32_t pos, n;
    char buff[32] = { 0 };

    ptr = strrchr(file_name, sep);
    pos = ptr - file_name;
    n = strlen(file_name) - (pos + 1);
    strncpy(buff, file_name + (pos + 1), n);

    if (strcmp(buff, "jpg") == 0 || strcmp(buff, "jpeg") == 0 || strcmp(buff, "JPG") == 0 ||
        strcmp(buff, "JPEG") == 0) {
        type = NN_FILE_JPG;
    } else if (strcmp(buff, "tensor") == 0 || strcmp(buff, "txt") == 0) {
        char* qnt_suffix = ".qnt.tensor";
        ptr = strstr(file_name, qnt_suffix);
        if (ptr && strlen(qnt_suffix)) {
            type = NN_FILE_QTENSOR;
        } else {
            type = NN_FILE_TENSOR;
        }
    } else if (strcmp(buff, "qtensor") == 0) {
        type = NN_FILE_QTENSOR;
    } else if (strcmp(buff, "bin") == 0 || strcmp(buff, "dat") == 0) {
        type = NN_FILE_BINARY;
    } else {
        type = NN_FILE_NONE;
    }

    return type;
}

static void _get_jpeg_size(const char* name, vsi_size_t* bmpWidth, vsi_size_t* bmpHeight, vsi_size_t* channel)
{
    FILE* jpgFile = NULL;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    jpgFile = fopen(name, "rb");
    TEST_CHECK_PTR(jpgFile, final);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, jpgFile);
    jpeg_read_header(&cinfo, (boolean)TRUE);

    cinfo.dct_method = JDCT_IFAST;
    jpeg_start_decompress(&cinfo);

    *bmpWidth = (vsi_size_t)cinfo.output_width;
    *bmpHeight = (vsi_size_t)cinfo.output_height;
    *channel = (vsi_size_t)cinfo.output_components;
    jpeg_destroy_decompress(&cinfo);

final:
    if (jpgFile) {
        fclose(jpgFile);
    }
}

static vsi_status _jpeg_to_bmp(
    FILE* inputFile, unsigned char* bmpData, vsi_size_t bmpWidth, vsi_size_t bmpHeight, vsi_size_t channel)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    unsigned char* point = NULL;
    unsigned long width;
    unsigned long height;
    unsigned short depth = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, inputFile);
    jpeg_read_header(&cinfo, (boolean)TRUE);

    cinfo.dct_method = JDCT_IFAST;

    if (bmpData == NULL) {
        return VSI_FAILURE;
    } else {
        jpeg_start_decompress(&cinfo);

        width = cinfo.output_width;
        height = cinfo.output_height;
        depth = cinfo.output_components;
        if (width * height * depth != bmpWidth * bmpHeight * channel) {
            printf("wrong jpg file , the jpg file size should be %u %u %u\n", bmpWidth, bmpHeight, channel);
            return VSI_FAILURE;
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

    return VSI_SUCCESS;
}

static uint8_t* _float32_to_dtype(float* fdata, vsi_nn_tensor_t* tensor)
{
    vsi_status status;
    uint8_t* data;
    vsi_size_t sz;
    vsi_size_t i;
    vsi_size_t stride;

    sz = vsi_nn_GetElementNum(tensor);
    stride = vsi_nn_TypeGetBytes(tensor->attr.dtype.vx_type);
    data = (uint8_t*)malloc(stride * sz * sizeof(uint8_t));
    TEST_CHECK_PTR(data, final);
    memset(data, 0, stride * sz * sizeof(uint8_t));

    for (i = 0; i < sz; i++) {
        status = vsi_nn_Float32ToDtype(fdata[i], &data[stride * i], &tensor->attr.dtype);
        if (status != VSI_SUCCESS) {
            if (data) {
                free(data);
            }
            return NULL;
        }
    }

final:
    return data;
}

static float* _imageData_to_float32(uint8_t* bmpData, vsi_nn_tensor_t* tensor)
{
    float* fdata;
    vsi_size_t sz;
    vsi_size_t i;
    fdata = NULL;
    sz = vsi_nn_GetElementNum(tensor);
    if (sz <= 0) {
        // error
        return NULL;
    }
    fdata = (float*)malloc(sz * sizeof(float));
    TEST_CHECK_PTR(fdata, final);
    for (i = 0; i < sz; i++) {
        fdata[i] = (float)bmpData[i];
    }

final:
    return fdata;
}

/*
    jpg file --> BMP data(dataformat: RGBRGBRGB...)
*/
static uint8_t* _decode_jpeg(const char* name, vsi_nn_tensor_t* tensor)
{
    FILE* bmpFile;
    uint8_t* bmpData;
    vsi_size_t sz;
    vsi_size_t w;
    vsi_size_t h;
    vsi_size_t c;
    vsi_status status;

    bmpFile = NULL;
    bmpData = NULL;
    w = tensor->attr.size[0L];
    h = tensor->attr.size[1L];
    c = tensor->attr.size[2L];
    sz = vsi_nn_GetElementNum(tensor);

    bmpFile = fopen(name, "rb");
    TEST_CHECK_PTR(bmpFile, final);

    if (sz <= 0) {
        // error
        return NULL;
    }
    bmpData = (uint8_t*)malloc(sz * sizeof(uint8_t));
    TEST_CHECK_PTR(bmpData, final);
    memset(bmpData, 0, sz * sizeof(uint8_t));

    status = _jpeg_to_bmp(bmpFile, bmpData, w, h, c);
    if (status == VSI_FAILURE) {
        free(bmpData);
        fclose(bmpFile);
        return NULL;
    }

final:
    if (bmpFile) {
        fclose(bmpFile);
    }
    return bmpData;
}

static void _prepare_imageprocess_params(
    vsi_nn_imageprocess_param* imageprocess, int32_t* crop_start, int32_t* crop_length, int32_t* resize_length)
{
    float scale = input_meta_tab[0L].image.scale;
    float* mean_value = input_meta_tab[0L].image.mean;
    uint32_t* reorder = input_meta_tab[0L].image.reorder;

    imageprocess->platform_type = VSI_NN_PLATFORM_CAFFE;
    imageprocess->crop.enable = TRUE;
    imageprocess->crop.dim_num = 4L;
    imageprocess->crop.start = crop_start;
    imageprocess->crop.length = crop_length;

    if (mean_value[0L] != 0 && mean_value[1L] != 0 && mean_value[2L] != 0) {
        imageprocess->mean.type = VSI_NN_IMAGEPROCESS_MEAN_CHANNEL;
        imageprocess->mean.mean_value_size = 3L;
        imageprocess->mean.mean_value = mean_value;
    } else {
        imageprocess->mean.type = VSI_NN_IMAGEPROCESS_MEAN_NONE;
    }
    if (scale == 0.0f) {
        imageprocess->mean.scale = 1.0f;
    } else {
        imageprocess->mean.scale = scale;
    }

    imageprocess->resize.type = VSI_NN_IMAGEPROCESS_RESIZE_BILINEAR;
    imageprocess->resize.dim_num = 2L;
    imageprocess->resize.length = resize_length;
    imageprocess->reverse_channel = FALSE;
    if (reorder[0L] == 0L && reorder[1L] == 1L && reorder[2L] == 2L) {
        imageprocess->reverse_channel = vx_true_e;
    }
}

static void _data_scale(float* fdata, vnn_input_meta_t* meta, vsi_nn_tensor_t* tensor)
{
    vsi_size_t i;
    vsi_size_t sz;
    float val;
    float scale;

    sz = vsi_nn_GetElementNum(tensor);
    scale = meta->image.scale;
    if (0 != scale) {
        for (i = 0; i < sz; i++) {
            val = fdata[i] * scale;
            fdata[i] = val;
        }
    }
}

static void _data_mean(float* fdata, vnn_input_meta_t* meta, vsi_nn_tensor_t* tensor)
{
    vsi_size_t s0;
    vsi_size_t s1;
    vsi_size_t s2;
    vsi_size_t i;
    vsi_size_t j;
    vsi_size_t offset;
    float val;
    float mean;

    s0 = tensor->attr.size[0L];
    s1 = tensor->attr.size[1L];
    s2 = tensor->attr.size[2L];

    for (i = 0; i < s2; i++) {
        offset = s0 * s1 * i;
        mean = meta->image.mean[i];
        for (j = 0; j < s0 * s1; j++) {
            val = fdata[offset + j] - mean;
            fdata[offset + j] = val;
        }
    }
}

/*
    caffe: transpose + reorder
    tf: reorder
*/
static void _data_transform(float* fdata, vnn_input_meta_t* meta, vsi_nn_tensor_t* tensor)
{
    vsi_size_t s0;
    vsi_size_t s1;
    vsi_size_t s2;
    vsi_size_t i;
    vsi_size_t j;
    vsi_size_t offset;
    vsi_size_t sz;
    vsi_size_t order;
    float* data;
    uint32_t* reorder;
    data = NULL;
    reorder = meta->image.reorder;
    s0 = tensor->attr.size[0L];
    s1 = tensor->attr.size[1L];
    s2 = tensor->attr.size[2L];
    sz = vsi_nn_GetElementNum(tensor);
    if (sz <= 0) {
        // error
        return;
    }
    data = (float*)malloc(sz * sizeof(float));
    TEST_CHECK_PTR(data, final);
    memset(data, 0, sizeof(float) * sz);

    for (i = 0; i < s2; i++) {
        if (s2 > 1 && reorder[i] <= s2) {
            order = reorder[i];
        } else {
            order = i;
        }

        offset = s0 * s1 * i;
        for (j = 0; j < s0 * s1; j++) {
            data[j + offset] = fdata[j * s2 + order];
        }
    }
    memcpy(fdata, data, sz * sizeof(float));
final:
    if (data) {
        free(data);
    }
}

static uint8_t* _get_binary_data(vsi_nn_tensor_t* tensor, const char* name)
{
    uint8_t* tensorData;
    vsi_size_t sz;
    vsi_size_t stride;
    vsi_size_t ret;
    vsi_size_t total_sz;
    FILE* tensorFile;

    tensorData = NULL;
    tensorFile = fopen(name, "rb");
    TEST_CHECK_PTR(tensorFile, error);

    sz = vsi_nn_GetElementNum(tensor);
    stride = vsi_nn_TypeGetBytes(tensor->attr.dtype.vx_type);
    total_sz = sz * stride;
    tensorData = (uint8_t*)malloc(total_sz * sizeof(uint8_t));
    TEST_CHECK_PTR(tensorData, error);

    memset(tensorData, 0, total_sz * sizeof(uint8_t));
    ret = fread(tensorData, 1L, total_sz, tensorFile);
    if (ret != total_sz) {
        printf("Read %s fail\n", name);
        printf("read data %u != tensor sz %u\n", ret, total_sz);
        if (tensorData)
            free(tensorData);
        goto error;
    }

    if (tensorFile) {
        fclose(tensorFile);
    }
    return tensorData;
error:
    if (tensorFile) {
        fclose(tensorFile);
    }
    return NULL;
}

static uint8_t* _get_qtensor_data(vsi_nn_tensor_t* tensor, const char* name)
{
    vsi_size_t i = 0L;
    float fval = 0.0f;
    uint8_t* tensorData;
    vsi_size_t sz = 1L;
    vsi_size_t stride = 1L;
    FILE* tensorFile;
    uint16_t uint16_temp_value = 0L;
    int16_t int16_temp_value = 0L;

    tensorData = NULL;
    tensorFile = fopen(name, "rb");
    TEST_CHECK_PTR(tensorFile, error);

    sz = vsi_nn_GetElementNum(tensor);
    stride = vsi_nn_TypeGetBytes(tensor->attr.dtype.vx_type);
    tensorData = (uint8_t*)malloc(sz * stride * sizeof(uint8_t));
    TEST_CHECK_PTR(tensorData, error);
    memset(tensorData, 0, sz * stride * sizeof(uint8_t));

    for (i = 0; i < sz; i++) {
        if (fscanf(tensorFile, "%f ", &fval) != 1) {
            printf("Read tensor file fail.\n");
            printf("Please check file lines or if the file contains illegal "
                   "characters\n");
            goto error;
        }
        if (1 == stride) {
            if (VSI_NN_TYPE_INT8 == tensor->attr.dtype.vx_type)
                tensorData[i * stride] = (int8_t)fval;
            else
                tensorData[i * stride] = (uint8_t)fval;
        } else if (2L == stride) {
            if (VSI_NN_TYPE_INT16 == tensor->attr.dtype.vx_type) {
                int16_temp_value = (int16_t)fval;
                memcpy(tensorData + i * stride, &int16_temp_value, stride * sizeof(uint8_t));
            } else {
                uint16_temp_value = (uint16_t)fval;
                memcpy(tensorData + i * stride, &uint16_temp_value, stride * sizeof(uint8_t));
            }
        } else {
            printf("Do not support quant data with length of %u.\n", stride);
            goto error;
        }
    }

    if (tensorFile) {
        fclose(tensorFile);
    }
    return tensorData;
error:
    if (tensorFile) {
        fclose(tensorFile);
    }
    return NULL;
}

static uint8_t* _get_tensor_data(vsi_nn_tensor_t* tensor, const char* name)
{
    vsi_status status = VSI_FAILURE;
    vsi_size_t i = 0L;
    float fval = 0.0f;
    uint8_t* tensorData;
    vsi_size_t sz = 1L;
    vsi_size_t stride = 1L;
    FILE* tensorFile;

    tensorData = NULL;
    tensorFile = fopen(name, "rb");
    TEST_CHECK_PTR(tensorFile, error);

    sz = vsi_nn_GetElementNum(tensor);
    stride = vsi_nn_TypeGetBytes(tensor->attr.dtype.vx_type);
    tensorData = (uint8_t*)malloc(stride * sz * sizeof(uint8_t));
    TEST_CHECK_PTR(tensorData, error);
    memset(tensorData, 0, stride * sz * sizeof(uint8_t));

    for (i = 0L; i < sz; i++) {
        if (fscanf(tensorFile, "%f ", &fval) != 1) {
            printf("Read tensor file fail.\n");
            printf("Please check file lines or if the file contains illegal "
                   "characters\n");
            goto error;
        }
        status = vsi_nn_Float32ToDtype(fval, &tensorData[stride * i], &tensor->attr.dtype);
        TEST_CHECK_STATUS(status, error);
    }

    if (tensorFile) {
        fclose(tensorFile);
    }
    return tensorData;
error:
    if (tensorFile) {
        fclose(tensorFile);
    }
    return NULL;
}

static uint8_t* _get_jpeg_data(vsi_nn_tensor_t* tensor, vnn_input_meta_t* meta, const char* filename)
{
    uint32_t i;
    uint8_t* bmpData;
    uint8_t* data;
    float* fdata;
    vsi_bool use_image_process = vnn_UseImagePreprocessNode();

    bmpData = NULL;
    fdata = NULL;
    data = NULL;

    bmpData = _decode_jpeg(filename, tensor);
    TEST_CHECK_PTR(bmpData, final);

    if (use_image_process) {
        data = bmpData;
        goto final;
    }

    fdata = _imageData_to_float32(bmpData, tensor);
    TEST_CHECK_PTR(fdata, final);

    for (i = 0L; i < _cnt_of_array(meta->image.preprocess); i++) {
        switch (meta->image.preprocess[i]) {
            case VNN_PREPRO_NONE:
                break;
            case VNN_PREPRO_REORDER:
                _data_transform(fdata, meta, tensor);
                break;
            case VNN_PREPRO_MEAN:
                _data_mean(fdata, meta, tensor);
                break;
            case VNN_PREPRO_SCALE:
                _data_scale(fdata, meta, tensor);
                break;
            default:
                break;
        }
    }

    data = _float32_to_dtype(fdata, tensor);
    TEST_CHECK_PTR(data, final);
final:
    if (fdata) {
        free(fdata);
        fdata = NULL;
    }
    if (use_image_process) {
        ;
    } else {
        if (bmpData) {
            free(bmpData);
            bmpData = NULL;
        }
    }

    return data;
}

#define IMAGE_ADDR_ALIGN_START_SIZE 64
#define IMAGE_ADDR_ALIGN_BLOCK_SIZE 64

static uint8_t* buffer_img = NULL;
static uint8_t* buffer_img_align_addr = NULL;

static void _get_image_handle_buffer(
    vsi_size_t width, vsi_size_t height, vsi_size_t channels, vsi_size_t align_start_size, vsi_size_t align_block_size)
{
    vsi_size_t sz;
    uint64_t temp;

    sz = width * height * channels + align_start_size + align_block_size;
    buffer_img = (uint8_t*)malloc(sz * sizeof(uint8_t));
    memset(buffer_img, 0, sizeof(uint8_t) * sz);

    if (0 == align_start_size) {
        return;
    }
    temp = (uint64_t)(buffer_img) % align_start_size;
    if (temp == 0) {
        buffer_img_align_addr = buffer_img;
    } else {
        buffer_img_align_addr = buffer_img + align_start_size - temp;
    }
}

static void _get_jpeg_image_align(const char* name, vsi_size_t width, vsi_size_t height, vsi_size_t channels)
{
    FILE* bmpFile;

    bmpFile = NULL;
    bmpFile = fopen(name, "rb");
    TEST_CHECK_PTR(bmpFile, final);

    if (buffer_img == NULL) {
        _get_image_handle_buffer(width, height, channels, IMAGE_ADDR_ALIGN_START_SIZE, IMAGE_ADDR_ALIGN_BLOCK_SIZE);
    }

    _jpeg_to_bmp(bmpFile, buffer_img_align_addr, width, height, channels);

final:
    if (bmpFile) {
        fclose(bmpFile);
    }
}

static vsi_status _handle_multiple_inputs(vsi_nn_graph_t* graph, uint32_t idx, const char* input_file)
{
    vsi_status status;
    vsi_nn_tensor_t* tensor;
    uint8_t* data;
    vnn_input_meta_t meta;
    vsi_enum fileType;
    char dumpInput[128];

    status = VSI_FAILURE;
    data = NULL;
    tensor = NULL;
    memset(&meta, 0, sizeof(vnn_input_meta_t));
    tensor = vsi_nn_GetTensor(graph, graph->input.tensors[idx]);
    meta = input_meta_tab[idx];
    fileType = _get_file_type(input_file);
    switch (fileType) {
        case NN_FILE_JPG:
            data = _get_jpeg_data(tensor, &meta, input_file);
            TEST_CHECK_PTR(data, final);
            break;
        case NN_FILE_TENSOR:
            data = _get_tensor_data(tensor, input_file);
            TEST_CHECK_PTR(data, final);
            break;
        case NN_FILE_QTENSOR:
            data = _get_qtensor_data(tensor, input_file);
            TEST_CHECK_PTR(data, final);
            break;
        case NN_FILE_BINARY:
            data = _get_binary_data(tensor, input_file);
            TEST_CHECK_PTR(data, final);
            break;
        default:
            printf("error input file type\n");
            break;
    }

    /* Copy the Pre-processed data to input tensor */
    status = vsi_nn_CopyDataToTensor(graph, tensor, data);
    TEST_CHECK_STATUS(status, final);

    /* Save the image data to file */
    snprintf(dumpInput, sizeof(dumpInput), "input_%d.dat", idx);
    vsi_nn_SaveTensorToBinary(graph, tensor, dumpInput);

    status = VSI_SUCCESS;
final:
    if (data)
        free(data);
    return status;
}

void vnn_ReleaseBufferImage(void)
{
    if (buffer_img)
        free(buffer_img);
    buffer_img = NULL;
}

vsi_bool vnn_UseImagePreprocessNode()
{
    int32_t use_img_process;
    char* use_img_process_s;
    use_img_process = 0; /* default is 0 */
    use_img_process_s = getenv("VSI_USE_IMAGE_PROCESS");
    if (use_img_process_s) {
        use_img_process = atoi(use_img_process_s);
    }
    if (use_img_process) {
        return TRUE;
    }
    return FALSE;
}

/*
    image process pipeline:
    1.crop
    2.resize
    3.(val-mean)*scale
    4.RGBRGBRGB ---> BBBGGGRRR
    5.revert channel: BBBGGGRRR ---> RRRGGGBBB
*/
vsi_status vnn_PreProcessYolov5s_ImageProcess(vsi_nn_graph_t* graph, const char** inputs, uint32_t input_num)
{
    vsi_status status = VSI_FAILURE;
    vsi_enum fileType;
    vsi_nn_tensor_t* tensor;
    uint8_t* data = NULL;
    const char* image_name;

    if (1 != input_num) {
        return VSI_FAILURE;
    }

    _load_input_meta();
    image_name = inputs[0L];
    tensor = vsi_nn_GetTensor(graph, graph->input.tensors[0]);
    fileType = _get_file_type(image_name);
    if (fileType == NN_FILE_JPG) {
        vsi_nn_tensor_attr_t attr;
        vsi_nn_imageprocess_param imageprocess;

        _get_jpeg_size(image_name, &(attr.size[0L]), &(attr.size[1L]), &(attr.size[2L]));

        /* src image size */

        attr.size[3L] = 1L;
        attr.dim_num = 4L;

        {
            int32_t crop_start[] = { 0, 0, 0, 0 };
            int32_t crop_length[] = { attr.size[0L], attr.size[1L], 3L, 1L };
            int32_t resize_length[] = { tensor->attr.size[0L], tensor->attr.size[1L] };

            _prepare_imageprocess_params(&imageprocess, crop_start, crop_length, resize_length);
            _get_jpeg_image_align(image_name, attr.size[0L], attr.size[1L], attr.size[2L]);
            status = vsi_nn_op_imageprocess_single_node(graph, &attr, &imageprocess, buffer_img_align_addr, tensor);
        }
    } else {
        printf("vnn_PreProcessYolov5s_ImageProcess Only support JPG image file\n");
        status = VSI_FAILURE;
        TEST_CHECK_STATUS(status, final);
    }

final:
    if (data)
        free(data);
    return status;
}

vsi_status vnn_PreProcessYolov5s(vsi_nn_graph_t* graph, const char** inputs, uint32_t input_num)
{
    uint32_t i;
    vsi_status status;

    status = VSI_FAILURE;
    _load_input_meta();
    if (input_num != graph->input.num) {
        printf("Graph need %u inputs, but enter %u inputs!!!\n", graph->input.num, input_num);
        return status;
    }
    for (i = 0; i < input_num; i++) {
        status = _handle_multiple_inputs(graph, i, inputs[i]);
        TEST_CHECK_STATUS(status, final);
    }

    status = VSI_SUCCESS;
final:
    return status;
}

vsi_size_t vnn_LoadFP32DataFromTextFile(const char* fname, uint8_t** buffer_ptr, vsi_size_t* buffer_sz)
{
    float fval = 0.0;
    vsi_size_t i = 0;
    uint8_t* buffer = NULL;
    vsi_size_t item_ount = 0;
    vsi_size_t read_size = 0;
    vsi_size_t stride = sizeof(fval);
    FILE* fp = NULL;

    if (!fname || !buffer_ptr || !buffer_sz) {
        return read_size;
    }

    fp = fopen(fname, "rb");
    if (fp) {
        while (!feof(fp) && fscanf(fp, "%f ", &fval) == 1) {
            item_ount++;
        }

        if (item_ount > 0) {
            read_size = item_ount * stride;
            buffer = (uint8_t*)malloc(read_size);
            if (buffer) {
                int fail_to_read = FALSE;
                fseek(fp, 0, SEEK_SET);
                for (i = 0; i < item_ount && !fail_to_read; i++) {
                    if (fscanf(fp, "%f ", (float*)&buffer[stride * i]) != 1) {
                        printf("Read tensor file fail.\n");
                        printf("Please check file lines or if the file contains illegal "
                               "characters\n");
                        free(buffer);
                        fail_to_read = TRUE;
                        read_size = 0;
                        break;
                    }
                }
                if (!fail_to_read) {
                    *buffer_ptr = buffer;
                    *buffer_sz = read_size;
                }
            } else {
                read_size = 0;
                printf("Allocate memory fail!\n");
            }
        } else {
            printf("No available data found!\n");
        }
        fclose(fp);
    } else {
        printf("Fail to open %s\n", fname);
    }

    if (!read_size) {
        printf("Load data from %s fail!\n", fname);
    }

    return read_size;
}

vsi_size_t vnn_LoadRawDataFromBinaryFile(const char* fname, uint8_t** buffer_ptr, vsi_size_t* buffer_sz)
{
    FILE* fp = NULL;
    vsi_size_t fsize = 0;
    vsi_size_t read_size = 0;
    uint8_t* buffer = NULL;

    if (!fname || !buffer_ptr || !buffer_sz) {
        return fsize;
    }

    fp = fopen(fname, "rb");
    if (fp) {
        fsize = fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        if (fsize <= 0) {
            // error
            return NULL;
        }
        buffer = (uint8_t*)malloc(fsize);
        if (buffer) {
            fseek(fp, 0, SEEK_SET);
            read_size = fread(buffer, 1, fsize, fp);
            if (read_size == fsize) {
                *buffer_ptr = buffer;
                *buffer_sz = read_size;
            } else {
                fsize = 0;
                free(buffer);
                buffer = NULL;
            }
        } else {
            fsize = 0;
            printf("Allocate memory fail!\n");
        }

        if (fp) {
            fclose(fp);
        }
    }

    if (!fsize) {
        printf("Load data from %s fail!\n", fname);
    }
    return fsize;
}

const vsi_nn_preprocess_map_element_t* vnn_GetPreProcessMap(void)
{
    return preprocess_map;
}

uint32_t vnn_GetPreProcessMapCount()
{
    if (preprocess_map == NULL) {
        return 0;
    } else {
        return sizeof(preprocess_map) / sizeof(vsi_nn_preprocess_map_element_t);
    }
}
