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
#include "functionalext.h"

const int COUNT = 2;
const int SIZE = 128;
const int OTHER_COUNT = 13;
const int RET_TRUE = 1;
const int RET_FALSE = 0;

/**
 * @tc.name      : iswblank_l_0100
 * @tc.desc      : Verify iswblank_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is one of a space in {'\t', ' '}),
 *                 and the input character is judged to be a space.
 * @tc.level     : Level 0
 */
void iswblank_l_0100(void)
{
    int ret = RET_FALSE;
    const int arr[] = {'\t', ' '};
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(unsigned int i = 0; i < sizeof(arr)/sizeof(int); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_0200
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is control character,
 *                  which determines that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_0200(void)
{
    int ret = RET_FALSE;
    const char *str = "\r\n\b\0\f";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_0300
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set 
 *                 When the parameter c is an English letter,
 *                 and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_0300(void)
{
    int ret = RET_FALSE;
    const char *str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_0400
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set
 *                 When the parameter c is numeric,
 *                 and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_0400(void)
{
    int ret = RET_FALSE;
    const char *str = "1234567890";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_0500
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set
 *                 When the parameter c is a special character,
 *           and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_0500(void)
{
    int ret = RET_FALSE;
    const char *str = "!@#$%^()+{}[]";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_0600
 * @tc.desc      : Verify iswblank_l process when using the en_US.UTF-8 character set
 *                 Determine the number of space characters in the ascii code table.
 * @tc.level     : Level 1
 */
void iswblank_l_0600(void)
{
    int ret = RET_FALSE;
    int total = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = iswblank_l((char)i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_0700
 * @tc.desc      : Verify iswblank_l process success. when using the en_US.UTF-8 character set.
 *                 the parameter c is one of a space in {L'\t', L' '}),
 *                 and the input wide character is judged to be a space.
 * @tc.level     : Level 0
 */
void iswblank_l_0700(void)
{
    int ret = RET_FALSE;
    const wchar_t arr[] = {L'\t', L' '};
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(unsigned int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_0800
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set.
 *                 the parameter c is a wide char control character,
 *                  which determines that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_0800(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"\r\n\b\0\f";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_0900
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set 
 *                 When the parameter c is a wide char English letter,
 *                 and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_0900(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_1000
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set
 *                 When the parameter c is a wide char alphanumeric,
 *                 and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_1000(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"1234567890";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_1100
 * @tc.desc      : Verify iswblank_l process fail when using the en_US.UTF-8 character set
 *                 When the parameter c is a wide char special character,
 *           and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_1100(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!@#$%^()+{}[]";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_1200
 * @tc.desc      : Verify iswblank_l process success. when using the zh_CN character set.
 *                 the parameter c is one of a space in {'\t', ' '}),
 *                 and the input character is judged to be a space.
 * @tc.level     : Level 0
 */
void iswblank_l_1200(void)
{
    int ret = RET_FALSE;
    const int arr[] = {'\t', ' '};
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(unsigned int i = 0; i < sizeof(arr)/sizeof(int); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_1300
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set.
 *                 the parameter c is control character,
 *                  which determines that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_1300(void)
{
    int ret = RET_FALSE;
    const char *str = "\r\n\b\0\f";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_1400
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set 
 *                 When the parameter c is an English letter,
 *                 and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_1400(void)
{
    int ret = RET_FALSE;
    const char *str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_1500
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set
 *                 When the parameter c is alphanumeric,
 *                 and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_1500(void)
{
    int ret = RET_FALSE;
    const char *str = "1234567890";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_1600
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set
 *                 When the parameter c is a special character,
 *           and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_1600(void)
{
    int ret = RET_FALSE;
    const char *str = "!@#$%^()+{}[]";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_1700
 * @tc.desc      : Verify iswblank_l process when using the zh_CN character set
 *                 Determine the number of space characters in the ascii code table.
 * @tc.level     : Level 1
 */
void iswblank_l_1700(void)
{
    int ret = RET_FALSE;
    int total = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = iswblank_l((char)i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_1800
 * @tc.desc      : Verify iswblank_l process success. when using the zh_CN character set.
 *                 the parameter c is one of a space in {L'\t', L' '},
 *                 and the input wide character is judged to be a space.
 * @tc.level     : Level 0
 */
void iswblank_l_1800(void)
{
    int ret = RET_FALSE;
    const wchar_t arr[] = {L'\t', L' '};
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(unsigned int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_1900
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set.
 *                 the parameter c is a wide char control character,
 *                  which determines that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_1900(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"\r\n\b\0\f";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_2000
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set 
 *                 When the parameter c is a wide char English letter,
 *                 and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2000(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_2100
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set
 *                 When the parameter c is a wide char alphanumeric,
 *                 and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2100(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"1234567890";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_2200
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set
 *                 When the parameter c is a wide char special character,
 *           and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2200(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!@#$%^()+{}[]";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_2300
 * @tc.desc      : Verify iswblank_l process success when using the zh_CN character set
 *                 When the parameter c is a SPACE-like character,
 * @tc.level     : Level 0
 */
void iswblank_l_2300(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /*
    * 0x1680:   OGHAM SPACE MARK
    * 0x2002:   EN SPACE
    * 0x2003:   EM SPACE
    * 0x2004:   THREE-PER-EM SPACE
    * 0x2005:   FOUR-PER-EM SPACE
    * 0x2006:   SIX-PER-EM SPACE
    * 0x2007:   FIGURE SPACE
    * 0x2008:   PUNCTUATION SPACE
    * 0x2009:   THIN SPACE
    * 0x200a:   HAIR SPACE
    * 0x202f:   NARROW NO-BREAK SPACE
    * 0x205f:   MEDIUM MATHEMATICAL SPACE
    * 0x3000:   IDEOGRAPHIC SPACE
    */
    const wchar_t arr[] = {0x1680, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200a, 0x202f, 0x205f, 0x3000};
    
    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, OTHER_COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_2400
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set
 *                 When the parameter c is a CJK RADICAL or Arabic character,
 *           and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2400(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //CJK RADICAL characters(U+2E80 - U+2EF3)
    for (i = 0x2E80; i <= 0x2EF3; i++) {
        ret = iswblank_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    //Arabic characters(U+0600 - U+06FF)
    for (i = 0x0660; i <= 0x0669; i++) {
        ret = iswblank_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_2500
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set.
 *                 the parameter c is a CJK Ideograph character,
 *                 and the input character is judged to be not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2500(void)
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
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_2600
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN character set.
 *                 the parameter c is a LATIN SMALL LETTER,
 *                 and the input character is judged to be not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2600(void)
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
        ret = iswblank_l(*p, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_2700
 * @tc.desc      : Verify iswblank_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is one of a space in {'\t', ' '}),
 *                 and the input character is judged to be a space.
 * @tc.level     : Level 0
 */
void iswblank_l_2700(void)
{
    int ret = RET_FALSE;
    const int arr[] = {'\t', ' '};
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(unsigned int i = 0; i < sizeof(arr)/sizeof(int); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_2800
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is control character,
 *                  which determines that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2800(void)
{
    int ret = RET_FALSE;
    const char *str = "\r\n\b\0\f";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_2900
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set 
 *                 When the parameter c is an English letter,
 *                 and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_2900(void)
{
    int ret = RET_FALSE;
    const char *str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_3000
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set
 *                 When the parameter c is alphanumeric,
 *                 and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_3000(void)
{
    int ret = RET_FALSE;
    const char *str = "1234567890";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_3100
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set
 *                 When the parameter c is a special character,
 *           and it is judged that the input character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_3100(void)
{
    int ret = RET_FALSE;
    const char *str = "!@#$%^()+{}[]";
    const char *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != '\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_3200
 * @tc.desc      : Verify iswblank_l process when using the zh_CN.UTF-8 character set
 *                 Determine the number of space characters in the ascii code table.
 * @tc.level     : Level 1
 */
void iswblank_l_3200(void)
{
    int ret = RET_FALSE;
    int total = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = iswblank_l((char)i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_3300
 * @tc.desc      : Verify iswblank_l process success. when using the zh_CN.UTF-8 character set.
 *                 the parameter c is one of a space in {L'\t', L' '},
 *                 and the input wide character is judged to be a space.
 * @tc.level     : Level 0
 */
void iswblank_l_3300(void)
{
    int ret = RET_FALSE;
    const wchar_t arr[] = {L'\t', L' '};
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for(unsigned int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(!ret) {
            t_printf("Test case %s failed while testing character, index:%d\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }
    
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_3400
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a wide char control character,
 *                  which determines that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_3400(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"\r\n\b\0\f";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_3500
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set 
 *                 When the parameter c is a wide char English letter,
 *                 and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_3500(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_3600
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set
 *                 When the parameter c is a wide char alphanumeric,
 *                 and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_3600(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"1234567890";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_3700
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set
 *                 When the parameter c is a wide char special character,
 *           and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_3700(void)
{
    int ret = RET_FALSE;
    const wchar_t *str = L"!@#$%^()+{}[]";
    const wchar_t *p = str;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if(!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while(*p && *p != L'\0') {
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_3800
 * @tc.desc      : Verify iswblank_l process success when using the zh_CN.UTF-8 character set
 *                 When the parameter c is a SPACE-like character,
 * @tc.level     : Level 0
 */
void iswblank_l_3800(void)
{
    int ret = RET_FALSE;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    /*
    * 0x1680:   OGHAM SPACE MARK
    * 0x2002:   EN SPACE
    * 0x2003:   EM SPACE
    * 0x2004:   THREE-PER-EM SPACE
    * 0x2005:   FOUR-PER-EM SPACE
    * 0x2006:   SIX-PER-EM SPACE
    * 0x2007:   FIGURE SPACE
    * 0x2008:   PUNCTUATION SPACE
    * 0x2009:   THIN SPACE
    * 0x200a:   HAIR SPACE
    * 0x202f:   NARROW NO-BREAK SPACE
    * 0x205f:   MEDIUM MATHEMATICAL SPACE
    * 0x3000:   IDEOGRAPHIC SPACE
    */
    const wchar_t arr[] = {0x1680, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200a, 0x202f, 0x205f, 0x3000};
    
    for(int i = 0; i < sizeof(arr)/sizeof(wchar_t); i++) {
        ret = iswblank_l(arr[i], m_locale);
        if(ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, OTHER_COUNT);

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_3900
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set
 *                 When the parameter c is a CJK RADICAL or Arabic character,
 *           and it is judged that the input wide character is not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_3900(void)
{
    int ret = RET_FALSE;
    int i = 0;
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //CJK RADICAL characters(U+2E80 - U+2EF3)
    for (i = 0x2E80; i <= 0x2EF3; i++) {
        ret = iswblank_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    //Arabic characters(U+0600 - U+06FF)
    for (i = 0x0660; i <= 0x0669; i++) {
        ret = iswblank_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswblank_l_4000
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a CJK Ideograph character,
 *                 and the input character is judged to be not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_4000(void)
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
        ret = iswblank_l(*p, m_locale);
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
 * @tc.name      : iswblank_l_4100
 * @tc.desc      : Verify iswblank_l process fail when using the zh_CN.UTF-8 character set.
 *                 the parameter c is a LATIN SMALL LETTER,
 *                 and the input character is judged to be not a space.
 * @tc.level     : Level 2
 */
void iswblank_l_4100(void)
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
        ret = iswblank_l(*p, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

int main(void)
{
    iswblank_l_0100();
    iswblank_l_0200();
    iswblank_l_0300();
    iswblank_l_0400();
    iswblank_l_0500();
    iswblank_l_0600();
    iswblank_l_0700();
    iswblank_l_0800();
    iswblank_l_0900();
    iswblank_l_1000();
    iswblank_l_1100();
    iswblank_l_1200();
    iswblank_l_1300();
    iswblank_l_1400();
    iswblank_l_1500();
    iswblank_l_1600();
    iswblank_l_1700();
    iswblank_l_1800();
    iswblank_l_1900();
    iswblank_l_2000();
    iswblank_l_2100();
    iswblank_l_2200();
    iswblank_l_2300();
    iswblank_l_2400();
    iswblank_l_2500();
    iswblank_l_2600();
    iswblank_l_2700();
    iswblank_l_2800();
    iswblank_l_2900();
    iswblank_l_3000();
    iswblank_l_3100();
    iswblank_l_3200();
    iswblank_l_3300();
    iswblank_l_3400();
    iswblank_l_3500();
    iswblank_l_3600();
    iswblank_l_3700();
    iswblank_l_3800();
    iswblank_l_3900();
    iswblank_l_4000();
    iswblank_l_4100();
    return t_status;
}