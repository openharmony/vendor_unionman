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

#include <wctype.h>
#include <locale.h>
#include <stdlib.h>
#include <wchar.h>
#include "functionalext.h"

const int COUNT = 94;
const int SIZE = 128;
const int RET_TRUE = 1;
const int RET_FALSE = 0;

/*
 * @tc.name      : iswgraph_l_0100
 * @tc.desc      : Verify iswgraph_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is a English letter,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_0100(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_0200
 * @tc.desc      : Verify iswgraph_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is a number,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_0200(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_0300
 * @tc.desc      : Verify iswgraph_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is a special character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_0300(void)
{
    int ret = RET_FALSE;
    const char *str = "!@#$%^&*()_+{}[]";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_0400
 * @tc.desc      : Verify iswgraph_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_0400(void)
{
    int ret = RET_FALSE;
    const char arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(char); i++) {
        ret = iswgraph_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}


/**
 * @tc.name      : iswgraph_l_0500
 * @tc.desc      : Verify iswgraph_l  when using the en_US.UTF-8 character set.
 *                 Determine the number in the ascii code table.
 * @tc.level     : Level 1
 */
void iswgraph_l_0500(void)
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
        ret = iswgraph_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : iswgraph_l_0600
 * @tc.desc      : Verify iswgraph_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char English letter,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_0600(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_0700
 * @tc.desc      : Verify iswgraph_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char number,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_0700(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_0800
 * @tc.desc      : Verify iswgraph_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char special character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_0800(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!@#$%^&*()_+{}[]";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_0900
 * @tc.desc      : Verify iswgraph_l process fail. when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char control character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_0900(void)
{
    int ret = RET_FALSE;
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswgraph_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : iswgraph_l_1000
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN character set.
 *                 the parameter c is a English letter,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_1000(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_1100
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN character set.
 *                 the parameter c is a number,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_1100(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_1200
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN character set.
 *                 the parameter c is a special character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_1200(void)
{
    int ret = RET_FALSE;
    const char *str = "!@#$%^&*()_+{}[]";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_1300
 * @tc.desc      : Verify iswgraph_l process fail. when using the zh_CN character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_1300(void)
{
    int ret = RET_FALSE;
    const char arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(char); i++) {
        ret = iswgraph_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}


/**
 * @tc.name      : iswgraph_l_1400
 * @tc.desc      : Verify iswgraph_l  when using the zh_CN character set.
 *                 Determine the number in the ascii code table.
 * @tc.level     : Level 1
 */
void iswgraph_l_1400(void)
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
        ret = iswgraph_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : iswgraph_l_1500
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN character set.
 *                 the parameter c is a wide char English letter,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_1500(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_1600
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN character set.
 *                 the parameter c is a wide char number,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_1600(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_1700
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN character set.
 *                 the parameter c is a wide char special character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_1700(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!@#$%^&*()_+{}[]";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_1800
 * @tc.desc      : Verify iswgraph_l process fail. when using the zh_CN character set.
 *                 the parameter c is a wide char control character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_1800(void)
{
    int ret = RET_FALSE;
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswgraph_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswgraph_l_1900
 * @tc.desc      : Verify iswgraph_l process success when using the zh_CN character set.
 *                 the parameter c is a BOPOMOFO LETTER or ARABIC LETTER character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_1900(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /* BOPOMOFO LETTER character */
    for (i = 0x3105; i <= 0x312F; i++) {
        ret = iswgraph_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    //ARABIC LETTER characters(U+0620 - U+0669)
    for (i = 0x0620; i <= 0x0669; i++) {
        ret = iswgraph_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswgraph_l_2000
 * @tc.desc      : Verify iswgraph_l process fail when using the zh_CN character set.
 *                 the parameter c is a Arabic character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_2000(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    // Arabic characters(U+0600 - U+0605)
    // 0x0600 : ARABIC NUMBER SIGN
    // 0x0601 : ARABIC SIGN SANAH
    // 0x0602 : ARABIC FOOTNOTE MARKER
    // 0x0603 : ARABIC SIGN SAFHA
    // 0x0604 : ARABIC SIGN SAMVAT
    // 0x0605 : ARABIC NUMBER MARK
    for (int i = 0x0600; i <= 0x0605; i++) {
        ret = iswgraph_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}


/**
 * @tc.name      : iswgraph_l_2100
 * @tc.desc      : Verify iswgraph_l process success when using the zh_CN character set.
 *                 the parameter c is a CJK Ideograph character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_2100(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *arr = L"零一二三四五六七八九十";
    const wchar_t *p = arr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_2200
 * @tc.desc      : Verify iswgraph_l process success when using the zh_CN character set.
 *                 the parameter c is a LATIN SMALL LETTER,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_2200(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *arr = L"āōēīūǖ";
    const wchar_t *p = arr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //LATIN SMALL LETTER A WITH MACRON
    //LATIN SMALL LETTER O WITH MACRON
    //LATIN SMALL LETTER E WITH MACRON
    //LATIN SMALL LETTER I WITH MACRON
    //LATIN SMALL LETTER U WITH MACRON
    //LATIN SMALL LETTER U WITH DIAERESIS AND MACRON;
    while (p && *p != L'\0') {
        ret = iswgraph_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : iswgraph_l_2300
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a English letter,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_2300(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_2400
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a number,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_2400(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_2500
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a special character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_2500(void)
{
    int ret = RET_FALSE;
    const char *str = "!@#$%^&*()_+{}[]";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_2600
 * @tc.desc      : Verify iswgraph_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a control character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_2600(void)
{
    int ret = RET_FALSE;
    const char arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};

    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(char); i++) {
        ret = iswgraph_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}


/**
 * @tc.name      : iswgraph_l_2700
 * @tc.desc      : Verify iswgraph_l  when using the zh_CN.UTF-8 character set.
 *                 Determine the number in the ascii code table.
 * @tc.level     : Level 1
 */
void iswgraph_l_2700(void)
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
        ret = iswgraph_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/*
 * @tc.name      : iswgraph_l_2800
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char English letter,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_2800(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_2900
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char number,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_2900(void)
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
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_3000
 * @tc.desc      : Verify iswgraph_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char special character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_3000(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!@#$%^&*()_+{}[]";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_3100
 * @tc.desc      : Verify iswgraph_l process fail. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char control character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_3100(void)
{
    int ret = RET_FALSE;
    const wchar_t arr[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f};
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswgraph_l(arr[i], m_locale);
        if(ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswgraph_l_3200
 * @tc.desc      : Verify iswgraph_l process success when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a BOPOMOFO LETTER or ARABIC LETTER character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_3200(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /* BOPOMOFO LETTER character */
    for (i = 0x3105; i <= 0x312F; i++) {
        ret = iswgraph_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    //ARABIC LETTER characters(U+0620 - U+0669)
    for (i = 0x0620; i <= 0x0669; i++) {
        ret = iswgraph_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswgraph_l_3300
 * @tc.desc      : Verify iswgraph_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a Arabic character,
 *                 and the input character is judged to be not a graph character.
 * @tc.level     : Level 2
 */
void iswgraph_l_3300(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    // Arabic characters(U+0600 - U+0605)
    // 0x0600 : ARABIC NUMBER SIGN
    // 0x0601 : ARABIC SIGN SANAH
    // 0x0602 : ARABIC FOOTNOTE MARKER
    // 0x0603 : ARABIC SIGN SAFHA
    // 0x0604 : ARABIC SIGN SAMVAT
    // 0x0605 : ARABIC NUMBER MARK
    for (int i = 0x0600; i <= 0x0605; i++) {
        ret = iswgraph_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswgraph_l_3400
 * @tc.desc      : Verify iswgraph_l process success when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a CJK Ideograph character,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_3400(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *arr = L"零一二三四五六七八九十";
    const wchar_t *p = arr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswgraph_l(*p, m_locale);
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
 * @tc.name      : iswgraph_l_3500
 * @tc.desc      : Verify iswgraph_l process success when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a LATIN SMALL LETTER,
 *                 and the input character is judged to be a graph character.
 * @tc.level     : Level 0
 */
void iswgraph_l_3500(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *arr = L"āōēīūǖ";
    const wchar_t *p = arr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //LATIN SMALL LETTER A WITH MACRON
    //LATIN SMALL LETTER O WITH MACRON
    //LATIN SMALL LETTER E WITH MACRON
    //LATIN SMALL LETTER I WITH MACRON
    //LATIN SMALL LETTER U WITH MACRON
    //LATIN SMALL LETTER U WITH DIAERESIS AND MACRON;
    while (p && *p != L'\0') {
        ret = iswgraph_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

int main(void)
{
    iswgraph_l_0100();
    iswgraph_l_0200();
    iswgraph_l_0300();
    iswgraph_l_0400();
    iswgraph_l_0500();
    iswgraph_l_0600();
    iswgraph_l_0700();
    iswgraph_l_0800();
    iswgraph_l_0900();
    iswgraph_l_1000();
    iswgraph_l_1100();
    iswgraph_l_1200();
    iswgraph_l_1300();
    iswgraph_l_1400();
    iswgraph_l_1500();
    iswgraph_l_1600();
    iswgraph_l_1700();
    iswgraph_l_1800();
    iswgraph_l_1900();
    iswgraph_l_2000();
    iswgraph_l_2100();
    iswgraph_l_2200();
    iswgraph_l_2300();
    iswgraph_l_2400();
    iswgraph_l_2500();
    iswgraph_l_2600();
    iswgraph_l_2700();
    iswgraph_l_2800();
    iswgraph_l_2900();
    iswgraph_l_3000();
    iswgraph_l_3100();
    iswgraph_l_3200();
    iswgraph_l_3300();
    iswgraph_l_3400();
    iswgraph_l_3500();
    return t_status;
}