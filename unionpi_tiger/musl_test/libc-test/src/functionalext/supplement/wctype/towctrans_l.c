/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include "functionalext.h"

const int RET_TRUE = 1;
const int RET_FALSE = 0;

/**
 * @tc.name      : towctrans_l_0100
 * @tc.desc      : Verify towctrans_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void towctrans_l_0100(void)
{
    wchar_t up = L'A';
    wchar_t lp = L'a';

    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wctrans_t wt = wctrans_l("toupper", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);

    int result = towctrans_l(lp, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, (result != up) ? RET_FALSE : RET_TRUE, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towctrans_l_0200
 * @tc.desc      : Verify towctrans_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void towctrans_l_0200(void)
{
    wchar_t up = L'A';
    wchar_t lp = L'a';

    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wctrans_t wt = wctrans_l("tolower", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);

    int result = towctrans_l(up, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, (result != lp) ? RET_FALSE : RET_TRUE, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towctrans_l_0300
 * @tc.desc      : Verify towctrans_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void towctrans_l_0300(void)
{
    wchar_t up = L'A';
    wchar_t lp = L'a';

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wctrans_t wt = wctrans_l("toupper", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);

    int result = towctrans_l(lp, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, (result != up) ? RET_FALSE : RET_TRUE, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towctrans_l_0400
 * @tc.desc      : Verify towctrans_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void towctrans_l_0400(void)
{
    wchar_t up = L'A';
    wchar_t lp = L'a';

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wctrans_t wt = wctrans_l("tolower", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);

    int result = towctrans_l(up, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, (result != lp) ? RET_FALSE : RET_TRUE, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towctrans_l_0500
 * @tc.desc      : Verify towctrans_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void towctrans_l_0500(void)
{
    wchar_t up = L'A';
    wchar_t lp = L'a';

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wctrans_t wt = wctrans_l("toupper", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);

    int result = towctrans_l(lp, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, (result != up) ? RET_FALSE : RET_TRUE, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towctrans_l_0600
 * @tc.desc      : Verify towctrans_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void towctrans_l_0600(void)
{
    wchar_t up = L'A';
    wchar_t lp = L'a';

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wctrans_t wt = wctrans_l("tolower", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);

    int result = towctrans_l(up, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, (result != lp) ? RET_FALSE : RET_TRUE, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

int main(int argc, char *argv[])
{
    towctrans_l_0100();
    towctrans_l_0200();
    towctrans_l_0300();
    towctrans_l_0400();
    towctrans_l_0500();
    towctrans_l_0600();
    return t_status;
}