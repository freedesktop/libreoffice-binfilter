/*************************************************************************
 *
 *  $RCSfile: XMLFootnoteConfigurationImportContext.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jmeng $ $Date: 2003-12-04 01:45:28 $
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

#ifndef _XMLOFF_XMLFOOTNOTECONFIGURATIONIMPORTCONTEXT_HXX
#define _XMLOFF_XMLFOOTNOTECONFIGURATIONIMPORTCONTEXT_HXX

#ifndef _XMLOFF_XMLSTYLE_HXX
#include "xmlstyle.hxx"
#endif

namespace com { namespace sun { namespace star {
    namespace uno { template<class X> class Reference; }
    namespace xml { namespace sax { class XAttributeList; } }
    namespace beans { class XPropertySet; }
} } }
namespace rtl { class OUString; }
namespace binfilter {
class SvXMLImport;

/// import footnote and endnote configuration elements
class XMLFootnoteConfigurationImportContext : public SvXMLStyleContext
{
    const ::rtl::OUString sPropertyAnchorCharStyleName;
    const ::rtl::OUString sPropertyCharStyleName;
    const ::rtl::OUString sPropertyNumberingType;
    const ::rtl::OUString sPropertyPageStyleName;
    const ::rtl::OUString sPropertyParagraphStyleName;
    const ::rtl::OUString sPropertyPrefix;
    const ::rtl::OUString sPropertyStartAt;
    const ::rtl::OUString sPropertySuffix;
    const ::rtl::OUString sPropertyPositionEndOfDoc;
    const ::rtl::OUString sPropertyFootnoteCounting;
    const ::rtl::OUString sPropertyEndNotice;
    const ::rtl::OUString sPropertyBeginNotice;

    ::rtl::OUString sCitationStyle;
    ::rtl::OUString sAnchorStyle;
    ::rtl::OUString sDefaultStyle;
    ::rtl::OUString sPageStyle;
    ::rtl::OUString sPrefix;
    ::rtl::OUString sSuffix;
    ::rtl::OUString sNumFormat;
    ::rtl::OUString sNumSync;
    ::rtl::OUString sBeginNotice;
    ::rtl::OUString sEndNotice;

    SvXMLTokenMap* pAttrTokenMap;

    sal_Int16 nOffset;
    sal_Int16 nNumbering;
    sal_Bool bPosition;
    sal_Bool bIsEndnote;

public:

    TYPEINFO();

    XMLFootnoteConfigurationImportContext(
        SvXMLImport& rImport,
        sal_uInt16 nPrfx,
        const ::rtl::OUString& rLName,
        const ::com::sun::star::uno::Reference<
                ::com::sun::star::xml::sax::XAttributeList> & xAttrList,
        sal_Bool bEndnote);

    virtual ~XMLFootnoteConfigurationImportContext();

    /// parse attributes
    virtual void StartElement(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList> & xAttrList );

    /// for footnotes, also parse begin and end notices
    virtual SvXMLImportContext *CreateChildContext(
        USHORT nPrefix,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList> & xAttrList );

    /// get token map for attributes
    const SvXMLTokenMap& GetFtnConfigAttrTokenMap();

    /// set configuration at document; calls ProcessSettings
    /// Uses CreateAndInsertLate() to ensure that all styles it references
    /// have been set.
    virtual void XMLFootnoteConfigurationImportContext::CreateAndInsertLate(
        sal_Bool bOverwrite );

    /// set configuration at document
    void ProcessSettings(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySet> & rConfig);

    /// for helper class: set begin notice
    void SetBeginNotice( ::rtl::OUString sText);

    /// for helper class: set end notice
    void SetEndNotice( ::rtl::OUString sText);
};

}//end of namespace binfilter
#endif
