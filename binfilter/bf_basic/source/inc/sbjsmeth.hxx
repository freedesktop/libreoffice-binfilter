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


#ifndef _SB_SBJSMETH_HXX
#define _SB_SBJSMETH_HXX

#include "sbmeth.hxx"

namespace binfilter {

// Basic-Modul fuer JavaScript-Sourcen.
// Alle Basic-spezifischen Methoden muessen virtuell ueberladen und deaktiviert
// werden. Die Unterscheidung von normalen Modulen erfolgt uebr RTTI.

class SbJScriptMethod : public SbMethod
{
public:
    SbJScriptMethod( const String&, SbxDataType, SbModule* );
    virtual ~SbJScriptMethod();

    SBX_DECL_PERSIST_NODATA(SBXCR_SBX,SBXID_JSCRIPTMETH,2);
    TYPEINFO();
};

#ifndef __SB_SBJSCRIPTMETHODREF_HXX
#define __SB_SBJSCRIPTMETHODREF_HXX
SV_DECL_IMPL_REF(SbJScriptMethod)
#endif

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
