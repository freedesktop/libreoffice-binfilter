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
#ifndef SXMSEITM_HXX
#define SXMSEITM_HXX

#include <bf_svx/svddef.hxx>

#include <bf_svtools/itemset.hxx>

#include <bf_svtools/poolitem.hxx>
namespace binfilter {

//-------------------------
// class SdrMeasureSetItem
//-------------------------
class SdrMeasureSetItem: public SfxSetItem {
public:
    TYPEINFO();
    SdrMeasureSetItem(SfxItemSet* pItemSet)          : SfxSetItem(SDRATTRSET_MEASURE,pItemSet) {}
    SdrMeasureSetItem(SfxItemPool* pItemPool)        : SfxSetItem(SDRATTRSET_MEASURE,new SfxItemSet(*pItemPool,SDRATTR_MEASURE_FIRST,SDRATTR_MEASURE_LAST)) {}
    SdrMeasureSetItem(const SdrMeasureSetItem& rAttr, SfxItemPool* pItemPool=NULL): SfxSetItem(rAttr,pItemPool) {}
    virtual SfxPoolItem*    Clone(SfxItemPool* pToPool) const;
    virtual SfxPoolItem*    Create(SvStream& rStream, USHORT nVersion) const;
};

}//end of namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
