/*************************************************************************
 *
 *  $RCSfile: txtprmap.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jmeng $ $Date: 2003-12-04 01:45:44 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef _XMLOFF_TEXTPRMAP_HXX_
#define _XMLOFF_TEXTPRMAP_HXX_

#ifndef _XMLOFF_CONTEXTID_HXX_
#include <bf_xmloff/contextid.hxx>
#endif
#ifndef _XMLOFF_PROPERTYSETMAPPER_HXX
#include <bf_xmloff/xmlprmap.hxx>
#endif
namespace binfilter {

#define CTF_CHARHEIGHT               (XML_TEXT_CTF_START + 1)
#define CTF_CHARHEIGHT_REL           (XML_TEXT_CTF_START + 2)
#define CTF_PARALEFTMARGIN           (XML_TEXT_CTF_START + 3)
#define CTF_PARALEFTMARGIN_REL       (XML_TEXT_CTF_START + 4)
#define CTF_PARARIGHTMARGIN          (XML_TEXT_CTF_START + 5)
#define CTF_PARARIGHTMARGIN_REL      (XML_TEXT_CTF_START + 6)
#define CTF_PARAFIRSTLINE            (XML_TEXT_CTF_START + 7)
#define CTF_PARAFIRSTLINE_REL        (XML_TEXT_CTF_START + 8)
#define CTF_PARATOPMARGIN            (XML_TEXT_CTF_START + 9)
#define CTF_PARATOPMARGIN_REL       (XML_TEXT_CTF_START + 10)
#define CTF_PARABOTTOMMARGIN        (XML_TEXT_CTF_START + 11)
#define CTF_PARABOTTOMMARGIN_REL    (XML_TEXT_CTF_START + 12)
#define CTF_ALLBORDERWIDTH          (XML_TEXT_CTF_START + 13)
#define CTF_LEFTBORDERWIDTH         (XML_TEXT_CTF_START + 14)
#define CTF_RIGHTBORDERWIDTH        (XML_TEXT_CTF_START + 15)
#define CTF_TOPBORDERWIDTH          (XML_TEXT_CTF_START + 16)
#define CTF_BOTTOMBORDERWIDTH       (XML_TEXT_CTF_START + 17)
#define CTF_ALLBORDERDISTANCE       (XML_TEXT_CTF_START + 18)
#define CTF_LEFTBORDERDISTANCE      (XML_TEXT_CTF_START + 19)
#define CTF_RIGHTBORDERDISTANCE     (XML_TEXT_CTF_START + 20)
#define CTF_TOPBORDERDISTANCE       (XML_TEXT_CTF_START + 21)
#define CTF_BOTTOMBORDERDISTANCE    (XML_TEXT_CTF_START + 22)
#define CTF_ALLBORDER               (XML_TEXT_CTF_START + 23)
#define CTF_LEFTBORDER              (XML_TEXT_CTF_START + 24)
#define CTF_RIGHTBORDER             (XML_TEXT_CTF_START + 25)
#define CTF_TOPBORDER               (XML_TEXT_CTF_START + 26)
#define CTF_BOTTOMBORDER            (XML_TEXT_CTF_START + 27)
#define CTF_DROPCAPWHOLEWORD        (XML_TEXT_CTF_START + 28)
#define CTF_DROPCAPFORMAT           (XML_TEXT_CTF_START + 29)
#define CTF_DROPCAPCHARSTYLE        (XML_TEXT_CTF_START + 30)
#define CTF_TABSTOP                 (XML_TEXT_CTF_START + 31)
#define CTF_NUMBERINGSTYLENAME      (XML_TEXT_CTF_START + 32)
#define CTF_OLDTEXTBACKGROUND       (XML_TEXT_CTF_START + 33)
#define CTF_FONTFAMILYNAME          (XML_TEXT_CTF_START + 34)
#define CTF_FONTSTYLENAME           (XML_TEXT_CTF_START + 35)
#define CTF_FONTFAMILY              (XML_TEXT_CTF_START + 36)
#define CTF_FONTPITCH               (XML_TEXT_CTF_START + 37)
#define CTF_FONTCHARSET             (XML_TEXT_CTF_START + 38)
//  (XML_TEXT_CTF_START + 39)
#define CTF_SYNCHEIGHT_MIN          (XML_TEXT_CTF_START + 40)
#define CTF_FRAMEHEIGHT_ABS         (XML_TEXT_CTF_START + 41)
#define CTF_FRAMEHEIGHT_REL         (XML_TEXT_CTF_START + 42)
#define CTF_FRAMEHEIGHT_MIN_ABS     (XML_TEXT_CTF_START + 43)
#define CTF_FRAMEHEIGHT_MIN_REL     (XML_TEXT_CTF_START + 44)
#define CTF_SIZETYPE                (XML_TEXT_CTF_START + 45)
#define CTF_SYNCHEIGHT              (XML_TEXT_CTF_START + 46)
#define CTF_WRAP                    (XML_TEXT_CTF_START + 47)
#define CTF_WRAP_CONTOUR            (XML_TEXT_CTF_START + 48)
#define CTF_WRAP_CONTOUR_MODE       (XML_TEXT_CTF_START + 49)
#define CTF_WRAP_PARAGRAPH_ONLY     (XML_TEXT_CTF_START + 50)
#define CTF_ANCHORTYPE              (XML_TEXT_CTF_START + 51)
#define CTF_ANCHORPAGENUMBER        (XML_TEXT_CTF_START + 52)
#define CTF_TEXTCOLUMNS             (XML_TEXT_CTF_START + 53)
#define CTF_HORIZONTALPOS           (XML_TEXT_CTF_START + 54)
#define CTF_HORIZONTALPOS_MIRRORED  (XML_TEXT_CTF_START + 55)
#define CTF_HORIZONTALREL           (XML_TEXT_CTF_START + 56)
#define CTF_HORIZONTALREL_FRAME     (XML_TEXT_CTF_START + 57)
#define CTF_HORIZONTALMIRROR        (XML_TEXT_CTF_START + 58)
#define CTF_VERTICALPOS             (XML_TEXT_CTF_START + 59)
#define CTF_VERTICALREL             (XML_TEXT_CTF_START + 60)
#define CTF_VERTICALREL_PAGE        (XML_TEXT_CTF_START + 61)
#define CTF_VERTICALREL_FRAME       (XML_TEXT_CTF_START + 62)
#define CTF_VERTICALREL_ASCHAR      (XML_TEXT_CTF_START + 63)
#define CTF_PAGEDESCNAME            (XML_TEXT_CTF_START + 64)
#define CTF_BACKGROUND_URL          (XML_TEXT_CTF_START + 66)
#define CTF_BACKGROUND_POS          (XML_TEXT_CTF_START + 67)
#define CTF_BACKGROUND_FILTER       (XML_TEXT_CTF_START + 68)
#define CTF_CHARHEIGHT_DIFF         (XML_TEXT_CTF_START + 69)
#define CTF_FONTNAME                (XML_TEXT_CTF_START + 70)
#define CTF_FONTNAME_CJK            (XML_TEXT_CTF_START + 71)
#define CTF_FONTFAMILYNAME_CJK      (XML_TEXT_CTF_START + 72)
#define CTF_FONTSTYLENAME_CJK       (XML_TEXT_CTF_START + 73)
#define CTF_FONTFAMILY_CJK          (XML_TEXT_CTF_START + 74)
#define CTF_FONTPITCH_CJK           (XML_TEXT_CTF_START + 75)
#define CTF_FONTCHARSET_CJK         (XML_TEXT_CTF_START + 76)
#define CTF_CHARHEIGHT_CJK          (XML_TEXT_CTF_START + 77)
#define CTF_CHARHEIGHT_REL_CJK      (XML_TEXT_CTF_START + 78)
#define CTF_CHARHEIGHT_DIFF_CJK     (XML_TEXT_CTF_START + 79)
#define CTF_FONTNAME_CTL            (XML_TEXT_CTF_START + 80)
#define CTF_FONTFAMILYNAME_CTL      (XML_TEXT_CTF_START + 81)
#define CTF_FONTSTYLENAME_CTL       (XML_TEXT_CTF_START + 82)
#define CTF_FONTFAMILY_CTL          (XML_TEXT_CTF_START + 83)
#define CTF_FONTPITCH_CTL           (XML_TEXT_CTF_START + 84)
#define CTF_FONTCHARSET_CTL         (XML_TEXT_CTF_START + 85)
#define CTF_CHARHEIGHT_CTL          (XML_TEXT_CTF_START + 86)
#define CTF_CHARHEIGHT_REL_CTL      (XML_TEXT_CTF_START + 87)
#define CTF_CHARHEIGHT_DIFF_CTL     (XML_TEXT_CTF_START + 88)
#define CTF_UNDERLINE               (XML_TEXT_CTF_START + 89)
#define CTF_UNDERLINE_COLOR         (XML_TEXT_CTF_START + 90)
#define CTF_UNDERLINE_HASCOLOR      (XML_TEXT_CTF_START + 91)
#define CTF_SECTION_FOOTNOTE_END            (XML_TEXT_CTF_START +  92)
#define CTF_SECTION_FOOTNOTE_NUM_RESTART    (XML_TEXT_CTF_START +  93)
#define CTF_SECTION_FOOTNOTE_NUM_RESTART_AT (XML_TEXT_CTF_START +  94)
#define CTF_SECTION_FOOTNOTE_NUM_TYPE       (XML_TEXT_CTF_START +  95)
#define CTF_SECTION_FOOTNOTE_NUM_PREFIX     (XML_TEXT_CTF_START +  96)
#define CTF_SECTION_FOOTNOTE_NUM_SUFFIX     (XML_TEXT_CTF_START +  97)
#define CTF_SECTION_FOOTNOTE_NUM_OWN        (XML_TEXT_CTF_START +  98)
#define CTF_SECTION_ENDNOTE_END             (XML_TEXT_CTF_START +  99)
#define CTF_SECTION_ENDNOTE_NUM_RESTART     (XML_TEXT_CTF_START + 100)
#define CTF_SECTION_ENDNOTE_NUM_RESTART_AT  (XML_TEXT_CTF_START + 101)
#define CTF_SECTION_ENDNOTE_NUM_TYPE        (XML_TEXT_CTF_START + 102)
#define CTF_SECTION_ENDNOTE_NUM_PREFIX      (XML_TEXT_CTF_START + 103)
#define CTF_SECTION_ENDNOTE_NUM_SUFFIX      (XML_TEXT_CTF_START + 104)
#define CTF_SECTION_ENDNOTE_NUM_OWN         (XML_TEXT_CTF_START + 105)
#define CTF_PAGE_FOOTNOTE                   (XML_TEXT_CTF_START + 106)
#define CTF_FRAME_DISPLAY_SCROLLBAR         (XML_TEXT_CTF_START + 107)
#define CTF_FRAME_DISPLAY_BORDER            (XML_TEXT_CTF_START + 108)
#define CTF_FRAME_MARGIN_HORI               (XML_TEXT_CTF_START + 109)
#define CTF_FRAME_MARGIN_VERT               (XML_TEXT_CTF_START + 110)
#define CTF_COMBINED_CHARACTERS_FIELD       (XML_TEXT_CTF_START + 111)
#define CTF_OLE_VIS_AREA_TOP                (XML_TEXT_CTF_START + 112)
#define CTF_OLE_VIS_AREA_LEFT               (XML_TEXT_CTF_START + 113)
#define CTF_OLE_VIS_AREA_WIDTH              (XML_TEXT_CTF_START + 114)
#define CTF_OLE_VIS_AREA_HEIGHT             (XML_TEXT_CTF_START + 115)
#define CTF_OLE_DRAW_ASPECT                 (XML_TEXT_CTF_START + 116)
#define CTF_HYPERLINK_URL                   (XML_TEXT_CTF_START + 117)
#define CTF_CHAR_STYLE_NAME                 (XML_TEXT_CTF_START + 118)
#define CTF_VERTICALPOS_ATCHAR              (XML_TEXT_CTF_START + 119)
#define CTF_BACKGROUND_TRANSPARENCY         (XML_TEXT_CTF_START + 120)
#define CTF_BACKGROUND_TRANSPARENT          (XML_TEXT_CTF_START + 121)

#define TEXT_PROP_MAP_TEXT 0
#define TEXT_PROP_MAP_PARA 1
#define TEXT_PROP_MAP_FRAME 2
#define TEXT_PROP_MAP_AUTO_FRAME 3
#define TEXT_PROP_MAP_SECTION 4
#define TEXT_PROP_MAP_SHAPE 5
#define TEXT_PROP_MAP_RUBY 6
#define TEXT_PROP_MAP_SHAPE_PARA 7

class XMLTextPropertySetMapper : public XMLPropertySetMapper
{

public:

    XMLTextPropertySetMapper( sal_uInt16 nType );
    virtual ~XMLTextPropertySetMapper();
};

}//end of namespace binfilter
#endif
