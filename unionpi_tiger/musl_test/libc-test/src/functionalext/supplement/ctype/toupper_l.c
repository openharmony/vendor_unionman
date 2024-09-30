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
#include "functionalext.h"

const int RET_TRUE = 1;
const int RET_FALSE = 0;

/**
 * @tc.name      : toupper_l_0100
 * @tc.desc      : Verify toupper_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters are Lowercase letters, verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_0100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *upstr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lowstr = "abcdefghijklmnopqrstuvwxyz";
    const char *up = upstr;
    const char *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }
    while (up && *up != '\0' && lp && *lp != '\0') {
        result = toupper_l(*lp, m_locale);
        if (result != *up) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *up);
        }
        EXPECT_EQ(__FUNCTION__, (result != *up) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0200
 * @tc.desc      : Verify toupper_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as numbers, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_0200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "0123456789";
    const char *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0300
 * @tc.desc      : Verify toupper_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as special characte, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_0300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *str = "@#$%^&";
    const char *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0400
 * @tc.desc      : Verify toupper_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters as Uppercase letters, verify Uppercase to Lowercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_0400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *upstr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int result = RET_FALSE;
    const char *p = upstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0500
 * @tc.desc      : Verify toupper_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters are wide Lowercase letters, verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_0500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *upstr = 	L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *lowstr = L"abcdefghijklmnopqrstuvwxyz";
    const wchar_t *up = upstr;
    const wchar_t *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }
    while (up && *up != L'\0' && lp && *lp != L'\0') {
        result = toupper_l(*lp, m_locale);
        if (result != *up) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *up);
        }
        EXPECT_EQ(__FUNCTION__, (result != *up) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0600
 * @tc.desc      : Verify toupper_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as wide numbers, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_0600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"0123456789";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0700
 * @tc.desc      : Verify toupper_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as wide special characte, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_0700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *str = L"@#$%^&";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0800
 * @tc.desc      : Verify toupper_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters as wide Uppercase letters, verify Uppercase to Lowercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_0800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *upstr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int result = RET_FALSE;
    const wchar_t *p = upstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }


    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_0900
 * @tc.desc      : Verify toupper_l process success when using the zh_CN character set.
 *                 Input parameters are uppercase letters, verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_0900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *upstr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lowstr = "abcdefghijklmnopqrstuvwxyz";
    const char *up = upstr;
    const char *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }
    while (up && *up != '\0' && lp && *lp != '\0') {
        result = toupper_l(*lp, m_locale);
        if (result != *up) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *up);
        }
        EXPECT_EQ(__FUNCTION__, (result != *up) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1000
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN character set.
 *                 Input parameters as numbers, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "0123456789";
    const char *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1100
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN character set.
 *                 Input parameters as special characte, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *str = "@#$%^&";
    const char *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1200
 * @tc.desc      : Verify toupper_l process success when using the zh_CN character set.
 *                 Input parameters as Uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *upstr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int result = RET_FALSE;
    const char *p = upstr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }


    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1300
 * @tc.desc      : Verify toupper_l process success when using the zh_CN character set.
 *                 Input parameters are wide Lowercase letters, verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_1300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *upstr = 	L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *lowstr = L"abcdefghijklmnopqrstuvwxyz";
    const wchar_t *up = upstr;
    const wchar_t *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }
    while (up && *up != L'\0' && lp && *lp != L'\0') {
        result = toupper_l(*lp, m_locale);
        if (result != *up) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *up);
        }
        EXPECT_EQ(__FUNCTION__, (result != *up) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1400
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN character set.
 *                 Input parameters as wide numbers, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"0123456789";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1500
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN character set.
 *                 Input parameters as wide special characte, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"@#$%^&";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1600
 * @tc.desc      : Verify toupper_l process success when using the zh_CN character set.
 *                 Input parameters as wide Uppercase letters, verify Uppercase to Lowercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *upstr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int result = RET_FALSE;
    const wchar_t *p = upstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }


    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1700
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int result = RET_FALSE;
    int i = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //Arabic characters(U+0660 - U+0669)
    for (i = 0x0660; i <= 0x0669; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    //ARMENIAN characters(U+0531 - U+0556)
    //ARMENIAN CAPITAL LETTER
    for (i = 0x0531; i <= 0x0556; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1800
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 verify conversion to Uppercase letters..
 * @tc.level     : Level 2
 */
void toupper_l_1800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int result = RET_FALSE;
    int i = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //GREEK TONOS
    //GREEK DIALYTIKA TONOS
    for (i = 0x0384; i <= 0x0385; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    //ARMENIAN APOSTROPHE
    //ARMENIAN EMPHASIS MARK
    //ARMENIAN EXCLAMATION MARK
    //ARMENIAN COMMA
    //ARMENIAN QUESTION MARK
    //ARMENIAN ABBREVIATION MARK
    for (i = 0x055A; i <= 0x055F; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_1900
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN character set.
 *                 verify BOPOMOFO LETTER conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_1900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int result = RET_FALSE;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //0x3105(BOPOMOFO LETTER B) ~ 0x312f(BOPOMOFO LETTER NN)
    for (int i = 0x3105; i <= 0x312f; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);

    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2000
 * @tc.desc      : Verify toupper_l process success when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_2000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //GREEK characters(U+0370 - U+03FF)
    //A portion of consecutive GREEK LOWERCASR LETTER(0x03B1-0x03C1)
    //A portion of consecutive GREEK UPPERCASE LETTER((0x0391-0x03A1))
    for (int i = 0x03B1,j = 0x0391; i <= 0x03C1; i++,j++) {
        result = toupper_l(i, m_locale);
        if (result != j) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != j) ? RET_FALSE : RET_TRUE, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2100
 * @tc.desc      : Verify toupper_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters are uppercase letters, verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_2100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *upstr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *lowstr = "abcdefghijklmnopqrstuvwxyz";
    const char *up = upstr;
    const char *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }
    while (up && *up != '\0' && lp && *lp != '\0') {
        result = toupper_l(*lp, m_locale);
        if (result != *up) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *up);
        }
        EXPECT_EQ(__FUNCTION__, (result != *up) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2200
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as numbers, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_2200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "0123456789";
    const char *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2300
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as special characte, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_2300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *str = "@#$%^&";
    const char *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2400
 * @tc.desc      : Verify toupper_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters as Uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_2400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *upstr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int result = RET_FALSE;
    const char *p = upstr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }


    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2500
 * @tc.desc      : Verify toupper_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters are wide Lowercase letters, verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_2500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *upstr = 	L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t *lowstr = L"abcdefghijklmnopqrstuvwxyz";
    const wchar_t *up = upstr;
    const wchar_t *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }
    while (up && *up != L'\0' && lp && *lp != L'\0') {
        result = toupper_l(*lp, m_locale);
        if (result != *up) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *up);
        }
        EXPECT_EQ(__FUNCTION__, (result != *up) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2600
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as wide numbers, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_2600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"0123456789";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2700
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as wide special characte, verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_2700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"@#$%^&";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2800
 * @tc.desc      : Verify toupper_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters as wide Uppercase letters, verify Uppercase to Lowercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_2800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *upstr = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int result = RET_FALSE;
    const wchar_t *p = upstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = toupper_l(*p, m_locale);
        if (result != *p) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }


    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_2900
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 verify conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_2900(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int result = RET_FALSE;
    int i = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //Arabic characters(U+0660 - U+0669)
    for (i = 0x0660; i <= 0x0669; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    //ARMENIAN characters(U+0531 - U+0556)
    //ARMENIAN CAPITAL LETTER
    for (i = 0x0531; i <= 0x0556; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_3000
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 verify conversion to Uppercase letters..
 * @tc.level     : Level 2
 */
void toupper_l_3000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int result = RET_FALSE;
    int i = 0;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //GREEK TONOS
    //GREEK DIALYTIKA TONOS
    for (i = 0x0384; i <= 0x0385; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    //ARMENIAN APOSTROPHE
    //ARMENIAN EMPHASIS MARK
    //ARMENIAN EXCLAMATION MARK
    //ARMENIAN COMMA
    //ARMENIAN QUESTION MARK
    //ARMENIAN ABBREVIATION MARK
    for (i = 0x055A; i <= 0x055F; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_3100
 * @tc.desc      : Verify toupper_l process fail when using the zh_CN.UTF-8 character set.
 *                 verify BOPOMOFO LETTER conversion to Uppercase letters.
 * @tc.level     : Level 2
 */
void toupper_l_3100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int result = RET_FALSE;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //0x3105(BOPOMOFO LETTER B) ~ 0x312f(BOPOMOFO LETTER NN)
    for (int i = 0x3105; i <= 0x312f; i++) {
        result = toupper_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);

    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : toupper_l_3200
 * @tc.desc      : Verify toupper_l process success when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 verify conversion to Uppercase letters.
 * @tc.level     : Level 0
 */
void toupper_l_3200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //GREEK characters(U+0370 - U+03FF)
    //A portion of consecutive GREEK LOWERCASR LETTER(0x03B1-0x03C1)
    //A portion of consecutive GREEK UPPERCASE LETTER((0x0391-0x03A1))
    for (int i = 0x03B1,j = 0x0391; i <= 0x03C1; i++,j++) {
        result = toupper_l(i, m_locale);
        if (result != j) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != j) ? RET_FALSE : RET_TRUE, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

int main(int argc, char *argv[])
{
    toupper_l_0100();
    toupper_l_0200();
    toupper_l_0300();
    toupper_l_0400();
    toupper_l_0500();
    toupper_l_0600();
    toupper_l_0700();
    toupper_l_0800();
    toupper_l_0900();
    toupper_l_1000();
    toupper_l_1100();
    toupper_l_1200();
    toupper_l_1300();
    toupper_l_1400();
    toupper_l_1500();
    toupper_l_1600();
    toupper_l_1700();
    toupper_l_1800();
    toupper_l_1900();
    toupper_l_2000();
    toupper_l_2100();
    toupper_l_2200();
    toupper_l_2300();
    toupper_l_2400();
    toupper_l_2500();
    toupper_l_2600();
    toupper_l_2700();
    toupper_l_2800();
    toupper_l_2900();
    toupper_l_3000();
    toupper_l_3100();
    toupper_l_3200();
    return t_status;
}
