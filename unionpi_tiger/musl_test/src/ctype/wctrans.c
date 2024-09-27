#include <wctype.h>
#include <string.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"

#define TOWCTRANS_UPPER	1
#define TOWCTRANS_LOWER	2
#endif

wctrans_t wctrans(const char *class)
{
	if (!strcmp(class, "toupper")) return (wctrans_t)1;
	if (!strcmp(class, "tolower")) return (wctrans_t)2;
	return 0;
}

wint_t towctrans(wint_t wc, wctrans_t trans)
{
	if (trans == (wctrans_t)1) return towupper(wc);
	if (trans == (wctrans_t)2) return towlower(wc);
	return wc;
}

wctrans_t __wctrans_l(const char *s, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		if (!strcmp(s, "toupper")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_toupper), ICU_UCHAR_TOUPPER_SYMBOL);
			if (g_icu_opt_func.u_toupper) {
				return (wctrans_t)TOWCTRANS_UPPER;
			}
		} else if (!strcmp(s, "tolower")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_tolower), ICU_UCHAR_TOLOWER_SYMBOL);
			if (g_icu_opt_func.u_tolower) {
				return (wctrans_t)TOWCTRANS_LOWER;
			}
		}
	}
#endif
	return wctrans(s);
}

wint_t __towctrans_l(wint_t c, wctrans_t t, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		if (t == (wctrans_t)TOWCTRANS_UPPER) return towupper_l(c, l);
		if (t == (wctrans_t)TOWCTRANS_LOWER) return towlower_l(c, l);
	}
#endif
	return towctrans(c, t);
}

weak_alias(__wctrans_l, wctrans_l);
weak_alias(__towctrans_l, towctrans_l);
