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

#ifndef _SVX_XLNEDIT_HXX
#define _SVX_XLNEDIT_HXX

#include <bf_svx/xpoly.hxx>
#include <bf_svx/xit.hxx>
namespace binfilter {

class SdrModel;

//----------------------
// class XLineEndItem
//----------------------
class XLineEndItem : public NameOrIndex
{
    XPolygon    aXPolygon;

public:
            TYPEINFO();
            XLineEndItem(long nIndex = -1);
            XLineEndItem(const String& rName, const XPolygon& rXPolygon);
            XLineEndItem(SfxItemPool* pPool, const XPolygon& rXPolygon);
            XLineEndItem(const XLineEndItem& rItem);
            XLineEndItem(SvStream& rIn);

    virtual int             operator==(const SfxPoolItem& rItem) const;
    virtual SfxPoolItem*    Clone(SfxItemPool* pPool = 0) const;
    virtual SfxPoolItem*    Create(SvStream& rIn, USHORT nVer) const;
    virtual SvStream&       Store(SvStream& rOut, USHORT) const { return rOut; }

    virtual bool            QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual bool            PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


    const XPolygon& GetValue() const;
    void            SetValue(const XPolygon& rXPolygon) { aXPolygon = rXPolygon; Detach(); }

    XLineEndItem* checkForUniqueItem( SdrModel* pModel ) const;
};

}//end of namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
