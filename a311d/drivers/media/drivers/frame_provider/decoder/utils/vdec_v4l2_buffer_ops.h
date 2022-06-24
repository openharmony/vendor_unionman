/*
 * Copyright (c) 2022 Unionman Co., Ltd.
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
 */

#ifndef _AML_VDEC_V4L2_BUFFER_H_
#define _AML_VDEC_V4L2_BUFFER_H_

#include "../../../amvdec_ports/vdec_drv_base.h"
#include "../../../amvdec_ports/aml_vcodec_adapt.h"

int vdec_v4l_get_buffer(
	struct aml_vcodec_ctx *ctx,
	struct vdec_v4l2_buffer **out);

int vdec_v4l_get_pic_info(
	struct aml_vcodec_ctx *ctx,
	struct vdec_pic_info *pic);

int vdec_v4l_set_ps_infos(
	struct aml_vcodec_ctx *ctx,
	struct aml_vdec_ps_infos *ps);

int vdec_v4l_set_hdr_infos(
	struct aml_vcodec_ctx *ctx,
	struct aml_vdec_hdr_infos *hdr);

int vdec_v4l_write_frame_sync(
	struct aml_vcodec_ctx *ctx);

int vdec_v4l_post_evet(
	struct aml_vcodec_ctx *ctx,
	u32 event);

int vdec_v4l_res_ch_event(
	struct aml_vcodec_ctx *ctx);

#endif
