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

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include "functionalext.h"

const int RET_TRUE = 1;
const int RET_FALSE = 0;

/**
 * @tc.name      : wctype_l_0100
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are alnum and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("alnum", m_locale);
    wctype_t target = 1;
    if (expect0 != target) {
        t_error("%s expect type is not alnum", __func__);
    }

    wctype_t expect1 = wctype_l("alnumm", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is alnum", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0200
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are alpha and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("alpha", m_locale);
    wctype_t target = 2;
    if (expect0 != target) {
        t_error("%s expect type is not alpha", __func__);
    }

    wctype_t expect1 = wctype_l("alphaa", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is alpha", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0300
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are blank and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("blank", m_locale);
    wctype_t target = 3;
    if (expect0 != target) {
        t_error("%s expect type is not blank", __func__);
    }

    wctype_t expect1 = wctype_l("blankk", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is blank", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0400
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are cntrl and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("cntrl", m_locale);
    wctype_t target = 4;
    if (expect0 != target) {
        t_error("%s expect type is not cntrl", __func__);
    }

    wctype_t expect1 = wctype_l("cntrll", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is cntrl", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0500
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are digit and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("digit", m_locale);
    wctype_t target = 5;
    if (expect0 != target) {
        t_error("%s expect type is not digit", __func__);
    }

    wctype_t expect1 = wctype_l("digitt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is digit", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0600
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are graph and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("graph", m_locale);
    wctype_t target = 6;
    if (expect0 != target) {
        t_error("%s expect type is not graph", __func__);
    }

    wctype_t expect1 = wctype_l("graphh", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is graph", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0700
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are lower and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("lower", m_locale);
    wctype_t target = 7;
    if (expect0 != target) {
        t_error("%s expect type is not lower", __func__);
    }

    wctype_t expect1 = wctype_l("lowerr", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is lower", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0800
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are print and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("print", m_locale);
    wctype_t target = 8;
    if (expect0 != target) {
        t_error("%s expect type is not print", __func__);
    }

    wctype_t expect1 = wctype_l("printt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is print", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_0900
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are punct and error strings
 * @tc.level     : Level 0
 */
void wctype_l_0900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("punct", m_locale);
    wctype_t target = 9;
    if (expect0 != target) {
        t_error("%s expect type is not punct", __func__);
    }

    wctype_t expect1 = wctype_l("punctt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is punct", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1000
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are space and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("space", m_locale);
    wctype_t target = 10;
    if (expect0 != target) {
        t_error("%s expect type is not space", __func__);
    }

    wctype_t expect1 = wctype_l("spacee", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is space", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1100
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are upper and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("upper", m_locale);
    wctype_t target = 11;
    if (expect0 != target) {
        t_error("%s expect type is not upper", __func__);
    }

    wctype_t expect1 = wctype_l("upperr", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is upper", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1200
 * @tc.desc      : Verify wctype_l process success when using the en_US.UTF-8 character set.
                   Test the return value when wctype_l input parameters are xdigit and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("xdigit", m_locale);
    wctype_t target = 12;
    if (expect0 != target) {
        t_error("%s expect type is not xdigit", __func__);
    }

    wctype_t expect1 = wctype_l("xdigitt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is xdigit", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1300
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are alnum and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("alnum", m_locale);
    wctype_t target = 1;
    if (expect0 != target) {
        t_error("%s expect type is not alnum", __func__);
    }

    wctype_t expect1 = wctype_l("alnumm", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is alnum", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1400
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are alpha and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("alpha", m_locale);
    wctype_t target = 2;
    if (expect0 != target) {
        t_error("%s expect type is not alpha", __func__);
    }

    wctype_t expect1 = wctype_l("alphaa", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is alpha", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1500
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are blank and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("blank", m_locale);
    wctype_t target = 3;
    if (expect0 != target) {
        t_error("%s expect type is not blank", __func__);
    }

    wctype_t expect1 = wctype_l("blankk", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is blank", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1600
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are cntrl and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("cntrl", m_locale);
    wctype_t target = 4;
    if (expect0 != target) {
        t_error("%s expect type is not cntrl", __func__);
    }

    wctype_t expect1 = wctype_l("cntrll", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is cntrl", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1700
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are digit and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("digit", m_locale);
    wctype_t target = 5;
    if (expect0 != target) {
        t_error("%s expect type is not digit", __func__);
    }

    wctype_t expect1 = wctype_l("digitt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is digit", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1800
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are graph and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("graph", m_locale);
    wctype_t target = 6;
    if (expect0 != target) {
        t_error("%s expect type is not graph", __func__);
    }

    wctype_t expect1 = wctype_l("graphh", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is graph", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_1900
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are lower and error strings
 * @tc.level     : Level 0
 */
void wctype_l_1900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("lower", m_locale);
    wctype_t target = 7;
    if (expect0 != target) {
        t_error("%s expect type is not lower", __func__);
    }

    wctype_t expect1 = wctype_l("lowerr", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is lower", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2000
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are print and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("print", m_locale);
    wctype_t target = 8;
    if (expect0 != target) {
        t_error("%s expect type is not print", __func__);
    }

    wctype_t expect1 = wctype_l("printt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is print", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2100
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are punct and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("punct", m_locale);
    wctype_t target = 9;
    if (expect0 != target) {
        t_error("%s expect type is not punct", __func__);
    }

    wctype_t expect1 = wctype_l("punctt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is punct", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2200
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are space and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("space", m_locale);
    wctype_t target = 10;
    if (expect0 != target) {
        t_error("%s expect type is not space", __func__);
    }

    wctype_t expect1 = wctype_l("spacee", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is space", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2300
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are upper and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("upper", m_locale);
    wctype_t target = 11;
    if (expect0 != target) {
        t_error("%s expect type is not upper", __func__);
    }

    wctype_t expect1 = wctype_l("upperr", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is upper", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2400
 * @tc.desc      : Verify wctype_l process success when using the zh_CN.UTF-8 character set.
                   Test the return value when wctype_l input parameters are xdigit and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("xdigit", m_locale);
    wctype_t target = 12;
    if (expect0 != target) {
        t_error("%s expect type is not xdigit", __func__);
    }

    wctype_t expect1 = wctype_l("xdigitt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is xdigit", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2500
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are alnum and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("alnum", m_locale);
    wctype_t target = 1;
    if (expect0 != target) {
        t_error("%s expect type is not alnum", __func__);
    }

    wctype_t expect1 = wctype_l("alnumm", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is alnum", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2600
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are alpha and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("alpha", m_locale);
    wctype_t target = 2;
    if (expect0 != target) {
        t_error("%s expect type is not alpha", __func__);
    }

    wctype_t expect1 = wctype_l("alphaa", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is alpha", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2700
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are blank and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("blank", m_locale);
    wctype_t target = 3;
    if (expect0 != target) {
        t_error("%s expect type is not blank", __func__);
    }

    wctype_t expect1 = wctype_l("blankk", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is blank", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2800
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are cntrl and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("cntrl", m_locale);
    wctype_t target = 4;
    if (expect0 != target) {
        t_error("%s expect type is not cntrl", __func__);
    }

    wctype_t expect1 = wctype_l("cntrll", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is cntrl", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_2900
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are digit and error strings
 * @tc.level     : Level 0
 */
void wctype_l_2900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("digit", m_locale);
    wctype_t target = 5;
    if (expect0 != target) {
        t_error("%s expect type is not digit", __func__);
    }

    wctype_t expect1 = wctype_l("digitt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is digit", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_3000
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are graph and error strings
 * @tc.level     : Level 0
 */
void wctype_l_3000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("graph", m_locale);
    wctype_t target = 6;
    if (expect0 != target) {
        t_error("%s expect type is not graph", __func__);
    }

    wctype_t expect1 = wctype_l("graphh", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is graph", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_3100
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are lower and error strings
 * @tc.level     : Level 0
 */
void wctype_l_3100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("lower", m_locale);
    wctype_t target = 7;
    if (expect0 != target) {
        t_error("%s expect type is not lower", __func__);
    }

    wctype_t expect1 = wctype_l("lowerr", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is lower", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_3200
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are print and error strings
 * @tc.level     : Level 0
 */
void wctype_l_3200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("print", m_locale);
    wctype_t target = 8;
    if (expect0 != target) {
        t_error("%s expect type is not print", __func__);
    }

    wctype_t expect1 = wctype_l("printt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is print", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_3300
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are punct and error strings
 * @tc.level     : Level 0
 */
void wctype_l_3300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("punct", m_locale);
    wctype_t target = 9;
    if (expect0 != target) {
        t_error("%s expect type is not punct", __func__);
    }

    wctype_t expect1 = wctype_l("punctt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is punct", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_3400
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are space and error strings
 * @tc.level     : Level 0
 */
void wctype_l_3400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("space", m_locale);
    wctype_t target = 10;
    if (expect0 != target) {
        t_error("%s expect type is not space", __func__);
    }

    wctype_t expect1 = wctype_l("spacee", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is space", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_3500
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are upper and error strings
 * @tc.level     : Level 0
 */
void wctype_l_3500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("upper", m_locale);
    wctype_t target = 11;
    if (expect0 != target) {
        t_error("%s expect type is not upper", __func__);
    }

    wctype_t expect1 = wctype_l("upperr", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is upper", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctype_l_3600
 * @tc.desc      : Verify wctype_l process success when using the zh_CN character set.
                   Test the return value when wctype_l input parameters are xdigit and error strings
 * @tc.level     : Level 0
 */
void wctype_l_3600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        //EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    wctype_t expect0 = wctype_l("xdigit", m_locale);
    wctype_t target = 12;
    if (expect0 != target) {
        t_error("%s expect type is not xdigit", __func__);
    }

    wctype_t expect1 = wctype_l("xdigitt", m_locale);
    if (expect1 == target) {
        t_error("%s error input type is xdigit", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

int main(int argc, char *argv[])
{
    wctype_l_0100();
    wctype_l_0200();
    wctype_l_0300();
    wctype_l_0400();
    wctype_l_0500();
    wctype_l_0600();
    wctype_l_0700();
    wctype_l_0800();
    wctype_l_0900();
    wctype_l_1000();
    wctype_l_1100();
    wctype_l_1200();
    wctype_l_1300();
    wctype_l_1400();
    wctype_l_1500();
    wctype_l_1600();
    wctype_l_1700();
    wctype_l_1800();
    wctype_l_1900();
    wctype_l_2000();
    wctype_l_2100();
    wctype_l_2200();
    wctype_l_2300();
    wctype_l_2400();
    wctype_l_2500();
    wctype_l_2600();
    wctype_l_2700();
    wctype_l_2800();
    wctype_l_2900();
    wctype_l_3000();
    wctype_l_3100();
    wctype_l_3200();
    wctype_l_3300();
    wctype_l_3400();
    wctype_l_3500();
    wctype_l_3600();

    return t_status;
}

