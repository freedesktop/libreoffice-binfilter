/*************************************************************************
 *
 *  $RCSfile: xmloff_XMLIndexUserSourceContext.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-08-03 20:27:50 $
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


#ifndef _XMLOFF_XMLINDEXUSERSOURCECONTEXT_HXX_
#include "XMLIndexUserSourceContext.hxx"
#endif

// auto strip #ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
// auto strip #include <com/sun/star/beans/XPropertySet.hpp>
// auto strip #endif

// auto strip #ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
// auto strip #include <com/sun/star/container/XIndexReplace.hpp>
// auto strip #endif

#ifndef _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_
#include "XMLIndexTemplateContext.hxx"
#endif

// auto strip #ifndef _XMLOFF_XMLINDEXTITLETEMPLATECONTEXT_HXX_
// auto strip #include "XMLIndexTitleTemplateContext.hxx"
// auto strip #endif

// auto strip #ifndef _XMLOFF_XMLINDEXTOCSTYLESCONTEXT_HXX_
// auto strip #include "XMLIndexTOCStylesContext.hxx"
// auto strip #endif

// auto strip #ifndef _XMLOFF_XMLICTXT_HXX
// auto strip #include "xmlictxt.hxx"
// auto strip #endif

// auto strip #ifndef _XMLOFF_XMLIMP_HXX
// auto strip #include "xmlimp.hxx"
// auto strip #endif

// auto strip #ifndef _XMLOFF_TEXTIMP_HXX_
// auto strip #include "txtimp.hxx"
// auto strip #endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

// auto strip #ifndef _XMLOFF_NMSPMAP_HXX
// auto strip #include "nmspmap.hxx"
// auto strip #endif

// auto strip #ifndef _XMLOFF_XMLTOKEN_HXX
// auto strip #include "xmltoken.hxx"
// auto strip #endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

// auto strip #ifndef _TOOLS_DEBUG_HXX
// auto strip #include <tools/debug.hxx>
// auto strip #endif

// auto strip #ifndef _RTL_USTRING_HXX_
// auto strip #include <rtl/ustring.hxx>
// auto strip #endif
namespace binfilter {


using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_USER_INDEX_ENTRY_TEMPLATE;
using ::binfilter::xmloff::token::XML_OUTLINE_LEVEL;


const sal_Char sAPI_CreateFromEmbeddedObjects[] = "CreateFromEmbeddedObjects";
const sal_Char sAPI_CreateFromGraphicObjects[] = "CreateFromGraphicObjects";
const sal_Char sAPI_CreateFromMarks[] = "CreateFromMarks";
const sal_Char sAPI_CreateFromTables[] = "CreateFromTables";
const sal_Char sAPI_CreateFromTextFrames[] = "CreateFromTextFrames";
const sal_Char sAPI_UseLevelFromSource[] = "UseLevelFromSource";
const sal_Char sAPI_CreateFromLevelParagraphStyles[] = "CreateFromLevelParagraphStyles";
const sal_Char sAPI_UserIndexName[] = "UserIndexName";


TYPEINIT1(XMLIndexUserSourceContext, XMLIndexSourceBaseContext);


XMLIndexUserSourceContext::XMLIndexUserSourceContext(
    SvXMLImport& rImport,
    sal_uInt16 nPrfx,
    const OUString& rLocalName,
    Reference<XPropertySet> & rPropSet) :
        XMLIndexSourceBaseContext(rImport, nPrfx, rLocalName,
                                  rPropSet, sal_True),
        sCreateFromEmbeddedObjects(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromEmbeddedObjects)),
        sCreateFromGraphicObjects(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromGraphicObjects)),
        sCreateFromMarks(RTL_CONSTASCII_USTRINGPARAM(sAPI_CreateFromMarks)),
        sCreateFromTables(RTL_CONSTASCII_USTRINGPARAM(sAPI_CreateFromTables)),
        sCreateFromTextFrames(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromTextFrames)),
        sUseLevelFromSource(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_UseLevelFromSource)),
        sCreateFromLevelParagraphStyles(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromLevelParagraphStyles)),
        sUserIndexName(RTL_CONSTASCII_USTRINGPARAM(sAPI_UserIndexName)),
        bUseObjects(sal_False),
        bUseGraphic(sal_False),
        bUseMarks(sal_False),
        bUseTables(sal_False),
        bUseFrames(sal_False),
        bUseLevelFromSource(sal_False),
        bUseLevelParagraphStyles(sal_False)
{
}

XMLIndexUserSourceContext::~XMLIndexUserSourceContext()
{
}

void XMLIndexUserSourceContext::ProcessAttribute(
    enum IndexSourceParamEnum eParam,
    const OUString& rValue)
{
    sal_Bool bTmp;

    switch (eParam)
    {
        case XML_TOK_INDEXSOURCE_USE_INDEX_MARKS:
            if (SvXMLUnitConverter::convertBool(bTmp, rValue))
            {
                bUseMarks = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_OBJECTS:
            if (SvXMLUnitConverter::convertBool(bTmp, rValue))
            {
                bUseObjects = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_GRAPHICS:
            if (SvXMLUnitConverter::convertBool(bTmp, rValue))
            {
                bUseGraphic = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_TABLES:
            if (SvXMLUnitConverter::convertBool(bTmp, rValue))
            {
                bUseTables = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_FRAMES:
            if (SvXMLUnitConverter::convertBool(bTmp, rValue))
            {
                bUseFrames = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_COPY_OUTLINE_LEVELS:
            if (SvXMLUnitConverter::convertBool(bTmp, rValue))
            {
                bUseLevelFromSource = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_INDEX_SOURCE_STYLES:
            if (SvXMLUnitConverter::convertBool(bTmp, rValue))
            {
                bUseLevelParagraphStyles = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USER_INDEX_NAME:
            sIndexName = rValue;
            break;

        default:
            XMLIndexSourceBaseContext::ProcessAttribute(eParam, rValue);
            break;
    }
}


void XMLIndexUserSourceContext::EndElement()
{
    Any aAny;

    aAny.setValue(&bUseObjects, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromEmbeddedObjects, aAny);

    aAny.setValue(&bUseGraphic, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromGraphicObjects, aAny);

    aAny.setValue(&bUseLevelFromSource, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUseLevelFromSource, aAny);

    aAny.setValue(&bUseMarks, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromMarks, aAny);

    aAny.setValue(&bUseTables, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromTables, aAny);

    aAny.setValue(&bUseFrames, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromTextFrames, aAny);

    aAny.setValue(&bUseLevelParagraphStyles, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromLevelParagraphStyles, aAny);

    if( sIndexName.getLength() > 0 )
    {
        aAny <<= sIndexName;
        rIndexPropertySet->setPropertyValue(sUserIndexName, aAny);
    }

    XMLIndexSourceBaseContext::EndElement();
}


SvXMLImportContext* XMLIndexUserSourceContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference<XAttributeList> & xAttrList )
{
    if ( (XML_NAMESPACE_TEXT == nPrefix) &&
         (IsXMLToken(rLocalName, XML_USER_INDEX_ENTRY_TEMPLATE)) )
    {
        return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet,
                                           nPrefix, rLocalName,
                                           aLevelNameTOCMap,
                                           XML_OUTLINE_LEVEL,
                                           aLevelStylePropNameTOCMap,
                                           aAllowedTokenTypesUser);
    }
    else
    {
        return XMLIndexSourceBaseContext::CreateChildContext(nPrefix,
                                                             rLocalName,
                                                             xAttrList);
    }

}
}//end of namespace binfilter
