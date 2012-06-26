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

#ifndef _XIMPGROUP_HXX
#define _XIMPGROUP_HXX

#include "xmlictxt.hxx"

#include "sdxmlimp_impl.hxx"

#include "nmspmap.hxx"

#include <com/sun/star/drawing/XShapes.hpp>

#include <tools/rtti.hxx>

#include "ximpshap.hxx"
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////
// draw:g context (RECURSIVE)

class SdXMLGroupShapeContext : public SdXMLShapeContext
{
    // the shape group this group is working on
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > mxChilds;

protected:
    void SetLocalShapesContext( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rNew)
        { mxShapes = rNew; }

public:
    TYPEINFO();

    SdXMLGroupShapeContext( SvXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
    virtual ~SdXMLGroupShapeContext();

    virtual SvXMLImportContext *CreateChildContext(
        USHORT nPrefix, const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList );
    virtual void StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
    virtual void EndElement();

    const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& GetLocalShapesContext() const
        { return mxShapes; }
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& GetLocalShapesContext()
        { return mxShapes; }
};


}//end of namespace binfilter
#endif  //  _XIMPGROUP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
