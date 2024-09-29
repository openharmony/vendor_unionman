#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

int iswcntrl(wint_t wc)
{
	return (unsigned)wc < 32
	    || (unsigned)(wc-0x7f) < 33
	    || (unsigned)(wc-0x2028) < 2
	    || (unsigned)(wc-0xfff9) < 3;
}

int __iswcntrl_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_iscntrl), ICU_UCHAR_ISCNTRL_SYMBOL);
		if (g_icu_opt_func.u_iscntrl) {
			return g_icu_opt_func.u_iscntrl(c);
		}
	}
#endif
	return iswcntrl(c);
}

weak_alias(__iswcntrl_l, iswcntrl_l);
