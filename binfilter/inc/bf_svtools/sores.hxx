/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sores.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2008-03-12 12:29:05 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

//  Strings
#define STR_INS_OBJECT                          32000
#define STR_INS_OBJECT_ICON                     32001
#define STR_INS_FILE                            32002
#define STR_INS_FILE_ICON                       32003
#define STR_INS_FILE_LINK                       32004
#define STR_INS_FILE_ICON_LINK                  32005
#define STR_PASTE                               32012
#define STR_ERROR_DDE                           32013
#define STR_ERROR_OBJNOCREATE                   32014
#define STR_ERROR_OBJNOCREATE_FROM_FILE         32015
#define STR_VERB_OPEN                           32016
#define STR_PLUGIN_CANT_SHOW                    32017
#define STR_ERROR_OBJNOCREATE_PLUGIN            32018
#define STR_INS_PLUGIN                          32019
#define STR_CONVERT_TO                          32010
#define STR_ACTIVATE_AS                         32021
#define STR_QUERYUPDATELINKS                    32022
#define STR_INS_APPLET                          32023
#define STR_VERB_PROPS                          32025
#define STR_FURTHER_OBJECT                      32026
#define STR_EDIT_APPLET                         32029
#define STR_UNKNOWN_SOURCE                      32027

#define BMP_PLUGIN                      32000
#define BMP_OLEOBJ                      32001
#define MB_PLUGIN                       32000
#define MI_PLUGIN                       32000
#define MI_PLUGIN_DEACTIVATE            1

// Sot Format Strings
#define STR_FORMAT_START                        32100
#define STR_FORMAT_STRING                       (STR_FORMAT_START + 1)
#define STR_FORMAT_BITMAP                       (STR_FORMAT_START + 2)
#define STR_FORMAT_GDIMETAFILE                  (STR_FORMAT_START + 3)
    //  #define STR_FORMAT_PRIVATE                      (STR_FORMAT_START + 4)
    //  #define STR_FORMAT_FILE                         (STR_FORMAT_START + 5)
    //  #define STR_FORMAT_FILE_LIST                    (STR_FORMAT_START + 6)
#define STR_FORMAT_RTF                          (STR_FORMAT_START + 7)
#define STR_FORMAT_ID_DRAWING                   (STR_FORMAT_START + 8)
#define STR_FORMAT_ID_SVXB                      (STR_FORMAT_START + 9)
    //  #define STR_FORMAT_ID_SVIM                  (STR_FORMAT_START + 10)
    //  #define STR_FORMAT_ID_XFA                   (STR_FORMAT_START + 11)
    //  #define STR_FORMAT_ID_EDITENGINE                (STR_FORMAT_START + 12)
#define STR_FORMAT_ID_INTERNALLINK_STATE        (STR_FORMAT_START + 13)
#define STR_FORMAT_ID_SOLK                      (STR_FORMAT_START + 14)
#define STR_FORMAT_ID_NETSCAPE_BOOKMARK         (STR_FORMAT_START + 15)
    //  #define STR_FORMAT_ID_TREELISTBOX           (STR_FORMAT_START + 16)
    //  #define STR_FORMAT_ID_NATIVE                    (STR_FORMAT_START + 17)
    //  #define STR_FORMAT_ID_OWNERLINK             (STR_FORMAT_START + 18)
#define STR_FORMAT_ID_STARSERVER                (STR_FORMAT_START + 19)
#define STR_FORMAT_ID_STAROBJECT                (STR_FORMAT_START + 20)
#define STR_FORMAT_ID_APPLETOBJECT              (STR_FORMAT_START + 21)
#define STR_FORMAT_ID_PLUGIN_OBJECT             (STR_FORMAT_START + 22)
#define STR_FORMAT_ID_STARWRITER_30             (STR_FORMAT_START + 23)
#define STR_FORMAT_ID_STARWRITER_40             (STR_FORMAT_START + 24)
#define STR_FORMAT_ID_STARWRITER_50             (STR_FORMAT_START + 25)
#define STR_FORMAT_ID_STARWRITERWEB_40          (STR_FORMAT_START + 26)
#define STR_FORMAT_ID_STARWRITERWEB_50          (STR_FORMAT_START + 27)
#define STR_FORMAT_ID_STARWRITERGLOB_40         (STR_FORMAT_START + 28)
#define STR_FORMAT_ID_STARWRITERGLOB_50         (STR_FORMAT_START + 29)
#define STR_FORMAT_ID_STARDRAW                  (STR_FORMAT_START + 30)
#define STR_FORMAT_ID_STARDRAW_40               (STR_FORMAT_START + 31)
#define STR_FORMAT_ID_STARIMPRESS_50            (STR_FORMAT_START + 32)
#define STR_FORMAT_ID_STARDRAW_50               (STR_FORMAT_START + 33)
#define STR_FORMAT_ID_STARCALC                  (STR_FORMAT_START + 34)
#define STR_FORMAT_ID_STARCALC_40               (STR_FORMAT_START + 35)
#define STR_FORMAT_ID_STARCALC_50               (STR_FORMAT_START + 36)
#define STR_FORMAT_ID_STARCHART                 (STR_FORMAT_START + 37)
#define STR_FORMAT_ID_STARCHART_40              (STR_FORMAT_START + 38)
#define STR_FORMAT_ID_STARCHART_50              (STR_FORMAT_START + 39)
#define STR_FORMAT_ID_STARIMAGE                 (STR_FORMAT_START + 40)
#define STR_FORMAT_ID_STARIMAGE_40              (STR_FORMAT_START + 41)
#define STR_FORMAT_ID_STARIMAGE_50              (STR_FORMAT_START + 42)
#define STR_FORMAT_ID_STARMATH                  (STR_FORMAT_START + 43)
#define STR_FORMAT_ID_STARMATH_40               (STR_FORMAT_START + 44)
#define STR_FORMAT_ID_STARMATH_50               (STR_FORMAT_START + 45)
#define STR_FORMAT_ID_STAROBJECT_PAINTDOC       (STR_FORMAT_START + 46)
    //  #define STR_FORMAT_ID_FILLED_AREA           (STR_FORMAT_START + 47)
#define STR_FORMAT_ID_HTML                      (STR_FORMAT_START + 48)
#define STR_FORMAT_ID_HTML_SIMPLE               (STR_FORMAT_START + 49)
    //  #define STR_FORMAT_ID_CHAOS                 (STR_FORMAT_START + 50)
    //  #define STR_FORMAT_ID_CNT_MSGATTACHFILE     (STR_FORMAT_START + 51)
#define STR_FORMAT_ID_BIFF_5                    (STR_FORMAT_START + 52)
#define STR_FORMAT_ID_BIFF_8                    (STR_FORMAT_START + 53)
#define STR_FORMAT_ID_SYLK                      (STR_FORMAT_START + 54)
    //  #define STR_FORMAT_ID_SYLK_BIGCAPS          (STR_FORMAT_START + 55)
#define STR_FORMAT_ID_LINK                      (STR_FORMAT_START + 56)
#define STR_FORMAT_ID_DIF                       (STR_FORMAT_START + 57)
    //  #define STR_FORMAT_ID_STARDRAW_TABBAR       (STR_FORMAT_START + 58)
    //  #define STR_FORMAT_ID_SONLK                 (STR_FORMAT_START + 59)
#define STR_FORMAT_ID_MSWORD_DOC                (STR_FORMAT_START + 60)
#define STR_FORMAT_ID_STAR_FRAMESET_DOC         (STR_FORMAT_START + 61)
#define STR_FORMAT_ID_OFFICE_DOC                (STR_FORMAT_START + 62)
#define STR_FORMAT_ID_NOTES_DOCINFO             (STR_FORMAT_START + 63)
    //  #define STR_FORMAT_ID_NOTES_HNOTE           (STR_FORMAT_START + 64)
    //  #define STR_FORMAT_ID_NOTES_NATIVE          (STR_FORMAT_START + 65)
#define STR_FORMAT_ID_SFX_DOC                   (STR_FORMAT_START + 66)
    //  #define STR_FORMAT_ID_EVDF                  (STR_FORMAT_START + 67)
    //  #define STR_FORMAT_ID_ESDF                  (STR_FORMAT_START + 68)
    //  #define STR_FORMAT_ID_IDF                   (STR_FORMAT_START + 69)
    //  #define STR_FORMAT_ID_EFTP                  (STR_FORMAT_START + 70)
    //  #define STR_FORMAT_ID_EFD                   (STR_FORMAT_START + 71)
    //  #define STR_FORMAT_ID_SVX_FORMFIELDEXCH     (STR_FORMAT_START + 72)
    //  #define STR_FORMAT_ID_EXTENDED_TABBAR       (STR_FORMAT_START + 73)
    //  #define STR_FORMAT_ID_SBA_DATAEXCHANGE      (STR_FORMAT_START + 74)
    //  #define STR_FORMAT_ID_SBA_FIELDDATAEXCHANGE (STR_FORMAT_START + 75)
    //  #define STR_FORMAT_ID_SBA_PRIVATE_URL       (STR_FORMAT_START + 76)
    //  #define STR_FORMAT_ID_SBA_TABED             (STR_FORMAT_START + 77)
    //  #define STR_FORMAT_ID_SBA_TABID             (STR_FORMAT_START + 78)
    //  #define STR_FORMAT_ID_SBA_JOIN              (STR_FORMAT_START + 79)
    //  #define STR_FORMAT_ID_OBJECTDESCRIPTOR      (STR_FORMAT_START + 80)
    //  #define STR_FORMAT_ID_LINKSRCDESCRIPTOR     (STR_FORMAT_START + 81)
    //  #define STR_FORMAT_ID_EMBED_SOURCE          (STR_FORMAT_START + 82)
    //  #define STR_FORMAT_ID_LINK_SOURCE           (STR_FORMAT_START + 83)
    //  #define STR_FORMAT_ID_EMBEDDED_OBJ          (STR_FORMAT_START + 84)
    //  #define STR_FORMAT_ID_FILECONTENT           (STR_FORMAT_START + 85)
#define STR_FORMAT_ID_FILEGRPDESCRIPTOR     (STR_FORMAT_START + 86)
    //  #define STR_FORMAT_ID_FILENAME              (STR_FORMAT_START + 87)
    //  #define STR_FORMAT_ID_SD_OLE                (STR_FORMAT_START + 88)
    //  #define STR_FORMAT_ID_EMBEDDED_OBJ_OLE      (STR_FORMAT_START + 89)
    //  #define STR_FORMAT_ID_EMBED_SOURCE_OLE      (STR_FORMAT_START + 90)
    //  #define STR_FORMAT_ID_OBJECTDESCRIPTOR_OLE  (STR_FORMAT_START + 91)
    //  #define STR_FORMAT_ID_LINKSRCDESCRIPTOR_OLE (STR_FORMAT_START + 92)
    //  #define STR_FORMAT_ID_LINK_SOURCE_OLE       (STR_FORMAT_START + 93)
    //  #define STR_FORMAT_ID_SBA_CTRLDATAEXCHANGE  (STR_FORMAT_START + 94)
    //  #define STR_FORMAT_ID_OUTPLACE_OBJ          (STR_FORMAT_START + 95)
    //  #define STR_FORMAT_ID_CNT_OWN_CLIP          (STR_FORMAT_START + 96)
    //  #define STR_FORMAT_ID_INET_IMAGE            (STR_FORMAT_START + 97)
    //  #define STR_FORMAT_ID_NETSCAPE_IMAGE        (STR_FORMAT_START + 98)
    //  #define STR_FORMAT_ID_SBA_FORMEXCHANGE      (STR_FORMAT_START + 99)
    //  #define STR_FORMAT_ID_SBA_REPORTEXCHANGE    (STR_FORMAT_START + 100)
    //  #define STR_FORMAT_ID_UNIFORMRESOURCELOCATOR (STR_FORMAT_START + 101)
#define STR_FORMAT_ID_STARCHARTDOCUMENT_50      (STR_FORMAT_START + 102)
#define STR_FORMAT_ID_GRAPHOBJ                  (STR_FORMAT_START + 103)
#define STR_FORMAT_ID_STARWRITER_60             (STR_FORMAT_START + 104)
#define STR_FORMAT_ID_STARWRITERWEB_60          (STR_FORMAT_START + 105)
#define STR_FORMAT_ID_STARWRITERGLOB_60         (STR_FORMAT_START + 106)
#define STR_FORMAT_ID_STARDRAW_60               (STR_FORMAT_START + 107)
#define STR_FORMAT_ID_STARIMPRESS_60            (STR_FORMAT_START + 108)
#define STR_FORMAT_ID_STARCALC_60               (STR_FORMAT_START + 109)
#define STR_FORMAT_ID_STARCHART_60              (STR_FORMAT_START + 110)
#define STR_FORMAT_ID_STARMATH_60               (STR_FORMAT_START + 111)
#define STR_FORMAT_ID_WMF                       (STR_FORMAT_START + 112)
#define STR_FORMAT_ID_DBACCESS_QUERY            (STR_FORMAT_START + 113)
#define STR_FORMAT_ID_DBACCESS_TABLE            (STR_FORMAT_START + 114)
#define STR_FORMAT_ID_DBACCESS_COMMAND          (STR_FORMAT_START + 115)
#define STR_FORMAT_ID_DIALOG_60                 (STR_FORMAT_START + 116)
    // #define STR_FORMAT_ID_EMF                    (STR_FORMAT_START + 117)
    // #define STR_FORMAT_ID_BIFF_8                 (STR_FORMAT_START + 118)
#define STR_FORMAT_ID_HTML_NO_COMMENT           (STR_FORMAT_START + 119)
#define STR_FORMAT_END                          (STR_FORMAT_ID_HTML_NO_COMMENT)
