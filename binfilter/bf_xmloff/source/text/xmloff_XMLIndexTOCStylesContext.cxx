/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include "XMLIndexTOCStylesContext.hxx"


#include <com/sun/star/container/XIndexReplace.hpp>


#include "xmlimp.hxx"


#include "xmlnmspe.hxx"

#include "nmspmap.hxx"


#include "xmluconv.hxx"



namespace binfilter {


using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::container::XIndexReplace;
using ::com::sun::star::xml::sax::XAttributeList;


const sal_Char sAPI_LevelParagraphStyles[] = "LevelParagraphStyles";

TYPEINIT1( XMLIndexTOCStylesContext, SvXMLImportContext );


XMLIndexTOCStylesContext::XMLIndexTOCStylesContext(
    SvXMLImport& rInImport,
    Reference<XPropertySet> & rPropSet,
    sal_uInt16 nPrfx,
    const OUString& rLocalName ) :
        SvXMLImportContext(rInImport, nPrfx, rLocalName),
        sLevelParagraphStyles(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_LevelParagraphStyles)),
        rTOCPropertySet(rPropSet),
        aStyleNames(),
        nOutlineLevel(-1)
{
}

XMLIndexTOCStylesContext::~XMLIndexTOCStylesContext()
{
}

void XMLIndexTOCStylesContext::StartElement(
    const Reference<XAttributeList> & xAttrList )
{
    // find text:outline-level attribute
    sal_Int16 nCount = xAttrList->getLength();
    for(sal_Int16 nAttr = 0; nAttr < nCount; nAttr++)
    {
        OUString sLocalName;
        sal_uInt16 nLclPrefix = GetImport().GetNamespaceMap().
            GetKeyByAttrName( xAttrList->getNameByIndex(nAttr),
                              &sLocalName );
        if ( (XML_NAMESPACE_TEXT == nLclPrefix) &&
             (IsXMLToken(sLocalName, XML_OUTLINE_LEVEL)) )
        {
            sal_Int32 nTmp;
            if (SvXMLUnitConverter::convertNumber(
                    nTmp, xAttrList->getValueByIndex(nAttr), 1,
                    GetImport().GetTextImport()->GetChapterNumbering()->
                                                                getCount()))
            {
                // API numbers 0..9, we number 1..10
                nOutlineLevel = nTmp-1;
            }
        }
    }
}

void XMLIndexTOCStylesContext::EndElement()
{
    // if valid...
    if (nOutlineLevel >= 0)
    {
        // copy vector into sequence
        sal_Int32 nCount = aStyleNames.size();
        Sequence<OUString> aStyleNamesSequence(nCount);
        for(sal_Int32 i = 0; i < nCount; i++)
        {
            aStyleNamesSequence[i] = aStyleNames[i];
        }

        // get index replace
        Any aAny = rTOCPropertySet->getPropertyValue(sLevelParagraphStyles);
        Reference<XIndexReplace> xIndexReplace;
        aAny >>= xIndexReplace;

        // set style names
        aAny <<= aStyleNamesSequence;
        xIndexReplace->replaceByIndex(nOutlineLevel, aAny);
    }
}

SvXMLImportContext *XMLIndexTOCStylesContext::CreateChildContext(
    sal_uInt16 nInPrefix,
    const OUString& rLocalName,
    const Reference<XAttributeList> & xAttrList )
{
    // check for index-source-style
    if ( (XML_NAMESPACE_TEXT == nInPrefix) &&
         IsXMLToken( rLocalName, XML_INDEX_SOURCE_STYLE ) )
    {
        // find text:style-name attribute and record in aStyleNames
        sal_Int16 nCount = xAttrList->getLength();
        for(sal_Int16 nAttr = 0; nAttr < nCount; nAttr++)
        {
            OUString sLocalName;
            sal_uInt16 nLclPrefix = GetImport().GetNamespaceMap().
                GetKeyByAttrName( xAttrList->getNameByIndex(nAttr),
                                  &sLocalName );
            if ( (XML_NAMESPACE_TEXT == nLclPrefix) &&
                 IsXMLToken( sLocalName, XML_STYLE_NAME ) )
            {
                aStyleNames.push_back(xAttrList->getValueByIndex(nAttr));
            }
        }
    }

    // always return default context; we already got the interesting info
    return SvXMLImportContext::CreateChildContext(nInPrefix, rLocalName,
                                                  xAttrList);
}
}//end of namespace binfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
