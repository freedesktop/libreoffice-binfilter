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


#include "XMLIndexBodyContext.hxx"


#include "xmlimp.hxx"



namespace binfilter {

using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XAttributeList;


TYPEINIT1( XMLIndexBodyContext, SvXMLImportContext);

XMLIndexBodyContext::XMLIndexBodyContext(
    SvXMLImport& rInImport,
    sal_uInt16 nPrfx,
    const ::rtl::OUString& rLocalName ) :
        SvXMLImportContext(rInImport, nPrfx, rLocalName),
        bHasContent(sal_False)
{
}

XMLIndexBodyContext::~XMLIndexBodyContext()
{
}

SvXMLImportContext* XMLIndexBodyContext::CreateChildContext(
    sal_uInt16 nInPrefix,
    const OUString& rLocalName,
    const Reference<XAttributeList> & xAttrList)
{
    SvXMLImportContext* pContext = NULL;

    // return text content (if possible)
    pContext = GetImport().GetTextImport()->CreateTextChildContext(
        GetImport(), nInPrefix, rLocalName, xAttrList, XML_TEXT_TYPE_SECTION );
    if (NULL == pContext)
    {
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLocalName );
    }
    else
        bHasContent = sal_True;

    return pContext;
}
}//end of namespace binfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
