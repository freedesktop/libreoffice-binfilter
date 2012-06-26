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

#ifndef _SFX_BINDETECT_HXX
#define _SFX_BINDETECT_HXX

#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/document/XExtendedFilterDetection.hpp>
#include <cppuhelper/implbase2.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <bf_sfx2/sfxuno.hxx>

namespace com
{
    namespace sun
    {
        namespace star
        {
            namespace lang
            {
                class XMultiServiceFactory;
            }
            namespace beans
            {
                struct PropertyValue;
            }
        }
    }
}

#define REFERENCE ::com::sun::star::uno::Reference
#define SEQUENCE ::com::sun::star::uno::Sequence
#define RUNTIME_EXCEPTION ::com::sun::star::uno::RuntimeException

namespace binfilter {

class BinFilterDetect : public ::cppu::WeakImplHelper2< ::com::sun::star::document::XExtendedFilterDetection, ::com::sun::star::lang::XServiceInfo >
{
    com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory > m_xFact;


public:
                            BinFilterDetect( const REFERENCE < ::com::sun::star::lang::XMultiServiceFactory >& xFactory );
    virtual                 ~BinFilterDetect();

    SFX_DECL_XSERVICEINFO

    //----------------------------------------------------------------------------------
    // XExtendedFilterDetect
    //----------------------------------------------------------------------------------
    virtual ::rtl::OUString SAL_CALL detect( SEQUENCE< ::com::sun::star::beans::PropertyValue >& lDescriptor ) throw( RUNTIME_EXCEPTION );
};

}//end of namespace binfilter

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
