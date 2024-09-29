/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include "functionalext.h"

const int COUNT = 26;
const int EOK = 0;

/**
 * @tc.name      : islower_0100
 * @tc.desc      : The parameter c is a lowercase letter,which determines the case of the input character.
 * @tc.level     : Level 0
 */
void islower_0100(void)
{
    int ret = islower('a');
    EXPECT_NE("islower_0100", ret, EOK);
}

/**
 * @tc.name      : islower_0200
 * @tc.desc      : The parameter c is a uppercase letter,which determines the case of the input character.
 * @tc.level     : Level 2
 */
void islower_0200(void)
{
    int ret = islower('A');
    EXPECT_EQ("islower_0200", ret, EOK);
}

/**
 * @tc.name      : islower_0300
 * @tc.desc      : The parameter c is a number,which determines the case of the input character.
 * @tc.level     : Level 2
 */
void islower_0300(void)
{
    int ret = islower('1');
    EXPECT_EQ("islower_0300", ret, EOK);
}

/**
 * @tc.name      : islower_0400
 * @tc.desc      : The parameter c is a special character,which determines the case of the input character.
 * @tc.level     : Level 2
 */
void islower_0400(void)
{
    int ret = islower('[');
    EXPECT_EQ("islower_0400", ret, EOK);
}

/**
 * @tc.name      : islower_0500
 * @tc.desc      : Determine the number of lowercase letters in the ascii code table.
 * @tc.level     : Level 1
 */
void islower_0500(void)
{
    int total = 0;
    for (int i = 0; i < 128; i++) {
        int ret = islower((char)i);
        if (ret) {
            total++;
        }
    }
    EXPECT_EQ("islower_0500", total, COUNT);
}

int main(void)
{
    islower_0100();
    islower_0200();
    islower_0300();
    islower_0400();
    islower_0500();

    return t_status;
}