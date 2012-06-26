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


#ifndef __SBX_SBXMETHOD_HXX
#define __SBX_SBXMETHOD_HXX

#include "sbxvar.hxx"

namespace binfilter {

class SbxMethodImpl;

class SbxMethod : public SbxVariable
{
    SbxMethodImpl* mpSbxMethodImpl; // Impl data

public:
    SBX_DECL_PERSIST_NODATA(SBXCR_SBX,SBXID_METHOD,1);
    TYPEINFO();
    SbxMethod( const String& r, SbxDataType t )
    : SbxVariable( t ) { SetName( r ); }
    SbxMethod( const SbxMethod& r ) : SvRefBase( r ), SbxVariable( r ) {}
    SbxMethod& operator=( const SbxMethod& r )
    { SbxVariable::operator=( r ); return *this; }
    virtual SbxClassType GetClass() const;
};

#ifndef __SBX_SBXMETHODREF_HXX
#define __SBX_SBXMETHODREF_HXX

#ifndef SBX_METHOD_DECL_DEFINED
#define SBX_METHOD_DECL_DEFINED
SV_DECL_REF(SbxMethod)
#endif
SV_IMPL_REF(SbxMethod)

#endif

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
