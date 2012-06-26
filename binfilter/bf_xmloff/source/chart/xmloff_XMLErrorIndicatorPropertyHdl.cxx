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
#include "XMLErrorIndicatorPropertyHdl.hxx"

#include "xmluconv.hxx"
#include <com/sun/star/chart/ChartErrorIndicatorType.hpp>
#include <rtl/ustrbuf.hxx>
namespace binfilter {

using namespace ::com::sun::star;

XMLErrorIndicatorPropertyHdl::~XMLErrorIndicatorPropertyHdl()
{}

sal_Bool XMLErrorIndicatorPropertyHdl::importXML( const ::rtl::OUString& rStrImpValue,
                                                  uno::Any& rValue, const SvXMLUnitConverter& /*rUnitConverter*/ ) const
{
    sal_Bool bValue;
    SvXMLUnitConverter::convertBool( bValue, rStrImpValue );

    // modify existing value
    chart::ChartErrorIndicatorType eType = chart::ChartErrorIndicatorType_NONE;
    if( rValue.hasValue())
        rValue >>= eType;

    if( bValue )    // enable flag
    {
        if( eType != chart::ChartErrorIndicatorType_TOP_AND_BOTTOM )
        {
            if( mbUpperIndicator )
                eType = ( eType == chart::ChartErrorIndicatorType_LOWER )
                    ? chart::ChartErrorIndicatorType_TOP_AND_BOTTOM
                    : chart::ChartErrorIndicatorType_UPPER;
            else
                eType = ( eType == chart::ChartErrorIndicatorType_UPPER )
                    ? chart::ChartErrorIndicatorType_TOP_AND_BOTTOM
                    : chart::ChartErrorIndicatorType_LOWER;
        }
    }
    else            // disable flag
    {
        if( eType != chart::ChartErrorIndicatorType_NONE )
        {
            if( mbUpperIndicator )
                eType = ( eType == chart::ChartErrorIndicatorType_UPPER )
                    ? chart::ChartErrorIndicatorType_NONE
                    : chart::ChartErrorIndicatorType_LOWER;
            else
                eType = ( eType == chart::ChartErrorIndicatorType_LOWER )
                    ? chart::ChartErrorIndicatorType_NONE
                    : chart::ChartErrorIndicatorType_UPPER;
        }
    }

    rValue <<= eType;

    return sal_True;
}

sal_Bool XMLErrorIndicatorPropertyHdl::exportXML( ::rtl::OUString& rStrExpValue,
                                                  const uno::Any& rValue, const SvXMLUnitConverter& /*rUnitConverter*/ ) const
{
    ::rtl::OUStringBuffer aBuffer;
    chart::ChartErrorIndicatorType eType;

    rValue >>= eType;
    sal_Bool bValue = ( eType == chart::ChartErrorIndicatorType_TOP_AND_BOTTOM ||
                        ( mbUpperIndicator
                          ? ( eType == chart::ChartErrorIndicatorType_UPPER )
                          : ( eType == chart::ChartErrorIndicatorType_LOWER )));

    if( bValue )
    {
        SvXMLUnitConverter::convertBool( aBuffer, bValue );
        rStrExpValue = aBuffer.makeStringAndClear();
    }

    // only export if set to true
    return bValue;
}
}//end of namespace binfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
