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

#include "xoutx.hxx"     // fuer XOutCreatePolygon
#include "svdopath.hxx"
#include "svdtxhdl.hxx"  // DrawTextToPath fuer Convert
#include "svdmodel.hxx"  // fuer Convert

namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@@ @@@@@ @@   @@ @@@@@@  @@@@  @@@@@  @@@@@@
//    @@   @@    @@@ @@@   @@   @@  @@ @@  @@     @@
//    @@   @@     @@@@@    @@   @@  @@ @@  @@     @@
//    @@   @@@@    @@@     @@   @@  @@ @@@@@      @@
//    @@   @@     @@@@@    @@   @@  @@ @@  @@     @@
//    @@   @@    @@@ @@@   @@   @@  @@ @@  @@ @@  @@
//    @@   @@@@@ @@   @@   @@    @@@@  @@@@@   @@@@
//
//  Transformationen
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ void SdrTextObj::NbcSetSnapRect(const Rectangle& rRect)
/*N*/ {
/*N*/   if (aGeo.nDrehWink!=0 || aGeo.nShearWink!=0) {
/*?*/       Rectangle aSR0(GetSnapRect());
/*?*/       long nWdt0=aSR0.Right()-aSR0.Left();
/*?*/       long nHgt0=aSR0.Bottom()-aSR0.Top();
/*?*/       long nWdt1=rRect.Right()-rRect.Left();
/*?*/       long nHgt1=rRect.Bottom()-rRect.Top();
/*?*/       SdrTextObj::NbcResize(maSnapRect.TopLeft(),Fraction(nWdt1,nWdt0),Fraction(nHgt1,nHgt0));
/*?*/       SdrTextObj::NbcMove(Size(rRect.Left()-aSR0.Left(),rRect.Top()-aSR0.Top()));
/*N*/   } else {
/*N*/       long nHDist=GetTextLeftDistance()+GetTextRightDistance();
/*N*/       long nVDist=GetTextUpperDistance()+GetTextLowerDistance();
/*N*/       long nTWdt0=aRect.GetWidth ()-1-nHDist; if (nTWdt0<0) nTWdt0=0;
/*N*/       long nTHgt0=aRect.GetHeight()-1-nVDist; if (nTHgt0<0) nTHgt0=0;
/*N*/       long nTWdt1=rRect.GetWidth ()-1-nHDist; if (nTWdt1<0) nTWdt1=0;
/*N*/       long nTHgt1=rRect.GetHeight()-1-nVDist; if (nTHgt1<0) nTHgt1=0;
/*N*/       aRect=rRect;
/*N*/       ImpJustifyRect(aRect);
/*N*/       if (bTextFrame && (pModel==NULL || !pModel->IsPasteResize())) { // #51139#
/*N*/           if (nTWdt0!=nTWdt1 && IsAutoGrowWidth() ) NbcSetMinTextFrameWidth(nTWdt1);
/*N*/           if (nTHgt0!=nTHgt1 && IsAutoGrowHeight()) NbcSetMinTextFrameHeight(nTHgt1);
/*N*/           if (GetFitToSize()==SDRTEXTFIT_RESIZEATTR) {DBG_BF_ASSERT(0, "STRIP");
/*N*/           }
/*N*/           NbcAdjustTextFrameWidthAndHeight();
/*N*/       }
/*N*/       ImpCheckShear();
/*N*/       SetRectsDirty();
/*N*/   }
/*N*/ }

/*N*/ const Rectangle& SdrTextObj::GetLogicRect() const
/*N*/ {
/*N*/   return aRect;
/*N*/ }

/*N*/ void SdrTextObj::NbcSetLogicRect(const Rectangle& rRect)
/*N*/ {
/*N*/   long nHDist=GetTextLeftDistance()+GetTextRightDistance();
/*N*/   long nVDist=GetTextUpperDistance()+GetTextLowerDistance();
/*N*/   long nTWdt0=aRect.GetWidth ()-1-nHDist; if (nTWdt0<0) nTWdt0=0;
/*N*/   long nTHgt0=aRect.GetHeight()-1-nVDist; if (nTHgt0<0) nTHgt0=0;
/*N*/   long nTWdt1=rRect.GetWidth ()-1-nHDist; if (nTWdt1<0) nTWdt1=0;
/*N*/   long nTHgt1=rRect.GetHeight()-1-nVDist; if (nTHgt1<0) nTHgt1=0;
/*N*/   aRect=rRect;
/*N*/   ImpJustifyRect(aRect);
/*N*/   if (bTextFrame) {
/*N*/       if (nTWdt0!=nTWdt1 && IsAutoGrowWidth() ) NbcSetMinTextFrameWidth(nTWdt1);
/*N*/       if (nTHgt0!=nTHgt1 && IsAutoGrowHeight()) NbcSetMinTextFrameHeight(nTHgt1);
/*N*/       if (GetFitToSize()==SDRTEXTFIT_RESIZEATTR) {DBG_BF_ASSERT(0, "STRIP");
/*N*/       }
/*N*/       NbcAdjustTextFrameWidthAndHeight();
/*N*/   }
/*N*/   SetRectsDirty();
/*N*/ }

/*N*/ long SdrTextObj::GetRotateAngle() const
/*N*/ {
/*N*/   return aGeo.nDrehWink;
/*N*/ }

/*N*/ long SdrTextObj::GetShearAngle(bool /*bVertical*/) const
/*N*/ {
/*N*/   return (aGeo.nDrehWink==0 || aGeo.nDrehWink==18000) ? aGeo.nShearWink : 0;
/*N*/ }

/*N*/ void SdrTextObj::NbcMove(const Size& rSiz)
/*N*/ {
/*N*/   MoveRect(aRect,rSiz);
/*N*/   MoveRect(aOutRect,rSiz);
/*N*/   MoveRect(maSnapRect,rSiz);
/*N*/   SetRectsDirty(TRUE);
/*N*/ }

/*N*/ void SdrTextObj::NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact)
/*N*/ {
/*N*/   bool bNoShearMerk=aGeo.nShearWink==0;
/*N*/   bool bRota90Merk=bNoShearMerk && aGeo.nDrehWink % 9000 ==0;
/*N*/   long nHDist=GetTextLeftDistance()+GetTextRightDistance();
/*N*/   long nVDist=GetTextUpperDistance()+GetTextLowerDistance();
/*N*/   long nTWdt0=aRect.GetWidth ()-1-nHDist; if (nTWdt0<0) nTWdt0=0;
/*N*/   long nTHgt0=aRect.GetHeight()-1-nVDist; if (nTHgt0<0) nTHgt0=0;
/*N*/   bool bXMirr=(xFact.GetNumerator()<0) != (xFact.GetDenominator()<0);
/*N*/   bool bYMirr=(yFact.GetNumerator()<0) != (yFact.GetDenominator()<0);
/*N*/
/*N*/   if (aGeo.nDrehWink==0 && aGeo.nShearWink==0) {
/*N*/       ResizeRect(aRect,rRef,xFact,yFact);
/*N*/       if (bYMirr) {
/*N*/           aRect.Justify();
/*N*/           aRect.Move(aRect.Right()-aRect.Left(),aRect.Bottom()-aRect.Top());
/*N*/           aGeo.nDrehWink=18000;
/*N*/           aGeo.RecalcSinCos();
/*N*/       }
/*N*/   }
/*N*/   else
/*N*/   {
/*N*/       // #100663# aRect is NOT initialized for lines (polgon objects with two
/*N*/       // exceptionally handled points). Thus, after this call the text rotaion is
/*N*/       // gone. This error must be present since day one of this old drawing layer.
/*N*/       // It's astonishing that noone discovered it earlier.
/*N*/       // Polygon aPol(Rect2Poly(aRect,aGeo));
/*N*/       // Polygon aPol(Rect2Poly(GetSnapRect(), aGeo));
/*N*/
/*N*/       // #101412# go back to old method, side effects are impossible
/*N*/       // to calculate.
/*N*/       Polygon aPol(Rect2Poly(aRect,aGeo));
/*N*/
/*N*/       for(sal_uInt16 a(0); a < aPol.GetSize(); a++)
/*N*/       {
/*N*/            ResizePoint(aPol[a], rRef, xFact, yFact);
/*N*/       }
/*N*/
/*N*/       if(bXMirr != bYMirr)
/*N*/       {
/*N*/           // Polygon wenden und etwas schieben
/*N*/           Polygon aPol0(aPol);
/*N*/
/*N*/           aPol[0] = aPol0[1];
/*N*/           aPol[1] = aPol0[0];
/*N*/           aPol[2] = aPol0[3];
/*N*/           aPol[3] = aPol0[2];
/*N*/           aPol[4] = aPol0[1];
/*N*/       }
/*N*/
/*N*/       Poly2Rect(aPol, aRect, aGeo);
/*N*/   }
/*N*/
/*N*/   if (bRota90Merk) {
/*N*/       bool bRota90=aGeo.nDrehWink % 9000 ==0;
/*N*/       if (!bRota90) { // Scheinbar Rundungsfehler: Korregieren
/*N*/           long a=NormAngle360(aGeo.nDrehWink);
/*N*/           if (a<4500) a=0;
/*N*/           else if (a<13500) a=9000;
/*N*/           else if (a<22500) a=18000;
/*N*/           else if (a<31500) a=27000;
/*N*/           else a=0;
/*N*/           aGeo.nDrehWink=a;
/*N*/           aGeo.RecalcSinCos();
/*N*/       }
/*N*/       if (bNoShearMerk!=(aGeo.nShearWink==0)) { // Shear ggf. korregieren wg. Rundungsfehler
/*N*/           aGeo.nShearWink=0;
/*N*/           aGeo.RecalcTan();
/*N*/       }
/*N*/   }
/*N*/
/*N*/   ImpJustifyRect(aRect);
/*N*/   long nTWdt1=aRect.GetWidth ()-1-nHDist; if (nTWdt1<0) nTWdt1=0;
/*N*/   long nTHgt1=aRect.GetHeight()-1-nVDist; if (nTHgt1<0) nTHgt1=0;
/*N*/   if (bTextFrame && (pModel==NULL || !pModel->IsPasteResize())) { // #51139#
/*N*/       if (nTWdt0!=nTWdt1 && IsAutoGrowWidth() ) NbcSetMinTextFrameWidth(nTWdt1);
/*N*/       if (nTHgt0!=nTHgt1 && IsAutoGrowHeight()) NbcSetMinTextFrameHeight(nTHgt1);
/*?*/       NbcAdjustTextFrameWidthAndHeight();
/*N*/   }
/*N*/   ImpCheckShear();
/*N*/   SetRectsDirty();
/*N*/ }

/*N*/ void SdrTextObj::NbcRotate(const Point& rRef, long nWink, double sn, double cs)
/*N*/ {
/*N*/   long dx=aRect.Right()-aRect.Left();
/*N*/   long dy=aRect.Bottom()-aRect.Top();
/*N*/   Point aP(aRect.TopLeft());
/*N*/   RotatePoint(aP,rRef,sn,cs);
/*N*/   aRect.Left()=aP.X();
/*N*/   aRect.Top()=aP.Y();
/*N*/   aRect.Right()=aRect.Left()+dx;
/*N*/   aRect.Bottom()=aRect.Top()+dy;
/*N*/   if (aGeo.nDrehWink==0) {
/*N*/       aGeo.nDrehWink=NormAngle360(nWink);
/*N*/       aGeo.nSin=sn;
/*N*/       aGeo.nCos=cs;
/*N*/   } else {
/*N*/       aGeo.nDrehWink=NormAngle360(aGeo.nDrehWink+nWink);
/*N*/       aGeo.RecalcSinCos();
/*N*/   }
/*N*/   SetRectsDirty();
/*N*/ }

/*N*/ void SdrTextObj::NbcShear(const Point& rRef, double tn, bool bVShear)
/*N*/ {
/*N*/   // #75889# when this is a SdrPathObj aRect maybe not initialized
/*N*/   Polygon aPol(Rect2Poly(aRect.IsEmpty() ? GetSnapRect() : aRect, aGeo));
/*N*/
/*N*/   USHORT nPointCount=aPol.GetSize();
/*N*/   for (USHORT i=0; i<nPointCount; i++) {
/*N*/        ShearPoint(aPol[i],rRef,tn,bVShear);
/*N*/   }
/*N*/   Poly2Rect(aPol,aRect,aGeo);
/*N*/   ImpJustifyRect(aRect);
/*N*/   if (bTextFrame) {
/*?*/       NbcAdjustTextFrameWidthAndHeight();
/*N*/   }
/*N*/   ImpCheckShear();
/*N*/   SetRectsDirty();
/*N*/ }




/*N*/ void SdrTextObj::ImpConvertSetAttrAndLayer(SdrObject* pObj, bool bNoSetAttr) const
/*N*/ {
/*N*/   if (pObj!=NULL) {
/*N*/       pObj->ImpSetAnchorPos(aAnchor);
/*N*/       pObj->NbcSetLayer(SdrLayerID(nLayerId));
/*N*/       if (pModel!=NULL) {
/*N*/           pObj->SetModel(pModel);
/*N*/           if (!bNoSetAttr) {
/*N*/               SdrBroadcastItemChange aItemChange(*pObj);
/*N*/               pObj->ClearItem();
/*N*/               pObj->SetItemSet(GetItemSet());
/*N*/               pObj->BroadcastItemChange(aItemChange);
/*N*/
/*N*/               pObj->NbcSetStyleSheet(GetStyleSheet(),TRUE);
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/ }

/*N*/ SdrObject* SdrTextObj::ImpConvertMakeObj(const XPolyPolygon& rXPP, bool bClosed, bool bBezier, bool bNoSetAttr) const
/*N*/ {
/*N*/   SdrObjKind ePathKind=bClosed?OBJ_PATHFILL:OBJ_PATHLINE;
/*N*/   XPolyPolygon aXPP(rXPP);
/*N*/   if (!bBezier && pModel!=NULL) {
/*N*/       // Polygon aus Bezierkurve interpolieren
/*N*/       VirtualDevice   aVDev;
/*N*/       XPolyPolygon    aXPolyPoly;
/*N*/
/*N*/       MapMode aMap = aVDev.GetMapMode();
/*N*/       aMap.SetMapUnit(pModel->GetScaleUnit());
/*N*/       aMap.SetScaleX(pModel->GetScaleFraction());
/*N*/       aMap.SetScaleY(pModel->GetScaleFraction());
/*N*/       aVDev.SetMapMode(aMap);
/*N*/
/*N*/       for (USHORT i=0; i<aXPP.Count(); i++)
/*N*/           aXPolyPoly.Insert(XOutCreatePolygon(aXPP[i],&aVDev));
/*N*/       aXPP=aXPolyPoly;
/*N*/       ePathKind=bClosed?OBJ_POLY:OBJ_PLIN;
/*N*/   }
/*N*/   SdrPathObj* pPathObj=new SdrPathObj(ePathKind,aXPP);
/*N*/   ImpConvertSetAttrAndLayer(pPathObj,bNoSetAttr);
/*N*/   return pPathObj;
/*N*/ }

/*N*/ SdrObject* SdrTextObj::ImpConvertAddText(SdrObject* pObj, bool /*bBezier*/) const
/*N*/ {
/*N*/   if (!ImpCanConvTextToCurve()) return pObj;
/*?*/   return pObj;
/*N*/ }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
