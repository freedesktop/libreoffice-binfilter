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

#ifndef _SVDFIELD_HXX
#define _SVDFIELD_HXX

#ifndef _FLDITEM_HXX
#ifndef ITEMID_FIELD
#include <bf_svx/editdata.hxx>  /* das include wird wg. EE_FEATURE_FIELD benoetigt */
#define ITEMID_FIELD EE_FEATURE_FIELD  /* wird fuer #include <bf_svx/flditem.hxx> benoetigt */
#endif
#include <bf_svx/flditem.hxx>
#endif
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
// Do not remove this, it is still used in src536a!
void SdrRegisterFieldClasses();
////////////////////////////////////////////////////////////////////////////////////////////////////

#define SDR_MEASUREFIELD 50

class SdrMeasureObj;

enum SdrMeasureFieldKind {SDRMEASUREFIELD_VALUE,SDRMEASUREFIELD_UNIT,SDRMEASUREFIELD_ROTA90BLANCS};

class SdrMeasureField: public SvxFieldData {
    SdrMeasureFieldKind eMeasureFieldKind;
public:
    SV_DECL_PERSIST1(SdrMeasureField,SvxFieldData,SDR_MEASUREFIELD)
    SdrMeasureField(): eMeasureFieldKind(SDRMEASUREFIELD_VALUE) {}
    SdrMeasureField(SdrMeasureFieldKind eNewKind): eMeasureFieldKind(eNewKind) {}
    virtual ~SdrMeasureField();
    virtual SvxFieldData* Clone() const;
    virtual int operator==(const SvxFieldData&) const;
    SdrMeasureFieldKind GetMeasureFieldKind() const { return eMeasureFieldKind; }
    void SetMeasureFieldKind(SdrMeasureFieldKind eNewKind) { eMeasureFieldKind=eNewKind; }
    void TakeRepresentation(const SdrMeasureObj& rObj, XubString& rStr) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDFIELD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
