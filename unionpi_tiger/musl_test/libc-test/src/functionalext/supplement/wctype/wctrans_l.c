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
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>
#include <locale.h>
#include "test.h"
#include "functionalext.h"



/**
 * @tc.name      : wctrans_l_0100
 * @tc.desc      : Verify wctrans_l process success when using the en_US.UTF-8 character set.
                   Test result of wctrans_l when incoming lowercase conversion
 * @tc.level     : Level 0
 */
void wctrans_l_0100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (!wctrans_l("tolower", m_locale)) {
        t_error("%s wctrans_l error get result is NULL\n", __FUNCTION__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0200
 * @tc.desc      : Verify wctrans_l process success when using the en_US.UTF-8 character set.
                   Test result of wctrans_l when incoming uppercase conversion
 * @tc.level     : Level 0
 */
void wctrans_l_0200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (!wctrans_l("toupper", m_locale)) {
        t_error("%s wctrans_l error get result is NULL\n", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0300
 * @tc.desc      : Verify wctrans_l process success when using the en_US.UTF-8 character set.
                   Test the result of wctrans_l when passing in an exception parameter
 * @tc.level     : Level 2
 */
void wctrans_l_0300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (wctrans_l("monkeys", m_locale)) {
        t_error("%s wctrans_l error get result is not NULL\n", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0400
 * @tc.desc      : Verify wctrans_l process success when using the zh_CN.UTF-8 character set.
                   Test result of wctrans_l when incoming lowercase conversion
 * @tc.level     : Level 0
 */
void wctrans_l_0400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (!wctrans_l("tolower", m_locale)) {
        t_error("%s wctrans_l error get result is NULL\n", __FUNCTION__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0500
 * @tc.desc      : Verify wctrans_l process success when using the zh_CN.UTF-8 character set.
                   Test result of wctrans_l when incoming uppercase conversion
 * @tc.level     : Level 0
 */
void wctrans_l_0500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (!wctrans_l("toupper", m_locale)) {
        t_error("%s wctrans_l error get result is NULL\n", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0600
 * @tc.desc      : Verify wctrans_l process success when using the zh_CN.UTF-8 character set.
                   Test the result of wctrans_l when passing in an exception parameter
 * @tc.level     : Level 2
 */
void wctrans_l_0600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (wctrans_l("monkeys", m_locale)) {
        t_error("%s wctrans_l error get result is not NULL\n", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0700
 * @tc.desc      : Verify wctrans_l process success when using the zh_CN character set.
                   Test result of wctrans_l when incoming lowercase conversion
 * @tc.level     : Level 0
 */
void wctrans_l_0700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (!wctrans_l("tolower", m_locale)) {
        t_error("%s wctrans_l error get result is NULL\n", __FUNCTION__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0800
 * @tc.desc      : Verify wctrans_l process success when using the zh_CN character set.
                   Test result of wctrans_l when incoming uppercase conversion
 * @tc.level     : Level 0
 */
void wctrans_l_0800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (!wctrans_l("toupper", m_locale)) {
        t_error("%s wctrans_l error get result is NULL\n", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : wctrans_l_0900
 * @tc.desc      : Verify wctrans_l process success when using the zh_CN character set.
                   Test the result of wctrans_l when passing in an exception parameter
 * @tc.level     : Level 2
 */
void wctrans_l_0900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        return ;
    }

    if (wctrans_l("monkeys", m_locale)) {
        t_error("%s wctrans_l error get result is not NULL\n", __func__);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

int main(int argc, char *argv[])
{
    wctrans_l_0100();
    wctrans_l_0200();
    wctrans_l_0300();
    wctrans_l_0400();
    wctrans_l_0500();
    wctrans_l_0600();
    wctrans_l_0700();
    wctrans_l_0800();
    wctrans_l_0900();
    return t_status;
}