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
 * @tc.name      : towlower_l_0100
 * @tc.desc      : Verify towlower_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters are Uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_0100(void)
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
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_0200
 * @tc.desc      : Verify towlower_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as numbers, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_0200(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_0300
 * @tc.desc      : Verify towlower_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_0300(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_0400
 * @tc.desc      : Verify towlower_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters as Lowercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_0400(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const char *lowstr = "abcdefghijklmnopqrstuvwxyz";
    int result = RET_FALSE;
    const char *p = lowstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_0500
 * @tc.desc      : Verify towlower_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters are wide Uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_0500(void)
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
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_0600
 * @tc.desc      : Verify towlower_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as wide numbers, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_0600(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_0700
 * @tc.desc      : Verify towlower_l process fail when using the en_US.UTF-8 character set.
 *                 Input parameters as wide special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_0700(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_0800
 * @tc.desc      : Verify towlower_l process success when using the en_US.UTF-8 character set.
 *                 Input parameters as wide Lowercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_0800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    const wchar_t *lowstr = L"abcdefghijklmnopqrstuvwxyz";
    int result = RET_FALSE;
    const wchar_t *p = lowstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_0900
 * @tc.desc      : Verify towlower_l process success when using the zh_CN character set.
 *                 Input parameters are uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_0900(void)
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
    while (up && *up != '\0'&& lp && *lp != '\0') {
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (char)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_1000
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 Input parameters as numbers, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1000(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_1100
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 Input parameters as special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1100(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_1200
 * @tc.desc      : Verify towlower_l process success when using the zh_CN character set.
 *                 Input parameters as Lowercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const char *lowstr = "abcdefghijklmnopqrstuvwxyz";
    int result = RET_FALSE;
    const char *p = lowstr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_1300
 * @tc.desc      : Verify towlower_l process success when using the zh_CN character set.
 *                 Input parameters are wide Uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_1300(void)
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
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_1400
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 Input parameters as wide numbers, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1400(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_1500
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 Input parameters as wide special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1500(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_1600
 * @tc.desc      : Verify towlower_l process success when using the zh_CN character set.
 *                 Input parameters as wide Lowercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *lowstr = L"abcdefghijklmnopqrstuvwxyz";
    int result = RET_FALSE;
    const wchar_t *p = lowstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_1700
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1700(void)
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
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    //GREEK characters(U+03AC - U+03CE)
    //GREEK SMALL LETTER
    for (i = 0x03AC; i <= 0x03CE; i++) {
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_1800
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 verify conversion to Lowercase letters..
 * @tc.level     : Level 2
 */
void towlower_l_1800(void)
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
        result = towlower_l(i, m_locale);
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
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_1900
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 verify BOPOMOFO LETTER conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_1900(void)
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
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);

    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_2000
 * @tc.desc      : Verify towlower_l process success when using the zh_CN character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN character sets,
 *                 verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_2000(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //ARMENIAN characters(U+1D360 - U+1D37F)
    //ARMENIAN UPPERCASE LETTER(0x0531-0x0556)
    //ARMENIAN LOWERCASR LETTER(0x0561-0x0586)
    for (int i = 0x0531,j = 0x0561; i <= 0x0556; i++,j++) {
        result = towlower_l(i, m_locale);
        if (result != j) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != j) ? RET_FALSE : RET_TRUE, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_2100
 * @tc.desc      : Verify towlower_l process success when using the zh_CN character set.
 *                 Input parameters are CYRILLIC CAPITAL LETTER, verify conversion to CYRILLIC SMALL LETTER.
 * @tc.level     : Level 0
 */
void towlower_l_2100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *upstr = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    const wchar_t *lowstr = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    const wchar_t *up = upstr;
    const wchar_t *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (up && *up != L'\0' && lp && *lp != L'\0') {
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_2200
 * @tc.desc      : Verify towlower_l process success when using the zh_CN character set.
 *                 Input parameters are GREEK CAPITAL LETTER, verify conversion to GREEK SMALL LETTER.
 * @tc.level     : Level 0
 */
void towlower_l_2200(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *upstr =  L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ";
    const wchar_t *lowstr = L"αβγδεζηθικλμνξοπρστυφχψω";
    const wchar_t *up = upstr;
    const wchar_t *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (up && *up != L'\0' && lp && *lp != L'\0') {
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_2300
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN character set.
 *                 Input parameters as special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_2300(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN", NULL);
    const wchar_t *str = L"₽&$€£¥₩";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //RUBLE SIGN
    //AMPERSAND
    //DOLLAR SIGN
    //EURO SIGN
    //POUND SIGN
    //YEN SIGN
    //WON SIGN
    while (p && *p != L'\0') {
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_2400
 * @tc.desc      : Verify towlower_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters are uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_2400(void)
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
    while (up && *up != '\0'&& lp && *lp != '\0') {
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_2500
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as numbers, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_2500(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_2600
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_2600(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_2700
 * @tc.desc      : Verify towlower_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters as Lowercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_2700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const char *lowstr = "abcdefghijklmnopqrstuvwxyz";
    int result = RET_FALSE;
    const char *p = lowstr;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != '\0') {
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_2800
 * @tc.desc      : Verify towlower_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters are wide Uppercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_2800(void)
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
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_2900
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as wide numbers, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_2900(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_3000
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as wide special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_3000(void)
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
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_3100
 * @tc.desc      : Verify towlower_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters as wide Lowercase letters, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_3100(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *lowstr = L"abcdefghijklmnopqrstuvwxyz";
    int result = RET_FALSE;
    const wchar_t *p = lowstr;
 
    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (p && *p != L'\0') {
        result = towlower_l(*p, m_locale);
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
 * @tc.name      : towlower_l_3200
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_3200(void)
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
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    //GREEK characters(U+03AC - U+03CE)
    //GREEK SMALL LETTER
    for (i = 0x03AC; i <= 0x03CE; i++) {
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_3300
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 verify conversion to Lowercase letters..
 * @tc.level     : Level 2
 */
void towlower_l_3300(void)
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
        result = towlower_l(i, m_locale);
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
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_3400
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 verify BOPOMOFO LETTER conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_3400(void)
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
        result = towlower_l(i, m_locale);
        if (result != i) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != i) ? RET_TRUE : RET_FALSE, RET_FALSE);

    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_3500
 * @tc.desc      : Verify towlower_l process success when using the zh_CN.UTF-8 character set.
 *                 The characters are not in the en_US.UTF-8 and zh_CN.UTF-8 character sets,
 *                 verify conversion to Lowercase letters.
 * @tc.level     : Level 0
 */
void towlower_l_3500(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //ARMENIAN characters(U+1D360 - U+1D37F)
    //ARMENIAN UPPERCASE LETTER(0x0531-0x0556)
    //ARMENIAN LOWERCASR LETTER(0x0561-0x0586)
    for (int i = 0x0531,j = 0x0561; i <= 0x0556; i++,j++) {
        result = towlower_l(i, m_locale);
        if (result != j) {
            t_printf("Test case %s failed while testing unicode %04x\n", __FUNCTION__, i);
        }
        EXPECT_EQ(__FUNCTION__, (result != j) ? RET_FALSE : RET_TRUE, RET_TRUE);
    }

    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_3600
 * @tc.desc      : Verify towlower_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters are CYRILLIC CAPITAL LETTER, verify conversion to CYRILLIC SMALL LETTER.
 * @tc.level     : Level 0
 */
void towlower_l_3600(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *upstr = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    const wchar_t *lowstr = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    const wchar_t *up = upstr;
    const wchar_t *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (up && *up != L'\0' && lp && *lp != L'\0') {
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_3800
 * @tc.desc      : Verify towlower_l process success when using the zh_CN.UTF-8 character set.
 *                 Input parameters are GREEK CAPITAL LETTER, verify conversion to GREEK SMALL LETTER.
 * @tc.level     : Level 0
 */
void towlower_l_3700(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *upstr =  L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ";
    const wchar_t *lowstr = L"αβγδεζηθικλμνξοπρστυφχψω";
    const wchar_t *up = upstr;
    const wchar_t *lp = lowstr;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    while (up && *up != L'\0' && lp && *lp != L'\0') {
        result = towlower_l(*up, m_locale);
        if (result != *lp) {
            t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *lp);
        }
        EXPECT_EQ(__FUNCTION__, (result != *lp) ? RET_FALSE : RET_TRUE, RET_TRUE);
        up++;
        lp++;
    }
    freelocale(m_locale);
    m_locale = NULL;
}

/**
 * @tc.name      : towlower_l_3700
 * @tc.desc      : Verify towlower_l process fail when using the zh_CN.UTF-8 character set.
 *                 Input parameters as special characte, verify conversion to Lowercase letters.
 * @tc.level     : Level 2
 */
void towlower_l_3800(void)
{
    locale_t m_locale = newlocale(LC_ALL_MASK, "zh_CN.UTF-8", NULL);
    const wchar_t *str = L"₽&$€£¥₩";
    const wchar_t *p = str;
    int result = RET_FALSE;

    if (!m_locale) {
        t_printf("Test case %s failed to call function newlocale\n", __FUNCTION__);
        EXPECT_NE(__FUNCTION__, result, RET_FALSE);
        return ;
    }

    //RUBLE SIGN
    //AMPERSAND
    //DOLLAR SIGN
    //EURO SIGN
    //POUND SIGN
    //YEN SIGN
    //WON SIGN
    while (p && *p != L'\0') {
        result = towlower_l(*p, m_locale);
        if (result != *p) {
           t_printf("Test case %s failed, get result is %c but want get value %c\n ", __FUNCTION__, (wchar_t)result, *p);
        }
        EXPECT_EQ(__FUNCTION__, (result != *p) ? RET_TRUE : RET_FALSE, RET_FALSE);
        p++;
    }

    freelocale(m_locale);
    m_locale = NULL;
}

int main(int argc, char *argv[])
{
    towlower_l_0100();
    towlower_l_0200();
    towlower_l_0300();
    towlower_l_0400();
    towlower_l_0500();
    towlower_l_0600();
    towlower_l_0700();
    towlower_l_0800();
    towlower_l_0900();
    towlower_l_1000();
    towlower_l_1100();
    towlower_l_1200();
    towlower_l_1300();
    towlower_l_1400();
    towlower_l_1500();
    towlower_l_1600();
    towlower_l_1700();
    towlower_l_1800();
    towlower_l_1900();
    towlower_l_2000();
    towlower_l_2100();
    towlower_l_2200();
    towlower_l_2300();
    towlower_l_2400();
    towlower_l_2500();
    towlower_l_2600();
    towlower_l_2700();
    towlower_l_2800();
    towlower_l_2900();
    towlower_l_3000();
    towlower_l_3100();
    towlower_l_3200();
    towlower_l_3300();
    towlower_l_3400();
    towlower_l_3500();
    towlower_l_3600();
    towlower_l_3700();
    towlower_l_3800();
    return t_status;
}
