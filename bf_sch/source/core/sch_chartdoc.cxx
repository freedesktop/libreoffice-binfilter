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

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include <bf_svx/eeitem.hxx>

#ifndef _ZFORLIST_HXX
#ifndef _ZFORLIST_DECLARE_TABLE
#define _ZFORLIST_DECLARE_TABLE
#endif
#include <bf_svtools/zforlist.hxx>
#endif



#include <bf_sfx2/printer.hxx>

#include "schattr.hxx"
#ifndef _SVX_CHRTITEM_HXX //autogen
#define ITEMID_DOUBLE           0
#define ITEMID_CHARTDATADESCR   SCHATTR_DATADESCR_DESCR

#include <bf_svtools/eitem.hxx>

#endif
#include <bf_svx/svxids.hrc>
#include "stlpool.hxx"
#include "schiocmp.hxx"
#include "schresid.hxx"
#include "docshell.hxx"

#ifdef ITEMID_FONT
#undef ITEMID_FONT
#endif
#define ITEMID_FONT        EE_CHAR_FONTINFO

#ifdef ITEMID_COLOR
#undef ITEMID_COLOR
#endif
#define ITEMID_COLOR       EE_CHAR_COLOR

#ifdef ITEMID_FONTHEIGHT
#undef ITEMID_FONTHEIGHT
#endif
#define ITEMID_FONTHEIGHT  EE_CHAR_FONTHEIGHT

#include "glob.hrc"
#include "globfunc.hxx"

#include <bf_svx/fontitem.hxx>
#include <bf_svx/fhgtitem.hxx>


#include "chaxis.hxx"

#include "memchrt.hxx"

#include <bf_svtools/itempool.hxx>

namespace binfilter {

/*************************************************************************
|*
|* Diese Methode erzeugt eine neue Seite (SchPage) und gibt einen Zeiger
|* darauf zurueck. Die Drawing Engine benutzt diese Methode beim Laden
|* zur Erzeugung von Seiten (deren Typ sie ja nicht kennt, da es ABLEITUNGEN
|* der SdrPage sind).
|*
\************************************************************************/

/*N*/ SdrPage* ChartModel::AllocPage(bool bMasterPage)
/*N*/ {
/*N*/   return new SdrPage(*this, bMasterPage);
/*N*/ }

/*************************************************************************
|*
|* Inserter fuer SvStream zum Speichern
|*
\************************************************************************/
/*N*/ SvStream& operator << (SvStream& rOut, const ChartModel& rDoc)
/*N*/ {
/*N*/   // die daemlichen blockklammern muessen erhalten bleiben, damit
/*N*/   // im destruktor ~SchIOCompat dei korrekte blockgr��e geschrieben
/*N*/   // wird. ansonsten gibt es beim einlesen mit der 304 jede menge
/*N*/   // aerger
/*N*/   {
/*N*/       SchIOCompat aIO(rOut, STREAM_WRITE, 1);
/*N*/
/*N*/       // AF: Don't write VCItemPool any longer (01/2001)
/*N*/       rOut << (sal_uInt32)0L;
/*N*/
        /**************************************************************************
        * Frueher (StarChart Version 3.0, File-Format Version 1) wurde hier das
        * JobSetup geschrieben, nun der Printer (binaer-kompatibel, daher wurde
        * die Version des File-Formats nicht geaendert)
        **************************************************************************/
/*N*/       if (rDoc.pDocShell)
/*N*/       {
/*N*/           SfxPrinter* pPrinter =
/*N*/                ((SchChartDocShell*)rDoc.pDocShell)->GetPrinter();
/*N*/
/*N*/
/*N*/           if (pPrinter)
/*N*/           {
/*N*/               pPrinter->Store(rOut);
/*N*/           }
/*N*/           else
/*N*/           {
/*?*/               // Es ist kein Printer vorhanden -> muss erzeugt werden
/*?*/               SfxBoolItem aItem(SID_PRINTER_NOTFOUND_WARN, TRUE);
/*?*/               SfxItemSet* pSet = new SfxItemSet(((ChartModel&) rDoc).GetPool(),
/*?*/                               SID_PRINTER_NOTFOUND_WARN,
/*?*/                               SID_PRINTER_NOTFOUND_WARN, 0);
/*?*/               pSet->Put(aItem);
/*?*/               SfxPrinter* pLclPrinter = new SfxPrinter(pSet);
/*?*/
/*?*/               MapMode aMapMode = pLclPrinter->GetMapMode();
/*?*/               aMapMode.SetMapUnit(MAP_100TH_MM);
/*?*/               pLclPrinter->SetMapMode(aMapMode);
/*?*/
/*?*/               pLclPrinter->Store(rOut);
/*?*/               delete pLclPrinter;
/*?*/               //delete pSet; bloss nicht! Killt der Printer gleich mit!
/*N*/           }
/*N*/       }
/*N*/       else
/*N*/       {
/*?*/           // Keine DocShell, daher wird ein JobSetup geschrieben
/*?*/           JobSetup aJobSetup;
/*?*/           rOut << aJobSetup;
/*N*/       }
/*N*/   }
/*N*/
/*N*/
/*N*/   rOut << (SdrModel&) rDoc;
/*N*/
/*N*/   SchIOCompat aIO(rOut, STREAM_WRITE, 0);
/*N*/
/*N*/   rDoc.StoreAttributes(rOut);
/*N*/   //<- ehemals Basisklasse
/*N*/
/*N*/
/*N*/
/*N*/   return rOut;
/*N*/ }

/*************************************************************************
|*
|* Extractor fuer SvStream zum Laden
|*
\************************************************************************/

/*N*/ SvStream& operator >> (SvStream& rIn, ChartModel& rDoc)
/*N*/ {
/*N*/   // auch hier muessen die blockklammern erhalten bleiben,
/*N*/   // um 304-er dokumente sauber lesen zu koennen.
/*N*/   {
/*N*/       SchIOCompat aIO(rIn, STREAM_READ);
/*N*/
/*N*/       sal_uInt32 n;
/*N*/       rIn >> n;
/*N*/       // must be 0 or 1. Otherwise we have an error
/*N*/       // most probably a wrong password
/*N*/       if( n == 1L )
/*N*/       {
/*N*/           // AF: Skip VCItemPool.
/*N*/           rIn.SeekRel (74);
/*N*/       }
/*N*/       else if( n != 0L )
/*N*/       {
/*?*/           rIn.SetError( ERRCODE_IO_GENERAL );
/*?*/           return rIn;
/*N*/       }
/*N*/
/*N*/       if (aIO.GetVersion() >= 1)
/*N*/       {

            /******************************************************************
            * Frueher (StarChart Version 3.0, File-Format Version 1) wurde hier
            * das JobSetup eingelesen, nun wird der Printer erzeugt
            * (binaer-kompatibel)
            *******************************************************************/
            // ItemSet mit speziellem Poolbereich anlegen
/*N*/           SfxItemSet* pSet =
/*N*/               new SfxItemSet(rDoc.GetPool(),
/*N*/                              SID_PRINTER_NOTFOUND_WARN,
/*N*/                              SID_PRINTER_NOTFOUND_WARN, 0);
/*N*/           pSet->Put(SfxBoolItem(SID_PRINTER_NOTFOUND_WARN, TRUE));
/*N*/
/*N*/           SfxPrinter* pPrinter = SfxPrinter::Create(rIn, pSet);
/*N*/
/*N*/           MapMode aMM(pPrinter->GetMapMode());
/*N*/           aMM.SetMapUnit(MAP_100TH_MM);
/*N*/           pPrinter->SetMapMode(aMM);
/*N*/
/*N*/           if (rDoc.pDocShell->GetCreateMode() != SFX_CREATE_MODE_EMBEDDED)
/*N*/           {
/*N*/               ((SchChartDocShell*)rDoc.pDocShell)->SetPrinter( pPrinter, TRUE );  // will be deleted by DocShell
/*N*/           }
/*N*/           else
/*N*/           {
/*?*/               delete pPrinter;
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/
/*N*/   rIn >> (SdrModel&)rDoc;
/*N*/
/*N*/   if (rIn.GetError() != SVSTREAM_FILEFORMAT_ERROR)
/*N*/   {
/*N*/       SchIOCompat aIO(rIn, STREAM_READ);
/*N*/
/*N*/       rDoc.LoadAttributes(rIn);
/*N*/   }
        if( rIn.GetError() != 0 ) //read error
        {
            return rIn;//handle error occured maybe in rDoc.LoadAttributes e.g. if memchart could not be read correctly
        }

/*N*/   // <- Basisaufruf
/*N*/
/*N*/   rDoc.GetItemPool().LoadCompleted();
/*N*/
/*N*/   rDoc.ReadError () = FALSE;
/*N*/
/*N*/   return rIn;
/*N*/ }

/*************************************************************************
|*
|* SetChanged(), das Model wurde geaendert
|*
\************************************************************************/

/*N*/ void ChartModel::SetChanged(bool bFlag)
/*N*/ {
/*N*/   if (pDocShell)
/*N*/   {
/*N*/       if (bNewOrLoadCompleted && pDocShell->IsEnableSetModified())
/*N*/       {
/*N*/           // weitergeben an Basisklasse
/*N*/           //ChartModel
/*N*/           SdrModel::SetChanged(bFlag);
/*N*/
/*N*/           // an ObjectShell weiterleiten
/*N*/           pDocShell->SetModified(bFlag);
/*N*/       }
/*N*/   }
/*N*/   else
/*N*/   {
/*?*/       // weitergeben an Basisklasse
/*?*/       //ChartModel
/*?*/       SdrModel::SetChanged(bFlag);
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|* NewOrLoadCompleted
|*
|* Wird gerufen, wenn das Dokument geladen wurde bzw. feststeht, dass es
|* nicht mehr geladen wird.
|*
\************************************************************************/

/*N*/ void ChartModel::NewOrLoadCompleted(USHORT eMode)
/*N*/ {
/*N*/   if (eMode == NEW_DOC)
/*N*/   {
/*N*/       // StyleSheets generieren
/*N*/       Font aFont( OutputDevice::GetDefaultFont( DEFAULTFONT_SANS, GetLanguage( EE_CHAR_LANGUAGE ), DEFAULTFONT_FLAGS_ONLYONE ) );
/*N*/       SvxFontItem aFontItem( aFont.GetFamily(), aFont.GetName(), aFont.GetStyleName(), aFont.GetPitch(),
/*N*/                              aFont.GetCharSet(), EE_CHAR_FONTINFO );
/*N*/
/*N*/       SfxStyleSheetBase* pSheet =
/*N*/           &pStyleSheetPool->Make(SchResId(STR_STLSHEET_TITLE_MAIN).toString(),
/*N*/                                         SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(493));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_TITLE_SUB).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(423));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_TITLE_X_AXIS).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(352));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_TITLE_Y_AXIS).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(352));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_TITLE_Z_AXIS).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(352));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_DATAROWS).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(282));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_DATACOLS).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(282));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_DATAVALUES).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(282));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_DATADESCR).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO ) ));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(282));
/*N*/
/*N*/       pSheet = &pStyleSheetPool->
/*N*/                   Make(SchResId(STR_STLSHEET_LEGEND).toString(),
/*N*/                        SFX_STYLE_FAMILY_PARA);
/*N*/       pSheet->GetItemSet().Put(aFontItem);
/*N*/       pSheet->GetItemSet().Put(SvxColorItem( Color( COL_AUTO )));
/*N*/       pSheet->GetItemSet().Put(SvxFontHeightItem(282));
/*N*/
/*N*/         pAxisAttr->ClearItem( SCHATTR_AXIS_SHOWDESCR );
/*N*/   }
/*N*/   else if (eMode == DOC_LOADED)
/*N*/   {
/*N*/         // set intersection of all axis attributes used as axis
/*N*/         // 'style' (that is an itemset for the 'all axes' dialog)
/*N*/         GetFullAxisAttr( NULL, true ).ClearInvalidItems();
/*N*/         pAxisAttr->ClearItem( SCHATTR_AXIS_SHOWDESCR );
/*N*/
/*N*/         // set 'all axis' attributes on all axes that are currently
/*N*/         // disabled
/*N*/         if( ! HasAxis( CHOBJID_DIAGRAM_X_AXIS ))
/*?*/             pChartXAxis->SetAttributes( pAxisAttr );
/*N*/         if( ! HasAxis( CHOBJID_DIAGRAM_Y_AXIS ))
/*?*/             pChartYAxis->SetAttributes( pAxisAttr );
/*N*/         if( ! HasAxis( CHOBJID_DIAGRAM_Z_AXIS ))
/*N*/             pChartZAxis->SetAttributes( pAxisAttr );
/*N*/         if( ! HasAxis( CHOBJID_DIAGRAM_A_AXIS ))
/*N*/             pChartAAxis->SetAttributes( pAxisAttr );
/*N*/         if( ! HasAxis( CHOBJID_DIAGRAM_B_AXIS ))
/*N*/             pChartBAxis->SetAttributes( pAxisAttr );
/*N*/         // #99528# change auto-font color according to diagram area
/*N*/         PageColorChanged( *pDiagramAreaAttr );
/*N*/
/*N*/         // #101591# set precision of own number-formatter to 2 if we reside in a
/*N*/         // calc or writer, which is assumed by checking the cell-range in the
/*N*/         // MemChart.
/*N*/         OSL_ASSERT( pChartData );
/*N*/         const SchChartRange & rRange = pChartData->GetChartRange();
/*N*/         if( rRange.maRanges.size() > 0 )
/*N*/         {
/*?*/             pOwnNumFormatter->ChangeStandardPrec( 2 );
/*N*/         }
/*N*/   }
/*N*/
/*N*/   bNewOrLoadCompleted = TRUE;
/*N*/ #ifdef DBG_UTIL
/*N*/   if(pChartData && GetRowCount() && GetColCount())
/*N*/   {
/*N*/       CHART_TRACE1( "ChartModel::NewOrLoadCompleted debugging ChartItems Model %s ",
/*N*/                     (eMode==DOC_LOADED)? "was loaded": ((eMode==NEW_DOC)? "is new" : "don't know") );
/*N*/       DBG_ITEMS((SfxItemSet&)GetDataRowAttr(0),this);
/*N*/       DBG_ITEMS((SfxItemSet&)GetDataPointAttr(0,0),this);
/*N*/   }
/*N*/ #endif
/*N*/ }


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
