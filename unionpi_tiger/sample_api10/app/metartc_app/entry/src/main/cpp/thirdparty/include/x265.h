/*****************************************************************************
 * Copyright (C) 2013-2020 MulticoreWare, Inc
 *
 * Authors: Steve Borho <steve@borho.org>
 *          Min Chen <chenm003@163.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at license @ x265.com.
 *****************************************************************************/

#ifndef X265_H
#define X265_H
#include <stdint.h>
#include <stdio.h>
#include "x265_config.h"
#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER
#pragma warning(disable: 4201) // non-standard extension used (nameless struct/union)
#endif

/* x265_encoder:
 *      opaque handler for encoder */
typedef struct x265_encoder x265_encoder;

/* x265_picyuv:
 *      opaque handler for PicYuv */
typedef struct x265_picyuv x265_picyuv;

/* Application developers planning to link against a shared library version of
 * libx265 from a Microsoft Visual Studio or similar development environment
 * will need to define X265_API_IMPORTS before including this header.
 * This clause does not apply to MinGW, similar development environments, or non
 * Windows platforms. */
#ifdef X265_API_IMPORTS
#define X265_API __declspec(dllimport)
#else
#define X265_API
#endif

typedef enum
{
    NAL_UNIT_CODED_SLICE_TRAIL_N = 0,
    NAL_UNIT_CODED_SLICE_TRAIL_R,
    NAL_UNIT_CODED_SLICE_TSA_N,
    NAL_UNIT_CODED_SLICE_TLA_R,
    NAL_UNIT_CODED_SLICE_STSA_N,
    NAL_UNIT_CODED_SLICE_STSA_R,
    NAL_UNIT_CODED_SLICE_RADL_N,
    NAL_UNIT_CODED_SLICE_RADL_R,
    NAL_UNIT_CODED_SLICE_RASL_N,
    NAL_UNIT_CODED_SLICE_RASL_R,
    NAL_UNIT_CODED_SLICE_BLA_W_LP = 16,
    NAL_UNIT_CODED_SLICE_BLA_W_RADL,
    NAL_UNIT_CODED_SLICE_BLA_N_LP,
    NAL_UNIT_CODED_SLICE_IDR_W_RADL,
    NAL_UNIT_CODED_SLICE_IDR_N_LP,
    NAL_UNIT_CODED_SLICE_CRA,
    NAL_UNIT_VPS = 32,
    NAL_UNIT_SPS,
    NAL_UNIT_PPS,
    NAL_UNIT_ACCESS_UNIT_DELIMITER,
    NAL_UNIT_EOS,
    NAL_UNIT_EOB,
    NAL_UNIT_FILLER_DATA,
    NAL_UNIT_PREFIX_SEI,
    NAL_UNIT_SUFFIX_SEI,
    NAL_UNIT_UNSPECIFIED = 62,
    NAL_UNIT_INVALID = 64,
} NalUnitType;

/* The data within the payload is already NAL-encapsulated; the type is merely
 * in the struct for easy access by the calling application.  All data returned
 * in an x265_nal, including the data in payload, is no longer valid after the
 * next call to x265_encoder_encode.  Thus it must be used or copied before
 * calling x265_encoder_encode again. */
typedef struct x265_nal
{
    uint32_t type;        /* NalUnitType */
    uint32_t sizeBytes;   /* size in bytes */
    uint8_t* payload;
} x265_nal;

#define X265_LOOKAHEAD_MAX 250

typedef struct x265_lookahead_data
{
    int64_t   plannedSatd[X265_LOOKAHEAD_MAX + 1];
    uint32_t  *vbvCost;
    uint32_t  *intraVbvCost;
    uint32_t  *satdForVbv;
    uint32_t  *intraSatdForVbv;
    int       keyframe;
    int       lastMiniGopBFrame;
    int       plannedType[X265_LOOKAHEAD_MAX + 1];
    int64_t   dts;
    int64_t   reorderedPts;
} x265_lookahead_data;

typedef struct x265_analysis_validate
{
    int     maxNumReferences;
    int     analysisReuseLevel;
    int     sourceWidth;
    int     sourceHeight;
    int     keyframeMax;
    int     keyframeMin;
    int     openGOP;
    int     bframes;
    int     bPyramid;
    int     maxCUSize;
    int     minCUSize;
    int     intraRefresh;
    int     lookaheadDepth;
    int     chunkStart;
    int     chunkEnd;
    int     cuTree;
    int     ctuDistortionRefine;
    int     rightOffset;
    int     bottomOffset;
    int     frameDuplication;
}x265_analysis_validate;

/* Stores intra analysis data for a single frame. This struct needs better packing */
typedef struct x265_analysis_intra_data
{
    uint8_t*  depth;
    uint8_t*  modes;
    char*     partSizes;
    uint8_t*  chromaModes;
    int8_t*    cuQPOff;
}x265_analysis_intra_data;

typedef struct x265_analysis_MV
{
    union{
        struct { int32_t x, y; };

        int64_t word;
    };
}x265_analysis_MV;

/* Stores inter analysis data for a single frame */
typedef struct x265_analysis_inter_data
{
    int32_t*    ref;
    uint8_t*    depth;
    uint8_t*    modes;
    uint8_t*    partSize;
    uint8_t*    mergeFlag;
    uint8_t*    interDir;
    uint8_t*    mvpIdx[2];
    int8_t*     refIdx[2];
    x265_analysis_MV*         mv[2];
    int64_t*     sadCost;
    int8_t*    cuQPOff;
}x265_analysis_inter_data;

typedef struct x265_weight_param
{
    uint32_t log2WeightDenom;
    int      inputWeight;
    int      inputOffset;
    int      wtPresent;
}x265_weight_param;

#if X265_DEPTH < 10
typedef uint32_t sse_t;
#else
typedef uint64_t sse_t;
#endif

#define CTU_DISTORTION_OFF 0
#define CTU_DISTORTION_INTERNAL 1
#define CTU_DISTORTION_EXTERNAL 2

typedef struct x265_analysis_distortion_data
{
    sse_t*        ctuDistortion;
    double*       scaledDistortion;
    double        averageDistortion;
    double        sdDistortion;
    uint32_t      highDistortionCtuCount;
    uint32_t      lowDistortionCtuCount;
    double*       offset;
    double*       threshold;

}x265_analysis_distortion_data;

#define MAX_NUM_REF 16
/* Stores all analysis data for a single frame */
typedef struct x265_analysis_data
{
    int64_t                           satdCost;
    uint32_t                          frameRecordSize;
    uint32_t                          poc;
    uint32_t                          sliceType;
    uint32_t                          numCUsInFrame;
    uint32_t                          numPartitions;
    uint32_t                          depthBytes;
    int                               bScenecut;
    x265_weight_param*                wt;
    x265_analysis_inter_data*         interData;
    x265_analysis_intra_data*         intraData;
    uint32_t                          numCuInHeight;
    x265_lookahead_data               lookahead;
    uint8_t*                          modeFlag[2];
    x265_analysis_validate            saveParam;
    x265_analysis_distortion_data*    distortionData;
    uint64_t                          frameBits;
    int                               list0POC[MAX_NUM_REF];
    int                               list1POC[MAX_NUM_REF];
    double                            totalIntraPercent;
} x265_analysis_data;

/* cu statistics */
typedef struct x265_cu_stats
{
    double      percentSkipCu[4];                // Percentage of skip cu in all depths
    double      percentMergeCu[4];               // Percentage of merge cu in all depths
    double      percentIntraDistribution[4][3];  // Percentage of DC, Planar, Angular intra modes in all depths
    double      percentInterDistribution[4][3];  // Percentage of 2Nx2N inter, rect and amp in all depths
    double      percentIntraNxN;                 // Percentage of 4x4 cu

    /* All the above values will add up to 100%. */
} x265_cu_stats;


/* pu statistics */
typedef struct x265_pu_stats
{
    double      percentSkipPu[4];               // Percentage of skip cu in all depths
    double      percentIntraPu[4];              // Percentage of intra modes in all depths
    double      percentAmpPu[4];                // Percentage of amp modes in all depths
    double      percentInterPu[4][3];           // Percentage of inter 2nx2n, 2nxn and nx2n in all depths
    double      percentMergePu[4][3];           // Percentage of merge 2nx2n, 2nxn and nx2n in all depth
    double      percentNxN;

    /* All the above values will add up to 100%. */
} x265_pu_stats;

/* Frame level statistics */
typedef struct x265_frame_stats
{
    double           qp;
    double           rateFactor;
    double           psnrY;
    double           psnrU;
    double           psnrV;
    double           psnr;
    double           ssim;
    double           decideWaitTime;
    double           row0WaitTime;
    double           wallTime;
    double           refWaitWallTime;
    double           totalCTUTime;
    double           stallTime;
    double           avgWPP;
    double           avgLumaDistortion;
    double           avgChromaDistortion;
    double           avgPsyEnergy;
    double           avgResEnergy;
    double           avgLumaLevel;
    double           bufferFill;
    uint64_t         bits;
    int              encoderOrder;
    int              poc;
    int              countRowBlocks;
    int              list0POC[MAX_NUM_REF];
    int              list1POC[MAX_NUM_REF];
    uint16_t         maxLumaLevel;
    uint16_t         minLumaLevel;

    uint16_t         maxChromaULevel;
    uint16_t         minChromaULevel;
    double           avgChromaULevel;


    uint16_t         maxChromaVLevel;
    uint16_t         minChromaVLevel;
    double           avgChromaVLevel;

    char             sliceType;
    int              bScenecut;
    double           ipCostRatio;
    int              frameLatency;
    x265_cu_stats    cuStats;
    x265_pu_stats    puStats;
    double           totalFrameTime;
    double           vmafFrameScore;
    double           bufferFillFinal;
    double           unclippedBufferFillFinal;
} x265_frame_stats;

typedef struct x265_ctu_info_t
{
    int32_t ctuAddress;
    int32_t ctuPartitions[64];
    void*    ctuInfo;
} x265_ctu_info_t;

typedef enum
{
    NO_CTU_INFO = 0,
    HAS_CTU_INFO = 1,
    CTU_INFO_CHANGE = 2,
}CTUInfo;

typedef enum
{
    DEFAULT = 0,
    AVC_INFO = 1,
    HEVC_INFO = 2,
}AnalysisRefineType;

/* Arbitrary User SEI
 * Payload size is in bytes and the payload pointer must be non-NULL. 
 * Payload types and syntax can be found in Annex D of the H.265 Specification.
 * SEI Payload Alignment bits as described in Annex D must be included at the 
 * end of the payload if needed. The payload should not be NAL-encapsulated.
 * Payloads are written in the order of input */

typedef enum
{
    BUFFERING_PERIOD                     = 0,
    PICTURE_TIMING                       = 1,
    PAN_SCAN_RECT                        = 2,
    FILLER_PAYLOAD                       = 3,
    USER_DATA_REGISTERED_ITU_T_T35       = 4,
    USER_DATA_UNREGISTERED               = 5,
    RECOVERY_POINT                       = 6,
    SCENE_INFO                           = 9,
    FULL_FRAME_SNAPSHOT                  = 15,
    PROGRESSIVE_REFINEMENT_SEGMENT_START = 16,
    PROGRESSIVE_REFINEMENT_SEGMENT_END   = 17,
    FILM_GRAIN_CHARACTERISTICS           = 19,
    POST_FILTER_HINT                     = 22,
    TONE_MAPPING_INFO                    = 23,
    FRAME_PACKING                        = 45,
    DISPLAY_ORIENTATION                  = 47,
    SOP_DESCRIPTION                      = 128,
    ACTIVE_PARAMETER_SETS                = 129,
    DECODING_UNIT_INFO                   = 130,
    TEMPORAL_LEVEL0_INDEX                = 131,
    DECODED_PICTURE_HASH                 = 132,
    SCALABLE_NESTING                     = 133,
    REGION_REFRESH_INFO                  = 134,
    MASTERING_DISPLAY_INFO               = 137,
    CONTENT_LIGHT_LEVEL_INFO             = 144,
    ALTERNATIVE_TRANSFER_CHARACTERISTICS = 147,
} SEIPayloadType;

typedef struct x265_sei_payload
{
    int payloadSize;
    SEIPayloadType payloadType;
    uint8_t* payload;
} x265_sei_payload;

typedef struct x265_sei
{
    int numPayloads;
    x265_sei_payload *payloads;
} x265_sei;

typedef struct x265_dolby_vision_rpu
{
    int payloadSize;
    uint8_t* payload;
}x265_dolby_vision_rpu;

/* Used to pass pictures into the encoder, and to get picture data back out of
 * the encoder.  The input and output semantics are different */
typedef struct x265_picture
{
    /* presentation time stamp: user-specified, returned on output */
    int64_t pts;

    /* display time stamp: ignored on input, copied from reordered pts. Returned
     * on output */
    int64_t dts;

    /* force quantizer for != X265_QP_AUTO */
    /* The value provided on input is returned with the same picture (POC) on
     * output */
    void*   userData;

    /* Must be specified on input pictures, the number of planes is determined
     * by the colorSpace value */
    void*   planes[3];

    /* Stride is the number of bytes between row starts */
    int     stride[3];

    /* Must be specified on input pictures. x265_picture_init() will set it to
     * the encoder's internal bit depth, but this field must describe the depth
     * of the input pictures. Must be between 8 and 16. Values larger than 8
     * imply 16bits per input sample. If input bit depth is larger than the
     * internal bit depth, the encoder will down-shift pixels. Input samples
     * larger than 8bits will be masked to internal bit depth. On output the
     * bitDepth will be the internal encoder bit depth */
    int     bitDepth;

    /* Must be specified on input pictures: X265_TYPE_AUTO or other.
     * x265_picture_init() sets this to auto, returned on output */
    int     sliceType;

    /* Ignored on input, set to picture count, returned on output */
    int     poc;

    /* Must be specified on input pictures: X265_CSP_I420 or other. It must
     * match the internal color space of the encoder. x265_picture_init() will
     * initialize this value to the internal color space */
    int     colorSpace;

    /* Force the slice base QP for this picture within the encoder. Set to 0
     * to allow the encoder to determine base QP */
    int     forceqp;

    /* If param.analysisLoad and param.analysisSave are disabled, this field is
     * ignored on input and output. Else the user must call x265_alloc_analysis_data()
     * to allocate analysis buffers for every picture passed to the encoder.
     *
     * On input when param.analysisLoad is enabled and analysisData
     * member pointers are valid, the encoder will use the data stored here to
     * reduce encoder work.
     *
     * On output when param.analysisSave is enabled and analysisData
     * member pointers are valid, the encoder will write output analysis into
     * this data structure */
    x265_analysis_data analysisData;

    /* An array of quantizer offsets to be applied to this image during encoding.
     * These are added on top of the decisions made by rateControl.
     * Adaptive quantization must be enabled to use this feature. These quantizer
     * offsets should be given for each 16x16 block (8x8 block, when qg-size is 8).
     * Behavior if quant offsets differ between encoding passes is undefined. */
    float            *quantOffsets;

    /* Frame level statistics */
    x265_frame_stats frameData;

    /* User defined SEI */
    x265_sei         userSEI;

    /* Ratecontrol statistics for collecting the ratecontrol information.
     * It is not used for collecting the last pass ratecontrol data in 
     * multi pass ratecontrol mode. */
    void*  rcData;

    size_t framesize;

    int    height;

    // pts is reordered in the order of encoding.
    int64_t reorderedPts;

    //Dolby Vision RPU metadata
    x265_dolby_vision_rpu rpu;

    int fieldNum;

    //SEI picture structure message
    uint32_t picStruct;

    int    width;
} x265_picture;

typedef enum
{
    X265_DIA_SEARCH,
    X265_HEX_SEARCH,
    X265_UMH_SEARCH,
    X265_STAR_SEARCH,
    X265_SEA,
    X265_FULL_SEARCH
} X265_ME_METHODS;

/* CPU flags */

/* x86 */
#define X265_CPU_MMX             (1 << 0)
#define X265_CPU_MMX2            (1 << 1)  /* MMX2 aka MMXEXT aka ISSE */
#define X265_CPU_MMXEXT          X265_CPU_MMX2
#define X265_CPU_SSE             (1 << 2)
#define X265_CPU_SSE2            (1 << 3)
#define X265_CPU_LZCNT           (1 << 4)
#define X265_CPU_SSE3            (1 << 5)
#define X265_CPU_SSSE3           (1 << 6)
#define X265_CPU_SSE4            (1 << 7)  /* SSE4.1 */
#define X265_CPU_SSE42           (1 << 8)  /* SSE4.2 */
#define X265_CPU_AVX             (1 << 9)  /* Requires OS support even if YMM registers aren't used. */
#define X265_CPU_XOP             (1 << 10)  /* AMD XOP */
#define X265_CPU_FMA4            (1 << 11)  /* AMD FMA4 */
#define X265_CPU_FMA3            (1 << 12)  /* Intel FMA3 */
#define X265_CPU_BMI1            (1 << 13)  /* BMI1 */
#define X265_CPU_BMI2            (1 << 14)  /* BMI2 */
#define X265_CPU_AVX2            (1 << 15)  /* AVX2 */
#define X265_CPU_AVX512          (1 << 16)  /* AVX-512 {F, CD, BW, DQ, VL}, requires OS support */
/* x86 modifiers */
#define X265_CPU_CACHELINE_32    (1 << 17)  /* avoid memory loads that span the border between two cachelines */
#define X265_CPU_CACHELINE_64    (1 << 18)  /* 32/64 is the size of a cacheline in bytes */
#define X265_CPU_SSE2_IS_SLOW    (1 << 19)  /* avoid most SSE2 functions on Athlon64 */
#define X265_CPU_SSE2_IS_FAST    (1 << 20)  /* a few functions are only faster on Core2 and Phenom */
#define X265_CPU_SLOW_SHUFFLE    (1 << 21)  /* The Conroe has a slow shuffle unit (relative to overall SSE performance) */
#define X265_CPU_STACK_MOD4      (1 << 22)  /* if stack is only mod4 and not mod16 */
#define X265_CPU_SLOW_ATOM       (1 << 23)  /* The Atom is terrible: slow SSE unaligned loads, slow
                                             * SIMD multiplies, slow SIMD variable shifts, slow pshufb,
                                             * cacheline split penalties -- gather everything here that
                                             * isn't shared by other CPUs to avoid making half a dozen
                                             * new SLOW flags. */
#define X265_CPU_SLOW_PSHUFB     (1 << 24)  /* such as on the Intel Atom */
#define X265_CPU_SLOW_PALIGNR    (1 << 25)  /* such as on the AMD Bobcat */

/* ARM */
#define X265_CPU_ARMV6           0x0000001
#define X265_CPU_NEON            0x0000002  /* ARM NEON */
#define X265_CPU_FAST_NEON_MRC   0x0000004  /* Transfer from NEON to ARM register is fast (Cortex-A9) */

/* IBM Power8 */
#define X265_CPU_ALTIVEC         0x0000001

#define X265_MAX_SUBPEL_LEVEL   7

/* Log level */
#define X265_LOG_NONE          (-1)
#define X265_LOG_ERROR          0
#define X265_LOG_WARNING        1
#define X265_LOG_INFO           2
#define X265_LOG_DEBUG          3
#define X265_LOG_FULL           4

#define X265_B_ADAPT_NONE       0
#define X265_B_ADAPT_FAST       1
#define X265_B_ADAPT_TRELLIS    2

#define X265_REF_LIMIT_DEPTH    1
#define X265_REF_LIMIT_CU       2

#define X265_TU_LIMIT_BFS       1
#define X265_TU_LIMIT_DFS       2
#define X265_TU_LIMIT_NEIGH     4

#define X265_BFRAME_MAX         16
#define X265_MAX_FRAME_THREADS  16

#define X265_TYPE_AUTO          0x0000  /* Let x265 choose the right type */
#define X265_TYPE_IDR           0x0001
#define X265_TYPE_I             0x0002
#define X265_TYPE_P             0x0003
#define X265_TYPE_BREF          0x0004  /* Non-disposable B-frame */
#define X265_TYPE_B             0x0005
#define IS_X265_TYPE_I(x) ((x) == X265_TYPE_I || (x) == X265_TYPE_IDR)
#define IS_X265_TYPE_B(x) ((x) == X265_TYPE_B || (x) == X265_TYPE_BREF)

#define X265_QP_AUTO                 0

#define X265_AQ_NONE                 0
#define X265_AQ_VARIANCE             1
#define X265_AQ_AUTO_VARIANCE        2
#define X265_AQ_AUTO_VARIANCE_BIASED 3
#define X265_AQ_EDGE                 4
#define x265_ADAPT_RD_STRENGTH   4
#define X265_REFINE_INTER_LEVELS 3
/* NOTE! For this release only X265_CSP_I420 and X265_CSP_I444 are supported */
/* Supported internal color space types (according to semantics of chroma_format_idc) */
#define X265_CSP_I400           0  /* yuv 4:0:0 planar */
#define X265_CSP_I420           1  /* yuv 4:2:0 planar */
#define X265_CSP_I422           2  /* yuv 4:2:2 planar */
#define X265_CSP_I444           3  /* yuv 4:4:4 planar */
#define X265_CSP_COUNT          4  /* Number of supported internal color spaces */

/* These color spaces will eventually be supported as input pictures. The pictures will
 * be converted to the appropriate planar color spaces at ingest */
#define X265_CSP_NV12           4  /* yuv 4:2:0, with one y plane and one packed u+v */
#define X265_CSP_NV16           5  /* yuv 4:2:2, with one y plane and one packed u+v */

/* Interleaved color-spaces may eventually be supported as input pictures */
#define X265_CSP_BGR            6  /* packed bgr 24bits   */
#define X265_CSP_BGRA           7  /* packed bgr 32bits   */
#define X265_CSP_RGB            8  /* packed rgb 24bits   */
#define X265_CSP_MAX            9  /* end of list */
#define X265_EXTENDED_SAR       255 /* aspect ratio explicitly specified as width:height */
/* Analysis options */
#define X265_ANALYSIS_OFF  0
#define X265_ANALYSIS_SAVE 1
#define X265_ANALYSIS_LOAD 2

typedef struct x265_cli_csp
{
    int planes;
    int width[3];
    int height[3];
} x265_cli_csp;

static const x265_cli_csp x265_cli_csps[] =
{
    { 1, { 0, 0, 0 }, { 0, 0, 0 } }, /* i400 */
    { 3, { 0, 1, 1 }, { 0, 1, 1 } }, /* i420 */
    { 3, { 0, 1, 1 }, { 0, 0, 0 } }, /* i422 */
    { 3, { 0, 0, 0 }, { 0, 0, 0 } }, /* i444 */
    { 2, { 0, 0 },    { 0, 1 } },    /* nv12 */
    { 2, { 0, 0 },    { 0, 0 } },    /* nv16 */
};

/* rate tolerance method */
typedef enum
{
    X265_RC_ABR,
    X265_RC_CQP,
    X265_RC_CRF
} X265_RC_METHODS;

/* slice type statistics */
typedef struct x265_sliceType_stats
{
    double        avgQp;
    double        bitrate;
    double        psnrY;
    double        psnrU;
    double        psnrV;
    double        ssim;
    uint32_t      numPics;
} x265_sliceType_stats;

/* Output statistics from encoder */
typedef struct x265_stats
{
    double                globalPsnrY;
    double                globalPsnrU;
    double                globalPsnrV;
    double                globalPsnr;
    double                globalSsim;
    double                elapsedEncodeTime;    /* wall time since encoder was opened */
    double                elapsedVideoTime;     /* encoded picture count / frame rate */
    double                bitrate;              /* accBits / elapsed video time */
    double                aggregateVmafScore;   /* aggregate VMAF score for input video*/
    uint64_t              accBits;              /* total bits output thus far */
    uint32_t              encodedPictureCount;  /* number of output pictures thus far */
    uint32_t              totalWPFrames;        /* number of uni-directional weighted frames used */
    x265_sliceType_stats  statsI;               /* statistics of I slice */
    x265_sliceType_stats  statsP;               /* statistics of P slice */
    x265_sliceType_stats  statsB;               /* statistics of B slice */
    uint16_t              maxCLL;               /* maximum content light level */
    uint16_t              maxFALL;              /* maximum frame average light level */
} x265_stats;

/* String values accepted by x265_param_parse() (and CLI) for various parameters */
static const char * const x265_motion_est_names[] = { "dia", "hex", "umh", "star", "sea", "full", 0 };
static const char * const x265_source_csp_names[] = { "i400", "i420", "i422", "i444", "nv12", "nv16", 0 };
static const char * const x265_video_format_names[] = { "component", "pal", "ntsc", "secam", "mac", "unknown", 0 };
static const char * const x265_fullrange_names[] = { "limited", "full", 0 };
static const char * const x265_colorprim_names[] = { "reserved", "bt709", "unknown", "reserved", "bt470m", "bt470bg", "smpte170m", "smpte240m", "film", "bt2020", "smpte428", "smpte431", "smpte432", 0 };
static const char * const x265_transfer_names[] = { "reserved", "bt709", "unknown", "reserved", "bt470m", "bt470bg", "smpte170m", "smpte240m", "linear", "log100",
                                                    "log316", "iec61966-2-4", "bt1361e", "iec61966-2-1", "bt2020-10", "bt2020-12",
                                                    "smpte2084", "smpte428", "arib-std-b67", 0 };
static const char * const x265_colmatrix_names[] = { "gbr", "bt709", "unknown", "", "fcc", "bt470bg", "smpte170m", "smpte240m",
                                                     "ycgco", "bt2020nc", "bt2020c", "smpte2085", "chroma-derived-nc", "chroma-derived-c", "ictcp", 0 };
static const char * const x265_sar_names[] = { "unknown", "1:1", "12:11", "10:11", "16:11", "40:33", "24:11", "20:11",
                                               "32:11", "80:33", "18:11", "15:11", "64:33", "160:99", "4:3", "3:2", "2:1", 0 };
static const char * const x265_interlace_names[] = { "prog", "tff", "bff", 0 };
static const char * const x265_analysis_names[] = { "off", "save", "load", 0 };

struct x265_zone;
struct x265_param;
/* Zones: override ratecontrol for specific sections of the video.
 * If zones overlap, whichever comes later in the list takes precedence. */
typedef struct x265_zone
{
    int   startFrame, endFrame; /* range of frame numbers */
    int   bForceQp;             /* whether to use qp vs bitrate factor */
    int   qp;
    float bitrateFactor;
    struct x265_param* zoneParam;
    double* relativeComplexity;
} x265_zone;
    
/* data to calculate aggregate VMAF score */
typedef struct x265_vmaf_data
{
    int width;
    int height;
    size_t offset; 
    int internalBitDepth;
    FILE *reference_file; /* FILE pointer for input file */
    FILE *distorted_file; /* FILE pointer for recon file generated*/
}x265_vmaf_data;

/* data to calculate frame level VMAF score */
typedef struct x265_vmaf_framedata
{
    int width;
    int height;
    int frame_set; 
    int internalBitDepth; 
    void *reference_frame; /* points to fenc of particular frame */
    void *distorted_frame; /* points to recon of particular frame */
}x265_vmaf_framedata;

/* common data needed to calculate both frame level and video level VMAF scores */
typedef struct x265_vmaf_commondata
{
    char *format;
    char *model_path;
    char *log_path;
    char *log_fmt;
    int disable_clip;
    int disable_avx;
    int enable_transform;
    int phone_model;
    int psnr;
    int ssim;
    int ms_ssim;
    char *pool;
    int thread;
    int subsample;
    int enable_conf_interval;
}x265_vmaf_commondata;

static const x265_vmaf_commondata vcd[] = { { NULL, (char *)"/usr/local/share/model/vmaf_v0.6.1.pkl", NULL, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 1, 0 } };

/* x265 input parameters
 *
 * For version safety you may use x265_param_alloc/free() to manage the
 * allocation of x265_param instances, and x265_param_parse() to assign values
 * by name.  By never dereferencing param fields in your own code you can treat
 * x265_param as an opaque data structure */
typedef struct x265_param
{
    /* x265_param_default() will auto-detect this cpu capability bitmap.  it is
     * recommended to not change this value unless you know the cpu detection is
     * somehow flawed on your target hardware. The asm function tables are
     * process global, the first encoder configures them for all encoders */
    int       cpuid;
    /*== Parallelism Features ==*/

    /* Number of concurrently encoded frames between 1 and X265_MAX_FRAME_THREADS
     * or 0 for auto-detection. By default x265 will use a number of frame
     * threads empirically determined to be optimal for your CPU core count,
     * between 2 and 6.  Using more than one frame thread causes motion search
     * in the down direction to be clamped but otherwise encode behavior is
     * unaffected. With CQP rate control the output bitstream is deterministic
     * for all values of frameNumThreads greater than 1. All other forms of
     * rate-control can be negatively impacted by increases to the number of
     * frame threads because the extra concurrency adds uncertainty to the
     * bitrate estimations. Frame parallelism is generally limited by the the
     * is generally limited by the the number of CU rows
     *
     * When thread pools are used, each frame thread is assigned to a single
     * pool and the frame thread itself is given the node affinity of its pool.
     * But when no thread pools are used no node affinity is assigned. */
    int       frameNumThreads;

    /* Comma seperated list of threads per NUMA node. If "none", then no worker
     * pools are created and only frame parallelism is possible. If NULL or ""
     * (default) x265 will use all available threads on each NUMA node.
     *
     * '+'  is a special value indicating all cores detected on the node
     * '*'  is a special value indicating all cores detected on the node and all
     *      remaining nodes.
     * '-'  is a special value indicating no cores on the node, same as '0'
     *
     * example strings for a 4-node system:
     *   ""        - default, unspecified, all numa nodes are used for thread pools
     *   "*"       - same as default
     *   "none"    - no thread pools are created, only frame parallelism possible
     *   "-"       - same as "none"
     *   "10"      - allocate one pool, using up to 10 cores on node 0
     *   "-,+"     - allocate one pool, using all cores on node 1
     *   "+,-,+"   - allocate two pools, using all cores on nodes 0 and 2
     *   "+,-,+,-" - allocate two pools, using all cores on nodes 0 and 2
     *   "-,*"     - allocate three pools, using all cores on nodes 1, 2 and 3
     *   "8,8,8,8" - allocate four pools with up to 8 threads in each pool
     *
     * The total number of threads will be determined by the number of threads
     * assigned to all nodes. The worker threads will each be given affinity for
     * their node, they will not be allowed to migrate between nodes, but they
     * will be allowed to move between CPU cores within their node.
     *
     * If the three pool features: bEnableWavefront, bDistributeModeAnalysis and
     * bDistributeMotionEstimation are all disabled, then numaPools is ignored
     * and no thread pools are created.
     *
     * If "none" is specified, then all three of the thread pool features are
     * implicitly disabled.
     *
     * Multiple thread pools will be allocated for any NUMA node with more than
     * 64 logical CPU cores. But any given thread pool will always use at most
     * one NUMA node.
     *
     * Frame encoders are distributed between the available thread pools, and
     * the encoder will never generate more thread pools than frameNumThreads */
    const char* numaPools;

    /* Enable wavefront parallel processing, greatly increases parallelism for
     * less than 1% compression efficiency loss. Requires a thread pool, enabled
     * by default */
    int       bEnableWavefront;

    /* Use multiple threads to measure CU mode costs. Recommended for many core
     * CPUs. On RD levels less than 5, it may not offload enough work to warrant
     * the overhead. It is useful with the slow preset since it has the
     * rectangular predictions enabled. At RD level 5 and 6 (preset slower and
     * below), this feature should be an unambiguous win if you have CPU
     * cores available for work. Default disabled */
    int       bDistributeModeAnalysis;

    /* Use multiple threads to perform motion estimation to (ME to one reference
     * per thread). Recommended for many core CPUs. The more references the more
     * motion searches there will be to distribute. This option is often not a
     * win, particularly in video sequences with low motion. Default disabled */
    int       bDistributeMotionEstimation;

    /*== Logging Features ==*/

    /* Enable analysis and logging distribution of CUs. Now deprecated */
    int       bLogCuStats;

    /* Enable the measurement and reporting of PSNR. Default is enabled */
    int       bEnablePsnr;

    /* Enable the measurement and reporting of SSIM. Default is disabled */
    int       bEnableSsim;

    /* The level of logging detail emitted by the encoder. X265_LOG_NONE to
     * X265_LOG_FULL, default is X265_LOG_INFO */
    int       logLevel;

    /* Level of csv logging. 0 is summary, 1 is frame level logging,
     * 2 is frame level logging with performance statistics */
    int       csvLogLevel;

    /* filename of CSV log. If csvLogLevel is non-zero, the encoder will emit
     * per-slice statistics to this log file in encode order. Otherwise the
     * encoder will emit per-stream statistics into the log file when
     * x265_encoder_log is called (presumably at the end of the encode) */
    const char* csvfn;

    /*== Internal Picture Specification ==*/

    /* Internal encoder bit depth. If x265 was compiled to use 8bit pixels
     * (HIGH_BIT_DEPTH=0), this field must be 8, else this field must be 10.
     * Future builds may support 12bit pixels. */
    int       internalBitDepth;

    /* Color space of internal pictures, must match color space of input
     * pictures */
    int       internalCsp;

    /* Numerator and denominator of frame rate */
    uint32_t  fpsNum;
    uint32_t  fpsDenom;

    /* Width (in pixels) of the source pictures. If this width is not an even
     * multiple of 4, the encoder will pad the pictures internally to meet this
     * minimum requirement. All valid HEVC widths are supported */
    int       sourceWidth;

    /* Height (in pixels) of the source pictures. If this height is not an even
     * multiple of 4, the encoder will pad the pictures internally to meet this
     * minimum requirement. All valid HEVC heights are supported */
    int       sourceHeight;

    /* Interlace type of source pictures. 0 - progressive pictures (default).
     * 1 - top field first, 2 - bottom field first. HEVC encodes interlaced
     * content as fields, they must be provided to the encoder in the correct
     * temporal order */
    int       interlaceMode;

    /* Total Number of frames to be encoded, calculated from the user input
     * (--frames) and (--seek). In case, the input is read from a pipe, this can
     * remain as 0. It is later used in 2 pass RateControl, hence storing the
     * value in param */
    int       totalFrames;

    /*== Profile / Tier / Level ==*/

    /* Note: the profile is specified by x265_param_apply_profile() */

    /* Minimum decoder requirement level. Defaults to 0, which implies auto-
     * detection by the encoder. If specified, the encoder will attempt to bring
     * the encode specifications within that specified level. If the encoder is
     * unable to reach the level it issues a warning and emits the actual
     * decoder requirement. If the requested requirement level is higher than
     * the actual level, the actual requirement level is signaled. The value is
     * an specified as an integer with the level times 10, for example level
     * "5.1" is specified as 51, and level "5.0" is specified as 50. */
    int       levelIdc;

    /* if levelIdc is specified (non-zero) this flag will differentiate between
     * Main (0) and High (1) tier. Default is Main tier (0) */
    int       bHighTier;

    /* Enable UHD Blu-ray compatibility support. If specified, the encoder will
     * attempt to modify/set the encode specifications. If the encoder is unable 
     * to do so, this option will be turned OFF. */
    int       uhdBluray;

    /* The maximum number of L0 references a P or B slice may use. This
     * influences the size of the decoded picture buffer. The higher this
     * number, the more reference frames there will be available for motion
     * search, improving compression efficiency of most video at a cost of
     * performance. Value must be between 1 and 16, default is 3 */
    int       maxNumReferences;

    /* Allow libx265 to emit HEVC bitstreams which do not meet strict level
     * requirements. Defaults to false */
    int       bAllowNonConformance;

    /*== Bitstream Options ==*/

    /* Flag indicating whether VPS, SPS and PPS headers should be output with
     * each keyframe. Default false */
    int       bRepeatHeaders;

    /* Flag indicating whether the encoder should generate start codes (Annex B
     * format) or length (file format) before NAL units. Default true, Annex B.
     * Muxers should set this to the correct value */
    int       bAnnexB;

    /* Flag indicating whether the encoder should emit an Access Unit Delimiter
     * NAL at the start of every access unit. Default false */
    int       bEnableAccessUnitDelimiters;

    /* Enables the buffering period SEI and picture timing SEI to signal the HRD
     * parameters. Default is disabled */
    int       bEmitHRDSEI;

    /* Enables the emission of a user data SEI with the stream headers which
     * describes the encoder version, build info, and parameters. This is
     * very helpful for debugging, but may interfere with regression tests.
     * Default enabled */
    int       bEmitInfoSEI;

    /* Enable the generation of SEI messages for each encoded frame containing
     * the hashes of the three reconstructed picture planes. Most decoders will
     * validate those hashes against the reconstructed images it generates and
     * report any mismatches. This is essentially a debugging feature.  Hash
     * types are MD5(1), CRC(2), Checksum(3).  Default is 0, none */
    int       decodedPictureHashSEI;

    /* Enable Temporal Sub Layers while encoding, signals NAL units of coded
     * slices with their temporalId. Output bitstreams can be extracted either
     * at the base temporal layer (layer 0) with roughly half the frame rate or
     * at a higher temporal layer (layer 1) that decodes all the frames in the
     * sequence. */
    int       bEnableTemporalSubLayers;

    /*== GOP structure and slice type decisions (lookahead) ==*/

    /* Enable open GOP - meaning I slices are not necessarily IDR and thus frames
     * encoded after an I slice may reference frames encoded prior to the I
     * frame which have remained in the decoded picture buffer.  Open GOP
     * generally has better compression efficiency and negligible encoder
     * performance impact, but the use case may preclude it.  Default true */
    int       bOpenGOP;

    /* Scene cuts closer together than this are coded as I, not IDR. */
    int       keyframeMin;

    /* Maximum keyframe distance or intra period in number of frames. If 0 or 1,
     * all frames are I frames. A negative value is casted to MAX_INT internally
     * which effectively makes frame 0 the only I frame. Default is 250 */
    int       keyframeMax;

    /* Maximum consecutive B frames that can be emitted by the lookahead. When
     * b-adapt is 0 and keyframMax is greater than bframes, the lookahead emits
     * a fixed pattern of `bframes` B frames between each P.  With b-adapt 1 the
     * lookahead ignores the value of bframes for the most part.  With b-adapt 2
     * the value of bframes determines the search (POC) distance performed in
     * both directions, quadratically increasing the compute load of the
     * lookahead.  The higher the value, the more B frames the lookahead may
     * possibly use consecutively, usually improving compression. Default is 3,
     * maximum is 16 */
    int       bframes;

    /* Sets the operating mode of the lookahead.  With b-adapt 0, the GOP
     * structure is fixed based on the values of keyframeMax and bframes.
     * With b-adapt 1 a light lookahead is used to chose B frame placement.
     * With b-adapt 2 (trellis) a viterbi B path selection is performed */
    int       bFrameAdaptive;

    /* When enabled, the encoder will use the B frame in the middle of each
     * mini-GOP larger than 2 B frames as a motion reference for the surrounding
     * B frames.  This improves compression efficiency for a small performance
     * penalty.  Referenced B frames are treated somewhere between a B and a P
     * frame by rate control.  Default is enabled. */
    int       bBPyramid;

    /* A value which is added to the cost estimate of B frames in the lookahead.
     * It may be a positive value (making B frames appear less expensive, which
     * biases the lookahead to choose more B frames) or negative, which makes the
     * lookahead choose more P frames. Default is 0, there are no limits */
    int       bFrameBias;

    /* The number of frames that must be queued in the lookahead before it may
     * make slice decisions. Increasing this value directly increases the encode
     * latency. The longer the queue the more optimally the lookahead may make
     * slice decisions, particularly with b-adapt 2. When cu-tree is enabled,
     * the length of the queue linearly increases the effectiveness of the
     * cu-tree analysis. Default is 40 frames, maximum is 250 */
    int       lookaheadDepth;

    /* Use multiple worker threads to measure the estimated cost of each frame
     * within the lookahead. When bFrameAdaptive is 2, most frame cost estimates
     * will be performed in batch mode, many cost estimates at the same time,
     * and lookaheadSlices is ignored for batched estimates. The effect on
     * performance can be quite small.  The higher this parameter, the less
     * accurate the frame costs will be (since context is lost across slice
     * boundaries) which will result in less accurate B-frame and scene-cut
     * decisions. Default is 0 - disabled. 1 is the same as 0. Max 16 */
    int       lookaheadSlices;

    /* An arbitrary threshold which determines how aggressively the lookahead
     * should detect scene cuts for cost based scenecut detection. 
     * The default (40) is recommended. */
    int       scenecutThreshold;

    /* Replace keyframes by using a column of intra blocks that move across the video
     * from one side to the other, thereby "refreshing" the image. In effect, instead of a
     * big keyframe, the keyframe is "spread" over many frames. */
    int       bIntraRefresh;

    /*== Coding Unit (CU) definitions ==*/

    /* Maximum CU width and height in pixels.  The size must be 64, 32, or 16.
     * The higher the size, the more efficiently x265 can encode areas of low
     * complexity, greatly improving compression efficiency at large
     * resolutions.  The smaller the size, the more effective wavefront and
     * frame parallelism will become because of the increase in rows. default 64
     * All encoders within the same process must use the same maxCUSize, until
     * all encoders are closed and x265_cleanup() is called to reset the value. */
    uint32_t  maxCUSize;

    /* Minimum CU width and height in pixels.  The size must be 64, 32, 16, or
     * 8. Default 8. All encoders within the same process must use the same
     * minCUSize. */
    uint32_t  minCUSize;

    /* Enable rectangular motion prediction partitions (vertical and
     * horizontal), available at all CU depths from 64x64 to 8x8. Default is
     * disabled */
    int       bEnableRectInter;

    /* Enable asymmetrical motion predictions.  At CU depths 64, 32, and 16, it
     * is possible to use 25%/75% split partitions in the up, down, right, left
     * directions. For some material this can improve compression efficiency at
     * the cost of extra analysis. bEnableRectInter must be enabled for this
     * feature to be used. Default disabled */
    int       bEnableAMP;

    /*== Residual Quadtree Transform Unit (TU) definitions ==*/

    /* Maximum TU width and height in pixels.  The size must be 32, 16, 8 or 4.
     * The larger the size the more efficiently the residual can be compressed
     * by the DCT transforms, at the expense of more computation */
    uint32_t  maxTUSize;

    /* The additional depth the residual quad-tree is allowed to recurse beyond
     * the coding quad-tree, for inter coded blocks. This must be between 1 and
     * 4. The higher the value the more efficiently the residual can be
     * compressed by the DCT transforms, at the expense of much more compute */
    uint32_t  tuQTMaxInterDepth;

    /* The additional depth the residual quad-tree is allowed to recurse beyond
     * the coding quad-tree, for intra coded blocks. This must be between 1 and
     * 4. The higher the value the more efficiently the residual can be
     * compressed by the DCT transforms, at the expense of much more compute */
    uint32_t  tuQTMaxIntraDepth;

    /* Enable early exit decisions for inter coded blocks to avoid recursing to
     * higher TU depths. Default: 0 */
    uint32_t  limitTU;

    /* Set the amount of rate-distortion analysis to use within quant. 0 implies
     * no rate-distortion optimization. At level 1 rate-distortion cost is used to
     * find optimal rounding values for each level (and allows psy-rdoq to be
     * enabled). At level 2 rate-distortion cost is used to make decimate decisions
     * on each 4x4 coding group (including the cost of signaling the group within
     * the group bitmap).  Psy-rdoq is less effective at preserving energy when
     * RDOQ is at level 2. Default: 0 */
    int       rdoqLevel;

    /* Enable the implicit signaling of the sign bit of the last coefficient of
     * each transform unit. This saves one bit per TU at the expense of figuring
     * out which coefficient can be toggled with the least distortion.
     * Default is enabled */
    int       bEnableSignHiding;

    /* Allow intra coded blocks to be encoded directly as residual without the
     * DCT transform, when this improves efficiency. Checking whether the block
     * will benefit from this option incurs a performance penalty. Default is
     * disabled */
    int       bEnableTransformSkip;

    /* An integer value in range of 0 to 2000, which denotes strength of noise
     * reduction in intra CUs. 0 means disabled */
    int       noiseReductionIntra;

    /* An integer value in range of 0 to 2000, which denotes strength of noise
     * reduction in inter CUs. 0 means disabled */
    int       noiseReductionInter;

    /* Quantization scaling lists. HEVC supports 6 quantization scaling lists to
     * be defined; one each for Y, Cb, Cr for intra prediction and one each for
     * inter prediction.
     *
     * - NULL and "off" will disable quant scaling (default)
     * - "default" will enable the HEVC default scaling lists, which
     *   do not need to be signaled since they are specified
     * - all other strings indicate a filename containing custom scaling lists
     *   in the HM format. The encode will fail if the file is not parsed
     *   correctly. Custom lists must be signaled in the SPS. */
    const char *scalingLists;

    /*== Intra Coding Tools ==*/

    /* Enable constrained intra prediction. This causes intra prediction to
     * input samples that were inter predicted. For some use cases this is
     * believed to me more robust to stream errors, but it has a compression
     * penalty on P and (particularly) B slices. Defaults to disabled */
    int       bEnableConstrainedIntra;

    /* Enable strong intra smoothing for 32x32 blocks where the reference
     * samples are flat. It may or may not improve compression efficiency,
     * depending on your source material. Defaults to disabled */
    int       bEnableStrongIntraSmoothing;

    /*== Inter Coding Tools ==*/

    /* The maximum number of merge candidates that are considered during inter
     * analysis.  This number (between 1 and 5) is signaled in the stream
     * headers and determines the number of bits required to signal a merge so
     * it can have significant trade-offs. The smaller this number the higher
     * the performance but the less compression efficiency. Default is 3 */
    uint32_t  maxNumMergeCand;

    /* Limit the motion references used for each search based on the results of
     * previous motion searches already performed for the same CU: If 0 all
     * references are always searched. If X265_REF_LIMIT_CU all motion searches
     * will restrict themselves to the references selected by the 2Nx2N search
     * at the same depth. If X265_REF_LIMIT_DEPTH the 2Nx2N motion search will
     * only use references that were selected by the best motion searches of the
     * 4 split CUs at the next lower CU depth.  The two flags may be combined */
    uint32_t  limitReferences;

    /* Limit modes analyzed for each CU using cost metrics from the 4 sub-CUs */
    uint32_t limitModes;

    /* ME search method (DIA, HEX, UMH, STAR, SEA, FULL). The search patterns
     * (methods) are sorted in increasing complexity, with diamond being the
     * simplest and fastest and full being the slowest.  DIA, HEX, UMH and SEA were
     * adapted from x264 directly. STAR is an adaption of the HEVC reference
     * encoder's three step search, while full is a naive exhaustive search. The
     * default is the star search, it has a good balance of performance and
     * compression efficiency */
    int       searchMethod;

    /* A value between 0 and X265_MAX_SUBPEL_LEVEL which adjusts the amount of
     * effort performed during sub-pel refine. Default is 5 */
    int       subpelRefine;

    /* The maximum distance from the motion prediction that the full pel motion
     * search is allowed to progress before terminating. This value can have an
     * effect on frame parallelism, as referenced frames must be at least this
     * many rows of reconstructed pixels ahead of the referencee at all times.
     * (When considering reference lag, the motion prediction must be ignored
     * because it cannot be known ahead of time).  Default is 60, which is the
     * default max CU size (64) minus the luma HPEL half-filter length (4). If a
     * smaller CU size is used, the search range should be similarly reduced */
    int       searchRange;

    /* Enable availability of temporal motion vector for AMVP, default is enabled */
    int       bEnableTemporalMvp;

    /* Enable 3-level Hierarchical motion estimation at One-Sixteenth, Quarter and Full resolution.
     * Default is disabled */
    int       bEnableHME;

    /* Enable HME search method (DIA, HEX, UMH, STAR, SEA, FULL) for level 0, 1 and 2.
     * Default is hex, umh, umh for L0, L1 and L2 respectively. */
    int       hmeSearchMethod[3];

    /* Enable weighted prediction in P slices.  This enables weighting analysis
     * in the lookahead, which influences slice decisions, and enables weighting
     * analysis in the main encoder which allows P reference samples to have a
     * weight function applied to them prior to using them for motion
     * compensation.  In video which has lighting changes, it can give a large
     * improvement in compression efficiency. Default is enabled */
    int       bEnableWeightedPred;


#ifdef __cplusplus
}
#endif

#endif // X265_H
