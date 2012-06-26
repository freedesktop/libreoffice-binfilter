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
#ifndef _XMLOFF_PAGEHEADERFOOTERCONTEXT_HXX
#define _XMLOFF_PAGEHEADERFOOTERCONTEXT_HXX

#include "xmlimp.hxx"
namespace binfilter {

class PageHeaderFooterContext : public SvXMLImportContext
{
    ::std::vector< XMLPropertyState > &     rProperties;
    sal_Int32                               nStartIndex;
    sal_Int32                               nEndIndex;
    sal_Bool                                bHeader;
    const UniReference < SvXMLImportPropertyMapper > rMap;

public:

    PageHeaderFooterContext( SvXMLImport& rImport, USHORT nPrfx,
                       const ::rtl::OUString& rLName,
                       const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                        ::std::vector< XMLPropertyState > & rProperties,
                        const UniReference < SvXMLImportPropertyMapper > &rMap,
                        sal_Int32 nStartIndex, sal_Int32 nEndIndex,
                        const sal_Bool bHeader);

    virtual ~PageHeaderFooterContext();

    virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

}//end of namespace binfilter
#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
