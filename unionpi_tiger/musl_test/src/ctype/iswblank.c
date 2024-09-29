#include <wctype.h>
#include <ctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

int iswblank(wint_t wc)
{
	return isblank(wc);
}

int __iswblank_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isblank), ICU_UCHAR_ISBLANK_SYMBOL);
		if (g_icu_opt_func.u_isblank) {
			return g_icu_opt_func.u_isblank(c);
		}
	}
#endif
	return iswblank(c);
}

weak_alias(__iswblank_l, iswblank_l);
