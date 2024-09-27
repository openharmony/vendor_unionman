#include <wctype.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

static const unsigned char table[] = {
#include "alpha.h"
};

int iswalpha(wint_t wc)
{
	if (wc<0x20000U)
		return (table[table[wc>>8]*32+((wc&255)>>3)]>>(wc&7))&1;
	if (wc<0x2fffeU)
		return 1;
	return 0;
}

int __iswalpha_l(wint_t c, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isalpha), ICU_UCHAR_ISALPHA_SYMBOL);
		if (g_icu_opt_func.u_isalpha) {
			return g_icu_opt_func.u_isalpha(c);
		}
	}
#endif
	return iswalpha(c);
}

weak_alias(__iswalpha_l, iswalpha_l);
