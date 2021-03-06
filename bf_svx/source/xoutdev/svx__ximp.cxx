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

#include <algorithm>

#include <stdlib.h>
#include <vcl/virdev.hxx>
#include <vcl/metaact.hxx>

#include <bf_svx/xdef.hxx>

#include "xoutx.hxx"
namespace binfilter {

// ----------------
// - XOuputDevice -
// ----------------

/*N*/ void XOutputDevice::DrawFillPolyPolygon( const PolyPolygon& rPolyPoly, BOOL bRect )
/*N*/ {
/*N*/   if( ( eFillStyle != XFILL_NONE ) && iRotTransGradient() &&
/*N*/       ( eFillStyle != XFILL_SOLID || iRotTransGradient()->GetStartColor() != iRotTransGradient()->GetEndColor() ) )
/*N*/   {
/*?*/       XGradient&      rTransGradient = *iRotTransGradient();
/*?*/       Gradient        aVCLGradient;
/*?*/       GDIMetaFile     aMtf;
/*?*/       VirtualDevice   aVDev;
/*?*/       OutputDevice*   pOldOut = pOut;
/*?*/       const Rectangle aBound( rPolyPoly.GetBoundRect() );
/*?*/       MapMode         aMap( pOldOut->GetMapMode() );
/*?*/
/*?*/       pOut = &aVDev;
/*?*/       aVDev.EnableOutput( FALSE );
/*?*/       aVDev.SetMapMode( pOldOut->GetMapMode() );
/*?*/       aMtf.Record( &aVDev );
/*?*/       aVDev.SetLineColor( pOldOut->GetLineColor() );
/*?*/       aVDev.SetFillColor( pOldOut->GetFillColor() );
/*?*/       aVDev.SetFont( pOldOut->GetFont() );
/*?*/       aVDev.SetDrawMode( pOldOut->GetDrawMode() );
/*?*/       aVDev.SetRefPoint( pOldOut->GetRefPoint() );
/*?*/       ImpDrawFillPolyPolygon( rPolyPoly, bRect );
/*?*/       aMtf.Stop();
/*?*/       aMtf.WindStart();
/*?*/       aMap.SetOrigin( aBound.TopLeft() );
/*?*/       aMtf.SetPrefMapMode( aMap );
/*?*/       aMtf.SetPrefSize( aBound.GetSize() );
/*?*/       pOut = pOldOut;
/*?*/
/*?*/       aVCLGradient.SetStyle((GradientStyle)rTransGradient.GetGradientStyle());
/*?*/       aVCLGradient.SetStartColor(rTransGradient.GetStartColor());
/*?*/       aVCLGradient.SetEndColor(rTransGradient.GetEndColor());
/*?*/       aVCLGradient.SetAngle((USHORT)rTransGradient.GetAngle());
/*?*/       aVCLGradient.SetBorder(rTransGradient.GetBorder());
/*?*/       aVCLGradient.SetOfsX(rTransGradient.GetXOffset());
/*?*/       aVCLGradient.SetOfsY(rTransGradient.GetYOffset());
/*?*/       aVCLGradient.SetStartIntensity(rTransGradient.GetStartIntens());
/*?*/       aVCLGradient.SetEndIntensity(rTransGradient.GetEndIntens());
/*?*/       aVCLGradient.SetSteps(rTransGradient.GetSteps());
/*?*/
/*?*/       pOut->DrawTransparent( aMtf, aBound.TopLeft(), aBound.GetSize(), aVCLGradient );
/*N*/   }
/*N*/   else
/*N*/       ImpDrawFillPolyPolygon( rPolyPoly, bRect );
/*N*/ }

// ------------------------------------------------------------------------

/*N*/ void XOutputDevice::ImpDrawFillPolyPolygon( const PolyPolygon& rPolyPoly, BOOL bRect )
/*N*/ {
/*N*/   if( eFillStyle != XFILL_NONE )
/*N*/   {
/*?*/       const Color aOldLineColor( pOut->GetLineColor() );
/*?*/       const ULONG nDrawMode = pOut->GetDrawMode();
/*?*/
/*?*/       pOut->SetLineColor();
/*?*/
/*?*/       if( eFillStyle == XFILL_SOLID )
/*?*/       {
/*?*/           if( nFillTransparence )
/*?*/               pOut->DrawTransparent( rPolyPoly, nFillTransparence );
/*?*/           else
/*?*/               pOut->DrawPolyPolygon( rPolyPoly );
/*?*/       }
/*?*/       else if( eFillStyle == XFILL_HATCH )
/*?*/       {
/*?*/           long        nAngle10 = aHatch.GetAngle() % 3600;
/*?*/           HatchStyle  eStyle;
/*?*/
/*?*/           if( nAngle10 < 0 )
/*?*/               nAngle10 += 3600;
/*?*/
/*?*/           switch( aHatch.GetHatchStyle() )
/*?*/           {
/*?*/               case( XHATCH_TRIPLE ): eStyle = HATCH_TRIPLE; break;
/*?*/               case( XHATCH_DOUBLE ): eStyle = HATCH_DOUBLE; break;
/*?*/
/*?*/               default:
/*?*/                   eStyle = HATCH_SINGLE;
/*?*/               break;
/*?*/           }
/*?*/
/*?*/           if( iSolidHatch() )
/*?*/               pOut->DrawPolyPolygon( rPolyPoly );
/*?*/
/*?*/           pOut->DrawHatch( rPolyPoly, Hatch( eStyle, aHatch.GetColor(), aHatch.GetDistance(), (USHORT) nAngle10 ) );
/*?*/       }
/*?*/       else if( eFillStyle == XFILL_GRADIENT )
/*?*/       {
/*?*/           /*GDIMetaFile* pMtf =*/ pOut->GetConnectMetaFile();
/*?*/           Gradient        aVCLGradient;
/*?*/
/*?*/
/*?*/           aVCLGradient.SetStyle((GradientStyle)aGradient.GetGradientStyle());
/*?*/           aVCLGradient.SetStartColor(aGradient.GetStartColor());
/*?*/           aVCLGradient.SetEndColor(aGradient.GetEndColor());
/*?*/           aVCLGradient.SetAngle((USHORT)aGradient.GetAngle());
/*?*/           aVCLGradient.SetBorder(aGradient.GetBorder());
/*?*/           aVCLGradient.SetOfsX(aGradient.GetXOffset());
/*?*/           aVCLGradient.SetOfsY(aGradient.GetYOffset());
/*?*/           aVCLGradient.SetStartIntensity(aGradient.GetStartIntens());
/*?*/           aVCLGradient.SetEndIntensity(aGradient.GetEndIntens());
/*?*/           aVCLGradient.SetSteps(aGradient.GetSteps());
/*?*/
/*?*/           if( bRect )
/*?*/               pOut->DrawGradient( rPolyPoly.GetBoundRect(), aVCLGradient );
/*?*/           else
/*?*/               pOut->DrawGradient( rPolyPoly, aVCLGradient );
/*?*/       }
/*?*/       else if( eFillStyle == XFILL_BITMAP )
/*?*/       {
/*?*/           if( nDrawMode & DRAWMODE_WHITEFILL )
/*?*/           {
/*?*/               const Color aOldFillColor( pOut->GetFillColor() );
/*?*/
/*?*/               pOut->SetFillColor( COL_WHITE );
/*?*/               pOut->DrawPolyPolygon( rPolyPoly );
/*?*/               pOut->SetFillColor( aOldFillColor );
/*?*/           }
/*?*/           else
/*?*/           {
/*?*/               /*GDIMetaFile*    pMtf = */ pOut->GetConnectMetaFile();
/*?*/
/*?*/               pOut->Push();
/*?*/               pOut->SetRasterOp( ROP_OVERPAINT );
/*?*/               pOut->Pop();
/*?*/           }
/*?*/       }
/*?*/
/*?*/       pOut->SetLineColor( aOldLineColor );
/*N*/   }
/*N*/ }

// ------------------------------------------------------------------------

} //namespace binfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
