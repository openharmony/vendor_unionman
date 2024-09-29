#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

static const unsigned char table[] = {
#include "punct.h"
};

int iswpunct(wint_t wc)
{
	if (wc<0x20000U)
		return (table[table[wc>>8]*32+((wc&255)>>3)]>>(wc&7))&1;
	return 0;
}

int __iswpunct_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isgraph), ICU_UCHAR_ISGRAPH_SYMBOL);
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isalnum), ICU_UCHAR_ISALNUM_SYMBOL);
		if (g_icu_opt_func.u_isgraph && g_icu_opt_func.u_isalnum) {
			return g_icu_opt_func.u_isgraph(c) && !g_icu_opt_func.u_isalnum(c);
		}
	}
#endif
	return iswpunct(c);
}

weak_alias(__iswpunct_l, iswpunct_l);
