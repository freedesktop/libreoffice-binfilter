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

#ifndef _FORMS_FIXEDTEXT_HXX_
#define _FORMS_FIXEDTEXT_HXX_

#include "FormComponent.hxx"
namespace binfilter {

//.........................................................................
namespace frm
{

//==================================================================
// OFixedTextModel
//==================================================================
class OFixedTextModel
        :public OControlModel
        ,public ::comphelper::OAggregationArrayUsageHelper< OFixedTextModel >
{
public:
    DECLARE_DEFAULT_LEAF_XTOR( OFixedTextModel );

// XPropertySetRef
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo> SAL_CALL   getPropertySetInfo() throw(::com::sun::star::uno::RuntimeException);
    virtual cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

// XServiceInfo
    IMPLEMENTATION_NAME(OFixedTextModel);
    virtual StringSequence SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);

// XPersistObject
    virtual ::rtl::OUString SAL_CALL    getServiceName() throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL
        write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL
        read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream) throw(::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

// OAggregationArrayUsageHelper
    virtual void fillProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps,
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
        ) const;
    IMPLEMENT_INFO_SERVICE()

protected:
    DECLARE_XCLONEABLE();
};

//.........................................................................
}
//.........................................................................

}//end of namespace binfilter
#endif // _FORMS_FIXEDTEXT_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
