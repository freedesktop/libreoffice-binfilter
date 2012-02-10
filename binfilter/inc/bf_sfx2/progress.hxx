/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _SFX_PROGRESS_HXX
#define _SFX_PROGRESS_HXX

#include <bf_svtools/bf_solar.h>

#include <tools/link.hxx>
namespace binfilter {
class String;
class SfxObjectShell;
class SfxStatusBarManager;
class INetHint;
struct SfxProgress_Impl;
struct PlugInLoadStatus;
struct SvProgressArg;

//=======================================================================

class SfxProgress
{
    SfxProgress_Impl*       pImp;
    ULONG                   nVal;
    BOOL                    bSuspended;

public:
                            SfxProgress( SfxObjectShell* pObjSh,
                                         const String& rText,
                                         ULONG nRange, BOOL bAllDocs = FALSE,
                                         BOOL bWait = TRUE );
    virtual                 ~SfxProgress();

    virtual BOOL            SetState( ULONG nVal, ULONG nNewRange = 0 );
    ULONG                   GetState() const { return nVal; }

    void                    Resume();
    void                    Suspend();
    BOOL                    IsSuspended() const { return bSuspended; }

    void                    Stop();


    static SfxProgress*     GetActiveProgress( SfxObjectShell *pDocSh = 0 );

#if _SOLAR__PRIVATE
    DECL_STATIC_LINK( SfxProgress, SetStateHdl, PlugInLoadStatus* );
#endif
};

}//end of namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
