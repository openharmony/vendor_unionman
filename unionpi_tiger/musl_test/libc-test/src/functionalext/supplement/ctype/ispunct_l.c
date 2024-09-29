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

#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#include "functionalext.h"

const int SIZE = 128;
const int COUNT = 32;
const int RET_TRUE = 1;
const int RET_FALSE = 0;

/*
 * @tc.name      : ispunct_l_0100
 * @tc.desc      : Verify ispunct_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_0100(void)
{
    int ret = RET_FALSE;
    const char *str = "!\"#$%&'()*+,-./:;<>=?@[]\\^_{}|~";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_0200
 * @tc.desc      : Verify ispunct_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a letter,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_0200(void)
{
    int ret = RET_FALSE;
    const char *str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_0300
 * @tc.desc      : Verify ispunct_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a number,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_0300(void)
{
    int ret = RET_FALSE;
    const char *str = "1234567890";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_0400
 * @tc.desc      : Verify ispunct_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_0400(void)
{
    int ret = RET_FALSE;
    //arr is a control character array
    const char arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(char); i++) {
        ret = ispunct_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : ispunct_l_0500
 * @tc.desc      : Verify ispunct_l  when using the en_US.UTF-8 character set.
 *                 Determine the number in the ascii code table.
 * @tc.level     : Level 1
 */
void ispunct_l_0500(void)
{
    int ret = RET_FALSE;
    int total = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = ispunct_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_0600
 * @tc.desc      : Verify ispunct_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is wide char punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_0600(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!\"#$%&'()*+,-./:;<>=?@[]\\^_{}|~";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_0700
 * @tc.desc      : Verify ispunct_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char letter,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_0700(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_0800
 * @tc.desc      : Verify ispunct_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char number,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_0800(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"1234567890";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_0900
 * @tc.desc      : Verify ispunct_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char control character,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_0900(void)
{
    int ret = RET_FALSE;
    //arr is a wide char control character array
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = ispunct_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1000
 * @tc.desc      : Verify ispunct_l process success. when using the zh_CN character set.
 *                 the parameter c is punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_1000(void)
{
    int ret = RET_FALSE;
    const char *str = "!\"#$%&'()*+,-./:;<>=?@[]\\^_{}|~";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1100
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN character set.
 *                 the parameter c is a letter,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_1100(void)
{
    int ret = RET_FALSE;
    const char *str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1200
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN character set.
 *                 the parameter c is a number,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_1200(void)
{
    int ret = RET_FALSE;
    const char *str = "1234567890";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1300
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_1300(void)
{
    int ret = RET_FALSE;
    //arr is a control character array
    const char arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(char); i++) {
        ret = ispunct_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : ispunct_l_1400
 * @tc.desc      : Verify ispunct_l  when using the zh_CNcharacter set.
 *                 Determine the number in the ascii code table.
 * @tc.level     : Level 1
 */
void ispunct_l_1400(void)
{
    int ret = RET_FALSE;
    int total = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = ispunct_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1500
 * @tc.desc      : Verify ispunct_l process success. when using the zh_CN character set.
 *                 the parameter c is wide char punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_1500(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!\"#$%&'()*+,-./:;<>=?@[]\\^_{}|~";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1600
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN character set.
 *                 the parameter c is a wide char letter,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_1600(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1700
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN character set.
 *                 the parameter c is a wide char number,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_1700(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"1234567890";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_1800
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN character set.
 *                 the parameter c is a wide char control character,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_1800(void)
{
    int ret = RET_FALSE;
    //arr is a wide char control character array
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = ispunct_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : ispunct_l_1900
 * @tc.desc      : Verify ispunct_l process success when using the zh_CN character set.
 *                 the parameter c is a IDEOGRAPHIC DESCRIPTION CHARACTER character or some punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_1900(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /* IDEOGRAPHIC DESCRIPTION CHARACTER */
    for (i = 0x2FF0; i <= 0x2FFB; i++) {
        ret = ispunct_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    /* Some punct character */
    // 0xFF01 : FULLWIDTH EXCLAMATION MARK
    // 0xFF02 : FULLWIDTH QUOTATION MARK
    // 0xFF03 : FULLWIDTH NUMBER SIGN
    // 0xFF04 : FULLWIDTH DOLLAR SIGN
    // 0xFF05 : FULLWIDTH PERCENT SIGN
    // 0xFF06 : FULLWIDTH AMPERSAND
    // 0xFF07 : FULLWIDTH APOSTROPHE
    // 0xFF08 : FULLWIDTH LEFT PARENTHESIS
    // 0xFF09 : FULLWIDTH RIGHT PARENTHESIS
    // 0xFF0A : FULLWIDTH ASTERISK
    // 0xFF0B : FULLWIDTH PLUS SIGN
    // 0xFF0C : FULLWIDTH COMMA
    // 0xFF0D : FULLWIDTH HYPHEN-MINUS
    // 0xFF0E : FULLWIDTH FULL STOP
    // 0xFF0F : FULLWIDTH SOLIDUS
    for (i = 0xFF01; i <= 0xFF0F; i++) {
        ret = ispunct_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : ispunct_l_2000
 * @tc.desc      : Verify ispunct_l process success when using the zh_CN character set.
 *                 the parameter c is a math punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_2000(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /* math punct character */
    for (int i = 0x2240; i <= 0x227e; i++) {
        ret = ispunct_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}


/**
 * @tc.name      : ispunct_l_2100
 * @tc.desc      : Verify ispunct_l process fail when using the zh_CN character set.
 *                 the parameter c is a GREEK character,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_2100(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //GREEK characters(U+0370 - U+03FF)
    for (int i = 0x03AC; i <= 0x03CE; i++) {
        ret = ispunct_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_2200
 * @tc.desc      : Verify ispunct_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_2200(void)
{
    int ret = RET_FALSE;
    const char *str = "!\"#$%&'()*+,-./:;<>=?@[]\\^_{}|~";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_2300
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a letter,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_2300(void)
{
    int ret = RET_FALSE;
    const char *str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_2400
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a number,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_2400(void)
{
    int ret = RET_FALSE;
    const char *str = "1234567890";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_2500
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_2500(void)
{
    int ret = RET_FALSE;
    //arr is a control character array
    const char arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(char); i++) {
        ret = ispunct_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : ispunct_l_2600
 * @tc.desc      : Verify ispunct_l  when using the zh_CN.UTF-8 character set.
 *                 Determine the number in the ascii code table.
 * @tc.level     : Level 1
 */
void ispunct_l_2600(void)
{
    int ret = RET_FALSE;
    int total = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = ispunct_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_2700
 * @tc.desc      : Verify ispunct_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is wide char punct character,
 *                 and the input character is judged to be a punct character.
 * @tc.level     : Level 0
 */
void ispunct_l_2700(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!\"#$%&'()*+,-./:;<>=?@[]\\^_{}|~";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_2800
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char letter,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_2800(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_2900
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char number,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_2900(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"1234567890";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = ispunct_l(*p, m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        *p++;
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : ispunct_l_3000
 * @tc.desc      : Verify ispunct_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char control character,
 *                 and the input character is judged to be not a punct character.
 * @tc.level     : Level 2
 */
void ispunct_l_3000(void)
{
    int ret = RET_FALSE;
    //arr is a wide char control character array
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = ispunct_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : ispunct_l_3100
 * @tc.desc      : Verify ispunct_l process success when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a IDEOGRAPHIC DESCRIPTION CHARACTER character or some punct character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void ispunct_l_3100(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /* IDEOGRAPHIC DESCRIPTION CHARACTER */
    for (i = 0x2FF0; i <= 0x2FFB; i++) {
        ret = ispunct_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    /* Some punct character */
    // 0xFF01 : FULLWIDTH EXCLAMATION MARK
    // 0xFF02 : FULLWIDTH QUOTATION MARK
    // 0xFF03 : FULLWIDTH NUMBER SIGN
    // 0xFF04 : FULLWIDTH DOLLAR SIGN
    // 0xFF05 : FULLWIDTH PERCENT SIGN
    // 0xFF06 : FULLWIDTH AMPERSAND
    // 0xFF07 : FULLWIDTH APOSTROPHE
    // 0xFF08 : FULLWIDTH LEFT PARENTHESIS
    // 0xFF09 : FULLWIDTH RIGHT PARENTHESIS
    // 0xFF0A : FULLWIDTH ASTERISK
    // 0xFF0B : FULLWIDTH PLUS SIGN
    // 0xFF0C : FULLWIDTH COMMA
    // 0xFF0D : FULLWIDTH HYPHEN-MINUS
    // 0xFF0E : FULLWIDTH FULL STOP
    // 0xFF0F : FULLWIDTH SOLIDUS
    for (i = 0xFF01; i <= 0xFF0F; i++) {
        ret = ispunct_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : ispunct_l_3200
 * @tc.desc      : Verify ispunct_l process success when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a math punct character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void ispunct_l_3200(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /* math punct character */
    for (int i = 0x2240; i <= 0x227e; i++) {
        ret = ispunct_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}


/**
 * @tc.name      : ispunct_l_3300
 * @tc.desc      : Verify ispunct_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a GREEK character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void ispunct_l_3300(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //GREEK characters(U+0370 - U+03FF)
    for (int i = 0x03AC; i <= 0x03CE; i++) {
        ret = ispunct_l(i, m_locale);
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
    ispunct_l_0100();
    ispunct_l_0200();
    ispunct_l_0300();
    ispunct_l_0400();
    ispunct_l_0500();
    ispunct_l_0600();
    ispunct_l_0700();
    ispunct_l_0800();
    ispunct_l_0900();
    ispunct_l_1000();
    ispunct_l_1100();
    ispunct_l_1200();
    ispunct_l_1300();
    ispunct_l_1400();
    ispunct_l_1500();
    ispunct_l_1600();
    ispunct_l_1700();
    ispunct_l_1800();
    ispunct_l_1900();
    ispunct_l_2000();
    ispunct_l_2100();
    ispunct_l_2200();
    ispunct_l_2300();
    ispunct_l_2400();
    ispunct_l_2500();
    ispunct_l_2600();
    ispunct_l_2700();
    ispunct_l_2800();
    ispunct_l_2900();
    ispunct_l_3000();
    ispunct_l_3100();
    ispunct_l_3200();
    ispunct_l_3300();
    return t_status;
}