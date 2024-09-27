#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

int iswupper(wint_t wc)
{
	return towlower(wc) != wc;
}

int __iswupper_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isupper), ICU_UCHAR_ISUPPER_SYMBOL);
		if (g_icu_opt_func.u_isupper) {
			return g_icu_opt_func.u_isupper(c);
		}
	}
#endif
	return iswupper(c);
}

weak_alias(__iswupper_l, iswupper_l);
