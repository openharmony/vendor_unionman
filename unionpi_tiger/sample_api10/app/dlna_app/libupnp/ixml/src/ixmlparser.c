/*******************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation
 * All rights reserved.
 * Copyright (c) 2012 France Telecom All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*!
 * \file
 */

#include "autoconfig.h"

#include "ixmlparser.h"

#include "ixmldebug.h"

#include <assert.h>
#include <stddef.h> /* for ptrdiff_t */
#include <stdio.h>
#include <stdlib.h> /* for free(), malloc() */
#include <string.h>

#include "posix_overwrites.h"

static char g_error_char = '\0';
#ifdef IXML_HAVE_SCRIPTSUPPORT
static IXML_BeforeFreeNode_t Before_Free_callback;
#endif

static const char LESSTHAN = '<';
static const char GREATERTHAN = '>';
static const char SLASH = '/';
static const char EQUALS = '=';
static const char QUOTE = '\"';
static const char SINGLEQUOTE = '\'';

static const char *WHITESPACE = "\n\t\r ";
static const char *COMPLETETAG = "/>";
static const char *ENDTAG = "</";
static const char *XMLDECL = "<?xml ";
static const char *XMLDECL2 = "<?xml?";
static const char *BEGIN_COMMENT = "<!--";
static const char *END_COMMENT = "-->";
static const char *BEGIN_PI = "<?";
static const char *END_PI = "?>";
static const char *BEGIN_DOCTYPE = "<!DOCTYPE";
static const char *CDSTART = "<![CDATA[";
static const char *CDEND = "]]>";
static const char *DEC_NUMBERS = "0123456789";
static const char *HEX_NUMBERS = "0123456789ABCDEFabcdef";
static const char *UTF8_BOM = "\xef\xbb\xbf";

typedef struct char_info
{
	unsigned short l;
	unsigned short h;
} char_info_t;

typedef char utf8char[8];

/*!
 * \brief The letter table contains all characters in XML 1.0 plus ":", "_" and
 * ideographic.
 *
 * This table contains all the characters that an element name can start with.
 * See XML 1.0 (2nd Edition) for more details.
 */
static char_info_t Letter[] = {{0x003A, 0x003A}, /* character ":" */
	{0x0041, 0x005A},
	{0x005F, 0x005F}, /* character "_" */
	{0x0061, 0x007A},
	{0x00C0, 0x00D6},
	{0x00D8, 0x00F6},
	{0x00F8, 0x00FF},
	{0x0100, 0x0131},
	{0x0134, 0x013E},
	{0x0141, 0x0148},
	{0x014A, 0x017E},
	{0x0180, 0x01C3},
	{0x01CD, 0x01F0},
	{0x01F4, 0x01F5},
	{0x01FA, 0x0217},
	{0x0250, 0x02A8},
	{0x02BB, 0x02C1},
	{0x0386, 0x0386},
	{0x0388, 0x038A},
	{0x038C, 0x038C},
	{0x038E, 0x03A1},
	{0x03A3, 0x03CE},
	{0x03D0, 0x03D6},
	{0x03DA, 0x03DA},
	{0x03DC, 0x03DC},
	{0x03DE, 0x03DE},
	{0x03E0, 0x03E0},
	{0x03E2, 0x03F3},
	{0x0401, 0x040C},
	{0x040E, 0x044F},
	{0x0451, 0x045C},
	{0x045E, 0x0481},
	{0x0490, 0x04C4},
	{0x04C7, 0x04C8},
	{0x04CB, 0x04CC},
	{0x04D0, 0x04EB},
	{0x04EE, 0x04F5},
	{0x04F8, 0x04F9},
	{0x0531, 0x0556},
	{0x0559, 0x0559},
	{0x0561, 0x0586},
	{0x05D0, 0x05EA},
	{0x05F0, 0x05F2},
	{0x0621, 0x063A},
	{0x0641, 0x064A},
	{0x0671, 0x06B7},
	{0x06BA, 0x06BE},
	{0x06C0, 0x06CE},
	{0x06D0, 0x06D3},
	{0x06D5, 0x06D5},
	{0x06E5, 0x06E6},
	{0x0905, 0x0939},
	{0x093D, 0x093D},
	{0x0958, 0x0961},
	{0x0985, 0x098C},
	{0x098F, 0x0990},
	{0x0993, 0x09A8},
	{0x09AA, 0x09B0},
	{0x09B2, 0x09B2},
	{0x09B6, 0x09B9},
	{0x09DC, 0x09DD},
	{0x09DF, 0x09E1},
	{0x09F0, 0x09F1},
	{0x0A05, 0x0A0A},
	{0x0A0F, 0x0A10},
	{0x0A13, 0x0A28},
	{0x0A2A, 0x0A30},
	{0x0A32, 0x0A33},
	{0x0A35, 0x0A36},
	{0x0A38, 0x0A39},
	{0x0A59, 0x0A5C},
	{0x0A5E, 0x0A5E},
	{0x0A72, 0x0A74},
	{0x0A85, 0x0A8B},
	{0x0A8D, 0x0A8D},
	{0x0A8F, 0x0A91},
	{0x0A93, 0x0AA8},
	{0x0AAA, 0x0AB0},
	{0x0AB2, 0x0AB3},
	{0x0AB5, 0x0AB9},
	{0x0ABD, 0x0ABD},
	{0x0AE0, 0x0AE0},
	{0x0B05, 0x0B0C},
	{0x0B0F, 0x0B10},
	{0x0B13, 0x0B28},
	{0x0B2A, 0x0B30},
	{0x0B32, 0x0B33},
	{0x0B36, 0x0B39},
	{0x0B3D, 0x0B3D},
	{0x0B5C, 0x0B5D},
	{0x0B5F, 0x0B61},
	{0x0B85, 0x0B8A},
	{0x0B8E, 0x0B90},
	{0x0B92, 0x0B95},
	{0x0B99, 0x0B9A},
	{0x0B9C, 0x0B9C},
	{0x0B9E, 0x0B9F},
	{0x0BA3, 0x0BA4},
	{0x0BA8, 0x0BAA},
	{0x0BAE, 0x0BB5},
	{0x0BB7, 0x0BB9},
	{0x0C05, 0x0C0C},
	{0x0C0E, 0x0C10},
	{0x0C12, 0x0C28},
	{0x0C2A, 0x0C33},
	{0x0C35, 0x0C39},
	{0x0C60, 0x0C61},
	{0x0C85, 0x0C8C},
	{0x0C8E, 0x0C90},
	{0x0C92, 0x0CA8},
	{0x0CAA, 0x0CB3},
	{0x0CB5, 0x0CB9},
	{0x0CDE, 0x0CDE},
	{0x0CE0, 0x0CE1},
	{0x0D05, 0x0D0C},
	{0x0D0E, 0x0D10},
	{0x0D12, 0x0D28},
	{0x0D2A, 0x0D39},
	{0x0D60, 0x0D61},
	{0x0E01, 0x0E2E},
	{0x0E30, 0x0E30},
	{0x0E32, 0x0E33},
	{0x0E40, 0x0E45},
	{0x0E81, 0x0E82},
	{0x0E84, 0x0E84},
	{0x0E87, 0x0E88},
	{0x0E8A, 0x0E8A},
	{0x0E8D, 0x0E8D},
	{0x0E94, 0x0E97},
	{0x0E99, 0x0E9F},
	{0x0EA1, 0x0EA3},
	{0x0EA5, 0x0EA5},
	{0x0EA7, 0x0EA7},
	{0x0EAA, 0x0EAB},
	{0x0EAD, 0x0EAE},
	{0x0EB0, 0x0EB0},
	{0x0EB2, 0x0EB3},
	{0x0EBD, 0x0EBD},
	{0x0EC0, 0x0EC4},
	{0x0F40, 0x0F47},
	{0x0F49, 0x0F69},
	{0x10A0, 0x10C5},
	{0x10D0, 0x10F6},
	{0x1100, 0x1100},
	{0x1102, 0x1103},
	{0x1105, 0x1107},
	{0x1109, 0x1109},
	{0x110B, 0x110C},
	{0x110E, 0x1112},
	{0x113C, 0x113C},
	{0x113E, 0x113E},
	{0x1140, 0x1140},
	{0x114C, 0x114C},
	{0x114E, 0x114E},
	{0x1150, 0x1150},
	{0x1154, 0x1155},
	{0x1159, 0x1159},
	{0x115F, 0x1161},
	{0x1163, 0x1163},
	{0x1165, 0x1165},
	{0x1167, 0x1167},
	{0x1169, 0x1169},
	{0x116D, 0x116E},
	{0x1172, 0x1173},
	{0x1175, 0x1175},
	{0x119E, 0x119E},
	{0x11A8, 0x11A8},
	{0x11AB, 0x11AB},
	{0x11AE, 0x11AF},
	{0x11B7, 0x11B8},
	{0x11BA, 0x11BA},
	{0x11BC, 0x11C2},
	{0x11EB, 0x11EB},
	{0x11F0, 0x11F0},
	{0x11F9, 0x11F9},
	{0x1E00, 0x1E9B},
	{0x1EA0, 0x1EF9},
	{0x1F00, 0x1F15},
	{0x1F18, 0x1F1D},
	{0x1F20, 0x1F45},
	{0x1F48, 0x1F4D},
	{0x1F50, 0x1F57},
	{0x1F59, 0x1F59},
	{0x1F5B, 0x1F5B},
	{0x1F5D, 0x1F5D},
	{0x1F5F, 0x1F7D},
	{0x1F80, 0x1FB4},
	{0x1FB6, 0x1FBC},
	{0x1FBE, 0x1FBE},
	{0x1FC2, 0x1FC4},
	{0x1FC6, 0x1FCC},
	{0x1FD0, 0x1FD3},
	{0x1FD6, 0x1FDB},
	{0x1FE0, 0x1FEC},
	{0x1FF2, 0x1FF4},
	{0x1FF6, 0x1FFC},
	{0x2126, 0x2126},
	{0x212A, 0x212B},
	{0x212E, 0x212E},
	{0x2180, 0x2182},
	{0x3007, 0x3007},
	{0x3021, 0x3029}, /* these two are ideographic */
	{0x3041, 0x3094},
	{0x30A1, 0x30FA},
	{0x3105, 0x312C},
	{0x4E00, 0x9FA5}, /* ideographic */
	{0xAC00, 0xD7A3}};

/*!
 * \brief The size of the letter table array.
 */
#define LETTERTABLESIZE (sizeof(Letter) / sizeof(Letter[0]))

/*!
 * \brief The NameChar table contains CombiningChar, Extender, Digit,
 * '-', '.', less '_', ':'
 *
 * NameChar ::= Digit | '-' | '.' | CombiningChar | Extender
 * See XML 1.0 2nd Edition
 */
static char_info_t NameChar[] = {
	{0x002D, 0x002D}, /* character "-" */
	{0x002E, 0x002E}, /* character "." */
	{0x0030, 0x0039}, /* digit */
	{0x00B7, 0x00B7},
	{0x02D0, 0x02D0},
	{0x02D1, 0x02D1}, /* extended */
	{0x0300, 0x0345},
	{0x0360, 0x0361},
	{0x0387, 0x0387}, /* extended */
	{0x0483, 0x0486},
	{0x0591, 0x05A1},
	{0x05A3, 0x05B9},
	{0x05BB, 0x05BD},
	{0x05BF, 0x05BF},
	{0x05C1, 0x05C2},
	{0x05C4, 0x05C4},
	{0x0640, 0x0640}, /* extended */
	{0x064B, 0x0652},
	{0x0660, 0x0669}, /* digit */
	{0x0670, 0x0670},
	{0x06D6, 0x06DC},
	{0x06DD, 0x06DF},
	{0x06E0, 0x06E4},
	{0x06E7, 0x06E8},
	{0x06EA, 0x06ED},
	{0x06F0, 0x06F9}, /* digit */
	{0x0901, 0x0903},
	{0x093C, 0x093C},
	{0x093E, 0x094C},
	{0x094D, 0x094D},
	{0x0951, 0x0954},
	{0x0962, 0x0963},
	{0x0966, 0x096F}, /* digit */
	{0x0981, 0x0983},
	{0x09BC, 0x09BC},
	{0x09BE, 0x09BE},
	{0x09BF, 0x09BF},
	{0x09C0, 0x09C4},
	{0x09C7, 0x09C8},
	{0x09CB, 0x09CD},
	{0x09D7, 0x09D7},
	{0x09E2, 0x09E3},
	{0x09E6, 0x09EF}, /* digit */
	{0x0A02, 0x0A02},
	{0x0A3C, 0x0A3C},
	{0x0A3E, 0x0A3E},
	{0x0A3F, 0x0A3F},
	{0x0A40, 0x0A42},
	{0x0A47, 0x0A48},
	{0x0A4B, 0x0A4D},
	{0x0A66, 0x0A6F}, /* digit */
	{0x0A70, 0x0A71},
	{0x0A81, 0x0A83},
	{0x0ABC, 0x0ABC},
	{0x0ABE, 0x0AC5},
	{0x0AC7, 0x0AC9},
	{0x0ACB, 0x0ACD},
	{0x0AE6, 0x0AEF}, /* digit */
	{0x0B01, 0x0B03},
	{0x0B3C, 0x0B3C},
	{0x0B3E, 0x0B43},
	{0x0B47, 0x0B48},
	{0x0B4B, 0x0B4D},
	{0x0B56, 0x0B57},
	{0x0B66, 0x0B6F}, /* digit */
	{0x0B82, 0x0B83},
	{0x0BBE, 0x0BC2},
	{0x0BC6, 0x0BC8},
	{0x0BCA, 0x0BCD},
	{0x0BD7, 0x0BD7},
	{0x0BE7, 0x0BEF}, /* digit */
	{0x0C01, 0x0C03},
	{0x0C3E, 0x0C44},
	{0x0C46, 0x0C48},
	{0x0C4A, 0x0C4D},
	{0x0C55, 0x0C56},
	{0x0C66, 0x0C6F}, /* digit */
	{0x0C82, 0x0C83},
	{0x0CBE, 0x0CC4},
	{0x0CC6, 0x0CC8},
	{0x0CCA, 0x0CCD},
	{0x0CD5, 0x0CD6},
	{0x0CE6, 0x0CEF}, /* digit */
	{0x0D02, 0x0D03},
	{0x0D3E, 0x0D43},
	{0x0D46, 0x0D48},
	{0x0D4A, 0x0D4D},
	{0x0D57, 0x0D57},
	{0x0D66, 0x0D6F}, /* digit */
	{0x0E31, 0x0E31},
	{0x0E34, 0x0E3A},
	{0x0E46, 0x0E46}, /* extended */
	{0x0E47, 0x0E4E},
	{0x0E50, 0x0E59}, /* digit */
	{0x0EB1, 0x0EB1},
	{0x0EB4, 0x0EB9},
	{0x0EBB, 0x0EBC},
	{0x0EC6, 0x0EC6}, /* extended */
	{0x0EC8, 0x0ECD},
	{0x0ED0, 0x0ED9}, /* digit */
	{0x0F18, 0x0F19},
	{0x0F20, 0x0F29}, /* digit */
	{0x0F35, 0x0F35},
	{0x0F37, 0x0F37},
	{0x0F39, 0x0F39},
	{0x0F3E, 0x0F3E},
	{0x0F3F, 0x0F3F},
	{0x0F71, 0x0F84},
	{0x0F86, 0x0F8B},
	{0x0F90, 0x0F95},
	{0x0F97, 0x0F97},
	{0x0F99, 0x0FAD},
	{0x0FB1, 0x0FB7},
	{0x0FB9, 0x0FB9},
	{0x20D0, 0x20DC},
	{0x20E1, 0x20E1},
	{0x3005, 0x3005}, /* extended */
	{0x302A, 0x302F},
	{0x3031, 0x3035}, /* extended */
	{0x3099, 0x3099},
	{0x309A, 0x309A}, /* combining char */
	{0x309D, 0x309E},
	{0x30FC, 0x30FE} /* extended */
};

/*!
 * \brief The name char table array size.
 */
#define NAMECHARTABLESIZE (sizeof(NameChar) / sizeof(NameChar[0]))

/*!
 * \brief Frees one ElementStack item.
 */
static void Parser_freeElementStackItem(
	/*! [in] The element stack item to free. */
	IXML_ElementStack *pItem)
{
	assert(pItem != NULL);
	if (pItem->element != NULL) {
		free(pItem->element);
		pItem->element = NULL;
	}
	if (pItem->namespaceUri != NULL) {
		free(pItem->namespaceUri);
		pItem->namespaceUri = NULL;
	}
	if (pItem->prefix != NULL) {
		free(pItem->prefix);
		pItem->prefix = NULL;
	}
}

/*!
 * \brief Frees namespaceURI item.
 */
static void Parser_freeNsURI(
	/*! [in] The name space URI item to free. */
	IXML_NamespaceURI *pNsURI)
{
	assert(pNsURI != NULL);
	if (pNsURI->nsURI != NULL) {
		free(pNsURI->nsURI);
	}
	if (pNsURI->prefix != NULL) {
		free(pNsURI->prefix);
	}
}

/*!
 * \brief Frees all temporary memory allocated by xmlparser.
 */
static void Parser_free(
	/*! [in] The XML parser. */
	Parser *xmlParser)
{
	IXML_ElementStack *pElement;
	IXML_ElementStack *pNextElement;
	IXML_NamespaceURI *pNsURI;
	IXML_NamespaceURI *pNextNsURI;

	if (xmlParser == NULL) {
		return;
	}

	if (xmlParser->dataBuffer != NULL) {
		free(xmlParser->dataBuffer);
	}

	ixml_membuf_destroy(&(xmlParser->tokenBuf));
	ixml_membuf_destroy(&(xmlParser->lastElem));

	pElement = xmlParser->pCurElement;
	while (pElement != NULL) {
		Parser_freeElementStackItem(pElement);

		pNsURI = pElement->pNsURI;
		while (pNsURI != NULL) {
			pNextNsURI = pNsURI->nextNsURI;
			Parser_freeNsURI(pNsURI);
			free(pNsURI);
			pNsURI = pNextNsURI;
		}

		pNextElement = pElement->nextElement;
		free(pElement);
		pElement = pNextElement;
	}

	free(xmlParser);
}

/*!
 * \brief Skips document type declaration
 */
static int Parser_skipDocType(
	/*! [in,out] The pointer to the skipped point. */
	char **pstr)
{
	char *pCur = *pstr;
	/* default there is no nested < */
	char *pNext = NULL;
	int num = 1;

	assert((*pstr) != NULL);
	if (*pstr == NULL) {
		return IXML_FAILED;
	}

	while ((pCur != NULL) && (num != 0) && (*pCur != 0)) {
		if (*pCur == '<') {
			num++;
		} else if (*pCur == '>') {
			num--;
		} else if (*pCur == '"') {
			pNext = strchr(pCur + 1, '"');
			if (pNext == NULL) {
				return IXML_SYNTAX_ERR;
			}

			pCur = pNext;
		}

		pCur++;
	}

	if (num == 0) {
		*pstr = pCur;
		return IXML_SUCCESS;
	} else {
		return IXML_SYNTAX_ERR;
	}
}

/*!
 * \brief Skips all characters in the string until it finds the skip key.
 * Then it skips the skip key and returns.
 */
static int Parser_skipString(
	/*! [in,out] The pointer to the skipped point. */
	char **pstrSrc,
	/*! [in] The skip key. */
	const char *strSkipKey)
{
	if (!(*pstrSrc) || !strSkipKey) {
		return IXML_FAILED;
	}

	while ((**pstrSrc) &&
		strncmp(*pstrSrc, strSkipKey, strlen(strSkipKey)) != 0) {
		(*pstrSrc)++;
	}

	if (**pstrSrc == '\0') {
		return IXML_SYNTAX_ERR;
	}
	*pstrSrc = *pstrSrc + strlen(strSkipKey);

	return IXML_SUCCESS;
}

/*!
 * \brief Skip UTF-8 byte order mark
 */
static void Parser_skipBom(
	/*! [in] The XML parser. */
	Parser *xmlParser)
{
	size_t bom_len = strlen(UTF8_BOM);

	if (strncmp(xmlParser->curPtr, UTF8_BOM, bom_len) == 0)
		xmlParser->curPtr += bom_len;
}

/*!
 * \brief Skip white spaces.
 */
static void Parser_skipWhiteSpaces(
	/*! [in] The XML parser. */
	Parser *xmlParser)
{
	while ((*(xmlParser->curPtr) != 0) &&
		(strchr(WHITESPACE, (int)*(xmlParser->curPtr)) != NULL)) {
		xmlParser->curPtr++;
	}
}

/*!
 * \brief Skips XML declarations.
 */
static int Parser_skipXMLDecl(
	/*! [in,out] The XML parser. */
	Parser *xmlParser)
{
	int rc = IXML_FAILED;

	assert(xmlParser);
	if (xmlParser == NULL) {
		return rc;
	}

	rc = Parser_skipString(&(xmlParser->curPtr), END_PI);
	Parser_skipWhiteSpaces(xmlParser);
	return rc;
}

/*!
 * \brief Skips all characters in the string until it finds the skip key.
 * Then it skips the skip key and returns.
 */
static int Parser_skipComment(
	/*! [in,out] The pointer to the skipped point. */
	char **pstrSrc)
{
	char *pStrFound = NULL;

	assert((*pstrSrc) != NULL);
	if (*pstrSrc == NULL) {
		return IXML_FAILED;
	}

	pStrFound = strstr(*pstrSrc, END_COMMENT);
	if ((pStrFound != NULL) && (pStrFound != *pstrSrc) &&
		(*(pStrFound - 1) != '-')) {
		*pstrSrc = pStrFound + strlen(END_COMMENT);
	} else {
		return IXML_SYNTAX_ERR;
	}

	return IXML_SUCCESS;
}

/*!
 * \brief Skip comment, PI and white space.
 */
static int Parser_skipMisc(
	/*! [in] The XML parser. */
	Parser *xmlParser)
{
	int rc = IXML_SUCCESS;
	int done = 0;

	while ((done == 0) && (rc == IXML_SUCCESS)) {
		if (strncasecmp(xmlParser->curPtr,
			    (char *)BEGIN_COMMENT,
			    strlen(BEGIN_COMMENT)) == 0) {
			/* <!-- */
			rc = Parser_skipComment(&(xmlParser->curPtr));

		} else if (strncasecmp(xmlParser->curPtr,
				   (char *)XMLDECL,
				   strlen(XMLDECL)) == 0 ||
			   strncasecmp(xmlParser->curPtr,
				   (char *)XMLDECL2,
				   strlen(XMLDECL2)) == 0) {
			/* <?xml or <?xml? */
			rc = IXML_SYNTAX_ERR;
		} else if (strncasecmp(xmlParser->curPtr,
				   (char *)BEGIN_PI,
				   strlen(BEGIN_PI)) == 0) {
			/* <? */
			rc = Parser_skipString(&xmlParser->curPtr, END_PI);
		} else {
			done = 1;
		}
		Parser_skipWhiteSpaces(xmlParser);
	}

	return rc;
}

/*!
 * \brief Skip prolog.
 */
static int Parser_skipProlog(
	/*! [in,out] The XML parser. */
	Parser *xmlParser)
{
	int rc = IXML_SUCCESS;

	assert(xmlParser != NULL);
	if (xmlParser == NULL) {
		return IXML_FAILED;
	}

	Parser_skipBom(xmlParser);
	Parser_skipWhiteSpaces(xmlParser);

	if (strncmp(xmlParser->curPtr, (char *)XMLDECL, strlen(XMLDECL)) == 0) {
		/* <?xml */
		rc = Parser_skipXMLDecl(xmlParser);
		if (rc != IXML_SUCCESS) {
			return rc;
		}
	}

	rc = Parser_skipMisc(xmlParser);
	if ((rc == IXML_SUCCESS) && strncmp(xmlParser->curPtr,
					    (char *)BEGIN_DOCTYPE,
					    strlen(BEGIN_DOCTYPE)) == 0) {
		/* <! DOCTYPE */
		xmlParser->curPtr++;
		rc = Parser_skipDocType(&(xmlParser->curPtr));
	}

	if (rc == IXML_SUCCESS) {
		rc = Parser_skipMisc(xmlParser);
	}

	return rc;
}

/*!
 * \brief Set the last element to be the given string.
 */
static int Parser_setLastElem(
	/*! [in] The XML parser. */
	Parser *xmlParser,
	/*! [in] The string to copy from. */
	const char *s)
{
	int rc;

	if ((xmlParser == NULL) || (s == NULL)) {
		return IXML_FAILED;
	}

	rc = ixml_membuf_assign_str(&(xmlParser->lastElem), s);
	return rc;
}

/*!
 * \brief Clear token buffer.
 */
static void Parser_clearTokenBuf(
	/*! [in] The XML parser. */
	Parser *xmlParser)
{
	ixml_membuf_destroy(&(xmlParser->tokenBuf));
}

/*!
 * \brief In UTF-8, characters are encoded using sequences of 1 to 6 octets.
 * This functions will return a UTF-8 character value and its octets number.
 *
 * \return The UTF-8 character converted to an int (32 bits).
 */
static int Parser_UTF8ToInt(
	/*! [in] The pointer to the character to encode. */
	const char *ss,
	/*! [out] The number of octets of the UTF-8 encoding of this character.
	 */
	ptrdiff_t *len)
{
	const unsigned char *s = (const unsigned char *)ss;
	int c = *s;

	if (c <= 127) {
		/* if c<=127, c is just the character. */
		*len = 1;
		return c;
	} else if ((c & 0xE0) == 0xC0 && (s[1] & 0xc0) == 0x80) {
		/* a sequence of 110xxxxx and 10xxxxxx? */
		*len = 2;
		return ((c & 0x1f) << 6) | (s[1] & 0x3f);
	} else if ((c & 0xF0) == 0xE0 && (s[1] & 0xc0) == 0x80 &&
		   (s[2] & 0xc0) == 0x80) {
		/* a sequence of 1110xxxx,10xxxxxx and 10xxxxxx ? */
		*len = 3;
		return ((c & 0x0f) << 12) | ((s[1] & 0x3f) << 6) |
		       (s[2] & 0x3f);
	} else if ((c & 0xf8) == 0xf0 && (s[1] & 0xc0) == 0x80 &&
		   (s[2] & 0xc0) == 0x80 && (s[3] & 0xc0) == 0x80) {
		/* a sequence of 11110xxx,10xxxxxx,10xxxxxx and 10xxxxxx ? */
		*len = 4;
		return ((c & 0x07) << 18) | ((s[1] & 0x3f) << 12) |
		       ((s[2] & 0x3f) << 6) | (s[3] & 0x3f);
	} else if ((c & 0xfc) == 0xf8 && (s[1] & 0xc0) == 0x80 &&
		   (s[2] & 0xc0) == 0x80 && (s[3] & 0xc0) == 0x80 &&
		   (s[4] & 0xc0) == 0x80) {
		/* a sequence of 111110xx,10xxxxxx,10xxxxxx,10xxxxxx,10xxxxxx ?
		 */
		*len = 5;
		return ((c & 0x03) << 24) | ((s[1] & 0x3f) << 18) |
		       ((s[2] & 0x3f) << 12) | ((s[3] & 0x3f) << 6) |
		       (s[4] & 0x3f);
	} else if ((c & 0xfe) == 0xfc && (s[1] & 0xc0) == 0x80 &&
		   (s[2] & 0xc0) == 0x80 && (s[3] & 0xc0) == 0x80 &&
		   (s[4] & 0xc0) == 0x80 && (s[5] & 0xc0) == 0x80) {
		/* a sequence of 1111110x,10xxxxxx,10xxxxxx,10xxxxxx,10xxxxxx
		 * and 10xxxxxx ? */
		*len = 6;
		return ((c & 0x01) << 30) | ((s[1] & 0x3f) << 24) |
		       ((s[2] & 0x3f) << 18) | ((s[3] & 0x3f) << 12) |
		       ((s[4] & 0x3f) << 6) | (s[5] & 0x3f);
	} else {
		/* none of above, error */
		int ret = 0;
		int line = __LINE__;
		if (g_error_char) {
			*len = 1;
			ret = g_error_char;
		} else {
			*len = 0;
			ret = -1;
		}
		IxmlPrintf(
			__FILE__, line, "Parser_UTF8ToInt", "Error %d\n", ret);
		return ret;
	}
}

/*!
 * \brief Will determine whether character c is in the table of tbl (either
 * Letter table or NameChar table).
 *
 * \return 1 or 0.
 */
static int Parser_isCharInTable(
	/*! [in] Character to check. */
	int c,
	/*! [in] Table to use. */
	char_info_t *tbl,
	/*! [in] Size of the table. */
	int sz)
{
	int t = 0;
	int b = sz - 1;
	int m;

	while (t <= b) {
		m = (t + b) / 2;
		if (c < tbl[m].l) {
			b = m - 1;
		} else if (c > tbl[m].h) {
			t = m + 1;
		} else {
			return 1;
		}
	}

	return 0;
}

/*!
 * \brief Check whether c (int) is in LetterTable or NameCharTable.
 */
static int Parser_isNameChar(
	/*! [in] The character to check. */
	int c,
	/*! [in] 1 if you also want to check in the NameChar table. */
	int bNameChar)
{
	if (Parser_isCharInTable(c, Letter, (int)LETTERTABLESIZE)) {
		return 1;
	}

	if (bNameChar &&
		Parser_isCharInTable(c, NameChar, (int)NAMECHARTABLESIZE)) {
		return 1;
	}

	return 0;
}

/*!
 * \brief see XML 1.0 (2nd Edition) 2.2.
 */
static int Parser_isXmlChar(
	/*! [in] The character to check. */
	int c)
{
	return c == 0x9 || c == 0xA || c == 0xD || (c >= 0x20 && c <= 0xD7FF) ||
	       (c >= 0xE000 && c <= 0xFFFD) || (c >= 0x10000 && c <= 0x10FFFF);
}

/*!
 * \brief Returns next char value and its length.
 */
static int Parser_getChar(
	/*! [in] . */
	const char *src,
	/*! [in,out] . */
	ptrdiff_t *cLen)
{
	int ret = -1;
	int line = 0;
	const char *pnum;
	int sum;
	char c;
	int i;

	if (src == NULL || cLen == NULL) {
		line = __LINE__;
		ret = -1;
		goto ExitFunction;
	}

	*cLen = 0;
	if (*src != '&') {
		if (*src > 0 && Parser_isXmlChar((int)*src)) {
			*cLen = 1;
			ret = *src;
			goto ExitFunction;
		}

		i = Parser_UTF8ToInt(src, cLen);
		if (!Parser_isXmlChar(i)) {
			line = __LINE__;
			ret = g_error_char ? g_error_char : -1;
			goto ExitFunction;
		}

		line = __LINE__;
		ret = i;
		goto ExitFunction;
	} else if (strncasecmp(src, QUOT, strlen(QUOT)) == 0) {
		*cLen = (int)strlen(QUOT);
		ret = '"';
		goto ExitFunction;
	} else if (strncasecmp(src, LT, strlen(LT)) == 0) {
		*cLen = (int)strlen(LT);
		ret = '<';
		goto ExitFunction;
	} else if (strncasecmp(src, GT, strlen(GT)) == 0) {
		*cLen = (int)strlen(GT);
		ret = '>';
		goto ExitFunction;
	} else if (strncasecmp(src, APOS, strlen(APOS)) == 0) {
		*cLen = (int)strlen(APOS);
		ret = '\'';
		goto ExitFunction;
	} else if (strncasecmp(src, AMP, strlen(AMP)) == 0) {
		*cLen = (int)strlen(AMP);
		ret = '&';
		goto ExitFunction;
	} else if (strncasecmp(src, ESC_HEX, strlen(ESC_HEX)) == 0) {
		/* Read in escape characters of type &#xnn where nn is a
		 * hexadecimal value */
		pnum = src + strlen(ESC_HEX);
		sum = 0;
		while (strchr(HEX_NUMBERS, (int)*pnum) != 0) {
			c = *pnum;
			if (c <= '9') {
				sum = sum * 16 + (c - '0');
			} else if (c <= 'F') {
				sum = sum * 16 + (c - 'A' + 10);
			} else {
				sum = sum * 16 + (c - 'a' + 10);
			}
			pnum++;
		}
		if (pnum == src || *pnum != ';' || !Parser_isXmlChar(sum)) {
			line = __LINE__;
			goto fail_entity;
		}
		*cLen = pnum - src + 1;
		ret = sum;
		goto ExitFunction;
	} else if (strncasecmp(src, ESC_DEC, strlen(ESC_DEC)) == 0) {
		/* Read in escape characters of type &#nn where nn is a decimal
		 * value */
		pnum = src + strlen(ESC_DEC);
		sum = 0;
		while (strchr(DEC_NUMBERS, (int)*pnum) != 0) {
			sum = sum * 10 + (*pnum - '0');
			pnum++;
		}
		if ((pnum == src) || *pnum != ';' || !Parser_isXmlChar(sum)) {
			line = __LINE__;
			goto fail_entity;
		}
		*cLen = pnum - src + 1;
		ret = sum;
		goto ExitFunction;
	}

fail_entity:
	if (g_error_char) {
		*cLen = 1;
		ret = '&';
		goto ExitFunction;
	}
	ret = -1;

ExitFunction:
	if (ret == -1 || (g_error_char && ret == g_error_char)) {
		IxmlPrintf(__FILE__, line, "Parser_getChar", "Error %d\n", ret);
	}

	return ret;
}

/*!
 * \brief Appends c to token buffer.
 */
static int Parser_appendTokBufChar(
	/*! [in] The XML parser. */
	Parser *xmlParser,
	/*! [in] The character to append. */
	char c)
{
	int rc;

	rc = ixml_membuf_append(&(xmlParser->tokenBuf), &c);
	return rc;
}

/*!
 * \brief Encodes a character to its UTF-8 character string, and return its
 * length.
 *
 * \return The length of the encoded string in bytes.
 */
static int Parser_intToUTF8(
	/*! [in] The character to encode. */
	int c,
	/*! [out] The resultant UTF-8 encoded string. */
	utf8char s)
{
	if (c < 0)
		return 0;

	if (c <= 127) {
		s[0] = (char)c;
		s[1] = (char)0;
		return 1;
	} else if (c <= 0x07FF) {
		/* 0x0080 < c <= 0x07FF */
		s[0] = (char)(0xC0 | (c >> 6));
		s[1] = (char)(0x80 | (c & 0x3f));
		s[2] = (char)0;
		return 2;
	} else if (c <= 0xFFFF) {
		/* 0x0800 < c <= 0xFFFF */
		s[0] = (char)(0xE0 | (c >> 12));
		s[1] = (char)(0x80 | ((c >> 6) & 0x3f));
		s[2] = (char)(0x80 | (c & 0x3f));
		s[3] = (char)0;
		return 3;
	} else if (c <= 0x1FFFFF) {
		/* 0x10000 < c <= 0x1FFFFF */
		s[0] = (char)(0xF0 | (c >> 18));
		s[1] = (char)(0x80 | ((c >> 12) & 0x3f));
		s[2] = (char)(0x80 | ((c >> 6) & 0x3f));
		s[3] = (char)(0x80 | (c & 0x3f));
		s[4] = (char)0;
		return 4;
	} else if (c <= 0x3FFFFFF) {
		/* 0x200000 < c <= 3FFFFFF */
		s[0] = (char)(0xF8 | (c >> 24));
		s[1] = (char)(0x80 | ((c >> 18) & 0x3f));
		s[2] = (char)(0x80 | ((c >> 12) & 0x3f));
		s[3] = (char)(0x80 | ((c >> 6) & 0x3f));
		s[4] = (char)(0x80 | (c & 0x3f));
		s[5] = (char)0;
		return 5;
	} else if (c <= 0x7FFFFFFF) {
		/* 0x4000000 < c <= 7FFFFFFF */
		s[0] = (char)(0xFC | (c >> 30));
		s[1] = (char)(0x80 | ((c >> 24) & 0x3f));
		s[2] = (char)(0x80 | ((c >> 18) & 0x3f));
		s[3] = (char)(0x80 | ((c >> 12) & 0x3f));
		s[4] = (char)(0x80 | ((c >> 6) & 0x3f));
		s[5] = (char)(0x80 | (c & 0x3f));
		s[6] = (char)0;
		return 6;
	} else {
		/* illegal */
		return 0;
	}
}

/*!
 * \brief Appends string s to token buffer.
 */
static int Parser_appendTokBufStr(
	/*! [in] The XML parser. */
	Parser *xmlParser,
	/*! [in] The string to append. */
	const char *s)
{
	int rc = IXML_SUCCESS;

	if (s != NULL) {
		rc = ixml_membuf_append_str(&(xmlParser->tokenBuf), s);
	}

	return rc;
}

/*!
 * \brief Copy string in src into xml parser token buffer
 */
static int Parser_copyToken(
	/*! [in] The XML parser. */
	Parser *xmlParser,
	/*! [in] The string to copy from. */
	const char *src,
	/*! [in] The lenght to copy. */
	ptrdiff_t len)
{
	int ret = IXML_SUCCESS;
	int line = 0;
	int i;
	int c;
	ptrdiff_t cl;
	const char *psrc;
	const char *pend;
	utf8char uch;

	if (!src || len <= 0) {
		line = __LINE__;
		ret = IXML_FAILED;
		goto ExitFunction;
	}

	psrc = src;
	pend = src + len;

	while (psrc < pend) {
		c = Parser_getChar(psrc, &cl);
		if (c <= 0) {
			line = __LINE__;
			ret = IXML_FAILED;
			goto ExitFunction;
		}

		if (cl == 1) {
			Parser_appendTokBufChar(xmlParser, (char)c);
			psrc++;
		} else {
			i = Parser_intToUTF8(c, uch);
			if (i == 0) {
				line = __LINE__;
				ret = IXML_FAILED;
				goto ExitFunction;
			}
			Parser_appendTokBufStr(xmlParser, uch);
			psrc += cl;
		}
	}

	if (psrc > pend) {
		line = __LINE__;
		ret = IXML_FAILED;
		goto ExitFunction;
	}

ExitFunction:
	if (ret != IXML_SUCCESS) {
		IxmlPrintf(
			__FILE__, line, "Parser_copyToken", "Error %d\n", ret);
	}

	return ret;
}

/*!
 * \brief Return the length of next token in tokenBuff.
 */
static ptrdiff_t Parser_getNextToken(
	/*! [in] The XML parser. */
	Parser *xmlParser)
{
	ptrdiff_t tokenLength = 0;
	int temp;
	ptrdiff_t tlen;
	int rc;

	Parser_clearTokenBuf(xmlParser);

	if (*(xmlParser->curPtr) == '\0') {
		return 0;
	}
	/* skip XML instructions */
	rc = Parser_skipMisc(xmlParser);
	if (rc != IXML_SUCCESS) {
		return 0;
	}
	/* Attribute value logic must come first, since all text untokenized
	 * until end-quote */
	if (*(xmlParser->curPtr) == QUOTE) {
		tokenLength = 1;
	} else if (*(xmlParser->curPtr) == SINGLEQUOTE) {
		tokenLength = 1;
	} else if (*(xmlParser->curPtr) == LESSTHAN) {
		/* Check for start tags */
		temp = Parser_UTF8ToInt(xmlParser->curPtr + 1, &tlen);
		if (temp == '/') {
			/* token is '</' end tag */
			tokenLength = 2;
		} else if (Parser_isNameChar(temp, 0)) {
			/* '<' found, so return '<' token */
			tokenLength = 1;
		} else {
			/* error */
			return 0;
		}
	} else if (*(xmlParser->curPtr) == EQUALS) {
		/* Check for '=' token, return it as a token */
		tokenLength = 1;
	} else if (*(xmlParser->curPtr) == SLASH) {
		if (*(xmlParser->curPtr + 1) == GREATERTHAN) {
			/* token '/>' found */
			tokenLength = 2;
			/* fix */
			xmlParser->savePtr = xmlParser->curPtr;
		}
	} else if (*(xmlParser->curPtr) == GREATERTHAN) {
		/* > found, so return it as a token */
		tokenLength = 1;
	} else if (Parser_isNameChar(
			   Parser_UTF8ToInt(xmlParser->curPtr, &tlen), 0)) {
		/* Check for name tokens, name found, so find out how long it is
		 */
		ptrdiff_t iIndex = tlen;

		while (Parser_isNameChar(
			Parser_UTF8ToInt(xmlParser->curPtr + iIndex, &tlen),
			1)) {
			iIndex += tlen;
		}
		tokenLength = iIndex;
	} else {
		return 0;
	}

	/* Copy the token to the return string */
	if (Parser_copyToken(xmlParser, xmlParser->curPtr, tokenLength) !=
		IXML_SUCCESS) {
		return 0;
	}

	xmlParser->curPtr += tokenLength;
	return tokenLength;
}

/*!
 * \brief Version of strdup() that handles NULL input.
 *
 * \return The same as strdup().
 */
static char *safe_strdup(
	/*! [in] String to be duplicated. */
	const char *s)
{
	assert(s != NULL);

	if (s == NULL) {
		return strdup((const char *)"");
	}
	return strdup(s);
}

/*!
 * \brief Processes the STag as defined by XML spec.
 */
static int Parser_processSTag(
	/*! [in] The XML parser. */
	Parser *xmlParser,
	/*! [in] The Node to process. */
	IXML_Node *node)
{
	char *pCurToken = NULL;
	int rc;

	if (Parser_getNextToken(xmlParser) == 0) {
		return IXML_SYNTAX_ERR;
	}

	pCurToken = (xmlParser->tokenBuf).buf;
	if (pCurToken != NULL) {
		node->nodeName = safe_strdup(pCurToken);
		if (node->nodeName == NULL) {
			return IXML_INSUFFICIENT_MEMORY;
		}
	} else {
		return IXML_SYNTAX_ERR;
	}

	rc = Parser_setLastElem(xmlParser, node->nodeName);
	if (rc != IXML_SUCCESS) {
		/* no need to free node->nodeName, main loop will free it */
		return IXML_FAILED;
	}

	rc = Parser_setNodePrefixAndLocalName(node);
	if (rc != IXML_SUCCESS) {
		/* no need to free node->nodeName, main loop will free it */
		return IXML_FAILED;
	}

	node->nodeValue = NULL;
	node->nodeType = eELEMENT_NODE;

	xmlParser->savePtr = xmlParser->curPtr;
	if (Parser_getNextToken(xmlParser) == 0) {
		/* no need to free node->nodeName, main loop will free it */
		return IXML_SYNTAX_ERR;
	}

	pCurToken = (xmlParser->tokenBuf).buf;
	/* check to see what is the next token */
	if (strcmp(pCurToken, "/>") == 0) {
		/* empty element */
		xmlParser->state = eELEMENT;
		/* backup to /> */
		xmlParser->curPtr = xmlParser->savePtr;
	} else if (strcmp(pCurToken, ">") == 0) {
		/* expecting text node */
		xmlParser->state = eCONTENT;
	} else {
		xmlParser->state = eATTRIBUTE;
		xmlParser->curPtr = xmlParser->savePtr;
	}

	return IXML_SUCCESS;
}

/*!
 * \brief
 */
static int Parser_skipPI(
	/*! [in,out] The pointer to the skipped point. */
	char **pSrc)
{
	char *pEnd = NULL;

	assert(*pSrc);
	if (*pSrc == NULL) {
		return IXML_FAILED;
	}

	if ((strncasecmp(*pSrc, (char *)XMLDECL, strlen(XMLDECL)) == 0) ||
		(strncasecmp(*pSrc, (char *)XMLDECL2, strlen(XMLDECL2)) == 0)) {
		/* not allowed */
		return IXML_SYNTAX_ERR;
	}

	if (strncasecmp(*pSrc, (char *)BEGIN_PI, strlen(BEGIN_PI)) == 0) {
		pEnd = strstr(*pSrc, END_PI);
		if ((pEnd != NULL) && (pEnd != *pSrc)) {
			*pSrc = pEnd + strlen(BEGIN_PI);
		} else {
			return IXML_SYNTAX_ERR;
		}
	}

	return IXML_SUCCESS;
}

/*!
 * \brief Processes CDSection as defined by XML spec.
 *
 * \return
 */
static int Parser_processCDSect(
	/*! [in] . */
	char **pSrc,
	/*! [in] The Node to process. */
	IXML_Node *node)
{
	char *pEnd;
	size_t tokenLength = (size_t)0;
	char *pCDataStart;

	if (*pSrc == NULL) {
		return IXML_FAILED;
	}

	pCDataStart = *pSrc + strlen(CDSTART);
	pEnd = pCDataStart;
	while ((Parser_isXmlChar((int)*pEnd)) && (*pEnd != '\0')) {
		if (strncmp(pEnd, CDEND, strlen(CDEND)) == 0) {
			break;
		} else {
			pEnd++;
		}
	}

	if ((pEnd - pCDataStart > 0) && (*pEnd != '\0')) {
		tokenLength = (size_t)pEnd - (size_t)pCDataStart;
		node->nodeValue = (char *)malloc(tokenLength + (size_t)1);
		if (node->nodeValue == NULL) {
			return IXML_INSUFFICIENT_MEMORY;
		}
		strncpy(node->nodeValue, pCDataStart, tokenLength);
		node->nodeValue[tokenLength] = '\0';

		node->nodeName = safe_strdup(CDATANODENAME);
		if (node->nodeName == NULL) {
			/* no need to free node->nodeValue at all, bacause node
			 * contents will be freed by the main loop. */
			return IXML_INSUFFICIENT_MEMORY;
		}

		node->nodeType = eCDATA_SECTION_NODE;
		*pSrc = pEnd + strlen(CDEND);
		return IXML_SUCCESS;
	} else {
		return IXML_SYNTAX_ERR;
	}
}
