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

#include "splithdl.hxx"

#include <com/sun/star/uno/Any.hxx>


// --

#include <xmltoken.hxx>
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFmtSplitPropHdl
//

XMLFmtSplitPropHdl::~XMLFmtSplitPropHdl()
{
    // nothing to do
}

sal_Bool XMLFmtSplitPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& /*rUnitConverter*/ ) const
{
    sal_Bool bValue = IsXMLToken( rStrImpValue, XML_COLUMNSPLIT_AUTO );
    rValue <<= sal_Bool(bValue);
    return  sal_True;
}

sal_Bool XMLFmtSplitPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& /*rUnitConverter*/ ) const
{
    sal_Bool bRet = sal_False;
    sal_Bool bValue(sal_False);

    if (rValue >>= bValue)
    {
        if( bValue )
        {
            rStrExpValue = GetXMLToken( XML_COLUMNSPLIT_AUTO );
        }
        else
        {
            rStrExpValue = GetXMLToken( XML_COLUMNSPLIT_AVOID );
        }

        bRet = sal_True;
    }

    return bRet;
}
}//end of namespace binfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
