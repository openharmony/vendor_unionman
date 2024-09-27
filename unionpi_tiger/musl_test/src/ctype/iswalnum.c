#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

int iswalnum(wint_t wc)
{
	return iswdigit(wc) || iswalpha(wc);
}

int __iswalnum_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isalnum), ICU_UCHAR_ISALNUM_SYMBOL);
		if (g_icu_opt_func.u_isalnum) {
			g_icu_opt_func.u_isalnum(c);
		}
	}
#endif
	return iswalnum(c);
}

weak_alias(__iswalnum_l, iswalnum_l);
