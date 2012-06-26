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
#ifndef _FMTCLBL_HXX
#define _FMTCLBL_HXX

#include <bf_svtools/bf_solar.h>


#include <bf_svtools/eitem.hxx>
#include <format.hxx>
class IntlWrapper;
namespace binfilter {



class SwFmtNoBalancedColumns : public SfxBoolItem
{
public:
    SwFmtNoBalancedColumns( BOOL bFlag = FALSE )
        : SfxBoolItem( RES_COLUMNBALANCE, bFlag ) {}

    // "pure virtual Methoden" vom SfxPoolItem
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*    Create(SvStream &, USHORT) const;
    virtual SvStream&       Store(SvStream &rStream, USHORT) const { return rStream; }
    virtual USHORT    GetVersion( USHORT nFFVer ) const;
};


inline const SwFmtNoBalancedColumns &SwAttrSet::GetBalancedColumns(BOOL bInP) const
    { return (const SwFmtNoBalancedColumns&)Get( RES_COLUMNBALANCE, bInP ); }

inline const SwFmtNoBalancedColumns &SwFmt::GetBalancedColumns(BOOL bInP) const
    { return aSet.GetBalancedColumns( bInP ); }

} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
