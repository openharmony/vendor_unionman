#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

int iswgraph(wint_t wc)
{
	/* ISO C defines this function as: */
	return !iswspace(wc) && iswprint(wc);
}

int __iswgraph_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isgraph), ICU_UCHAR_ISGRAPH_SYMBOL);
		if (g_icu_opt_func.u_isgraph) {
			return g_icu_opt_func.u_isgraph(c);
		}
	}
#endif
	return iswgraph(c);
}

weak_alias(__iswgraph_l, iswgraph_l);
