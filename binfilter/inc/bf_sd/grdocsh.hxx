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

#ifndef _SD_GRDOCSH_HXX
#define _SD_GRDOCSH_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_sfx2/docfac.hxx>
#include <bf_sfx2/interno.hxx>
#include <bf_sfx2/objsh.hxx>

#include "docshell.hxx"
#include "glob.hxx"
#include "sdmod.hxx"
#include "pres.hxx"
namespace binfilter {


/*************************************************************************
|*
|* Document-Shell fuer Draw-Dokumente
|*
\************************************************************************/

class SdGraphicDocShell : public SdDrawDocShell
{
 public:
    TYPEINFO();

    SFX_DECL_OBJECTFACTORY_DLL(SdGraphicDocShell, SD_MOD());

    SdGraphicDocShell(SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED,
                   BOOL bSdDataObj=FALSE, DocumentType=DOCUMENT_TYPE_DRAW);
    SdGraphicDocShell(SdDrawDocument* pDoc, SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED,
                   BOOL bSdDataObj=FALSE, DocumentType=DOCUMENT_TYPE_DRAW);
    ~SdGraphicDocShell();
};

#ifndef SV_DECL_SDGRAPHICDOCSHELL_DEFINED
#define SV_DECL_SDGRAPHICDOCSHELL_DEFINED
SV_DECL_REF(SdGraphicDocShell)
#endif

SV_IMPL_REF (SdGraphicDocShell)


} //namespace binfilter
#endif          // _SD_GRDOCSH_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
