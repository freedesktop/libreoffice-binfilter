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

#ifndef _XMLOFF_XMLSCRIPTI_HXX
#define _XMLOFF_XMLSCRIPTI_HXX

#include <bf_xmloff/xmlictxt.hxx>

#include <rtl/ustrbuf.hxx>

#include <com/sun/star/document/XDocumentInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

namespace com { namespace sun { namespace star { namespace frame {
    class XModel;
}}}}

namespace binfilter {

class SvXMLTokenMap;

class XMLScriptContext : public SvXMLImportContext
{
private:
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > m_xModel;

public:
    XMLScriptContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                        const ::rtl::OUString& rLName,
                        const ::com::sun::star::uno::Reference<
                            ::com::sun::star::frame::XModel>& rDocModel );
    virtual ~XMLScriptContext();

    // Create child element.
    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                const ::rtl::OUString& rLocalName,
                const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    virtual void EndElement();
};

}//end of namespace binfilter
#endif // _XMLOFF_XMLSCRIPTI_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
