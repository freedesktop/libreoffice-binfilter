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


#include "svdedxv.hxx"
#include <bf_svtools/solar.hrc>
#include <string.h>
#include "svditext.hxx"
#include "svdoutl.hxx"
#include "svdstr.hrc"   // Namen aus der Resource



// #98988#
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrObjEditView::ImpClearVars()
/*N*/ {
/*N*/     bQuickTextEditMode=TRUE;
/*N*/     bMacroMode=TRUE;
/*N*/     pTextEditObj=NULL;
/*N*/     pTextEditOutliner=NULL;
/*N*/     pTextEditOutlinerView=NULL;
/*N*/     pTextEditPV=NULL;
/*N*/     pTextEditWin=NULL;
/*N*/     pTextEditCursorMerker=NULL;
/*N*/     pEditPara=NULL;
/*N*/     bTextEditNewObj=FALSE;
/*N*/     bMacroDown=FALSE;
/*N*/     pMacroObj=NULL;
/*N*/     pMacroPV=NULL;
/*N*/     pMacroWin=NULL;
/*N*/     nMacroTol=0;
/*N*/     bTextEditDontDelete=FALSE;
/*N*/     bTextEditOnlyOneView=FALSE;
/*N*/ }

/*N*/ SdrObjEditView::SdrObjEditView(SdrModel* pModel1, OutputDevice* pOut):
/*N*/     SdrGlueEditView(pModel1,pOut)
/*N*/ {
/*N*/     ImpClearVars();
/*N*/ }

/*N*/ SdrObjEditView::~SdrObjEditView()
/*N*/ {
/*N*/   pTextEditWin = NULL;            // Damit es in EndTextEdit kein ShowCursor gibt
/*N*/   if (pTextEditOutliner!=NULL) {
/*?*/       delete pTextEditOutliner;
/*N*/   }
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ BOOL SdrObjEditView::IsAction() const
/*N*/ {
/*N*/     return IsMacroObj() || SdrGlueEditView::IsAction();
/*N*/ }




/*N*/ void SdrObjEditView::BrkAction()
/*N*/ {
/*N*/     SdrGlueEditView::BrkAction();
/*N*/ }


/*N*/ void SdrObjEditView::SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType)
/*N*/ {
/*N*/     SdrGlueEditView::SFX_NOTIFY(rBC,rBCType,rHint,rHintType);
/*N*/     // Printerwechsel waerend des Editierens
/*N*/     SdrHint* pSdrHint=PTR_CAST(SdrHint,&rHint);
/*N*/     if (pSdrHint!=NULL && pTextEditOutliner!=NULL) {
/*?*/         SdrHintKind eKind=pSdrHint->GetKind();
/*?*/         if (eKind==HINT_REFDEVICECHG) {
/*?*/             pTextEditOutliner->SetRefDevice(pMod->GetRefDevice());
/*?*/         }
/*?*/         if (eKind==HINT_DEFAULTTABCHG) {
/*?*/             pTextEditOutliner->SetDefTab(pMod->GetDefaultTabulator());
/*?*/         }
/*N*/     }
/*N*/ }

/*N*/ void SdrObjEditView::ModelHasChanged()
/*N*/ {
/*N*/     SdrGlueEditView::ModelHasChanged();
/*N*/ }

SdrEndTextEditKind SdrObjEditView::EndTextEdit(BOOL)
{
    SdrEndTextEditKind eRet=SDRENDTEXTEDIT_UNCHANGED;
    return eRet;
}

/*N*/ void SdrObjEditView::DelWin(OutputDevice* pWin1)
/*N*/ {
/*N*/     SdrGlueEditView::DelWin(pWin1);
/*N*/ }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
