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
#include <locale.h>
#include <wchar.h>
#include "functionalext.h"

const int COUNT = 22;
const int SIZE = 128;
const int RET_TRUE = 1;
const int RET_FALSE = 0;

/**
 * @tc.name      : isxdigit_l_0100
 * @tc.desc      : Verify isxdigit_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is hexadecimal digit character,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_0100(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "0123456789AaBbCcDdEeFf";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        int ret = isxdigit_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0200
 * @tc.desc      : Verify isxdigit_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is no-hexadecimal digit letter character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_0200(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "gGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0300
 * @tc.desc      : Verify isxdigit_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is a special character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_0300(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "~!@#$%^&*(){}[]<>,.|";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0400
 * @tc.desc      : Verify isxdigit_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_0400(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const int arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(int); i++) {
        ret = isxdigit_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0500
 * @tc.desc      : Verify isxdigit_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is wide char hexadecimal digit character,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_0500(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"0123456789AaBbCcDdEeFf";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        int ret = isxdigit_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0600
 * @tc.desc      : Verify isxdigit_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is wide char letter,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_0600(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"gGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0700
 * @tc.desc      : Verify isxdigit_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char special character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_0700(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"~!@#$%^&*(){}[]<>,.|";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0800
 * @tc.desc      : Verify isxdigit_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char control character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_0800(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = isxdigit_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_0900
 * @tc.desc      : Verify isxdigit_l using the en_US.UTF-8 character set.
 *                  Determine the number of hexadecimal digit characters in the ascii code table
 * @tc.level     : Level 1
 */
void isxdigit_l_0900(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    int total = 0;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = isxdigit_l(i, m_locale);
        if (ret) {
            total++;
        }
    }
    EXPECT_EQ(__FUNCTION__, total, COUNT);
}

/**
 * @tc.name      : isxdigit_l_1000
 * @tc.desc      : Verify isxdigit_l process success. when using the zh_CN character set.
 *                 the parameter c is hexadecimal digit character,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_1000(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "0123456789AaBbCcDdEeFf";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        int ret = isxdigit_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1100
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN character set.
 *                 the parameter c is no-hexadecimal digit letter,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_1100(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "gGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1200
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN character set.
 *                 the parameter c is a special character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_1200(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "~!@#$%^&*(){}[]<>,.|";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1300
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_1300(void)
{
    int ret = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const int arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(int); i++) {
        ret = isxdigit_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1400
 * @tc.desc      : Verify isxdigit_l process success. when using the zh_CN character set.
 *                 the parameter c is wide char hexadecimal digit character,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_1400(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"0123456789AaBbCcDdEeFf";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        int ret = isxdigit_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1500
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN character set.
 *                 the parameter c is wide char no-hexadecimal digit letter character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_1500(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"gGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1600
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN character set.
 *                 the parameter c is wide char special character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_1600(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"~!@#$%^&*(){}[]<>,.|";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1700
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN character set.
 *                 the parameter c is wide char control character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_1700(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = isxdigit_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_1800
 * @tc.desc      : Verify isxdigit_l using the zh_CN character set.
 *                  Determine the number of hexadecimal digit characters in the ascii code table
 * @tc.level     : Level 1
 */
void isxdigit_l_1800(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int total = 0;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        int ret = isxdigit_l(i, m_locale);
        if (ret) {
            total++;
        }
    }
    EXPECT_EQ(__FUNCTION__, total, COUNT);
}

/**
 * @tc.name      : isxdigit_l_1900
 * @tc.desc      : Verify isxdigit_l process success when using the zh_CN character set.
 *                 the parameter c is TIBETAN DIGIT characters,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_1900(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //TIBETAN DIGIT(U+0F20 - U+0F29)
    // 0x0F20 : TIBETAN DIGIT ZERO 
    // 0x0F29 : TIBETAN DIGIT NINE
    for (int i = 0x0F20; i <= 0x0F29; i++) {
        ret = isxdigit_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2000
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN character set.
 *                 the parameter c is CJK Ideograph or TIBETAN DIGIT HALF characters,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_2000(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //CJK Ideograph characters(U+4E00 - U+4EF0)
    for (i = 0x4E00; i <= 0x4EF0; i++) {
        ret = isxdigit_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    //TIBETAN DIGIT(U+0F2A - U+0F33)
    // 0x0F2A : TIBETAN DIGIT HALF ONE 
    // 0x0F33 : TIBETAN DIGIT HALF NINE
    for (i = 0x0F2A; i <= 0x0F33; i++) {
        ret = isxdigit_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2100
 * @tc.desc      : Verify isxdigit_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is hexadecimal digit character,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_2100(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "0123456789AaBbCcDdEeFf";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        int ret = isxdigit_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2200
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is no-hexadecimal digit letter,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_2200(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "gGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2300
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a special character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_2300(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "~!@#$%^&*(){}[]<>,.|";
    const char *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != '\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2400
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_2400(void)
{
    int ret = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const int arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(int); i++) {
        ret = isxdigit_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2500
 * @tc.desc      : Verify isxdigit_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is wide char hexadecimal digit character,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_2500(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"0123456789AaBbCcDdEeFf";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        int ret = isxdigit_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2600
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is wide char no-hexadecimal digit letter character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_2600(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"gGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2700
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is wide char special character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_2700(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"~!@#$%^&*(){}[]<>,.|";
    const wchar_t *p = str;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (*p && *p != L'\0') {
        ret = isxdigit_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2800
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is wide char control character,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_2800(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = isxdigit_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_2900
 * @tc.desc      : Verify isxdigit_l using the zh_CN.UTF-8 character set.
 *                  Determine the number of hexadecimal digit characters in the ascii code table
 * @tc.level     : Level 1
 */
void isxdigit_l_2900(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int total = 0;

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        int ret = isxdigit_l(i, m_locale);
        if (ret) {
            total++;
        }
    }
    EXPECT_EQ(__FUNCTION__, total, COUNT);
}

/**
 * @tc.name      : isxdigit_l_3000
 * @tc.desc      : Verify isxdigit_l process success when using the zh_CN.UTF-8 character set.
 *                 the parameter c is TIBETAN DIGIT characters,
 *                 and the input character is judged to be a hexadecimal digit character.
 * @tc.level     : Level 0
 */
void isxdigit_l_3000(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //TIBETAN DIGIT(U+0F20 - U+0F29)
    // 0x0F20 : TIBETAN DIGIT ZERO 
    // 0x0F29 : TIBETAN DIGIT NINE
    for (int i = 0x0F20; i <= 0x0F29; i++) {
        ret = isxdigit_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : isxdigit_l_3100
 * @tc.desc      : Verify isxdigit_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is CJK Ideograph or TIBETAN DIGIT HALF characters,
 *                 and the input character is judged to be a no-hexadecimal digit character.
 * @tc.level     : Level 2
 */
void isxdigit_l_3100(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //CJK Ideograph characters(U+4E00 - U+4EF0)
    for (i = 0x4E00; i <= 0x4EF0; i++) {
        ret = isxdigit_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    //TIBETAN DIGIT(U+0F2A - U+0F33)
    // 0x0F2A : TIBETAN DIGIT HALF ONE 
    // 0x0F33 : TIBETAN DIGIT HALF NINE
    for (i = 0x0F2A; i <= 0x0F33; i++) {
        ret = isxdigit_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

int main(void)
{
    isxdigit_l_0100();
    isxdigit_l_0200();
    isxdigit_l_0300();
    isxdigit_l_0400();
    isxdigit_l_0500();
    isxdigit_l_0600();
	isxdigit_l_0700();
	isxdigit_l_0800();
	isxdigit_l_0900();
	isxdigit_l_1000();
	isxdigit_l_1100();
	isxdigit_l_1200();
	isxdigit_l_1300();
	isxdigit_l_1400();
	isxdigit_l_1500();
	isxdigit_l_1600();
	isxdigit_l_1700();
	isxdigit_l_1800();
	isxdigit_l_1900();
	isxdigit_l_2000();
    isxdigit_l_2100();
    isxdigit_l_2200();
    isxdigit_l_2300();
    isxdigit_l_2400();
    isxdigit_l_2500();
    isxdigit_l_2600();
	isxdigit_l_2700();
	isxdigit_l_2800();
	isxdigit_l_2900();
	isxdigit_l_3000();
	isxdigit_l_3100();
    return t_status;
}