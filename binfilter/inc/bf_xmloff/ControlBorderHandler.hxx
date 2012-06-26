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

#ifndef _XMLOFF_CONTROLBORDERHDL_HXX_
#define _XMLOFF_CONTROLBORDERHDL_HXX_

#include "prhdlfac.hxx"
#include <rtl/ref.hxx>
#include <rtl/ustring.hxx>
#include "xmlexppr.hxx"
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

    //=====================================================================
    //= OControlBorderHandler
    //=====================================================================
    class OControlBorderHandler : public XMLPropertyHandler
    {
    public:
        OControlBorderHandler();

        virtual sal_Bool importXML( const ::rtl::OUString& _rStrImpValue, ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
        virtual sal_Bool exportXML( ::rtl::OUString& _rStrExpValue, const ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
    };

    //=====================================================================
    //= OControlBorderHandler
    //=====================================================================
    class OControlTextEmphasisHandler : public XMLPropertyHandler
    {
    public:
        OControlTextEmphasisHandler();

        virtual sal_Bool importXML( const ::rtl::OUString& _rStrImpValue, ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
        virtual sal_Bool exportXML( ::rtl::OUString& _rStrExpValue, const ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
    };

//.........................................................................
}   // namespace xmloff
//.........................................................................

}//end of namespace binfilter
#endif // _XMLOFF_CONTROLBORDERHDL_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
