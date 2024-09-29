#include <wctype.h>
#include <string.h>
#ifdef FEATURE_ICU_LOCALE_TMP
#include <locale.h>
#include "locale_impl.h"
#endif

#define WCTYPE_ALNUM  1
#define WCTYPE_ALPHA  2
#define WCTYPE_BLANK  3
#define WCTYPE_CNTRL  4
#define WCTYPE_DIGIT  5
#define WCTYPE_GRAPH  6
#define WCTYPE_LOWER  7
#define WCTYPE_PRINT  8
#define WCTYPE_PUNCT  9
#define WCTYPE_SPACE  10
#define WCTYPE_UPPER  11
#define WCTYPE_XDIGIT 12

int iswctype(wint_t wc, wctype_t type)
{
	switch (type) {
	case WCTYPE_ALNUM:
		return iswalnum(wc);
	case WCTYPE_ALPHA:
		return iswalpha(wc);
	case WCTYPE_BLANK:
		return iswblank(wc);
	case WCTYPE_CNTRL:
		return iswcntrl(wc);
	case WCTYPE_DIGIT:
		return iswdigit(wc);
	case WCTYPE_GRAPH:
		return iswgraph(wc);
	case WCTYPE_LOWER:
		return iswlower(wc);
	case WCTYPE_PRINT:
		return iswprint(wc);
	case WCTYPE_PUNCT:
		return iswpunct(wc);
	case WCTYPE_SPACE:
		return iswspace(wc);
	case WCTYPE_UPPER:
		return iswupper(wc);
	case WCTYPE_XDIGIT:
		return iswxdigit(wc);
	}
	return 0;
}

wctype_t wctype(const char *s)
{
	int i;
	const char *p;
	/* order must match! */
	static const char names[] =
		"alnum\0" "alpha\0" "blank\0"
		"cntrl\0" "digit\0" "graph\0"
		"lower\0" "print\0" "punct\0"
		"space\0" "upper\0" "xdigit";
	for (i=1, p=names; *p; i++, p+=6)
		if (*s == *p && !strcmp(s, p))
			return i;
	return 0;
}

int __iswctype_l(wint_t c, wctype_t t, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		switch (t) {
			case WCTYPE_ALNUM:
				return iswalnum_l(c, l);
			case WCTYPE_ALPHA:
				return iswalpha_l(c, l);
			case WCTYPE_BLANK:
				return iswblank_l(c, l);
			case WCTYPE_CNTRL:
				return iswcntrl_l(c, l);
			case WCTYPE_DIGIT:
				return iswdigit_l(c, l);
			case WCTYPE_GRAPH:
				return iswgraph_l(c, l);
			case WCTYPE_LOWER:
				return iswlower_l(c, l);
			case WCTYPE_PRINT:
				return iswprint_l(c, l);
			case WCTYPE_PUNCT:
				return iswpunct_l(c, l);
			case WCTYPE_SPACE:
				return iswspace_l(c, l);
			case WCTYPE_UPPER:
				return iswupper_l(c, l);
			case WCTYPE_XDIGIT:
				return iswxdigit_l(c, l);
			default:
				break;
		}
	}
#endif
	return iswctype(c, t);
}

wctype_t __wctype_l(const char *s, locale_t l)
{
#ifdef FEATURE_ICU_LOCALE_TMP
	if (l && l->cat[LC_CTYPE] && l->cat[LC_CTYPE]->flag == ICU_VALID) {
		if (!strcmp(s, "alnum")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isalnum), ICU_UCHAR_ISALNUM_SYMBOL);
			if (g_icu_opt_func.u_isalnum) {
				return WCTYPE_ALNUM;
			}
		} else if (!strcmp(s, "alpha")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isalpha), ICU_UCHAR_ISALPHA_SYMBOL);
			if (g_icu_opt_func.u_isalpha) {
				return WCTYPE_ALPHA;
			}
		} else if (!strcmp(s, "blank")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isblank), ICU_UCHAR_ISBLANK_SYMBOL);
			if (g_icu_opt_func.u_isblank) {
				return WCTYPE_BLANK;
			}
		} else if (!strcmp(s, "cntrl")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_iscntrl), ICU_UCHAR_ISCNTRL_SYMBOL);
			if (g_icu_opt_func.u_iscntrl) {
				return WCTYPE_CNTRL;
			}
		} else if (!strcmp(s, "digit")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isdigit), ICU_UCHAR_ISDIGIT_SYMBOL);
			if (g_icu_opt_func.u_isdigit) {
				return WCTYPE_DIGIT;
			}
		} else if (!strcmp(s, "graph")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isgraph), ICU_UCHAR_ISGRAPH_SYMBOL);
			if (g_icu_opt_func.u_isgraph) {
				return WCTYPE_GRAPH;
			}
		} else if (!strcmp(s, "lower")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_islower), ICU_UCHAR_ISLOWER_SYMBOL);
			if (g_icu_opt_func.u_islower) {
				return WCTYPE_LOWER;
			}
		} else if (!strcmp(s, "print")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isprint), ICU_UCHAR_ISPRINT_SYMBOL);
			if (g_icu_opt_func.u_isprint) {
				return WCTYPE_PRINT;
			}
		} else if (!strcmp(s, "punct")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isgraph), ICU_UCHAR_ISGRAPH_SYMBOL);
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isalnum), ICU_UCHAR_ISALNUM_SYMBOL);
			if (g_icu_opt_func.u_isgraph && g_icu_opt_func.u_isalnum) {
				return WCTYPE_PUNCT;
			}
		} else if (!strcmp(s, "space")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isspace), ICU_UCHAR_ISSPACE_SYMBOL);
			if (g_icu_opt_func.u_isspace) {
				return WCTYPE_SPACE;
			}
		} else if (!strcmp(s, "upper")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isupper), ICU_UCHAR_ISUPPER_SYMBOL);
			if (g_icu_opt_func.u_isupper) {
				return WCTYPE_UPPER;
			}
		} else if (!strcmp(s, "xdigit")) {
			get_icu_symbol(ICU_I18N, &(g_icu_opt_func.u_isxdigit), ICU_UCHAR_ISXDIGIT_SYMBOL);
			if (g_icu_opt_func.u_isxdigit) {
				return WCTYPE_XDIGIT;
			}
		}
	}
#endif
	return wctype(s);
}

weak_alias(__iswctype_l, iswctype_l);
weak_alias(__wctype_l, wctype_l);
