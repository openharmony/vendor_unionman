#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

int iswxdigit(wint_t wc)
{
	return (unsigned)(wc-'0') < 10 || (unsigned)((wc|32)-'a') < 6;
}

int __iswxdigit_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isxdigit), ICU_UCHAR_ISXDIGIT_SYMBOL);
		if (g_icu_opt_func.u_isxdigit) {
			return g_icu_opt_func.u_isxdigit(c);
		}
	}
#endif
	return iswxdigit(c);
}

weak_alias(__iswxdigit_l, iswxdigit_l);
