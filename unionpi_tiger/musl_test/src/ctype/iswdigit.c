#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

#undef iswdigit

int iswdigit(wint_t wc)
{
	return (unsigned)wc-'0' < 10;
}

int __iswdigit_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isdigit), ICU_UCHAR_ISDIGIT_SYMBOL);
		if (g_icu_opt_func.u_isdigit) {
			return g_icu_opt_func.u_isdigit(c);
		}
	}
#endif
	return iswdigit(c);
}

weak_alias(__iswdigit_l, iswdigit_l);
