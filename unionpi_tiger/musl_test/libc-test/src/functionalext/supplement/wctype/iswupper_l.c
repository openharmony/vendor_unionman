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

const int COUNT = 26;
const int SIZE = 128;
const int RET_TRUE = 1;
const int RET_FALSE = 0;

/**
 * @tc.name      : iswupper_l_0100
 * @tc.desc      : Verify iswupper_l process success when using the en_US.UTF-8 character set.
 *                 The parameter c is an English letter,
 *                 and it is judged that the input character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_0100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_0200
 * @tc.desc      : Verify iswupper_l process fail when using the en_US.UTF-8 character set.
 *                 The parameter c is an English letter,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_0200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "abcdefghijklmnopqrstuvwxyz";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_0300
 * @tc.desc      : Verify iswupper_l process fail when using the en_US.UTF-8 character set.
 *                 The parameter c is an English number,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_0300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "0123456789";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_0400
 * @tc.desc      : Verify iswupper_l process fail when using the en_US.UTF-8 character set.
 *                 The parameter c is an special character,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_0400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "@#$%^&";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_0500
 * @tc.desc      : Verify iswupper_l process success when using the en_US.UTF-8 character set.
 *                 Determine the number of Uppercase letter in the ascii code table.
 * @tc.level     : Level 1
 */
void iswupper_l_0500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    int ret = RET_FALSE;
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_0600
 * @tc.desc      : Verify iswupper_l process success when using the en_US.UTF-8 character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_0600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_0700
 * @tc.desc      : Verify iswupper_l process fail when using the en_US.UTF-8 character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_0700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyz";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_0800
 * @tc.desc      : Verify iswupper_l process fail when using the en_US.UTF-8 character set.
 *                 The parameter c is an wide English number,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_0800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"0123456789";
    const wchar_t *p = str;
    int ret = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_0900
 * @tc.desc      : Verify iswupper_l process fail when using the en_US.UTF-8 character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_0900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"@#$%^&";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_1000
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN character set.
 *                 The parameter c is an English letter,
 *                 and it is judged that the input character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_1000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_1100
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an English letter,
 *                 and it is judged that the input character not is a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_1100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "abcdefghijklmnopqrstuvwxyz";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_1200
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an English number,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_1200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "0123456789";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_1300
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an special character,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_1300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "@#$%^&";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_1400
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN character set.
 *                 Determine the number of Uppercase letters in the ascii code table.
 * @tc.level     : Level 1
 */
void iswupper_l_1400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int ret = RET_FALSE;
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_1500
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_1500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_1600
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_1600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyz";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_1700
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_1700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"0123456789";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_1800
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_1800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"@#$%^&";
    const wchar_t *p = str;
    int ret  = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_1900
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 and it is judged that the input character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_1900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //ARMENIAN characters(U+0531 - U+0556)
    //ARMENIAN CAPITAL LETTER
    for (int i = 0x0531; i <= 0x0556; i++) {
        ret = iswupper_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_2000
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_2000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int ret = RET_FALSE;
    int i = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //GREEK TONOS
    //GREEK DIALYTIKA TONOS
    for (i = 0x0384; i <= 0x0385; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    //ARMENIAN APOSTROPHE
    //ARMENIAN EMPHASIS MARK
    //ARMENIAN EXCLAMATION MARK
    //ARMENIAN COMMA
    //ARMENIAN QUESTION MARK
    //ARMENIAN ABBREVIATION MARK
    for (i = 0x055A; i <= 0x055F; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_2100
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 BOPOMOFO LETTER in the unicode is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_2100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int ret = RET_FALSE;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //0x3105(BOPOMOFO LETTER B) ~ 0x312f(BOPOMOFO LETTER NN)
    for (int i = 0x3105; i <= 0x312f; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed i = 0x%x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_2200
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN character set.
 *                 The parameter c is an CYRILLIC LETTER,
 *                 and it is judged that the input wide character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_2200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_2300
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an CYRILLIC LETTER,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_2300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_2400
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN character set.
 *                 The parameter c is an GREEK LETTER,
 *                 and it is judged that the input wide character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_2400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_2500
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN character set.
 *                 The parameter c is an GREEK LETTER,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_2500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"αβγδεζηθικλμνξοπρστυφχψω";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_2600
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an English letter,
 *                 and it is judged that the input character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_2600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_2700
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an English letter,
 *                 and it is judged that the input character not is a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_2700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "abcdefghijklmnopqrstuvwxyz";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_2800
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an English number,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_2800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "0123456789";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_2900
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an special character,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_2900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "@#$%^&";
    const char *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_3000
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN.UTF-8 character set.
 *                 Determine the number of Uppercase letters in the ascii code table.
 * @tc.level     : Level 1
 */
void iswupper_l_3000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int ret = RET_FALSE;
    int total = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    for (int i = 0; i < SIZE; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            total++;
        }
    }

    EXPECT_EQ(__FUNCTION__, total, COUNT);
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_3100
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_3100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_3200
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_3200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"abcdefghijklmnopqrstuvwxyz";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_3300
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_3300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"0123456789";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_3400
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an wide English character,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_3400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"@#$%^&";
    const wchar_t *p = str;
    int ret  = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_3500
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 and it is judged that the input character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_3500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //ARMENIAN characters(U+0531 - U+0556)
    //ARMENIAN CAPITAL LETTER
    for (int i = 0x0531; i <= 0x0556; i++) {
        ret = iswupper_l(i, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_3600
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 and it is judged that the input character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_3600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int ret = RET_FALSE;
    int i = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //GREEK TONOS
    //GREEK DIALYTIKA TONOS
    for (i = 0x0384; i <= 0x0385; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    //ARMENIAN APOSTROPHE
    //ARMENIAN EMPHASIS MARK
    //ARMENIAN EXCLAMATION MARK
    //ARMENIAN COMMA
    //ARMENIAN QUESTION MARK
    //ARMENIAN ABBREVIATION MARK
    for (i = 0x055A; i <= 0x055F; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_3700
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 BOPOMOFO LETTER in the unicode is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_3700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int ret = RET_FALSE;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    //0x3105(BOPOMOFO LETTER B) ~ 0x312f(BOPOMOFO LETTER NN)
    for (int i = 0x3105; i <= 0x312f; i++) {
        ret = iswupper_l(i, m_locale);
        if (ret) {
            t_printf("Test case %s failed i = 0x%x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_3800
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an CYRILLIC LETTER,
 *                 and it is judged that the input wide character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_3800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_3900
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an CYRILLIC LETTER,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_3900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
 * @tc.name      : iswupper_l_4000
 * @tc.desc      : Verify iswupper_l process success when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an GREEK LETTER,
 *                 and it is judged that the input wide character is a Uppercase letter.
 * @tc.level     : Level 0
 */
void iswupper_l_4000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
        if (!ret) {
            t_printf("Test case %s failed while testing character %c\n", __FUNCTION__, *p);
        }
        EXPECT_EQ(__FUNCTION__, ret, RET_TRUE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : iswupper_l_4100
 * @tc.desc      : Verify iswupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The parameter c is an GREEK LETTER,
 *                 and it is judged that the input wide character is not a Uppercase letter.
 * @tc.level     : Level 2
 */
void iswupper_l_4100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"αβγδεζηθικλμνξοπρστυφχψω";
    const wchar_t *p = str;
    int ret = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, ret, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        ret = iswupper_l(*p, m_locale);
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
    iswupper_l_0100();
    iswupper_l_0200();
    iswupper_l_0300();
    iswupper_l_0400();
    iswupper_l_0500();
    iswupper_l_0600();
    iswupper_l_0700();
    iswupper_l_0800();
    iswupper_l_0900();
    iswupper_l_1000();
    iswupper_l_1100();
    iswupper_l_1200();
    iswupper_l_1300();
    iswupper_l_1400();
    iswupper_l_1500();
    iswupper_l_1600();
    iswupper_l_1700();
    iswupper_l_1800();
    iswupper_l_1900();
    iswupper_l_2000();
    iswupper_l_2100();
    iswupper_l_2200();
    iswupper_l_2300();
    iswupper_l_2400();
    iswupper_l_2500();
    iswupper_l_2600();
    iswupper_l_2700();
    iswupper_l_2800();
    iswupper_l_2900();
    iswupper_l_3000();
    iswupper_l_3100();
    iswupper_l_3200();
    iswupper_l_3300();
	iswupper_l_3400();
	iswupper_l_3500();
	iswupper_l_3600();
	iswupper_l_3700();
	iswupper_l_3800();
	iswupper_l_3900();
	iswupper_l_4000();
	iswupper_l_4100();
    return t_status;
}
