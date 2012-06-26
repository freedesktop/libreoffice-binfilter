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

#ifndef _SC_XMLCONSOLIDATIONCONTEXT_HXX
#define _SC_XMLCONSOLIDATIONCONTEXT_HXX

#include "global.hxx"

#include <bf_xmloff/xmlimp.hxx>
namespace binfilter {

class ScXMLImport;


//___________________________________________________________________

class ScXMLConsolidationContext : public SvXMLImportContext
{
private:
    ::rtl::OUString             sSourceList;
    ::rtl::OUString             sUseLabel;
    ScAddress                   aTargetAddr;
    ScSubTotalFunc              eFunction;
    sal_Bool                    bLinkToSource : 1;
    sal_Bool                    bTargetAddr : 1;

protected:
    const ScXMLImport&          GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport&                GetScImport()       { return (ScXMLImport&)GetImport(); }

public:
                                ScXMLConsolidationContext(
                                    ScXMLImport& rImport,
                                    USHORT nPrfx,
                                    const ::rtl::OUString& rLName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    );
    virtual                     ~ScXMLConsolidationContext();

    virtual SvXMLImportContext* CreateChildContext(
                                    USHORT nPrefix,
                                    const ::rtl::OUString& rLocalName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    );
    virtual void                EndElement();
};


} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
