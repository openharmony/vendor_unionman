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

#include <locale.h>
#include <stdlib.h>
#include <wctype.h>
#include "functionalext.h"

const int RET_TRUE = 1;
const int RET_FALSE = 0;

/**
 * @tc.name      : iswctype_l_0100
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void iswctype_l_0100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'0';
    wctype_t wt = wctype_l("alnum", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0200
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_ALPHA,
 *                 and it is judged that the input character is a WCTYPE_ALPHA.
 * @tc.level     : Level 0
 */
void iswctype_l_0200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'a';
    wctype_t wt = wctype_l("alpha", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0300
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_BLANK,
 *                 and it is judged that the input character is a WCTYPE_BLANK.
 * @tc.level     : Level 0
 */
void iswctype_l_0300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L' ';
    wctype_t wt = wctype_l("blank", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0400
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_CNTRL,
 *                 and it is judged that the input character is a WCTYPE_CNTRL.
 * @tc.level     : Level 0
 */
void iswctype_l_0400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'\n';
    wctype_t wt = wctype_l("cntrl", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0500
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_DIGIT,
 *                 and it is judged that the input character is a WCTYPE_DIGIT.
 * @tc.level     : Level 0
 */
void iswctype_l_0500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'1';
    wctype_t wt = wctype_l("digit", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0600
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_GRAPH,
 *                 and it is judged that the input character is a WCTYPE_GRAPH.
 * @tc.level     : Level 0
 */
void iswctype_l_0600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'!';
    wctype_t wt = wctype_l("graph", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0700
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_LOWER,
 *                 and it is judged that the input character is a WCTYPE_LOWER.
 * @tc.level     : Level 0
 */
void iswctype_l_0700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'z';
    wctype_t wt = wctype_l("lower", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0800
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_PRINT,
 *                 and it is judged that the input character is a WCTYPE_PRINT.
 * @tc.level     : Level 0
 */
void iswctype_l_0800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'p';
    wctype_t wt = wctype_l("print", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_0900
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_PUNCT,
 *                 and it is judged that the input character is a WCTYPE_PUNCT.
 * @tc.level     : Level 0
 */
void iswctype_l_0900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'/';
    wctype_t wt = wctype_l("punct", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1000
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_SPACE,
 *                 and it is judged that the input character is a WCTYPE_SPACE.
 * @tc.level     : Level 0
 */
void iswctype_l_1000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'\n';
    wctype_t wt = wctype_l("space", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1100
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_UPPER,
 *                 and it is judged that the input character is a WCTYPE_UPPER.
 * @tc.level     : Level 0
 */
void iswctype_l_1100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'A';
    wctype_t wt = wctype_l("upper", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1200
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_XDIGIT,
 *                 and it is judged that the input character is a WCTYPE_XDIGIT.
 * @tc.level     : Level 0
 */
void iswctype_l_1200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'd';
    wctype_t wt = wctype_l("xdigit", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1300
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_ALNUM,
 *                 and it is judged that the input character is a WCTYPE_ALNUM.
 * @tc.level     : Level 0
 */
void iswctype_l_1300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'0';
    wctype_t wt = wctype_l("alnum", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1400
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_ALPHA,
 *                 and it is judged that the input character is a WCTYPE_ALPHA.
 * @tc.level     : Level 0
 */
void iswctype_l_1400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'a';
    wctype_t wt = wctype_l("alpha", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1500
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_BLANK,
 *                 and it is judged that the input character is a WCTYPE_BLANK.
 * @tc.level     : Level 0
 */
void iswctype_l_1500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L' ';
    wctype_t wt = wctype_l("blank", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1600
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_CNTRL,
 *                 and it is judged that the input character is a WCTYPE_CNTRL.
 * @tc.level     : Level 0
 */
void iswctype_l_1600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'\n';
    wctype_t wt = wctype_l("cntrl", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1700
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_DIGIT,
 *                 and it is judged that the input character is a WCTYPE_DIGIT.
 * @tc.level     : Level 0
 */
void iswctype_l_1700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'1';
    wctype_t wt = wctype_l("digit", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1800
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_GRAPH,
 *                 and it is judged that the input character is a WCTYPE_GRAPH.
 * @tc.level     : Level 0
 */
void iswctype_l_1800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'!';
    wctype_t wt = wctype_l("graph", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_1900
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_LOWER,
 *                 and it is judged that the input character is a WCTYPE_LOWER.
 * @tc.level     : Level 0
 */
void iswctype_l_1900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'z';
    wctype_t wt = wctype_l("lower", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_2000
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_PRINT,
 *                 and it is judged that the input character is a WCTYPE_PRINT.
 * @tc.level     : Level 0
 */
void iswctype_l_2000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'p';
    wctype_t wt = wctype_l("print", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_2100
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_PUNCT,
 *                 and it is judged that the input character is a WCTYPE_PUNCT.
 * @tc.level     : Level 0
 */
void iswctype_l_2100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'/';
    wctype_t wt = wctype_l("punct", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_2200
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_SPACE,
 *                 and it is judged that the input character is a WCTYPE_SPACE.
 * @tc.level     : Level 0
 */
void iswctype_l_2200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'\n';
    wctype_t wt = wctype_l("space", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_2300
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_UPPER,
 *                 and it is judged that the input character is a WCTYPE_UPPER.
 * @tc.level     : Level 0
 */
void iswctype_l_2300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'A';
    wctype_t wt = wctype_l("upper", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswctype_l_2400
 * @tc.desc      : Verify iswctype_l process success. The parameter c is a WCTYPE_XDIGIT,
 *                 and it is judged that the input character is a WCTYPE_XDIGIT.
 * @tc.level     : Level 0
 */
void iswctype_l_2400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    wchar_t c = L'd';
    wctype_t wt = wctype_l("xdigit", m_locale);
    EXPECT_NE(__FUNCTION__, wt, 0);
    int ret = iswctype_l(c, wt, m_locale);
    EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);

    freelocale(m_locale);
    m_locale = NULL;
}

int main(void)
{
    iswctype_l_0100();
    iswctype_l_0200();
    iswctype_l_0300();
    iswctype_l_0400();
    iswctype_l_0500();
    iswctype_l_0600();
    iswctype_l_0700();
    iswctype_l_0800();
    iswctype_l_0900();
    iswctype_l_1000();
    iswctype_l_1100();
    iswctype_l_1200();
    iswctype_l_1300();
    iswctype_l_1400();
    iswctype_l_1500();
    iswctype_l_1600();
    iswctype_l_1700();
    iswctype_l_1800();
    iswctype_l_1900();
    iswctype_l_2000();
    iswctype_l_2100();
    iswctype_l_2200();
    iswctype_l_2300();
    iswctype_l_2400();
    return t_status;
}