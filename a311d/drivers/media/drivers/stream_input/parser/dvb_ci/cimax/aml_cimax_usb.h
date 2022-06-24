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

#ifndef _AML_CIMAX_USB_H_
#define _AML_CIMAX_USB_H_

#include <linux/platform_device.h>
#include "aml_cimax.h"

int aml_cimax_usb_init(struct platform_device *pdev, struct aml_cimax *ci);
int aml_cimax_usb_exit(struct aml_cimax *ci);

#endif
