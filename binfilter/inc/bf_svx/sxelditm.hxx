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
#ifndef _SXELDITM_HXX
#define _SXELDITM_HXX

#include <bf_svx/svddef.hxx>

#include <bf_svx/sdmetitm.hxx>
namespace binfilter {

class SdrEdgeLineDeltaAnzItem: public SfxUInt16Item {
public:
    SdrEdgeLineDeltaAnzItem(UINT16 nVal=0): SfxUInt16Item(SDRATTR_EDGELINEDELTAANZ,nVal) {}
    SdrEdgeLineDeltaAnzItem(SvStream& rIn): SfxUInt16Item(SDRATTR_EDGELINEDELTAANZ,rIn)  {}
};

class SdrEdgeLine1DeltaItem: public SdrMetricItem {
public:
    SdrEdgeLine1DeltaItem(long nVal=0): SdrMetricItem(SDRATTR_EDGELINE1DELTA,nVal)  {}
    SdrEdgeLine1DeltaItem(SvStream& rIn): SdrMetricItem(SDRATTR_EDGELINE1DELTA,rIn) {}
};

class SdrEdgeLine2DeltaItem: public SdrMetricItem {
public:
    SdrEdgeLine2DeltaItem(long nVal=0): SdrMetricItem(SDRATTR_EDGELINE2DELTA,nVal)  {}
    SdrEdgeLine2DeltaItem(SvStream& rIn): SdrMetricItem(SDRATTR_EDGELINE2DELTA,rIn) {}
};

class SdrEdgeLine3DeltaItem: public SdrMetricItem {
public:
    SdrEdgeLine3DeltaItem(long nVal=0): SdrMetricItem(SDRATTR_EDGELINE3DELTA,nVal)  {}
    SdrEdgeLine3DeltaItem(SvStream& rIn): SdrMetricItem(SDRATTR_EDGELINE3DELTA,rIn) {}
};

}//end of namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
