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

#include "XMLScriptContextFactory.hxx"

#include "XMLEventsImportContext.hxx"


#include "xmlimp.hxx"

#include "nmspmap.hxx"

#include "xmlnmspe.hxx"

namespace binfilter {


using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::xml::sax::XAttributeList;
using ::com::sun::star::beans::PropertyValue;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;

XMLScriptContextFactory::XMLScriptContextFactory() :
    sEventType(RTL_CONSTASCII_USTRINGPARAM("EventType")),
    sScript(RTL_CONSTASCII_USTRINGPARAM("Script")),
    sURL(RTL_CONSTASCII_USTRINGPARAM("Script"))
{
}

XMLScriptContextFactory::~XMLScriptContextFactory()
{
}

SvXMLImportContext * XMLScriptContextFactory::CreateContext
(SvXMLImport & rImport,
 sal_uInt16 nPrefix,
 const OUString & rLocalName,
 const Reference<XAttributeList> & xAttrList,
 XMLEventsImportContext * rEvents,
 const OUString & rApiEventName,
 const OUString & /*rLanguage*/)
{
    OUString sURLVal;

    sal_Int16 nCount = xAttrList->getLength();
    for (sal_Int16 nAttr = 0; nAttr < nCount; nAttr++)
    {
        OUString sLocalName;
        sal_uInt16 nLclPrefix = rImport.GetNamespaceMap().
            GetKeyByAttrName(xAttrList->getNameByIndex(nAttr), &sLocalName);

        if (XML_NAMESPACE_XLINK == nLclPrefix)
        {
            if (IsXMLToken(sLocalName, XML_HREF))
                sURLVal = xAttrList->getValueByIndex(nAttr);
            // else: ignore
        }
        // else ignore
    }

    Sequence<PropertyValue> aValues(2);

    // EventType
    aValues[0].Name = sEventType;
    aValues[0].Value <<= sScript;

    // URL
    aValues[1].Name = sURL;
    aValues[1].Value <<= sURLVal;

    // add values for event now
    rEvents->AddEventValues(rApiEventName, aValues);

    // return dummy context
    return new SvXMLImportContext(rImport, nPrefix, rLocalName);
}

}//end of namespace binfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
