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

#ifndef _XIMPBODY_HXX
#define _XIMPBODY_HXX

#include "xmlictxt.hxx"

#include "sdxmlimp_impl.hxx"

#include "nmspmap.hxx"

#include "ximppage.hxx"
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////
// draw:page context

class SdXMLDrawPageContext : public SdXMLGenericPageContext
{
    ::rtl::OUString             maName;
    ::rtl::OUString             maStyleName;
    ::rtl::OUString             maMasterPageName;
    ::rtl::OUString             maHREF;

public:
    SdXMLDrawPageContext( SdXMLImport& rImport, USHORT nPrfx,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
    virtual ~SdXMLDrawPageContext();

    virtual SvXMLImportContext *CreateChildContext(
        USHORT nPrefix, const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList );
    virtual void EndElement();

};

//////////////////////////////////////////////////////////////////////////////
// office:body context

class SdXMLBodyContext : public SvXMLImportContext
{
    const SdXMLImport& GetSdImport() const { return (const SdXMLImport&)GetImport(); }
    SdXMLImport& GetSdImport() { return (SdXMLImport&)GetImport(); }

public:
    SdXMLBodyContext( SdXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLocalName );
    virtual ~SdXMLBodyContext();

    virtual SvXMLImportContext *CreateChildContext(
        USHORT nPrefix, const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList );
};


}//end of namespace binfilter
#endif  //  _XIMPBODY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
