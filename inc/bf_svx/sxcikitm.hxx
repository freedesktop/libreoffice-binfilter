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
#ifndef _SXCIKITM_HXX
#define _SXCIKITM_HXX

#include <bf_svx/svddef.hxx>

#include <bf_svtools/eitem.hxx>
namespace binfilter {

enum SdrCircKind {SDRCIRC_FULL,
                  SDRCIRC_SECT,
                  SDRCIRC_CUT,
                  SDRCIRC_ARC};

class SdrCircKindItem: public SfxEnumItem {
public:
    TYPEINFO();
    SdrCircKindItem(SdrCircKind eKind=SDRCIRC_FULL): SfxEnumItem(SDRATTR_CIRCKIND,eKind) {}
    SdrCircKindItem(SvStream& rIn)                 : SfxEnumItem(SDRATTR_CIRCKIND,rIn)   {}
    virtual SfxPoolItem* Clone(SfxItemPool* pPool=NULL) const;
    virtual SfxPoolItem* Create(SvStream& rIn, USHORT nVer) const;
    virtual USHORT       GetValueCount() const; // { return 4; }
            SdrCircKind  GetValue() const      { return (SdrCircKind)SfxEnumItem::GetValue(); }

    virtual bool         PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

};

}//end of namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
