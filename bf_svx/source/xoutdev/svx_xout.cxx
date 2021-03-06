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

#include <stdlib.h>
#include <vcl/svapp.hxx>
#include <bf_svtools/itemset.hxx>

#include <bf_svx/xdef.hxx>

#include "xattr.hxx"
#include "xoutx.hxx"

#include "xflbckit.hxx"
#include "xflgrit.hxx"
#include "xfltrit.hxx"
#include "xftadit.hxx"
#include "xftdiit.hxx"
#include "xftmrit.hxx"
#include "xftouit.hxx"
#include "xftsfit.hxx"
#include "xftshcit.hxx"
#include "xftshit.hxx"
#include "xftshtit.hxx"
#include "xftshxy.hxx"
#include "xftstit.hxx"
#include "xgrscit.hxx"
#include "xlntrit.hxx"
#include "xtextit0.hxx"


namespace binfilter {

#define ITEMVALUE(ItemSet,Id,Cast)  ((const Cast&)(ItemSet).Get(Id)).GetValue()
#define MTF_DIVISOR 2

/*************************************************************************
|*
|*    XOutputDevice::XOutputDevice()
|*
*************************************************************************/

/*N*/ XOutputDevice::XOutputDevice(OutputDevice* pOutDev) :
/*N*/   pImpData            ( new ImpData ),
/*N*/   pLinePattern        ( NULL ),
/*N*/   nLinePatternCnt     ( 0 ),
/*N*/   eLineStyle          ( XLINE_SOLID ),
/*N*/   nLineTransparence   ( 0 ),
/*N*/   nLineWidth          ( 0 ),
/*N*/   aLineStartPoly      ( 0 ),
/*N*/   aLineEndPoly        ( 0 ),
/*N*/   nLineStartSqLen     ( 0 ),
/*N*/   nLineEndSqLen       ( 0 ),
/*N*/   bLineStart          ( FALSE ),
/*N*/   bLineEnd            ( FALSE ),
/*N*/   bHair               ( FALSE ),
/*N*/   eFillStyle          ( XFILL_SOLID ),
/*N*/   nFillTransparence   ( 0 ),
/*N*/     mpFillGraphicObject ( new BfGraphicObject ),
/*N*/   mnBmpPerCentX       ( 0 ),
/*N*/   mnBmpPerCentY       ( 0 ),
/*N*/   mnBmpOffX           ( 0 ),
/*N*/   mnBmpOffY           ( 0 ),
/*N*/   mnBmpOffPosX        ( 0 ),
/*N*/   mnBmpOffPosY        ( 0 ),
/*N*/   meLastOutDevType    ( OUTDEV_WINDOW ),
/*N*/   meBmpRectPoint      ( RP_MM ),
/*N*/   mbBmpTile           ( FALSE ),
/*N*/   mbBmpStretch        ( TRUE ),
/*N*/   mbBmpLogSize        ( FALSE ),
/*N*/   mbRecalc            ( TRUE ),
/*N*/   aGradient           ( COL_BLACK, COL_WHITE ),
/*N*/   aHatch              ( COL_BLACK, XHATCH_SINGLE ),
/*N*/   bDither             ( ( pOutDev ? pOutDev : Application::GetDefaultDevice() )->GetColorCount() <= 256 ),
/*N*/   bIgnoreLineAttr     ( FALSE ),
/*N*/   bIgnoreLineStyle    ( FALSE ),
/*N*/   bIgnoreFillAttr     ( FALSE ),
/*N*/   eFormTextStyle      ( XFT_UPRIGHT ),
/*N*/   eFormTextAdjust     ( XFT_AUTOSIZE ),
/*N*/   nFormTextDistance   ( 0 ),
/*N*/   nFormTextStart      ( 0 ),
/*N*/   bFormTextMirror     ( FALSE ),
/*N*/   bFormTextOutline    ( FALSE ),
/*N*/   eFormTextShadow     ( XFTSHADOW_NONE ),
/*N*/   aFormTextShdwColor  ( COL_LIGHTGRAY ),
/*N*/   nFormTextShdwTransp ( 0 ),
/*N*/   nFormTextShdwXVal   ( 0 ),
/*N*/   nFormTextShdwYVal   ( 0 ),
/*N*/   eFormTextStdForm    ( XFTFORM_NONE ),
/*N*/   bFormTextHideForm   ( FALSE ),
/*N*/   nFtAutoHeightSave   ( 0 ),
/*N*/   pOut                ( pOutDev )
/*N*/ {
/*N*/   iRotTransGradient() = NULL;
/*N*/   iSolidHatch() = FALSE;
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::~XOutputDevice()
|*
*************************************************************************/

/*N*/ XOutputDevice::~XOutputDevice()
/*N*/ {
/*N*/   delete[] pLinePattern;
/*N*/     delete mpFillGraphicObject;
/*N*/
/*N*/     delete iRotTransGradient();
/*N*/   delete static_cast< ImpData* >( pImpData );
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::DrawLine()
|*
*************************************************************************/

/*N*/ void XOutputDevice::DrawLine( const Point& rStart, const Point& rEnd )
/*N*/ {
/*N*/   Polygon aPoly(2);
/*N*/
/*N*/   aPoly[0] = rStart;
/*N*/   aPoly[1] = rEnd;
/*N*/   DrawLinePolygon( aPoly, FALSE );
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::DrawXPolyLine()
|*
*************************************************************************/

/*N*/ void XOutputDevice::DrawXPolyLine( const XPolygon& rXPoly )
/*N*/ {
/*N*/   if ( rXPoly.GetPointCount() > 0 )
/*N*/   {
/*N*/       const Polygon aPoly( XOutCreatePolygon(rXPoly, pOut) );
/*N*/       DrawLinePolygon(aPoly, FALSE);
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::DrawRect()
|*
*************************************************************************/

/*N*/ void XOutputDevice::DrawRect( const Rectangle& rRect, ULONG nXRound,
/*N*/                             ULONG nYRound )
/*N*/ {
/*N*/   const Polygon aPoly( rRect, nXRound, nYRound );
/*N*/
/*N*/   DrawFillPolyPolygon( aPoly, TRUE );
/*N*/   DrawLinePolygon( aPoly, TRUE );
/*N*/ }


/*************************************************************************
|*
|*    XOutputDevice::DrawEllipse()
|*
*************************************************************************/

/*N*/ void XOutputDevice::DrawEllipse( const Rectangle& rRect )
/*N*/ {
/*N*/   const Polygon aPoly( rRect.Center(), rRect.GetWidth() >> 1, rRect.GetHeight() >> 1 );
/*N*/
/*N*/   DrawFillPolyPolygon( aPoly );
/*N*/   DrawLinePolygon( aPoly, TRUE );
/*N*/ }


/*************************************************************************
|*
|*    XOutputDevice::DrawArc()
|*
*************************************************************************/

/*N*/ void XOutputDevice::DrawArc( const Rectangle& rRect, const Point& rStart,
/*N*/                            const Point& rEnd )
/*N*/ {
/*N*/   const Polygon aPoly( rRect, rStart, rEnd, POLY_ARC );
/*N*/
/*N*/   DrawFillPolyPolygon( aPoly );
/*N*/   DrawLinePolygon( aPoly, TRUE );
/*N*/ }


/*************************************************************************
|*
|*    XOutputDevice::DrawPie()
|*
*************************************************************************/

/*N*/ void XOutputDevice::DrawPie( const Rectangle& rRect, const Point& rStart,
/*N*/                            const Point& rEnd )
/*N*/ {
/*N*/   const Polygon aPoly( rRect, rStart, rEnd, POLY_PIE );
/*N*/
/*N*/   DrawFillPolyPolygon( aPoly );
/*N*/   DrawLinePolygon( aPoly, TRUE );
/*N*/ }


/*************************************************************************
|*
|*    XOutputDevice::DrawXPolygon()
|*
|*    Beschreibung      Polygon mit Linien- und Fuellstilen zeichnen
|*
\************************************************************************/

/*N*/ void XOutputDevice::DrawXPolygon(const XPolygon& rXPoly)
/*N*/ {
/*N*/   if ( rXPoly.GetPointCount() > 0 )
/*N*/   {
/*N*/       const Polygon aPoly( XOutCreatePolygon(rXPoly, pOut) );
/*N*/
/*N*/       DrawFillPolyPolygon( aPoly );
/*N*/       DrawLinePolygon(aPoly, TRUE);
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::DrawXPolyPolygon()
|*
*************************************************************************/

/*N*/ void XOutputDevice::DrawXPolyPolygon( const XPolyPolygon& rXPolyPoly )
/*N*/ {
/*N*/   PolyPolygon aPolyPoly;
/*N*/
/*N*/   USHORT  nCount = rXPolyPoly.Count(), i;
/*N*/
/*N*/   for( i = 0; i < nCount; i++ )
/*N*/   {
/*N*/       if( rXPolyPoly[i].GetPointCount() > 0 )
/*N*/         {
/*N*/           aPolyPoly.Insert(XOutCreatePolygon(rXPolyPoly[i], pOut));
/*N*/         }
/*N*/   }
/*N*/
/*N*/   DrawFillPolyPolygon( aPolyPoly );
/*N*/
/*N*/   if( eLineStyle != XLINE_NONE )
/*N*/   {
/*N*/       nCount = aPolyPoly.Count();
/*N*/
/*N*/       for (i = 0; i < nCount; i++ )
/*N*/           DrawLinePolygon(aPolyPoly.GetObject(i), TRUE);
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::InitLineStartEnd()
|*
|*    Polygon fuer Linienanfang bzw. -ende initialisieren; gibt die
|*    Entfernung vom Polygon-Fixpunkt zum Linienansatz zurueck
|*
\************************************************************************/

/*N*/ long XOutputDevice::InitLineStartEnd(XPolygon& rXPoly, long nNewWidth,
/*N*/                                    BOOL bCenter)
/*N*/ {
/*N*/   Point           aTranslation;
/*N*/   const Rectangle aRect(rXPoly.GetBoundRect());
/*N*/   const long      nWidth = Max( (long)(aRect.GetWidth() - 1), 1L );
/*N*/
/*N*/   if ( bCenter )
/*N*/       aTranslation = aRect.Center();
/*N*/   else
/*N*/       aTranslation = aRect.TopCenter();
/*N*/
/*N*/   // Punkte verschieben und Skalieren
/*N*/   for (USHORT i = 0; i < rXPoly.GetPointCount(); i++)
/*N*/       rXPoly[i] = (rXPoly[i] - aTranslation) * nNewWidth / nWidth;
/*N*/
/*N*/   long nHeight = aRect.GetHeight() * nNewWidth / nWidth;
/*N*/
/*N*/   if ( bCenter )
/*N*/       nHeight >>= 1;
/*N*/
/*N*/   return nHeight;
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::SetLineAttr()
|*
|*    Linienattribute aus ItemSet auslesen und am OutputDevice setzen
|*
\************************************************************************/

/*N*/ void XOutputDevice::SetLineAttr(const SfxItemSet& rSet)
/*N*/ {
/*N*/   const BOOL bPureMtf = ( pOut->GetOutDevType() != OUTDEV_PRINTER ) && ( pOut->GetConnectMetaFile() != NULL );
/*N*/
/*N*/   aLineColor = ITEMVALUE( rSet, XATTR_LINECOLOR, XLineColorItem );
/*N*/   nLineTransparence = ITEMVALUE( rSet, XATTR_LINETRANSPARENCE, XLineTransparenceItem );
/*N*/
/*N*/   delete[] pLinePattern;
/*N*/   pLinePattern = NULL;
/*N*/   bLineStart = FALSE;
/*N*/   bLineEnd = FALSE;
/*N*/   bHair = TRUE;
/*N*/   nLineWidth = ITEMVALUE( rSet, XATTR_LINEWIDTH, XLineWidthItem );
/*N*/
/*N*/   if( bIgnoreLineAttr )
/*N*/   {
/*N*/       // fuer Linien mit Staerke Null wird im "Nur Haarlinienmodus" ein Hellgrau-Pen benutzt
/*N*/       if( nLineWidth == 0 )
/*N*/           pOut->SetLineColor( COL_LIGHTGRAY );
/*N*/       else
/*N*/       {
/*N*/           nLineWidth = 0;
/*N*/           pOut->SetLineColor( aLineColor );
/*N*/       }
/*N*/   }
/*N*/   else
/*N*/   {
/*N*/       long nMinLineWidth = Max( nLineWidth, pOut->PixelToLogic( Size( 2, 2 ) ) .Width() );
/*N*/
/*N*/       eLineStyle = ITEMVALUE( rSet, XATTR_LINESTYLE, XLineStyleItem );
/*N*/
/*N*/       // Falls wir Linienstil ignorieren, setzen wir Linienstil auf Solid
/*N*/       if ( bIgnoreLineStyle )
/*N*/       {
/*N*/           eLineStyle = XLINE_SOLID;
/*N*/           pOut->SetLineColor( COL_BLACK );
/*N*/       }
/*N*/
/*N*/       // bei reinen Metafiles auch sehr duenne Linien als Polygone ausgeben,
/*N*/       // damit wir hier eine hoehere Genauigkeit wegen der Systemunabhaegigkeit erreichen
/*N*/       // SB: aber nicht, wenn gedruckt wird (dabei wird immer ein Metafile aufgezeichnet)
/*N*/       if ( bPureMtf )
/*N*/           nMinLineWidth /= MTF_DIVISOR;
/*N*/
/*N*/       if ( eLineStyle == XLINE_DASH )
/*N*/       {
/*N*/           USHORT  i, nCnt, nDotCnt;
/*N*/           long    nDotLen, nDashLen, nDistance;
/*N*/           long    nMinLength = nMinLineWidth;
/*N*/
/*N*/           const XDash& rDash = ITEMVALUE( rSet, XATTR_LINEDASH, XLineDashItem );
/*N*/
/*N*/           // Sonderbehandlung fuer Haarlinien
/*N*/           if ( nLineWidth < nMinLineWidth )
/*N*/               nMinLength = 30;
/*N*/
/*N*/           nDotCnt = rDash.GetDots() << 1;
/*N*/           nLinePatternCnt = nDotCnt + ( rDash.GetDashes() << 1 ) + 1;
/*N*/           pLinePattern = new long[nLinePatternCnt];
/*N*/           nCnt = 0;
/*N*/           nDotLen = rDash.GetDotLen();
/*N*/           nDashLen = rDash.GetDashLen();
/*N*/           nDistance = rDash.GetDistance();
/*N*/
/*N*/           if ( rDash.GetDashStyle() == XDASH_RECTRELATIVE || rDash.GetDashStyle() == XDASH_ROUNDRELATIVE )
/*N*/           {
/*N*/               // Der Teilungsfaktor ist abhaengig davon,
/*N*/               // ob wir uns in einer reinen Mtf-Aufzeichnung
/*N*/               // befinden oder nicht. Wenn ja, muessen
/*N*/               // wir dieses bei der Laengenberechn. beruecks.,
/*N*/               // um gleiche Ergebnisse zu bekommen; die hoehere
/*N*/               // Genauigkeit im Mtf bezieht sich nur auf die Breite
/*N*/               const long nFactor = nMinLength * ( bPureMtf ? MTF_DIVISOR : 1 );
/*N*/
/*N*/               nDotLen = nDotLen * nFactor / 100;
/*N*/               nDashLen = nDashLen * nFactor / 100;
/*N*/               nDistance = nDistance * nFactor / 100;
/*N*/           }
/*N*/
/*N*/           if ( nDotLen == 0 )
/*N*/               nDotLen = nMinLength;
/*N*/           if ( nDashLen == 0 )
/*N*/               nDashLen = nMinLength;
/*N*/
/*N*/           for (i = 0; i < nDotCnt; i += 2)
/*N*/           {
/*N*/               if ( nDotLen )
/*N*/               {
/*N*/                   pLinePattern[nCnt++] = nDotLen;
/*N*/                   pLinePattern[nCnt++] = nDistance;
/*N*/               }
/*N*/           }
/*N*/           for ( ; i < nLinePatternCnt - 1; i += 2)
/*N*/           {
/*N*/               if ( nDashLen )
/*N*/               {
/*N*/                   pLinePattern[nCnt++] = nDashLen;
/*N*/                   pLinePattern[nCnt++] = nDistance;
/*N*/               }
/*N*/           }
/*N*/           if ( !nCnt )
/*N*/           {
/*N*/               eLineStyle = XLINE_SOLID;
/*N*/               delete pLinePattern;
/*N*/               pLinePattern = NULL;
/*N*/           }
/*N*/           else
/*N*/               pLinePattern[nCnt] = 0;
/*N*/       }
/*N*/
/*N*/
/*N*/       if ( nLineWidth < nMinLineWidth && eLineStyle == XLINE_SOLID )
/*N*/       {
/*N*/           nLineWidth = 0;
/*N*/           pOut->SetLineColor( aLineColor );
/*N*/       }
/*N*/       else
/*N*/       {
/*N*/           bHair = FALSE;
/*N*/           pOut->SetLineColor();
/*N*/
/*N*/           if( nLineWidth < nMinLineWidth )
/*N*/               nLineWidth = 0;
/*N*/       }
/*N*/       const SfxPoolItem* pPoolItem;
/*N*/
/*N*/       // Polygon und Daten fuer Linienanfang und -ende initialisieren
/*N*/       if ( rSet.GetItemState(XATTR_LINESTART, TRUE, &pPoolItem) == SFX_ITEM_SET )
/*N*/       {
/*N*/           long nWidth = ITEMVALUE( rSet, XATTR_LINESTARTWIDTH, XLineStartWidthItem );
/*N*/
/*N*/           aLineStartPoly = ((XLineStartItem*) pPoolItem)->GetValue();
/*N*/
/*N*/           // Nur aktivieren, wenn Polygonbreite > 0
/*N*/           if ( nWidth )
/*N*/           {
/*N*/               // Breite negativ: Prozentwert
/*N*/               if ( nWidth < 0 )
/*N*/               {
/*N*/                   nWidth = - nMinLineWidth * nWidth / 100;
/*N*/                   if ( nWidth == 0 ) nWidth = nMinLineWidth;
/*N*/               }
/*N*/               BOOL bCenter = ITEMVALUE( rSet, XATTR_LINESTARTCENTER, XLineStartCenterItem );
/*N*/               nLineStartSqLen = InitLineStartEnd(aLineStartPoly, nWidth, bCenter);
/*N*/               // 4/5, um Ueberschneidung zu Gewaehrleisten
/*N*/               nLineStartSqLen = nLineStartSqLen * 4 / 5;
/*N*/               nLineStartSqLen = nLineStartSqLen * nLineStartSqLen;
/*N*/               bLineStart = TRUE;
/*N*/           }
/*N*/       }
/*N*/       if ( rSet.GetItemState(XATTR_LINEEND, TRUE, &pPoolItem) == SFX_ITEM_SET )
/*N*/       {
/*N*/           long nWidth = ITEMVALUE( rSet, XATTR_LINEENDWIDTH, XLineEndWidthItem );
/*N*/
/*N*/           aLineEndPoly = ((XLineStartItem*) pPoolItem)->GetValue();
/*N*/
/*N*/           if ( nWidth )
/*N*/           {
/*N*/               // Breite negativ: Prozentwert
/*N*/               if ( nWidth < 0 )
/*N*/               {
/*N*/                   nWidth = - nMinLineWidth * nWidth / 100;
/*N*/                   if ( nWidth == 0 ) nWidth = nMinLineWidth;
/*N*/               }
/*N*/               BOOL bCenter = ITEMVALUE( rSet, XATTR_LINEENDCENTER, XLineEndCenterItem );
/*N*/               nLineEndSqLen = InitLineStartEnd(aLineEndPoly, nWidth, bCenter);
/*N*/               nLineEndSqLen = nLineEndSqLen * 4 / 5;
/*N*/               nLineEndSqLen = nLineEndSqLen * nLineEndSqLen;
/*N*/               bLineEnd = TRUE;
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::SetFillAttr()
|*
|*    Fuellattribute aus ItemSet auslesen und am OutputDevice setzen
|*
\************************************************************************/

/*N*/ void XOutputDevice::SetFillAttr(const SfxItemSet& rSet)
/*N*/ {
/*N*/   const XFillFloatTransparenceItem&   rFloatTransItem =  (const XFillFloatTransparenceItem&) rSet.Get( XATTR_FILLFLOATTRANSPARENCE );
/*N*/
/*N*/   eFillStyle = bIgnoreFillAttr ? XFILL_NONE : ITEMVALUE( rSet, XATTR_FILLSTYLE, XFillStyleItem );
/*N*/   nFillTransparence = ITEMVALUE( rSet, XATTR_FILLTRANSPARENCE, XFillTransparenceItem );
/*N*/
/*N*/   // clear bitmap
/*N*/   if( eFillStyle != XFILL_BITMAP )
/*N*/   {
/*N*/         maFillBitmap.SetEmpty();
/*N*/       maFillBitmapSize.Width() = maFillBitmapSize.Height() = 0L;
/*N*/   }
/*N*/
/*N*/   // assign transparent gradient?
/*N*/   if( ( eFillStyle != XFILL_NONE ) && ( rFloatTransItem.IsEnabled() || nFillTransparence ) )
/*N*/   {
/*N*/       XGradient aXGrad;
/*N*/
/*N*/       if( rFloatTransItem.IsEnabled() )
/*N*/           aXGrad = rFloatTransItem.GetValue();
/*N*/       else
/*N*/       {
/*N*/           const BYTE  cTrans = (BYTE) ( nFillTransparence * 255 / 100 );
/*N*/           const Color aTransCol( cTrans, cTrans, cTrans );
/*N*/
/*N*/           aXGrad.SetGradientStyle( XGRAD_LINEAR );
/*N*/           aXGrad.SetStartColor( aTransCol );
/*N*/           aXGrad.SetEndColor( aTransCol );
/*N*/           aXGrad.SetAngle( 0 );
/*N*/           aXGrad.SetBorder( 0 );
/*N*/           aXGrad.SetXOffset( 0 );
/*N*/           aXGrad.SetYOffset( 0 );
/*N*/           aXGrad.SetStartIntens( 100 );
/*N*/           aXGrad.SetEndIntens( 100 );
/*N*/           aXGrad.SetSteps( 3 );
/*N*/       }
/*N*/
/*N*/       if( iRotTransGradient() )
/*N*/           *iRotTransGradient() = aXGrad;
/*N*/       else
/*N*/           iRotTransGradient() = new XGradient( aXGrad );
/*N*/   }
/*N*/   else if( iRotTransGradient() )
/*N*/   {
/*N*/       delete iRotTransGradient();
/*N*/       iRotTransGradient() = NULL;
/*N*/   }
/*N*/
/*N*/   if( eFillStyle != XFILL_NONE )
/*N*/   {
/*N*/       pOut->SetFillColor( aFillColor = ITEMVALUE( rSet, XATTR_FILLCOLOR, XFillColorItem ) );
/*N*/
/*N*/       if( eFillStyle == XFILL_BITMAP )
/*N*/       {
/*N*/           MapMode     aMapMode( pOut->GetMapMode() );
/*N*/           Bitmap      aBmp( ITEMVALUE( rSet, XATTR_FILLBITMAP, XFillBitmapItem ).GetBitmap() );
/*N*/           USHORT      nOffX = ITEMVALUE( rSet, XATTR_FILLBMP_TILEOFFSETX, SfxUInt16Item );
/*N*/           USHORT      nOffY = ITEMVALUE( rSet, XATTR_FILLBMP_TILEOFFSETY, SfxUInt16Item );
/*N*/           USHORT      nOffPosX = ITEMVALUE( rSet, XATTR_FILLBMP_POSOFFSETX, SfxUInt16Item );
/*N*/           USHORT      nOffPosY = ITEMVALUE( rSet, XATTR_FILLBMP_POSOFFSETY, SfxUInt16Item );
/*N*/           RECT_POINT  eRectPoint = (RECT_POINT) ITEMVALUE( rSet, XATTR_FILLBMP_POS, SfxEnumItem );
/*N*/           BOOL        bTile = ITEMVALUE( rSet, XATTR_FILLBMP_TILE, SfxBoolItem );
/*N*/           BOOL        bStretch = ITEMVALUE( rSet, XATTR_FILLBMP_STRETCH, SfxBoolItem );
/*N*/           BOOL        bLogSize = ITEMVALUE( rSet, XATTR_FILLBMP_SIZELOG, SfxBoolItem );
/*N*/           Size        aSize( labs( ITEMVALUE( rSet, XATTR_FILLBMP_SIZEX, SfxMetricItem ) ),
/*N*/                                labs( ITEMVALUE( rSet, XATTR_FILLBMP_SIZEY, SfxMetricItem ) ) );
/*N*/
/*N*/           if( !mbRecalc && !pOut->GetConnectMetaFile() &&
/*N*/                 ( aBmp == maFillBitmap ) &&
/*N*/                 ( pOut->GetOutDevType() == meLastOutDevType ) &&
/*N*/               ( maLastMapMode.GetMapUnit() == aMapMode.GetMapUnit() ) &&
/*N*/               ( maLastMapMode.GetScaleX() == aMapMode.GetScaleX() ) &&
/*N*/               ( maLastMapMode.GetScaleY() == aMapMode.GetScaleY() ) &&
/*N*/               ( bTile == mbBmpTile ) &&
/*N*/               ( bStretch == mbBmpStretch ) &&
/*N*/               ( bLogSize == mbBmpLogSize ) &&
/*N*/               ( aSize == maBmpSize ) &&
/*N*/               ( eRectPoint == meBmpRectPoint ) &&
/*N*/               ( nOffX == mnBmpOffX ) &&
/*N*/               ( nOffY == mnBmpOffY ) &&
/*N*/               ( nOffPosX == mnBmpOffPosX ) &&
/*N*/               ( nOffPosY == mnBmpOffPosY ) )
/*N*/           {
/*N*/               return;
/*N*/           }
/*N*/           else
/*N*/           {
/*N*/               maFillBitmap = aBmp;
/*N*/               maLastMapMode = aMapMode;
/*N*/               meLastOutDevType = pOut->GetOutDevType();
/*N*/
/*N*/               mbBmpTile = bTile;
/*N*/               mbBmpStretch = bStretch;
/*N*/               mbBmpLogSize = bLogSize;
/*N*/               mnBmpOffX = nOffX;
/*N*/               mnBmpOffY = nOffY;
/*N*/               meBmpRectPoint = eRectPoint;
/*N*/               mnBmpOffPosX = nOffPosX;
/*N*/               mnBmpOffPosY = nOffPosY;
/*N*/
/*N*/               if( bLogSize )
/*N*/                   maBmpSize = aSize;
/*N*/               else
/*N*/               {
/*N*/                   mnBmpPerCentX = (USHORT) aSize.Width();
/*N*/                   mnBmpPerCentY = (USHORT) aSize.Height();
/*N*/               }
/*N*/
/*N*/               mbRecalc = TRUE;
/*N*/           }
/*N*/       }
/*N*/       else if (eFillStyle == XFILL_GRADIENT)
/*N*/       {
/*N*/           aGradient = ITEMVALUE( rSet, XATTR_FILLGRADIENT, XFillGradientItem );
/*N*/           aGradient.SetSteps( ITEMVALUE( rSet, XATTR_GRADIENTSTEPCOUNT, XGradientStepCountItem ) );
/*N*/       }
/*N*/       else if( eFillStyle == XFILL_HATCH )
/*N*/       {
/*N*/           iSolidHatch() = ITEMVALUE( rSet, XATTR_FILLBACKGROUND, XFillBackgroundItem );
/*N*/           aHatch = ITEMVALUE( rSet, XATTR_FILLHATCH, XFillHatchItem );
/*N*/       }
/*N*/   }
/*N*/   else
/*N*/       pOut->SetFillColor();
/*N*/ }


/*************************************************************************
|*
|*    XOutputDevice::SetTextAttr()
|*
|*    Textattribute aus ItemSet auslesen und am OutputDevice setzen
|*
\************************************************************************/

/*N*/ void XOutputDevice::SetTextAttr(const SfxItemSet& rSet)
/*N*/ {
/*N*/   eFormTextStyle = ITEMVALUE( rSet, XATTR_FORMTXTSTYLE, XFormTextStyleItem );
/*N*/   eFormTextAdjust = ITEMVALUE( rSet, XATTR_FORMTXTADJUST, XFormTextAdjustItem );
/*N*/   nFormTextDistance = ITEMVALUE( rSet, XATTR_FORMTXTDISTANCE, XFormTextDistanceItem );
/*N*/   nFormTextStart = ITEMVALUE( rSet, XATTR_FORMTXTSTART, XFormTextStartItem );
/*N*/   bFormTextMirror = ITEMVALUE( rSet, XATTR_FORMTXTMIRROR, XFormTextMirrorItem );
/*N*/
/*N*/   bFormTextOutline = ITEMVALUE( rSet, XATTR_FORMTXTOUTLINE, XFormTextOutlineItem );
/*N*/   eFormTextShadow = ITEMVALUE( rSet, XATTR_FORMTXTSHADOW, XFormTextShadowItem );
/*N*/   aFormTextShdwColor = ITEMVALUE( rSet, XATTR_FORMTXTSHDWCOLOR, XFormTextShadowColorItem );
/*N*/
/*N*/   nFormTextShdwTransp = ITEMVALUE( rSet, XATTR_FORMTXTSHDWTRANSP, XFormTextShadowTranspItem );
/*N*/   nFormTextShdwXVal = ITEMVALUE( rSet, XATTR_FORMTXTSHDWXVAL, XFormTextShadowXValItem );
/*N*/   nFormTextShdwYVal = ITEMVALUE( rSet, XATTR_FORMTXTSHDWYVAL, XFormTextShadowYValItem );
/*N*/   eFormTextStdForm = ITEMVALUE( rSet, XATTR_FORMTXTSTDFORM, XFormTextStdFormItem );
/*N*/   bFormTextHideForm = ITEMVALUE( rSet, XATTR_FORMTXTHIDEFORM, XFormTextHideFormItem );
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice:OverrideLineColor()
|*
|*    StarView-Linecolor temporaer ueberschreiben, wenn SetLineAttr zu
|*    zeitkritisch ist. !ACHTUNG! Vor weiteren XOut-Ausgaben muessen
|*    die Attribute mit SetLineAttr neu gesetzt werden
|*
\************************************************************************/

/*N*/ void XOutputDevice::OverrideLineColor(const Color& rColor )
/*N*/ {
/*N*/   nLineWidth = 0;
/*N*/   bHair = TRUE;
/*N*/   bLineStart = FALSE;
/*N*/   bLineEnd = FALSE;
/*N*/   eLineStyle = ( rColor.GetColor() == COL_TRANSPARENT ) ? XLINE_NONE : XLINE_SOLID;
/*N*/   pOut->SetLineColor( rColor );
/*N*/ }

/*************************************************************************
|*
|*    XOutputDevice::OverrideFillColor()
|*
|*    StarView-Brush temporaer ueberschreiben, wenn SetFillAttr zu
|*    zeitkritisch ist. !ACHTUNG! Vor weiteren XOut-Ausgaben muessen
|*    die Attribute mit SetFillAttr neu gesetzt werden
|*
\************************************************************************/

/*N*/ void XOutputDevice::OverrideFillColor( const Color& rColor )
/*N*/ {
/*N*/   eFillStyle = ( rColor.GetColor() == COL_TRANSPARENT ) ? XFILL_NONE : XFILL_SOLID;
/*N*/   pOut->SetFillColor( rColor );
/*N*/ }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
