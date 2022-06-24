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

#include "swdemux_internal.h"

void
swdmx_cb_list_add (SWDMX_List *l, SWDMX_Ptr cb, SWDMX_Ptr data)
{
	SWDMX_CbEntry *ent;

	SWDMX_ASSERT(l && cb);

	ent = swdmx_malloc(sizeof(SWDMX_CbEntry));
	SWDMX_ASSERT(ent);

	ent->cb   = cb;
	ent->data = data;

	swdmx_list_append(l, &ent->ln);
}

void
swdmx_cb_list_remove (SWDMX_List *l, SWDMX_Ptr cb, SWDMX_Ptr data)
{
	SWDMX_CbEntry *ent, *nent;

	SWDMX_LIST_FOR_EACH_SAFE(ent, nent, l, ln) {
		if ((ent->cb == cb) && (ent->data == data)) {
			swdmx_list_remove(&ent->ln);
			swdmx_free(ent);
			break;
		}
	}
}

void
swdmx_cb_list_clear (SWDMX_List *l)
{
	SWDMX_CbEntry *ent, *nent;

	SWDMX_LIST_FOR_EACH_SAFE(ent, nent, l, ln) {
		swdmx_list_remove(&ent->ln);
		swdmx_free(ent);
	}

	swdmx_list_init(l);
}

