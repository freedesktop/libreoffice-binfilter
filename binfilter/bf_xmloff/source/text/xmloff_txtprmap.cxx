/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xmloff_txtprmap.cxx,v $
 * $Revision: 1.5 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_TXTPRHDL_HXX
#include "txtprhdl.hxx"
#endif
#ifndef _XMLOFF_TXTPRMAP_HXX
#include "txtprmap.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

#define M_E( a, p, l, t, c ) \
    { a, sizeof(a)-1, XML_NAMESPACE_##p, XML_##l, t, c }

#define M_ED( a, p, l, t, c ) \
    { a, sizeof(a)-1, XML_NAMESPACE_##p, XML_##l, (t) | MID_FLAG_DEFAULT_ITEM_EXPORT, c }

#define M_END() \
    { NULL, 0, 0, XML_TOKEN_INVALID, 0, 0 }


XMLPropertyMapEntry aXMLParaPropMap[] =
{
    // RES_UNKNOWNATR_CONTAINER
    M_E( "ParaUserDefinedAttributes", TEXT, XMLNS, XML_TYPE_ATTRIBUTE_CONTAINER | MID_FLAG_SPECIAL_ITEM, 0 ),
    // RES_LR_SPACE
    M_E( "ParaLeftMargin",          FO, MARGIN_LEFT,        XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_PARALEFTMARGIN ),
    M_E( "ParaLeftMarginRelative",  FO, MARGIN_LEFT,        XML_TYPE_PERCENT16, CTF_PARALEFTMARGIN_REL ),
    M_E( "ParaRightMargin",         FO, MARGIN_RIGHT,       XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_PARARIGHTMARGIN ),
    M_E( "ParaRightMarginRelative", FO, MARGIN_RIGHT,       XML_TYPE_PERCENT16, CTF_PARARIGHTMARGIN_REL ),
    // RES_UL_SPACE
    M_E( "ParaTopMargin",           FO, MARGIN_TOP,         XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_PARATOPMARGIN ),
    M_E( "ParaTopMarginRelative",   FO, MARGIN_TOP,         XML_TYPE_PERCENT16, CTF_PARATOPMARGIN_REL ),
    M_E( "ParaBottomMargin",        FO, MARGIN_BOTTOM,      XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_PARABOTTOMMARGIN ),
    M_E( "ParaBottomMarginRelative",FO, MARGIN_BOTTOM,      XML_TYPE_PERCENT16, CTF_PARABOTTOMMARGIN_REL ),
    // RES_CHRATR_CASEMAP
    M_E( "CharCaseMap",     FO,     FONT_VARIANT,       XML_TYPE_TEXT_CASEMAP_VAR,  0 ),
    M_E( "CharCaseMap",     FO,     TEXT_TRANSFORM,     XML_TYPE_TEXT_CASEMAP,  0 ),
    // RES_CHRATR_COLOR
    M_ED( "CharColor",      FO,     COLOR,              XML_TYPE_COLORAUTO|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharColor",      STYLE,  USE_WINDOW_FONT_COLOR,  XML_TYPE_ISAUTOCOLOR|MID_FLAG_MERGE_PROPERTY,   0 ),
    // RES_CHRATR_CONTOUR
    M_E( "CharContoured",   STYLE,  TEXT_OUTLINE,       XML_TYPE_BOOL,  0 ),
    // RES_CHRATR_CROSSEDOUT
    M_E( "CharStrikeout",   STYLE,  TEXT_CROSSING_OUT,  XML_TYPE_TEXT_CROSSEDOUT,   0),
    // RES_CHRATR_ESCAPEMENT
    M_E( "CharEscapement",       STYLE, TEXT_POSITION,  XML_TYPE_TEXT_ESCAPEMENT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "CharEscapementHeight", STYLE, TEXT_POSITION,  XML_TYPE_TEXT_ESCAPEMENT_HEIGHT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    // RES_CHRATR_FONT
    M_ED( "CharFontName",   STYLE,  FONT_NAME,          XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTNAME ),
#ifdef CONV_STAR_FONTS
    M_ED( "CharFontName",   FO,     FONT_FAMILY,        XML_TYPE_TEXT_FONTFAMILYNAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTFAMILYNAME ),
#else
    M_ED( "CharFontName",   FO,     FONT_FAMILY,        XML_TYPE_TEXT_FONTFAMILYNAME, CTF_FONTFAMILYNAME ),
#endif
    M_ED( "CharFontStyleName",STYLE,    FONT_STYLE_NAME,    XML_TYPE_STRING, CTF_FONTSTYLENAME ),
    M_ED( "CharFontFamily", STYLE,  FONT_FAMILY_GENERIC,XML_TYPE_TEXT_FONTFAMILY, CTF_FONTFAMILY ),
    M_ED( "CharFontPitch",  STYLE,  FONT_PITCH,         XML_TYPE_TEXT_FONTPITCH, CTF_FONTPITCH ),
    M_ED( "CharFontCharSet",    STYLE,  FONT_CHARSET,       XML_TYPE_TEXT_FONTENCODING, CTF_FONTCHARSET ),
    // RES_CHRATR_FONTSIZE
    M_ED( "CharHeight",       FO,   FONT_SIZE,          XML_TYPE_CHAR_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT ),
    M_ED( "CharPropHeight",FO,  FONT_SIZE,          XML_TYPE_CHAR_HEIGHT_PROP|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_REL ),
    M_ED( "CharDiffHeight",STYLE,FONT_SIZE_REL,     XML_TYPE_CHAR_HEIGHT_DIFF, CTF_CHARHEIGHT_DIFF ),
    // RES_CHRATR_KERNING
    M_E( "CharKerning",     FO,     LETTER_SPACING,     XML_TYPE_TEXT_KERNING, 0 ),
    // RES_CHRATR_LANGUAGE
    M_ED( "CharLocale",     FO,     LANGUAGE,           XML_TYPE_CHAR_LANGUAGE|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharLocale",     FO,     COUNTRY,            XML_TYPE_CHAR_COUNTRY|MID_FLAG_MERGE_PROPERTY, 0 ),
    // RES_CHRATR_POSTURE
    M_E( "CharPosture",     FO,     FONT_STYLE,         XML_TYPE_TEXT_POSTURE, 0 ),
    // RES_CHRATR_PROPORTIONALFONTSIZE
    // TODO: not used?
    // RES_CHRATR_SHADOWED
    M_E( "CharShadowed",    FO,     TEXT_SHADOW,        XML_TYPE_TEXT_SHADOWED, 0 ),
    // RES_CHRATR_UNDERLINE
    M_E( "CharUnderline",   STYLE,  TEXT_UNDERLINE,     XML_TYPE_TEXT_UNDERLINE, CTF_UNDERLINE ),
    M_E( "CharUnderlineColor",  STYLE,  TEXT_UNDERLINE_COLOR,       XML_TYPE_TEXT_UNDERLINE_COLOR|MID_FLAG_MULTI_PROPERTY, CTF_UNDERLINE_COLOR  ),
    M_E( "CharUnderlineHasColor",   STYLE,  TEXT_UNDERLINE_COLOR,       XML_TYPE_TEXT_UNDERLINE_HASCOLOR|MID_FLAG_MERGE_ATTRIBUTE, CTF_UNDERLINE_HASCOLOR   ),
    // RES_CHRATR_WEIGHT
    M_E( "CharWeight",      FO,     FONT_WEIGHT,        XML_TYPE_TEXT_WEIGHT, 0 ),
    // RES_CHRATR_WORDLINEMODE
    M_E( "CharWordMode",    FO,     SCORE_SPACES,       XML_TYPE_NBOOL, 0 ),
    // RES_CHRATR_AUTOKERN
    M_E( "CharAutoKerning", STYLE,  LETTER_KERNING,     XML_TYPE_BOOL, 0 ),
    // RES_CHRATR_BLINK
    M_E( "CharFlash",       STYLE,  TEXT_BLINKING,      XML_TYPE_BOOL, 0 ),
    // RES_CHRATR_NOHYPHEN
    // TODO: not used?
    // RES_CHRATR_NOLINEBREAK
    // TODO: not used?
    // RES_CHRATR_BACKGROUND
    M_E( "CharBackColor",   STYLE,  TEXT_BACKGROUND_COLOR, XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "CharBackTransparent", STYLE,  TEXT_BACKGROUND_COLOR, XML_TYPE_ISTRANSPARENT|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    M_E( "CharBackColor",   FO, TEXT_BACKGROUND_COLOR, XML_TYPE_COLOR|MID_FLAG_SPECIAL_ITEM_EXPORT, CTF_OLDTEXTBACKGROUND ),
    // RES_CHRATR_CJK_FONT
    M_ED( "CharFontNameAsian",  STYLE,  FONT_NAME_ASIAN,            XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTNAME_CJK ),
#ifdef CONV_STAR_FONTS
    M_ED( "CharFontNameAsian",  STYLE,      FONT_FAMILY_ASIAN,      XML_TYPE_TEXT_FONTFAMILYNAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTFAMILYNAME_CJK ),
#else
    M_ED( "CharFontNameAsian",  STYLE,      FONT_FAMILY_ASIAN,      XML_TYPE_TEXT_FONTFAMILYNAME, CTF_FONTFAMILYNAME_CJK ),
#endif
    M_ED( "CharFontStyleNameAsian",STYLE,   FONT_STYLE_NAME_ASIAN,  XML_TYPE_STRING, CTF_FONTSTYLENAME_CJK ),
    M_ED( "CharFontFamilyAsian",    STYLE,  FONT_FAMILY_GENERIC_ASIAN,XML_TYPE_TEXT_FONTFAMILY, CTF_FONTFAMILY_CJK ),
    M_ED( "CharFontPitchAsian", STYLE,  FONT_PITCH_ASIAN,           XML_TYPE_TEXT_FONTPITCH, CTF_FONTPITCH_CJK ),
    M_ED( "CharFontCharSetAsian",   STYLE,  FONT_CHARSET_ASIAN,     XML_TYPE_TEXT_FONTENCODING, CTF_FONTCHARSET_CJK ),
    // RES_CHRATR_CJK_FONTSIZE
    M_ED( "CharHeightAsian",          STYLE,    FONT_SIZE_ASIAN,            XML_TYPE_CHAR_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_CJK ),
    M_ED( "CharPropHeightAsian",STYLE,  FONT_SIZE_ASIAN,            XML_TYPE_CHAR_HEIGHT_PROP|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_REL_CJK ),
    M_ED( "CharDiffHeightAsian",STYLE,FONT_SIZE_REL_ASIAN,      XML_TYPE_CHAR_HEIGHT_DIFF, CTF_CHARHEIGHT_DIFF_CJK ),
    // RES_CHRATR_CJK_LANGUAGE
    M_ED( "CharLocaleAsian",        STYLE,      LANGUAGE_ASIAN,             XML_TYPE_CHAR_LANGUAGE|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharLocaleAsian",        STYLE,      COUNTRY_ASIAN,          XML_TYPE_CHAR_COUNTRY|MID_FLAG_MERGE_PROPERTY, 0 ),
    // RES_CHRATR_CJK_POSTURE
    M_E( "CharPostureAsian",        STYLE,      FONT_STYLE_ASIAN,           XML_TYPE_TEXT_POSTURE, 0 ),
    // RES_CHRATR_CJK_WEIGHT
    M_E( "CharWeightAsian",     STYLE,      FONT_WEIGHT_ASIAN,      XML_TYPE_TEXT_WEIGHT, 0 ),
    // RES_CHRATR_CTL_FONT
    M_ED( "CharFontNameComplex",    STYLE,  FONT_NAME_COMPLEX,          XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTNAME_CTL ),
#ifdef CONV_STAR_FONTS
    M_ED( "CharFontNameComplex",    STYLE,      FONT_FAMILY_COMPLEX,        XML_TYPE_TEXT_FONTFAMILYNAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTFAMILYNAME_CTL ),
#else
    M_ED( "CharFontNameComplex",    STYLE,      FONT_FAMILY_COMPLEX,        XML_TYPE_TEXT_FONTFAMILYNAME, CTF_FONTFAMILYNAME_CTL ),
#endif
    M_ED( "CharFontStyleNameComplex",STYLE, FONT_STYLE_NAME_COMPLEX,    XML_TYPE_STRING, CTF_FONTSTYLENAME_CTL ),
    M_ED( "CharFontFamilyComplex",  STYLE,  FONT_FAMILY_GENERIC_COMPLEX,XML_TYPE_TEXT_FONTFAMILY, CTF_FONTFAMILY_CTL ),
    M_ED( "CharFontPitchComplex",   STYLE,  FONT_PITCH_COMPLEX,         XML_TYPE_TEXT_FONTPITCH, CTF_FONTPITCH_CTL ),
    M_ED( "CharFontCharSetComplex", STYLE,  FONT_CHARSET_COMPLEX,       XML_TYPE_TEXT_FONTENCODING, CTF_FONTCHARSET_CTL ),
    // RES_CHRATR_CTL_FONTSIZE
    M_ED( "CharHeightComplex",        STYLE,    FONT_SIZE_COMPLEX,          XML_TYPE_CHAR_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_CTL ),
    M_ED( "CharPropHeightComplex",STYLE,    FONT_SIZE_COMPLEX,          XML_TYPE_CHAR_HEIGHT_PROP|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_REL_CTL ),
    M_ED( "CharDiffHeightComplex",STYLE,FONT_SIZE_REL_COMPLEX,      XML_TYPE_CHAR_HEIGHT_DIFF, CTF_CHARHEIGHT_DIFF_CTL ),
    // RES_CHRATR_CTL_LANGUAGE
    M_ED( "CharLocaleComplex",      STYLE,      LANGUAGE_COMPLEX,           XML_TYPE_CHAR_LANGUAGE|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharLocaleComplex",      STYLE,      COUNTRY_COMPLEX,            XML_TYPE_CHAR_COUNTRY|MID_FLAG_MERGE_PROPERTY, 0 ),
    // RES_CHRATR_CTL_POSTURE
    M_E( "CharPostureComplex",      STYLE,      FONT_STYLE_COMPLEX,         XML_TYPE_TEXT_POSTURE, 0 ),
    // RES_CHRATR_CTL_WEIGHT
    M_E( "CharWeightComplex",       STYLE,      FONT_WEIGHT_COMPLEX,        XML_TYPE_TEXT_WEIGHT, 0 ),
    // RES_CHRATR_ROTATE
    M_E( "CharRotation",            STYLE,      TEXT_ROTATION_ANGLE,        XML_TYPE_TEXT_ROTATION_ANGLE, 0 ),
    M_E( "CharRotationIsFitToLine", STYLE,      TEXT_ROTATION_SCALE,        XML_TYPE_TEXT_ROTATION_SCALE, 0 ),
    // RES_CHRATR_EMPHASIS_MARK
    M_E( "CharEmphasis",            STYLE,      TEXT_EMPHASIZE,             XML_TYPE_TEXT_EMPHASIZE, 0 ),
    // RES_CHRATR_TWO_LINES
    M_E( "CharCombineIsOn",         STYLE,      TEXT_COMBINE,               XML_TYPE_TEXT_COMBINE, 0 ),
    M_E( "CharCombinePrefix",       STYLE,      TEXT_COMBINE_START_CHAR,    XML_TYPE_TEXT_COMBINECHAR, 0 ),
    M_E( "CharCombineSuffix",       STYLE,      TEXT_COMBINE_END_CHAR,      XML_TYPE_TEXT_COMBINECHAR, 0 ),
    // RES_CHRATR_SCALEW
    M_E( "CharScaleWidth",          STYLE,      TEXT_SCALE,                 XML_TYPE_PERCENT16, 0 ),
    //RES_CHRATR_RELIEF
    M_E( "CharRelief",              STYLE,      FONT_RELIEF,                XML_TYPE_TEXT_FONT_RELIEF, 0 ),
    // RES_TXTATR_INETFMT
    // TODO
    // RES_TXTATR_REFMARK
    // TODO
    // RES_TXTATR_TOXMARK
    // TODO
    // RES_TXTATR_CHARFMT
//  M_E_SI( TEXT,   style_name,         RES_TXTATR_CHARFMT, 0 ),
    // RES_TXTATR_CJK_RUBY
    // TODO
    // RES_TXTATR_FIELD
    // TODO
    // RES_TXTATR_FLYCNT
    // TODO
    // RES_TXTATR_FTN
    // TODO
    // RES_TXTATR_SOFTHYPH
    // TODO
    // RES_TXTATR_HARDBLANK
    // TODO

    // RES_PARATR_LINESPACING
    M_E( "ParaLineSpacing",     FO,     LINE_HEIGHT,            XML_TYPE_LINE_SPACE_FIXED, 0 ),
    M_E( "ParaLineSpacing",     STYLE,  LINE_HEIGHT_AT_LEAST,   XML_TYPE_LINE_SPACE_MINIMUM, 0 ),
    M_E( "ParaLineSpacing",     STYLE,  LINE_SPACING,           XML_TYPE_LINE_SPACE_DISTANCE, 0 ),
    // RES_PARATR_ADJUST
    M_E( "ParaAdjust",          FO,     TEXT_ALIGN,         XML_TYPE_TEXT_ADJUST, CTF_SD_SHAPE_PARA_ADJUST ),
    M_E( "ParaLastLineAdjust",  FO,     TEXT_ALIGN_LAST,    XML_TYPE_TEXT_ADJUSTLAST, 0 ),
    M_E( "ParaExpandSingleWord",STYLE,  JUSTIFY_SINGLE_WORD,XML_TYPE_BOOL, 0 ),
    // RES_PARATR_SPLIT
    M_E( "ParaSplit",           STYLE,  BREAK_INSIDE,       XML_TYPE_TEXT_SPLIT, 0 ),
    // RES_PARATR_ORPHANS
    M_E( "ParaOrphans",         FO,     ORPHANS,            XML_TYPE_NUMBER8, 0 ),
    // RES_PARATR_WIDOWS
    M_E( "ParaWidows",          FO,     WIDOWS,             XML_TYPE_NUMBER8, 0 ),
    // RES_PARATR_TABSTOP
    M_ED( "ParaTabStops",       STYLE,  TAB_STOPS,          MID_FLAG_ELEMENT_ITEM|XML_TYPE_TEXT_TABSTOP, CTF_TABSTOP ), // this is not realy a string!
    // RES_PARATR_HYPHENZONE
    M_E( "ParaIsHyphenation",   FO,     HYPHENATE,          XML_TYPE_BOOL, 0 ),
    M_E( "ParaHyphenationMaxLeadingChars", FO, HYPHENATION_REMAIN_CHAR_COUNT, XML_TYPE_NUMBER16, 0 ),
    M_E( "ParaHyphenationMaxTrailingChars",FO, HYPHENATION_PUSH_CHAR_COUNT, XML_TYPE_NUMBER16, 0 ),
    M_E( "ParaHyphenationMaxHyphens",   FO, HYPHENATION_LADDER_COUNT, XML_TYPE_NUMBER16_NONE, 0 ),
    // RES_PARATR_DROP
    M_E( "DropCapWholeWord",    STYLE,  LENGTH,     MID_FLAG_SPECIAL_ITEM|XML_TYPE_BOOL, CTF_DROPCAPWHOLEWORD ),
    M_E( "DropCapCharStyleName",    STYLE,  STYLE_NAME, MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING, CTF_DROPCAPCHARSTYLE ),
    M_E( "DropCapFormat",       STYLE,  DROP_CAP,   MID_FLAG_ELEMENT_ITEM|XML_TYPE_TEXT_DROPCAP, CTF_DROPCAPFORMAT ),
    // RES_PARATR_REGISTER
    M_E( "ParaRegisterModeActive",  STYLE,  REGISTER_TRUE,  XML_TYPE_BOOL, 0 ),
    // RES_PARATR_NUMRULE
    M_E( "NumberingStyleName",  STYLE,  LIST_STYLE_NAME,    MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING, CTF_NUMBERINGSTYLENAME ),
    M_E( "IsNumbering",         TEXT,   ENABLE_NUMBERING,   XML_TYPE_BOOL, 0 ),

    // RES_FILL_ORDER
    // not required
    // RES_FRM_SIZE
    // not required
    // RES_PAPER_BIN
    // not required
    // RES_LR_SPACE

    M_E( "ParaFirstLineIndent",     FO, TEXT_INDENT,        XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_PARAFIRSTLINE ),
    M_E( "ParaFirstLineIndentRelative", FO, TEXT_INDENT,    XML_TYPE_PERCENT, CTF_PARAFIRSTLINE_REL ),
    M_E( "ParaIsAutoFirstLineIndent",   STYLE, AUTO_TEXT_INDENT,    XML_TYPE_BOOL, 0 ),
    // RES_PAGEDESC
    M_E( "PageDescName",            STYLE,  MASTER_PAGE_NAME,           MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING, CTF_PAGEDESCNAME ),
    M_E( "PageNumberOffset",        STYLE,  PAGE_NUMBER,            XML_TYPE_NUMBER16, 0 ),
    // RES_BREAK : TODO: does this work?
    M_E( "BreakType",       FO, BREAK_BEFORE,       XML_TYPE_TEXT_BREAKBEFORE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "BreakType",       FO, BREAK_AFTER,        XML_TYPE_TEXT_BREAKAFTER, 0 ),
    // RES_CNTNT
    // not required
    // RES_HEADER
    // not required
    // RES_FOOTER
    // not required
    // RES_PRINT
    // not required
    // RES_OPAQUE
    // not required
    // RES_PROTECT
    // not required
    // RES_SURROUND
    // not required
    // RES_VERT_ORIENT
    // not required
    // RES_HORI_ORIENT
    // not required
    // RES_ANCHOR
    // not required
    // RES_BACKGROUND
    M_E( "ParaBackColor",   FO, BACKGROUND_COLOR,       XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "ParaBackTransparent", FO, BACKGROUND_COLOR,       XML_TYPE_ISTRANSPARENT|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    M_E( "ParaBackGraphicLocation", STYLE,  POSITION,   MID_FLAG_SPECIAL_ITEM|XML_TYPE_BUILDIN_CMP_ONLY, CTF_BACKGROUND_POS  ),
    M_E( "ParaBackGraphicFilter",STYLE, FILTER_NAME,    MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING, CTF_BACKGROUND_FILTER ),
    M_E( "ParaBackGraphicURL",  STYLE,  BACKGROUND_IMAGE,   MID_FLAG_ELEMENT_ITEM|XML_TYPE_STRING, CTF_BACKGROUND_URL ),
    // RES_BOX
    M_E( "LeftBorder",          STYLE,  BORDER_LINE_WIDTH,        XML_TYPE_BORDER_WIDTH, CTF_ALLBORDERWIDTH ),
    M_E( "LeftBorder",          STYLE,  BORDER_LINE_WIDTH_LEFT,   XML_TYPE_BORDER_WIDTH, CTF_LEFTBORDERWIDTH ),
    M_E( "RightBorder",         STYLE,  BORDER_LINE_WIDTH_RIGHT,  XML_TYPE_BORDER_WIDTH, CTF_RIGHTBORDERWIDTH ),
    M_E( "TopBorder",           STYLE,  BORDER_LINE_WIDTH_TOP,    XML_TYPE_BORDER_WIDTH, CTF_TOPBORDERWIDTH ),
    M_E( "BottomBorder",        STYLE,  BORDER_LINE_WIDTH_BOTTOM, XML_TYPE_BORDER_WIDTH, CTF_BOTTOMBORDERWIDTH ),

    M_E( "LeftBorderDistance",  FO,     PADDING,                  XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_ALLBORDERDISTANCE ), // need special import filtering
    M_E( "LeftBorderDistance",  FO,     PADDING_LEFT,             XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_LEFTBORDERDISTANCE ),
    M_E( "RightBorderDistance", FO,     PADDING_RIGHT,            XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_RIGHTBORDERDISTANCE ),
    M_E( "TopBorderDistance",   FO,     PADDING_TOP,              XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_TOPBORDERDISTANCE ),
    M_E( "BottomBorderDistance",FO,     PADDING_BOTTOM,           XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_BOTTOMBORDERDISTANCE ),

    M_E( "LeftBorder",          FO,     BORDER,                   XML_TYPE_BORDER, CTF_ALLBORDER ),
    M_E( "LeftBorder",          FO,     BORDER_LEFT,              XML_TYPE_BORDER, CTF_LEFTBORDER ),
    M_E( "RightBorder",         FO,     BORDER_RIGHT,             XML_TYPE_BORDER, CTF_RIGHTBORDER ),
    M_E( "TopBorder",           FO,     BORDER_TOP,               XML_TYPE_BORDER, CTF_TOPBORDER ),
    M_E( "BottomBorder",        FO,     BORDER_BOTTOM,            XML_TYPE_BORDER, CTF_BOTTOMBORDER ),
    // RES_SHADOW
    M_E( "ParaShadowFormat",    STYLE,  SHADOW,     XML_TYPE_TEXT_SHADOW, 0 ),
    // RES_FRMMACRO
    // not required
    // RES_COL
    // not required
    // RES_KEEP
    M_E( "ParaKeepTogether",    FO, KEEP_WITH_NEXT,     XML_TYPE_BOOL, 0 ),
    // RES_URL
    // not required
    // RES_EDIT_IN_READONLY
    // not required
    // RES_LAYOUT_SPLIT
    // not required
    // RES_CHAIN
    // not required

    // RES_LINENUMBER
    M_E( "ParaLineNumberCount", TEXT,   NUMBER_LINES,           XML_TYPE_BOOL, 0 ),
    M_E( "ParaLineNumberStartValue", TEXT, LINE_NUMBER,         XML_TYPE_NUMBER, 0 ),

    // RES_FTN_AT_TXTEND
    // not required
    // RES_END_AT_TXTEND
    // not required
    M_ED( "ParaIsCharacterDistance", STYLE, TEXT_AUTOSPACE, XML_TYPE_TEXT_AUTOSPACE, 0 ),
    M_ED( "ParaIsHangingPunctuation", STYLE, PUNCTUATION_WRAP, XML_TYPE_TEXT_PUNCTUATION_WRAP, 0 ),
    M_ED( "ParaIsForbiddenRules", STYLE, LINE_BREAK, XML_TYPE_TEXT_LINE_BREAK, 0 ),
    M_E( "TabStopDistance", STYLE, TAB_STOP_DISTANCE, XML_TYPE_MEASURE, 0 ),

    // RES_PARATR_VERTALIGN
    M_E( "ParaVertAlignment", STYLE, VERTICAL_ALIGN,    XML_TYPE_TEXT_VERTICAL_ALIGN, 0 ),

    // RES_PARATR_SNAPTOGRID
    M_E( "SnapToGrid", STYLE, SNAP_TO_LAYOUT_GRID, XML_TYPE_BOOL, 0 ),

    M_ED( "WritingMode",      STYLE, WRITING_MODE,       XML_TYPE_TEXT_WRITING_MODE_WITH_DEFAULT, 0 ),

    M_E( "ParaIsConnectBorder", STYLE, JOIN_BORDER,  XML_TYPE_BOOL, 0 ),

    M_END()
};

XMLPropertyMapEntry aXMLTextPropMap[] =
{
    // RES_CHRATR_CASEMAP
    M_E( "CharCaseMap",     FO,     FONT_VARIANT,       XML_TYPE_TEXT_CASEMAP_VAR,  0 ),
    M_E( "CharCaseMap",     FO,     TEXT_TRANSFORM,     XML_TYPE_TEXT_CASEMAP,  0 ),
    // RES_CHRATR_COLOR
    M_ED( "CharColor",      FO,     COLOR,              XML_TYPE_COLORAUTO|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharColor",      STYLE,  USE_WINDOW_FONT_COLOR,  XML_TYPE_ISAUTOCOLOR|MID_FLAG_MERGE_PROPERTY,   0 ),
    // RES_CHRATR_CONTOUR
    M_E( "CharContoured",   STYLE,  TEXT_OUTLINE,       XML_TYPE_BOOL,  0 ),
    // RES_CHRATR_CROSSEDOUT
    M_E( "CharStrikeout",   STYLE,  TEXT_CROSSING_OUT,  XML_TYPE_TEXT_CROSSEDOUT,   0),
    // RES_CHRATR_ESCAPEMENT
    M_E( "CharEscapement",       STYLE, TEXT_POSITION,  XML_TYPE_TEXT_ESCAPEMENT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "CharEscapementHeight", STYLE, TEXT_POSITION,  XML_TYPE_TEXT_ESCAPEMENT_HEIGHT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    // RES_CHRATR_FONT
    M_ED( "CharFontName",   STYLE,  FONT_NAME,          XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTNAME ),
#ifdef CONV_STAR_FONTS
    M_ED( "CharFontName",   FO,     FONT_FAMILY,        XML_TYPE_TEXT_FONTFAMILYNAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTFAMILYNAME ),
#else
    M_ED( "CharFontName",   FO,     FONT_FAMILY,        XML_TYPE_TEXT_FONTFAMILYNAME, CTF_FONTFAMILYNAME ),
#endif
    M_ED( "CharFontStyleName",STYLE,    FONT_STYLE_NAME,    XML_TYPE_STRING, CTF_FONTSTYLENAME ),
    M_ED( "CharFontFamily", STYLE,  FONT_FAMILY_GENERIC,XML_TYPE_TEXT_FONTFAMILY, CTF_FONTFAMILY ),
    M_ED( "CharFontPitch",  STYLE,  FONT_PITCH,         XML_TYPE_TEXT_FONTPITCH, CTF_FONTPITCH ),
    M_ED( "CharFontCharSet",    STYLE,  FONT_CHARSET,       XML_TYPE_TEXT_FONTENCODING, CTF_FONTCHARSET ),
    // RES_CHRATR_FONTSIZE
    M_ED( "CharHeight",       FO,   FONT_SIZE,          XML_TYPE_CHAR_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT ),
    M_ED( "CharPropHeight",FO,  FONT_SIZE,          XML_TYPE_CHAR_HEIGHT_PROP|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_REL ),
    M_ED( "CharDiffHeight",STYLE,FONT_SIZE_REL,     XML_TYPE_CHAR_HEIGHT_DIFF, CTF_CHARHEIGHT_DIFF ),
    // RES_CHRATR_KERNING
    M_E( "CharKerning",     FO,     LETTER_SPACING,     XML_TYPE_TEXT_KERNING, 0 ),
    // RES_CHRATR_LANGUAGE
    M_ED( "CharLocale",     FO,     LANGUAGE,           XML_TYPE_CHAR_LANGUAGE|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharLocale",     FO,     COUNTRY,            XML_TYPE_CHAR_COUNTRY|MID_FLAG_MERGE_PROPERTY, 0 ),
    // RES_CHRATR_POSTURE
    M_E( "CharPosture",     FO,     FONT_STYLE,         XML_TYPE_TEXT_POSTURE, 0 ),
    // RES_CHRATR_PROPORTIONALFONTSIZE
    // TODO: not used?
    // RES_CHRATR_SHADOWED
    M_E( "CharShadowed",    FO,     TEXT_SHADOW,        XML_TYPE_TEXT_SHADOWED, 0 ),
    // BIS HIER GEPRUEFT!
    // RES_CHRATR_UNDERLINE
    M_E( "CharUnderline",   STYLE,  TEXT_UNDERLINE,     XML_TYPE_TEXT_UNDERLINE, CTF_UNDERLINE ),
    M_E( "CharUnderlineColor",  STYLE,  TEXT_UNDERLINE_COLOR,       XML_TYPE_TEXT_UNDERLINE_COLOR|MID_FLAG_MULTI_PROPERTY, CTF_UNDERLINE_COLOR  ),
    M_E( "CharUnderlineHasColor",   STYLE,  TEXT_UNDERLINE_COLOR,       XML_TYPE_TEXT_UNDERLINE_HASCOLOR|MID_FLAG_MERGE_ATTRIBUTE, CTF_UNDERLINE_HASCOLOR   ),
    // RES_CHRATR_WEIGHT
    M_E( "CharWeight",      FO,     FONT_WEIGHT,        XML_TYPE_TEXT_WEIGHT, 0 ),
    // RES_CHRATR_WORDLINEMODE
    M_E( "CharWordMode",    FO,     SCORE_SPACES,       XML_TYPE_NBOOL, 0 ),
    // RES_CHRATR_AUTOKERN
    M_E( "CharAutoKerning", STYLE,  LETTER_KERNING,     XML_TYPE_BOOL, 0 ),
    // RES_CHRATR_BLINK
    M_E( "CharFlash",       STYLE,  TEXT_BLINKING,      XML_TYPE_BOOL, 0 ),
    // RES_CHRATR_NOHYPHEN
    // TODO: not used?
    // RES_CHRATR_NOLINEBREAK
    // TODO: not used?
    // RES_CHRATR_BACKGROUND
    M_E( "CharBackColor",   STYLE,  TEXT_BACKGROUND_COLOR, XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "CharBackTransparent", STYLE,  TEXT_BACKGROUND_COLOR, XML_TYPE_ISTRANSPARENT|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    M_E( "CharBackColor",   FO, TEXT_BACKGROUND_COLOR, XML_TYPE_COLOR|MID_FLAG_SPECIAL_ITEM_EXPORT, CTF_OLDTEXTBACKGROUND ),
    // RES_CHRATR_CJK_FONT
    M_ED( "CharFontNameAsian",  STYLE,  FONT_NAME_ASIAN,            XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTNAME_CJK ),
#ifdef CONV_STAR_FONTS
    M_ED( "CharFontNameAsian",  STYLE,      FONT_FAMILY_ASIAN,      XML_TYPE_TEXT_FONTFAMILYNAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTFAMILYNAME_CJK ),
#else
    M_ED( "CharFontNameAsian",  STYLE,      FONT_FAMILY_ASIAN,      XML_TYPE_TEXT_FONTFAMILYNAME, CTF_FONTFAMILYNAME_CJK ),
#endif
    M_ED( "CharFontStyleNameAsian",STYLE,   FONT_STYLE_NAME_ASIAN,  XML_TYPE_STRING, CTF_FONTSTYLENAME_CJK ),
    M_ED( "CharFontFamilyAsian",    STYLE,  FONT_FAMILY_GENERIC_ASIAN,XML_TYPE_TEXT_FONTFAMILY, CTF_FONTFAMILY_CJK ),
    M_ED( "CharFontPitchAsian", STYLE,  FONT_PITCH_ASIAN,           XML_TYPE_TEXT_FONTPITCH, CTF_FONTPITCH_CJK ),
    M_ED( "CharFontCharSetAsian",   STYLE,  FONT_CHARSET_ASIAN,     XML_TYPE_TEXT_FONTENCODING, CTF_FONTCHARSET_CJK ),
    // RES_CHRATR_CJK_FONTSIZE
    M_ED( "CharHeightAsian",          STYLE,    FONT_SIZE_ASIAN,            XML_TYPE_CHAR_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_CJK ),
    M_ED( "CharPropHeightAsian",STYLE,  FONT_SIZE_ASIAN,            XML_TYPE_CHAR_HEIGHT_PROP|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_REL_CJK ),
    M_ED( "CharDiffHeightAsian",STYLE,FONT_SIZE_REL_ASIAN,      XML_TYPE_CHAR_HEIGHT_DIFF, CTF_CHARHEIGHT_DIFF_CJK ),
    // RES_CHRATR_CJK_LANGUAGE
    M_ED( "CharLocaleAsian",        STYLE,      LANGUAGE_ASIAN,             XML_TYPE_CHAR_LANGUAGE|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharLocaleAsian",        STYLE,      COUNTRY_ASIAN,          XML_TYPE_CHAR_COUNTRY|MID_FLAG_MERGE_PROPERTY, 0 ),
    // RES_CHRATR_CJK_POSTURE
    M_E( "CharPostureAsian",        STYLE,      FONT_STYLE_ASIAN,           XML_TYPE_TEXT_POSTURE, 0 ),
    // RES_CHRATR_CJK_WEIGHT
    M_E( "CharWeightAsian",     STYLE,      FONT_WEIGHT_ASIAN,      XML_TYPE_TEXT_WEIGHT, 0 ),
    // RES_CHRATR_CTL_FONT
    M_ED( "CharFontNameComplex",    STYLE,  FONT_NAME_COMPLEX,          XML_TYPE_STRING|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTNAME_CTL ),
#ifdef CONV_STAR_FONTS
    M_ED( "CharFontNameComplex",    STYLE,      FONT_FAMILY_COMPLEX,        XML_TYPE_TEXT_FONTFAMILYNAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_FONTFAMILYNAME_CTL ),
#else
    M_ED( "CharFontNameComplex",    STYLE,      FONT_FAMILY_COMPLEX,        XML_TYPE_TEXT_FONTFAMILYNAME, CTF_FONTFAMILYNAME_CTL ),
#endif
    M_ED( "CharFontStyleNameComplex",STYLE, FONT_STYLE_NAME_COMPLEX,    XML_TYPE_STRING, CTF_FONTSTYLENAME_CTL ),
    M_ED( "CharFontFamilyComplex",  STYLE,  FONT_FAMILY_GENERIC_COMPLEX,XML_TYPE_TEXT_FONTFAMILY, CTF_FONTFAMILY_CTL ),
    M_ED( "CharFontPitchComplex",   STYLE,  FONT_PITCH_COMPLEX,         XML_TYPE_TEXT_FONTPITCH, CTF_FONTPITCH_CTL ),
    M_ED( "CharFontCharSetComplex", STYLE,  FONT_CHARSET_COMPLEX,       XML_TYPE_TEXT_FONTENCODING, CTF_FONTCHARSET_CTL ),
    // RES_CHRATR_CTL_FONTSIZE
    M_ED( "CharHeightComplex",        STYLE,    FONT_SIZE_COMPLEX,          XML_TYPE_CHAR_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_CTL ),
    M_ED( "CharPropHeightComplex",STYLE,    FONT_SIZE_COMPLEX,          XML_TYPE_CHAR_HEIGHT_PROP|MID_FLAG_MULTI_PROPERTY, CTF_CHARHEIGHT_REL_CTL ),
    M_ED( "CharDiffHeightComplex",STYLE,FONT_SIZE_REL_COMPLEX,      XML_TYPE_CHAR_HEIGHT_DIFF, CTF_CHARHEIGHT_DIFF_CTL ),
    // RES_CHRATR_CTL_LANGUAGE
    M_ED( "CharLocaleComplex",      STYLE,      LANGUAGE_COMPLEX,           XML_TYPE_CHAR_LANGUAGE|MID_FLAG_MERGE_PROPERTY, 0 ),
    M_ED( "CharLocaleComplex",      STYLE,      COUNTRY_COMPLEX,            XML_TYPE_CHAR_COUNTRY|MID_FLAG_MERGE_PROPERTY, 0 ),
    // RES_CHRATR_CTL_POSTURE
    M_E( "CharPostureComplex",      STYLE,      FONT_STYLE_COMPLEX,         XML_TYPE_TEXT_POSTURE, 0 ),
    // RES_CHRATR_CTL_WEIGHT
    M_E( "CharWeightComplex",       STYLE,      FONT_WEIGHT_COMPLEX,        XML_TYPE_TEXT_WEIGHT, 0 ),
    // RES_CHRATR_ROTATE
    M_E( "CharRotation",            STYLE,      TEXT_ROTATION_ANGLE,        XML_TYPE_TEXT_ROTATION_ANGLE, 0 ),
    M_E( "CharRotationIsFitToLine", STYLE,      TEXT_ROTATION_SCALE,        XML_TYPE_TEXT_ROTATION_SCALE, 0 ),
    // RES_CHRATR_EMPHASIS_MARK
    M_E( "CharEmphasis",            STYLE,      TEXT_EMPHASIZE,             XML_TYPE_TEXT_EMPHASIZE, 0 ),
    // RES_CHRATR_TWO_LINES
    M_E( "CharCombineIsOn",         STYLE,      TEXT_COMBINE,               XML_TYPE_TEXT_COMBINE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "CharCombinePrefix",       STYLE,      TEXT_COMBINE_START_CHAR,    XML_TYPE_TEXT_COMBINECHAR, 0 ),
    M_E( "CharCombineSuffix",       STYLE,      TEXT_COMBINE_END_CHAR,      XML_TYPE_TEXT_COMBINECHAR, 0 ),
    // RES_CHRATR_SCALEW
    M_E( "CharScaleWidth",          STYLE,      TEXT_SCALE,                 XML_TYPE_PERCENT16, 0 ),
    // combined characters field, does not correspond to a property
    M_E( "",                        STYLE,      TEXT_COMBINE,               XML_TYPE_TEXT_COMBINE_CHARACTERS|MID_FLAG_NO_PROPERTY, CTF_COMBINED_CHARACTERS_FIELD ),
    //RES_CHRATR_RELIEF
    M_E( "CharRelief",              STYLE,      FONT_RELIEF,                XML_TYPE_TEXT_FONT_RELIEF, 0 ),

    // RES_TXTATR_INETFMT
    M_E( "HyperLinkURL",            TEXT,       XMLNS,                      XML_TYPE_STRING|MID_FLAG_NO_PROPERTY_IMPORT,    CTF_HYPERLINK_URL ),
    // RES_TXTATR_REFMARK
    // TODO
    // RES_TXTATR_TOXMARK
    // TODO
    // RES_TXTATR_CHARFMT
    M_E( "CharStyleName",           TEXT,       STYLE_NAME,                 XML_TYPE_STRING|MID_FLAG_NO_PROPERTY_IMPORT,    CTF_CHAR_STYLE_NAME ),
    // RES_TXTATR_CJK_RUBY
    // TODO
    // RES_TXTATR_FIELD
    // TODO
    // RES_TXTATR_FLYCNT
    // TODO
    // RES_TXTATR_FTN
    // TODO
    // RES_TXTATR_SOFTHYPH
    // TODO
    // RES_TXTATR_HARDBLANK
    // TODO
    // RES_UNKNOWNATR_CONTAINER
    M_E( "TextUserDefinedAttributes", TEXT, XMLNS, XML_TYPE_ATTRIBUTE_CONTAINER | MID_FLAG_SPECIAL_ITEM, 0 ),
    M_ED( "ParaIsCharacterDistance", STYLE, TEXT_AUTOSPACE, XML_TYPE_TEXT_AUTOSPACE, 0 ),
    M_ED( "ParaIsHangingPunctuation", STYLE, PUNCTUATION_WRAP, XML_TYPE_TEXT_PUNCTUATION_WRAP, 0 ),
    M_ED( "ParaIsForbiddenRules", STYLE, LINE_BREAK, XML_TYPE_TEXT_LINE_BREAK, 0 ),
    M_E( "TabStopDistance", STYLE, TAB_STOP_DISTANCE, XML_TYPE_MEASURE, 0 ),

    M_END()
};

XMLPropertyMapEntry aXMLFramePropMap[] =
{
    // RES_FILL_ORDER
    // TODO: not required???
    // RES_FRM_SIZE
    M_ED( "Width",          SVG, WIDTH,             XML_TYPE_MEASURE, 0 ),
    M_ED( "RelativeWidth",  STYLE, REL_WIDTH,       XML_TYPE_TEXT_REL_WIDTH_HEIGHT, 0 ),
//  M_ED( "RelativeWidth",  STYLE, REL_WIDTH,       XML_TYPE_TEXT_REL_WIDTH_HEIGHT|MID_FLAG_MULTI_PROPERTY, 0 ),
//  M_ED( "IsSyncWidthToHeight",STYLE, REL_WIDTH,   XML_TYPE_TEXT_SYNC_WIDTH_HEIGHT|MID_FLAG_MULTI_PROPERTY, 0 ),

    M_ED( "Height",         SVG, HEIGHT,            XML_TYPE_MEASURE, CTF_FRAMEHEIGHT_ABS ),
    M_ED( "Height",         FO, MIN_HEIGHT,         XML_TYPE_MEASURE|MID_FLAG_MULTI_PROPERTY, CTF_FRAMEHEIGHT_MIN_ABS ),
    M_ED( "RelativeHeight", FO, MIN_HEIGHT,         XML_TYPE_TEXT_REL_WIDTH_HEIGHT, CTF_FRAMEHEIGHT_MIN_REL ),
    M_ED( "RelativeHeight", STYLE, REL_HEIGHT,      XML_TYPE_TEXT_REL_WIDTH_HEIGHT, CTF_FRAMEHEIGHT_REL ),
//  M_ED( "RelativeHeight", STYLE, REL_HEIGHT,      XML_TYPE_TEXT_REL_WIDTH_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_FRAMEHEIGHT_REL ),
//  M_ED( "IsSyncHeightToWidth",STYLE, REL_HEIGHT,  XML_TYPE_TEXT_SYNC_WIDTH_HEIGHT|MID_FLAG_MULTI_PROPERTY, CTF_SYNCHEIGHT ),
//  M_ED( "IsSyncHeightToWidth",STYLE, REL_HEIGHT,  XML_TYPE_TEXT_SYNC_WIDTH_HEIGHT_MIN, CTF_SYNCHEIGHT_MIN ),
    M_ED( "SizeType",       FO, TEXT_BOX,           XML_TYPE_NUMBER16|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_SIZETYPE ),
    // RES_PAPER_BIN
    // not required
    // RES_ANCHOR
    // moved to here because it is not used for automatic styles
    M_ED( "AnchorType",         TEXT,   ANCHOR_TYPE,            XML_TYPE_TEXT_ANCHOR_TYPE, CTF_ANCHORTYPE ),
    // AnchorPage number is not required for styles!
    M_ED( "HoriOrientPosition", SVG,    X,        XML_TYPE_MEASURE, 0 ),
    M_ED( "VertOrientPosition", SVG,    Y,        XML_TYPE_MEASURE, 0 ),
    // ***** The map for automatic styles starts here *****
    // RES_LR_SPACE
    M_E( "LeftMargin",              FO, MARGIN_LEFT,        XML_TYPE_MEASURE,  0),
    M_E( "RightMargin",             FO, MARGIN_RIGHT,       XML_TYPE_MEASURE, 0 ),
    // RES_UL_SPACE
    M_E( "TopMargin",               FO, MARGIN_TOP,         XML_TYPE_MEASURE, 0 ),
    M_E( "BottomMargin",            FO, MARGIN_BOTTOM,      XML_TYPE_MEASURE, 0 ),
    // RES_PAGEDESC
    // not required
    // RES_BREAK
    // not required
    // RES_CNTNT
    // not required (accessed using API)
    // RES_HEADER
    // not required
    // RES_FOOTER
    // not required
    // RES_PRINT
    M_E( "Print",                   STYLE,  PRINT_CONTENT,  XML_TYPE_BOOL, 0 ),
    // RES_OPAQUE
    M_ED( "Opaque",                 STYLE,  RUN_THROUGH,    XML_TYPE_TEXT_OPAQUE, 0 ),
    // RES_PROTECT
    M_E( "ContentProtected",        STYLE,  PROTECT,        XML_TYPE_TEXT_PROTECT_CONTENT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "SizeProtected",       STYLE,  PROTECT,        XML_TYPE_TEXT_PROTECT_SIZE|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "PositionProtected",       STYLE,  PROTECT,        XML_TYPE_TEXT_PROTECT_POSITION|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    // RES_SURROUND
    M_ED( "TextWrap",               STYLE,  WRAP,   XML_TYPE_TEXT_WRAP, CTF_WRAP ),
    M_ED( "SurroundAnchorOnly",     STYLE,  NUMBER_WRAPPED_PARAGRAPHS,  XML_TYPE_TEXT_PARAGRAPH_ONLY, CTF_WRAP_PARAGRAPH_ONLY ),
    M_E( "SurroundContour",         STYLE,  WRAP_CONTOUR,   XML_TYPE_BOOL, CTF_WRAP_CONTOUR ),
    M_E( "ContourOutside",          STYLE,  WRAP_CONTOUR_MODE,  XML_TYPE_TEXT_WRAP_OUTSIDE, CTF_WRAP_CONTOUR_MODE ),
    // RES_VERT_ORIENT
    M_ED( "VertOrient",             STYLE,  VERTICAL_POS,         XML_TYPE_TEXT_VERTICAL_POS, CTF_VERTICALPOS ),
    M_ED( "VertOrient",             STYLE,  VERTICAL_POS,         XML_TYPE_TEXT_VERTICAL_POS_AT_CHAR, CTF_VERTICALPOS_ATCHAR ),
    M_ED( "VertOrient",             STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL_AS_CHAR|MID_FLAG_MULTI_PROPERTY, CTF_VERTICALREL_ASCHAR ),
    M_ED( "VertOrientRelation",     STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL, CTF_VERTICALREL ),
    M_ED( "VertOrientRelation",     STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL_PAGE|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_VERTICALREL_PAGE ),
    M_ED( "VertOrientRelation",     STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL_FRAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_VERTICALREL_FRAME ),
    // RES_HORI_ORIENT
    M_ED( "HoriOrient",             STYLE,  HORIZONTAL_POS,       XML_TYPE_TEXT_HORIZONTAL_POS|MID_FLAG_MULTI_PROPERTY, CTF_HORIZONTALPOS ),
    M_ED( "PageToggle",     STYLE,  HORIZONTAL_POS,       XML_TYPE_TEXT_HORIZONTAL_MIRROR, CTF_HORIZONTALMIRROR ),
    M_ED( "HoriOrient",             STYLE,  HORIZONTAL_POS,       XML_TYPE_TEXT_HORIZONTAL_POS_MIRRORED|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_HORIZONTALPOS_MIRRORED ),
    M_ED( "HoriOrientRelation",     STYLE,  HORIZONTAL_REL,       XML_TYPE_TEXT_HORIZONTAL_REL, CTF_HORIZONTALREL ),
    M_ED( "HoriOrientRelation",     STYLE,  HORIZONTAL_REL,       XML_TYPE_TEXT_HORIZONTAL_REL_FRAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_HORIZONTALREL_FRAME ),
    // RES_ANCHOR
    // see above
    // RES_BACKGROUND
    M_ED( "BackColorRGB",   FO, BACKGROUND_COLOR,       XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_ED( "BackTransparent",    FO, BACKGROUND_COLOR,       XML_TYPE_ISTRANSPARENT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, CTF_BACKGROUND_TRANSPARENT ),
    M_ED( "BackColorTransparency", STYLE, BACKGROUND_TRANSPARENCY, XML_TYPE_PERCENT8, CTF_BACKGROUND_TRANSPARENCY ),

    M_E( "BackGraphicTransparency", STYLE, BACKGROUND_IMAGE_TRANSPARENCY, MID_FLAG_SPECIAL_ITEM|XML_TYPE_PERCENT8, CTF_BACKGROUND_TRANSPARENCY ),
    M_E( "BackGraphicLocation", STYLE,  POSITION,   MID_FLAG_SPECIAL_ITEM|XML_TYPE_BUILDIN_CMP_ONLY, CTF_BACKGROUND_POS  ),
    M_E( "BackGraphicFilter",STYLE, FILTER_NAME,    MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING, CTF_BACKGROUND_FILTER ),
    M_E( "BackGraphicURL",  STYLE,  BACKGROUND_IMAGE,   MID_FLAG_ELEMENT_ITEM|XML_TYPE_STRING, CTF_BACKGROUND_URL ),

    // RES_BOX
    M_ED( "LeftBorder",         STYLE,  BORDER_LINE_WIDTH,        XML_TYPE_BORDER_WIDTH, CTF_ALLBORDERWIDTH ),
    M_ED( "LeftBorder",         STYLE,  BORDER_LINE_WIDTH_LEFT,   XML_TYPE_BORDER_WIDTH, CTF_LEFTBORDERWIDTH ),
    M_ED( "RightBorder",            STYLE,  BORDER_LINE_WIDTH_RIGHT,  XML_TYPE_BORDER_WIDTH, CTF_RIGHTBORDERWIDTH ),
    M_ED( "TopBorder",          STYLE,  BORDER_LINE_WIDTH_TOP,    XML_TYPE_BORDER_WIDTH, CTF_TOPBORDERWIDTH ),
    M_ED( "BottomBorder",       STYLE,  BORDER_LINE_WIDTH_BOTTOM, XML_TYPE_BORDER_WIDTH, CTF_BOTTOMBORDERWIDTH ),

    M_ED( "LeftBorderDistance", FO,     PADDING,                  XML_TYPE_MEASURE, CTF_ALLBORDERDISTANCE ), // need special import filtering
    M_ED( "LeftBorderDistance", FO,     PADDING_LEFT,             XML_TYPE_MEASURE, CTF_LEFTBORDERDISTANCE ),
    M_ED( "RightBorderDistance",    FO,     PADDING_RIGHT,            XML_TYPE_MEASURE, CTF_RIGHTBORDERDISTANCE ),
    M_ED( "TopBorderDistance",  FO,     PADDING_TOP,              XML_TYPE_MEASURE, CTF_TOPBORDERDISTANCE ),
    M_ED( "BottomBorderDistance",FO,    PADDING_BOTTOM,           XML_TYPE_MEASURE, CTF_BOTTOMBORDERDISTANCE ),

    // There is an additional property for controls!
    M_ED( "LeftBorder",         FO,     BORDER,                   XML_TYPE_BORDER|MID_FLAG_MULTI_PROPERTY, CTF_ALLBORDER ),
    M_ED( "LeftBorder",         FO,     BORDER_LEFT,              XML_TYPE_BORDER, CTF_LEFTBORDER ),
    M_ED( "RightBorder",            FO,     BORDER_RIGHT,             XML_TYPE_BORDER, CTF_RIGHTBORDER ),
    M_ED( "TopBorder",          FO,     BORDER_TOP,               XML_TYPE_BORDER, CTF_TOPBORDER ),
    M_ED( "BottomBorder",       FO,     BORDER_BOTTOM,            XML_TYPE_BORDER, CTF_BOTTOMBORDER ),
    // RES_SHADOW
    M_E( "ShadowFormat",        STYLE,  SHADOW,                 XML_TYPE_TEXT_SHADOW, 0 ),
    // RES_FRMMACRO
    // TODO
    // RES_COL
    M_E( "TextColumns",         STYLE,  COLUMNS,    MID_FLAG_ELEMENT_ITEM|XML_TYPE_TEXT_COLUMNS, CTF_TEXTCOLUMNS ),
    // RES_KEEP
    // not required
    // RES_URL
    // not required (exprted as draw:a element)
    // RES_EDIT_IN_READONLY
    M_ED( "EditInReadonly",     STYLE,  EDITABLE,                   XML_TYPE_BOOL, 0 ),
    // RES_LAYOUT_SPLIT
    // not required
    // RES_CHAIN
    // not required (exported at text:text-box element)
    // RES_LINENUMBER
    // not required
    // RES_FTN_AT_TXTEND
    // not required
    // RES_END_AT_TXTEND
    // not required
    // RES_COLUMNBALANCE
    // TODO
    // RES_UNKNOWNATR_CONTAINER
//  M_E_SE( TEXT, xmlns, RES_UNKNOWNATR_CONTAINER, 0 ),
    // RES_GRFATR_MIRRORGRF (vertical MUST be processed after horizontal!)
    M_E( "HoriMirroredOnEvenPages",     STYLE,  MIRROR,     XML_TYPE_TEXT_MIRROR_HORIZONTAL_LEFT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "HoriMirroredOnOddPages",      STYLE,  MIRROR,     XML_TYPE_TEXT_MIRROR_HORIZONTAL_RIGHT|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "VertMirrored",        STYLE,  MIRROR,     XML_TYPE_TEXT_MIRROR_VERTICAL|MID_FLAG_MERGE_ATTRIBUTE|MID_FLAG_MULTI_PROPERTY, 0 ),
    // RES_GRFATR_CROPGRF
    M_E( "GraphicCrop",         FO,     CLIP,       XML_TYPE_TEXT_CLIP, 0 ),
    // RES_GRFATR_ROTATION
    // not required (exported as svg:transform attribute)
    // RES_GRFATR_LUMINANCE
    M_E( "AdjustLuminance",  DRAW, LUMINANCE,           XML_TYPE_PERCENT16, 0 ),        // signed?
    // RES_GRFATR_CONTRAST
    M_E( "AdjustContrast",  DRAW,   CONTRAST,           XML_TYPE_PERCENT16, 0 ),        // signed?
    // RES_GRFATR_CHANNELR
    M_E( "AdjustRed",       DRAW, RED,                  XML_TYPE_PERCENT16, 0 ),        // signed?
    // RES_GRFATR_CHANNELG
    M_E( "AdjustGreen",     DRAW, GREEN,                XML_TYPE_PERCENT16, 0 ),        // signed?
    // RES_GRFATR_CHANNELB
    M_E( "AdjustBlue",      DRAW, BLUE,                 XML_TYPE_PERCENT16, 0 ),        // signed?
    // RES_GRFATR_GAMMA
    M_E( "Gamma",           DRAW, GAMMA,                XML_TYPE_DOUBLE, 0 ),           // signed?
    // RES_GRFATR_INVERT
    M_E( "GraphicIsInverted", DRAW, COLOR_INVERSION,        XML_TYPE_BOOL, 0 ),
    // RES_GRFATR_TRANSPARENCY
    M_E( "Transparency",    DRAW, TRANSPARENCY,     XML_TYPE_PERCENT16|MID_FLAG_MULTI_PROPERTY, 0 ),
    // RES_GRFATR_DRAWMODE
    M_E( "GraphicColorMode", DRAW, COLOR_MODE,          XML_TYPE_COLOR_MODE, 0 ),
    M_E( "WritingMode",      STYLE, WRITING_MODE,       XML_TYPE_TEXT_WRITING_MODE_WITH_DEFAULT, 0 ),

    // special entries for floating frames
    M_E( "",            DRAW,   FRAME_DISPLAY_SCROLLBAR,    XML_TYPE_BOOL|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY, CTF_FRAME_DISPLAY_SCROLLBAR ),
    M_E( "",            DRAW,   FRAME_DISPLAY_BORDER,   XML_TYPE_BOOL|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY, CTF_FRAME_DISPLAY_BORDER ),
    M_E( "",            DRAW,   FRAME_MARGIN_HORIZONTAL,    XML_TYPE_MEASURE_PX|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY,   CTF_FRAME_MARGIN_HORI ),
    M_E( "",            DRAW,   FRAME_MARGIN_VERTICAL,  XML_TYPE_MEASURE_PX|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY,   CTF_FRAME_MARGIN_VERT ),
    M_E( "",            DRAW,   VISIBLE_AREA_LEFT,      XML_TYPE_MEASURE|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY,  CTF_OLE_VIS_AREA_LEFT ),
    M_E( "",            DRAW,   VISIBLE_AREA_TOP,       XML_TYPE_MEASURE|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY,  CTF_OLE_VIS_AREA_TOP ),
    M_E( "",            DRAW,   VISIBLE_AREA_WIDTH,     XML_TYPE_MEASURE|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY,  CTF_OLE_VIS_AREA_WIDTH ),
    M_E( "",            DRAW,   VISIBLE_AREA_HEIGHT,    XML_TYPE_MEASURE|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY,  CTF_OLE_VIS_AREA_HEIGHT ),
    M_E( "",            DRAW,   DRAW_ASPECT,            XML_TYPE_TEXT_DRAW_ASPECT|MID_FLAG_NO_PROPERTY|MID_FLAG_MULTI_PROPERTY, CTF_OLE_DRAW_ASPECT ),
    M_E( "UserDefinedAttributes", TEXT, XMLNS, XML_TYPE_ATTRIBUTE_CONTAINER | MID_FLAG_SPECIAL_ITEM, 0 ),

    M_END()
};

XMLPropertyMapEntry aXMLShapePropMap[] =
{
    // RES_LR_SPACE
    M_E( "LeftMargin",              FO, MARGIN_LEFT,        XML_TYPE_MEASURE,  0),
    M_E( "RightMargin",             FO, MARGIN_RIGHT,       XML_TYPE_MEASURE, 0 ),
    // RES_UL_SPACE
    M_E( "TopMargin",               FO, MARGIN_TOP,         XML_TYPE_MEASURE, 0 ),
    M_E( "BottomMargin",            FO, MARGIN_BOTTOM,      XML_TYPE_MEASURE, 0 ),
    // RES_OPAQUE
    M_ED( "Opaque",                 STYLE,  RUN_THROUGH,    XML_TYPE_TEXT_OPAQUE, 0 ),
    // RES_SURROUND
    M_E( "TextWrap",                STYLE,  WRAP,   XML_TYPE_TEXT_WRAP, CTF_WRAP ),
    M_E( "SurroundAnchorOnly",      STYLE,  NUMBER_WRAPPED_PARAGRAPHS,  XML_TYPE_TEXT_PARAGRAPH_ONLY, CTF_WRAP_PARAGRAPH_ONLY ),
    M_E( "SurroundContour",         STYLE,  WRAP_CONTOUR,   XML_TYPE_BOOL, CTF_WRAP_CONTOUR ),
    M_E( "ContourOutside",          STYLE,  WRAP_CONTOUR_MODE,  XML_TYPE_TEXT_WRAP_OUTSIDE, CTF_WRAP_CONTOUR_MODE ),
    // RES_VERT_ORIENT
    M_E( "VertOrient",              STYLE,  VERTICAL_POS,         XML_TYPE_TEXT_VERTICAL_POS, CTF_VERTICALPOS ),
    M_E( "VertOrient",              STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL_AS_CHAR|MID_FLAG_MULTI_PROPERTY, CTF_VERTICALREL_ASCHAR ),
    M_E( "VertOrientRelation",      STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL, CTF_VERTICALREL ),
    M_E( "VertOrientRelation",      STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL_PAGE|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_VERTICALREL_PAGE ),
    M_E( "VertOrientRelation",      STYLE,  VERTICAL_REL,         XML_TYPE_TEXT_VERTICAL_REL_FRAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_VERTICALREL_FRAME ),
    // RES_HORI_ORIENT
    M_E( "HoriOrient",              STYLE,  HORIZONTAL_POS,       XML_TYPE_TEXT_HORIZONTAL_POS|MID_FLAG_MULTI_PROPERTY, CTF_HORIZONTALPOS ),
    M_E( "PageToggle",      STYLE,  HORIZONTAL_POS,       XML_TYPE_TEXT_HORIZONTAL_MIRROR, CTF_HORIZONTALMIRROR ),
    M_E( "HoriOrient",              STYLE,  HORIZONTAL_POS,       XML_TYPE_TEXT_HORIZONTAL_POS_MIRRORED|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_HORIZONTALPOS_MIRRORED ),
    M_E( "HoriOrientRelation",      STYLE,  HORIZONTAL_REL,       XML_TYPE_TEXT_HORIZONTAL_REL, CTF_HORIZONTALREL ),
    M_E( "HoriOrientRelation",      STYLE,  HORIZONTAL_REL,       XML_TYPE_TEXT_HORIZONTAL_REL_FRAME|MID_FLAG_SPECIAL_ITEM_IMPORT, CTF_HORIZONTALREL_FRAME ),
    // UserDefinedAttributes is already contained in the map this one is
    // chained to.

    M_END()
};

XMLPropertyMapEntry aXMLSectionPropMap[] =
{
    // RES_COL
    M_E( "TextColumns",         STYLE,  COLUMNS,    MID_FLAG_ELEMENT_ITEM|XML_TYPE_TEXT_COLUMNS, CTF_TEXTCOLUMNS ),

    // RES_BACKGROUND
    M_E( "BackColor",   FO, BACKGROUND_COLOR,       XML_TYPE_COLORTRANSPARENT|MID_FLAG_MULTI_PROPERTY, 0 ),
    M_E( "BackTransparent", FO, BACKGROUND_COLOR,       XML_TYPE_ISTRANSPARENT|MID_FLAG_MERGE_ATTRIBUTE, 0 ),
    M_E( "BackGraphicLocation", STYLE,  POSITION,   MID_FLAG_SPECIAL_ITEM|XML_TYPE_BUILDIN_CMP_ONLY, CTF_BACKGROUND_POS  ),
    M_E( "BackGraphicFilter",STYLE, FILTER_NAME,    MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING, CTF_BACKGROUND_FILTER ),
    M_E( "BackGraphicURL",  STYLE,  BACKGROUND_IMAGE,   MID_FLAG_ELEMENT_ITEM|XML_TYPE_STRING, CTF_BACKGROUND_URL ),

    // move protect-flag into section element
//  M_E( "IsProtected",         STYLE,  PROTECT,    XML_TYPE_BOOL, 0 ),

    M_E( "DontBalanceTextColumns", TEXT, DONT_BALANCE_TEXT_COLUMNS, XML_TYPE_BOOL, 0 ),

    M_E( "WritingMode",      STYLE, WRITING_MODE,       XML_TYPE_TEXT_WRITING_MODE_WITH_DEFAULT, 0 ),

    M_E( "SectionLeftMargin",      FO, MARGIN_LEFT,     XML_TYPE_MEASURE,  0),
    M_E( "SectionRightMargin",     FO, MARGIN_RIGHT,        XML_TYPE_MEASURE,  0),

    // section footnote settings
    M_E( "FootnoteIsOwnNumbering",      TEXT,   _EMPTY, MID_FLAG_SPECIAL_ITEM|XML_TYPE_BOOL,    CTF_SECTION_FOOTNOTE_NUM_OWN ),
    M_E( "FootnoteIsRestartNumbering",  TEXT,   _EMPTY, MID_FLAG_SPECIAL_ITEM|XML_TYPE_BOOL,    CTF_SECTION_FOOTNOTE_NUM_RESTART ),
    M_E( "FootnoteRestartNumberingAt",  TEXT,   _EMPTY, MID_FLAG_SPECIAL_ITEM|XML_TYPE_NUMBER16,CTF_SECTION_FOOTNOTE_NUM_RESTART_AT ),
    M_E( "FootnoteNumberingType",       TEXT,   _EMPTY, MID_FLAG_SPECIAL_ITEM|XML_TYPE_NUMBER16,CTF_SECTION_FOOTNOTE_NUM_TYPE ),
    M_E( "FootnoteNumberingPrefix",     TEXT,   _EMPTY, MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING,  CTF_SECTION_FOOTNOTE_NUM_PREFIX ),
    M_E( "FootnoteNumberingSuffix",     TEXT,   _EMPTY, MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING,  CTF_SECTION_FOOTNOTE_NUM_SUFFIX ),
    M_E( "FootnoteIsCollectAtTextEnd",  TEXT,   FOOTNOTES_CONFIGURATION,    MID_FLAG_ELEMENT_ITEM|XML_TYPE_BOOL,    CTF_SECTION_FOOTNOTE_END ),

    // section footnote settings
    M_E( "EndnoteIsOwnNumbering",       TEXT,   _EMPTY,     MID_FLAG_SPECIAL_ITEM|XML_TYPE_BOOL,    CTF_SECTION_ENDNOTE_NUM_OWN ),
    M_E( "EndnoteIsRestartNumbering",   TEXT,   _EMPTY,     MID_FLAG_SPECIAL_ITEM|XML_TYPE_BOOL,    CTF_SECTION_ENDNOTE_NUM_RESTART ),
    M_E( "EndnoteRestartNumberingAt",   TEXT,   _EMPTY,     MID_FLAG_SPECIAL_ITEM|XML_TYPE_NUMBER16,CTF_SECTION_ENDNOTE_NUM_RESTART_AT ),
    M_E( "EndnoteNumberingType",        TEXT,   _EMPTY,     MID_FLAG_SPECIAL_ITEM|XML_TYPE_NUMBER16,CTF_SECTION_ENDNOTE_NUM_TYPE ),
    M_E( "EndnoteNumberingPrefix",      TEXT,   _EMPTY,     MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING,  CTF_SECTION_ENDNOTE_NUM_PREFIX ),
    M_E( "EndnoteNumberingSuffix",      TEXT,   _EMPTY,     MID_FLAG_SPECIAL_ITEM|XML_TYPE_STRING,  CTF_SECTION_ENDNOTE_NUM_SUFFIX ),
    M_E( "EndnoteIsCollectAtTextEnd",   TEXT,   ENDNOTES_CONFIGURATION,     MID_FLAG_ELEMENT_ITEM|XML_TYPE_BOOL,    CTF_SECTION_ENDNOTE_END ),
    M_E( "UserDefinedAttributes",       TEXT,   XMLNS,      XML_TYPE_ATTRIBUTE_CONTAINER | MID_FLAG_SPECIAL_ITEM, 0 ),

    M_END()
};

XMLPropertyMapEntry aXMLRubyPropMap[] =
{
    M_E( "RubyAdjust",  STYLE, RUBY_ALIGN, XML_TYPE_TEXT_RUBY_ADJUST, 0 ),
    M_E( "RubyIsAbove", STYLE, RUBY_POSITION, XML_TYPE_TEXT_RUBY_POSITION, 0 ),
    M_END()
};

XMLPropertyMapEntry *lcl_txtprmap_getMap( sal_uInt16 nType )
{
    XMLPropertyMapEntry *pMap = 0;
    switch( nType )
    {
    case TEXT_PROP_MAP_TEXT:
        pMap = aXMLTextPropMap;
        break;
    case TEXT_PROP_MAP_SHAPE_PARA:
        pMap = &(aXMLParaPropMap[1]);
        DBG_ASSERT( pMap->meXMLName == XML_MARGIN_LEFT, "shape para map changed" );
        break;
    case TEXT_PROP_MAP_PARA:
        pMap = aXMLParaPropMap;
        break;
    case TEXT_PROP_MAP_FRAME:
        pMap = aXMLFramePropMap;
        break;
    case TEXT_PROP_MAP_AUTO_FRAME:
        pMap = &(aXMLFramePropMap[10]);
        DBG_ASSERT( pMap->meXMLName == XML_MARGIN_LEFT, "frame map changed" );
        break;
    case TEXT_PROP_MAP_SHAPE:
        pMap = aXMLShapePropMap;
        break;
    case TEXT_PROP_MAP_SECTION:
        pMap = aXMLSectionPropMap;
        break;
    case TEXT_PROP_MAP_RUBY:
        pMap = aXMLRubyPropMap;
        break;
    }
    DBG_ASSERT( pMap, "illegal map type" );
    return pMap;
}

XMLTextPropertySetMapper::XMLTextPropertySetMapper( sal_uInt16 nType ) :
    XMLPropertySetMapper( lcl_txtprmap_getMap( nType ),
                          new XMLTextPropertyHandlerFactory )
{
}

XMLTextPropertySetMapper::~XMLTextPropertySetMapper()
{
}




}//end of namespace binfilter
