/*************************************************************************
 *
 *  $RCSfile: sw_sw3io.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: mwu $ $Date: 2003-11-20 04:58:55 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/


#pragma hdrstop
#include <sot/object.hxx>
#include <stdio.h>
#define _SVSTDARR_USHORTS
#include <svtools/svstdarr.hxx>
#ifndef _SVSTOR_HXX //autogen
#include <so3/svstor.hxx>
#endif
#ifndef _ZFORLIST_HXX //autogen
#include <svtools/zforlist.hxx>
#endif

#include "doc.hxx"
#include "pam.hxx"
#include "rootfrm.hxx"
#include "swerror.h"
#include "sw3io.hxx"
#include "sw3imp.hxx"
#include "ndgrf.hxx"
namespace binfilter {


/*N*/ Sw3Io::Sw3Io( SwDoc& r )
/*N*/ {
/*N*/   pImp = new Sw3IoImp( *this );
/*N*/   pImp->pDoc = &r;
/*N*/   r.AddLink();
/*N*/ }


/*N*/ Sw3Io::~Sw3Io()
/*N*/ {
/*N*/   delete pImp;
/*N*/ }


/*N*/ ULONG Sw3Io::Load( SvStorage* pStor, SwPaM* pPaM )
/*N*/ {
/*N*/   // Wenn ein PaM angegeben ist, wird eingefuegt!
/*N*/   // Beim Einfuegen werden vorhandene Vorlagen nicht uebergeplaettet
/*N*/   pImp->pOldRoot = pImp->pRoot;
/*N*/   pImp->pRoot = pStor;
/*N*/   BOOL bGood = pImp->OpenStreams( FALSE );
/*N*/   // Wenn die Streams nicht alle da sind, ist es KEIN FEHLER,
/*N*/   // da ein OLE-Container u.U. nicht voll ist.
/*N*/   if( !bGood )
/*N*/   {
/*?*/       pImp->pRoot = pImp->pOldRoot;
/*?*/       pImp->pOldRoot.Clear();
/*?*/       return pImp->nRes;
/*N*/   }
/*N*/   else
/*N*/   {
/*N*/       if ( pPaM )
/*N*/       {
/*?*/           pImp->bInsert = TRUE;
/*?*/           pImp->bInsIntoHdrFtr = pImp->pDoc->IsInHeaderFooter(
/*N*/                                                   pPaM->GetPoint()->nNode );
/*N*/       }
/*N*/       else
/*N*/           pImp->bInsert = pImp->bInsIntoHdrFtr = FALSE;
/*N*/
/*N*/       if( pImp->bNormal )                 // nur setzen wenn normal gelesen wird,
/*N*/           pImp->bAdditive = pImp->bInsert;// sonst ist es schon gueltig
/*N*/
/*N*/       pImp->OutputMode( FALSE );
/*N*/       // Man nehme die Doc-Groesse
/*N*/       pImp->pContents->Seek( STREAM_SEEK_TO_END );
/*N*/       ULONG nSize = pImp->pContents->Tell();
/*N*/       pImp->pContents->Seek( 0L );
/*N*/       pImp->OpenPercentBar( 0L, nSize );
/*N*/       if( pImp->bNormal )
/*N*/           pImp->LoadDrawingLayer();
/*N*/       if( ( pImp->bNormal || pImp->bTxtColls || pImp->bCharFmts || pImp->bFrmFmts ) && !pImp->nRes )
/*N*/           pImp->LoadStyleSheets( BOOL( !pImp->bAdditive ) );
/*N*/       if( ( pImp->bNormal || pImp->bNumRules || pImp->bPageDescs ) && !pImp->nRes )
/*N*/           pImp->LoadNumRules();
/*N*/       if( ( pImp->bNormal || pImp->bPageDescs ) && !pImp->nRes )
/*N*/           pImp->LoadPageStyles();
/*N*/       if( pImp->bNormal && !pImp->nRes )
/*N*/           pImp->LoadContents( pPaM );
/*N*/       pImp->ClosePercentBar();
/*N*/
/*N*/       //Benachrichtigungen fuer OLE-Objekte
/*N*/       if ( pImp->bInsert ) //Eingefuege wurden markiert
/*?*/           pImp->pDoc->PrtOLENotify( FALSE );
/*N*/       else if ( pImp->pDoc->IsOLEPrtNotifyPending() ||//Drucker geaendert
/*N*/                 pImp->nVersion < SWG_OLEPRTNOTIFY )   //Ruecksicht auf Math, dass
/*N*/                                                       //sowieso Hautpanwender ist.
/*N*/       {
/*N*/           pImp->pDoc->PrtOLENotify( TRUE );
/*N*/       }
/*N*/
/*N*/       // unbenutzte OLE-Objekte l�schen, falls ein 3.0-Dok
/*N*/       // geladen wird
/*N*/       if( pImp->nVersion<=SWG_SHORTFIELDS &&
/*N*/           pImp->bNormal && !pImp->bBlock && !pImp->bInsert && !pImp->nRes  )
/*N*/           pImp->RemoveUnusedObjects();
/*N*/
/*N*/       if( pImp->nRes )
/*?*/           pImp->nRes |= ERRCODE_CLASS_READ;
/*N*/       else if( pImp->bNoDrawings )
/*?*/           pImp->nRes = WARN_SWG_NO_DRAWINGS;
/*N*/       else if( pImp->nWarn )
/*?*/           pImp->nRes = pImp->nWarn | ERRCODE_CLASS_READ;
/*N*/
/*N*/       pImp->CloseStreams();
/*N*/       if( pPaM || pImp->bBlock )
/*?*/           pImp->pRoot = pImp->pOldRoot, pImp->pOldRoot.Clear();
/*N*/
/*N*/       SvNumberFormatter* pN;
/*N*/       if( pImp->bInsert &&
/*N*/           0 != ( pN = pImp->pDoc->GetNumberFormatter( FALSE ) ))
/*?*/           pN->ClearMergeTable();
/*N*/   }
/*N*/   return pImp->nRes;
/*N*/ }


/*N*/ ULONG Sw3Io::Save( SwPaM* pPaM, BOOL bSaveAll )
/*N*/ {
/*N*/   if( !pImp->pRoot.Is() )
/*?*/       pImp->pRoot = pImp->pDoc->GetPersist()->GetStorage();
/*N*/
/*N*/   if( pImp->bNormal && pImp->IsSw31Or40Export() &&
/*N*/       pImp->pDoc->GetNodes().GetEndOfContent().GetIndex() > 65200 )
/*N*/   {
/*N*/       // Das Dokument ist zu gross, um vom SW3.1/4.0 gelesen zu werden.
/*?*/       return ERR_SWG_LARGE_DOC_ERROR;
/*N*/   }
/*N*/
/*N*/   ULONG nHiddenDraws = ULONG_MAX;
/*N*/   if( pImp->bNormal )
/*N*/   {
/*N*/       pImp->InsertHiddenDrawObjs();
/*N*/       nHiddenDraws = pImp->nHiddenDrawObjs;
/*N*/   }
/*N*/   BOOL bGood = pImp->OpenStreams( TRUE );
/*N*/   ASSERT( bGood, "Es fehlen leider ein paar Streams!" );
/*N*/   pImp->nHiddenDrawObjs = nHiddenDraws; // OpenStreams loescht den Member!
/*N*/   if( !bGood )
/*N*/   {
/*?*/           pImp->RemoveHiddenDrawObjs();
/*?*/       return ERR_SWG_WRITE_ERROR;
/*N*/   }
/*N*/
/*N*/   // Bookmarks sammeln: Wenn kein Inhalt geschrieben wird, nur die
/*N*/   // in Seitenvorlagen
/*N*/   if( pImp->bNormal || pImp->bPageDescs )
/*N*/   {
/*N*/       pImp->CollectMarks( pPaM, !pImp->bNormal );
/*N*/       if( !pImp->IsSw31Or40Export() )
/*N*/           pImp->CollectRedlines( pPaM, !pImp->bNormal );
/*N*/       else
/*N*/           pImp->CollectTblLineBoxFmts40();
/*N*/   }
/*N*/
/*N*/
/*N*/   pImp->bSaveAll = bSaveAll;
/*N*/   BOOL bNewPaM = BOOL( pPaM == NULL );
/*N*/   if( bNewPaM )
/*N*/   {
/*?*/       pImp->bSaveAll = TRUE;
/*?*/       pPaM = new SwPaM( pImp->pDoc->GetNodes().GetEndOfContent() );
/*?*/       pPaM->Move( fnMoveForward, fnGoDoc );
/*?*/       pPaM->SetMark();
/*?*/       pPaM->Move( fnMoveBackward, fnGoDoc );
/*N*/   }
/*N*/   // Den Doc-Hauptbereich als Mass der Dinge beim Speichern nehmen
/*N*/   ULONG n1 = pImp->pDoc->GetNodes().GetEndOfExtras().GetIndex();
/*N*/   ULONG n2 = pImp->pDoc->GetNodes().GetEndOfContent().GetIndex();
/*N*/   USHORT nPages = pImp->pDoc->GetRootFrm() ?
/*N*/                   pImp->pDoc->GetRootFrm()->GetPageNum() : 0;
/*N*/   // Wir nehmen einfach 10 Nodes/Page an
/*N*/   n2 += nPages * 10;
/*N*/   pImp->OpenPercentBar( n1, n2 );
/*N*/   if( pImp->bNormal || pImp->bTxtColls )
/*N*/   {
/*N*/       // Stringpool fuellen, Namen im Doc erweitern
/*N*/       pImp->aStringPool.Setup( *pImp->pDoc, pImp->pRoot->GetVersion(),
/*N*/                                pImp->pExportInfo );
/*N*/       pImp->SaveStyleSheets( FALSE );
/*N*/       // Temporaere Namenserweiterungen entfernen
/*N*/       pImp->aStringPool.RemoveExtensions( *pImp->pDoc );
/*N*/   }
/*N*/   if( ( pImp->bNormal || pImp->bNumRules ) && !pImp->nRes )
/*N*/       pImp->SaveNumRules();
/*N*/   if( ( pImp->bNormal || pImp->bPageDescs ) && !pImp->nRes )
/*N*/       pImp->SavePageStyles();
/*N*/   if( pImp->bNormal && !pImp->nRes )
/*N*/       pImp->SaveDrawingLayer();
/*N*/   if( pImp->bNormal && !pImp->nRes )
/*N*/       pImp->SaveContents( *pPaM );
/*N*/   if( bNewPaM )
/*N*/       delete pPaM;
/*N*/
/*N*/       pImp->RemoveHiddenDrawObjs();
/*N*/
/*N*/   if( pImp->nRes )
/*N*/       pImp->nRes |= ERRCODE_CLASS_WRITE;
/*N*/   else if( pImp->nWarn )
/*N*/       pImp->nRes = pImp->nWarn | ERRCODE_CLASS_WRITE;
/*N*/
/*N*/   //pImp->pRoot->Commit();
/*N*/
/*N*/   ULONG nErr = pImp->pRoot->GetError();
/*N*/   if( nErr == SVSTREAM_DISK_FULL )
/*N*/       pImp->nRes = ERR_W4W_WRITE_FULL;
/*N*/   else if( nErr != SVSTREAM_OK )
/*N*/   {
/*N*/       if ( nErr == ERRCODE_IO_NOTSTORABLEINBINARYFORMAT )
/*N*/           pImp->nRes = nErr;
/*N*/       else
/*N*/           pImp->nRes = ERR_SWG_WRITE_ERROR;
/*N*/   }
/*N*/
/*N*/   pImp->ClosePercentBar();
/*N*/   pImp->CloseStreams();
/*N*/
/*N*/   return pImp->nRes;
/*N*/ }


// Speichern in einen frischen Storage.

/*N*/ ULONG Sw3Io::SaveAs( SvStorage* pStor, SwPaM* pPaM, BOOL bSaveAll )
/*N*/ {
/*N*/   pImp->pOldRoot = pImp->pRoot;
/*N*/   pImp->pRoot = pStor;
/*N*/   ULONG nRet = Save( pPaM, bSaveAll );
/*N*/   pImp->pRoot = pImp->pOldRoot;
/*N*/   pImp->pOldRoot.Clear();
/*N*/   return nRet;
/*N*/ }


/*N*/ void Sw3Io::HandsOff()
/*N*/ {
/*N*/   pImp->pRoot.Clear();
/*N*/ }

// Ende eines Save/SaveAs/HandsOff.
// Falls der Storage gewechselt hat, muessen die Streams
// neu geoeffnet werden.


/*N*/ BOOL Sw3Io::SaveCompleted( SvStorage* pNew )
/*N*/ {
/*N*/   BOOL bClearNm = !pNew || pNew == pImp->pRoot;
/*N*/
/*N*/   if( pNew )
/*N*/       pImp->pRoot = pNew;
/*N*/   else
/*?*/       pImp->pRoot = pImp->pDoc->GetDocStorage();
/*N*/
/*N*/   // Hier muss noch ueber die Grafiknodes iteriert werden, um
/*N*/   // ihnen zu sagen, wie ihr neuer Streamname lautet!
/*N*/   // Da Grafiken Flys sind, liegen die Nodes im Autotext-Bereich
/*N*/   SwNodes& rNds = pImp->pDoc->GetNodes();
/*N*/   ULONG nEnd = rNds.GetEndOfAutotext().GetIndex();
/*N*/   for( ULONG nIdx = rNds.GetEndOfInserts().GetIndex() + 1; nIdx < nEnd; ++nIdx)
/*N*/   {
/*N*/       SwGrfNode* pNd = rNds[ nIdx ]->GetGrfNode();
/*N*/       if( pNd )
/*N*/           pNd->SaveCompleted( bClearNm );
/*N*/   }
/*N*/
/*N*/   return TRUE;
/*N*/ }


/*N*/  SvStorage* Sw3Io::GetStorage()
/*N*/  {
/*N*/   if( !pImp->pRoot.Is() )
/*N*/       pImp->pRoot = pImp->pDoc->GetPersist()->GetStorage();
/*N*/   return &pImp->pRoot;
/*N*/  }


//STRIP001 void Sw3Io::SetDoc( SwDoc& r )
//STRIP001 {
//STRIP001  pImp->SetDoc( r );
//STRIP001 }


/*N*/ void Sw3Io::SetReadOptions( const SwgReaderOption& rOpt, BOOL bOverwrite )
/*N*/ {
/*N*/   pImp->SetReadOptions( rOpt, bOverwrite );
/*N*/ }


//STRIP001 void Sw3Io::SetSw31Export( BOOL b31 )
//STRIP001 {
//STRIP001  pImp->SetSw31Export( b31 );
//STRIP001 }


//STRIP001 ULONG Sw3Io::LoadStyles( SvStorage* pStor )
//STRIP001 {
//STRIP001  SvStorageRef aRoot = pImp->pRoot;
//STRIP001  pImp->pRoot = pStor;
//STRIP001  pImp->OpenStreams( FALSE );
//STRIP001  BOOL bGood = BOOL( pImp->pRoot.Is()
//STRIP001                  && pImp->pPageStyles.Is()
//STRIP001                  && pImp->pStyles.Is() );
//STRIP001  if( bGood && pImp->pRoot->GetVersion() > SOFFICE_FILEFORMAT_40 )
//STRIP001      bGood = pImp->pNumRules.Is();
//STRIP001  ASSERT( bGood, "Es fehlen leider ein paar Streams!" );
//STRIP001
//STRIP001  pImp->bOrganizer = TRUE;
//STRIP001  if( bGood )
//STRIP001  {
//STRIP001      pImp->LoadStyleSheets( BOOL( !pImp->bAdditive ) );
//STRIP001      pImp->LoadNumRules();
//STRIP001      pImp->LoadPageStyles();
//STRIP001  }
//STRIP001  pImp->CloseStreams();
//STRIP001  pImp->pRoot = aRoot;
//STRIP001
//STRIP001  SvNumberFormatter* pN;
//STRIP001  if( pImp->bInsert &&
//STRIP001      0 != ( pN = pImp->pDoc->GetNumberFormatter( FALSE ) ))
//STRIP001      pN->ClearMergeTable();
//STRIP001
//STRIP001  pImp->bOrganizer = FALSE;
//STRIP001
//STRIP001  if( pImp->nRes )
//STRIP001      pImp->nRes |= ERRCODE_CLASS_READ;
//STRIP001  else if( pImp->nWarn )
//STRIP001      pImp->nRes = pImp->nWarn | ERRCODE_CLASS_READ;
//STRIP001  return pImp->nRes;
//STRIP001 }


/*N*/  ULONG Sw3Io::SaveStyles()
/*N*/  {
/*N*/   BOOL bGood = pImp->OpenStreams( TRUE, FALSE );
/*N*/   ASSERT( bGood, "Es fehlen leider ein paar Streams!" );
/*N*/   if( !bGood )
/*N*/       return pImp->nRes = ERR_SWG_WRITE_ERROR;
/*N*/
/*N*/   pImp->bOrganizer = TRUE;
/*N*/
/*N*/   // Nur Bookmarks aus Seiten-Vorlagen sammeln
/*N*/   pImp->CollectMarks( NULL, TRUE );
/*N*/   if( !pImp->IsSw31Or40Export() )
/*N*/       pImp->CollectRedlines( NULL, TRUE );
/*N*/   else
/*N*/       pImp->CollectTblLineBoxFmts40();
/*N*/
/*N*/   // Stringpool fuellen, Namen im Doc erweitern
/*N*/   pImp->aStringPool.Setup( *pImp->pDoc, pImp->pRoot->GetVersion(),
/*N*/                            pImp->pExportInfo );
/*N*/   pImp->SaveStyleSheets( FALSE );
/*N*/   // Temporaere Namenserweiterungen entfernen
/*N*/   pImp->aStringPool.RemoveExtensions( *pImp->pDoc );
/*N*/   pImp->SaveNumRules( FALSE );
/*N*/   pImp->SavePageStyles();
/*N*/   pImp->CloseStreams();
/*N*/
/*N*/   pImp->bOrganizer = FALSE;
/*N*/
/*N*/   if( pImp->nRes )
/*N*/       pImp->nRes |= ERRCODE_CLASS_WRITE;
/*N*/   else if( pImp->nWarn )
/*N*/       pImp->nRes = pImp->nWarn | ERRCODE_CLASS_WRITE;
/*N*/   return pImp->nRes;
/*N*/  }

// Erzeugen eines eindeutigen Stream-Namens in einem Storage


/*N*/ String Sw3Io::UniqueName( SvStorage* pStg, const sal_Char* p )
/*N*/ {
/*N*/   String aName;
/*N*/   sal_Char cBuf[ 32 ];
/*N*/
/*N*/   // Man nehme die Adresse von cBuf auf dem Stack als Anfangswert
/*N*/   // aber nur ein einziges mal (bug fix 20976)
/*N*/ #ifdef SINIX
/*N*/   static ULONG nId = 0;
/*N*/   if ( ! nId )
/*N*/       nId = (ULONG) cBuf;
/*N*/ #else
/*N*/   static ULONG nId = (ULONG) cBuf;
/*N*/ #endif
/*N*/
/*N*/   nId++;
/*N*/   for( ;; )
/*N*/   {
/*N*/       snprintf( cBuf, sizeof(cBuf), "%s%08lX", p, nId );
/*N*/       aName.AssignAscii( cBuf );
/*N*/       if( !pStg->IsContained( aName ) )
/*N*/           break;
/*N*/       nId++;
/*N*/   }
/*N*/   return aName;
/*N*/ }


//STRIP001 ULONG Sw3Io::GetSectionList( SvStorage *pStor, SvStrings& rSectionList )
//STRIP001 {
//STRIP001  ULONG nRes = pImp->OpenStreamsForScan( pStor, TRUE );
//STRIP001  if( nRes )
//STRIP001      return nRes;
//STRIP001
//STRIP001  SvStringsDtor aBookmarks;
//STRIP001
//STRIP001  // zuerst die Page-Styles durchsuchen
//STRIP001  pImp->pPageStyles->Seek( 0L );
//STRIP001  pImp->pPageStyles->SetBufferSize( SW3_BSR_PAGESTYLES );
//STRIP001  pImp->pStrm = pImp->pPageStyles;
//STRIP001  pImp->GetSectionList( rSectionList, aBookmarks );
//STRIP001  pImp->pStrm = NULL;
//STRIP001  pImp->CheckIoError( pImp->pPageStyles );
//STRIP001  pImp->pPageStyles->SetBufferSize( 0 );
//STRIP001
//STRIP001  if( !pImp->nRes )
//STRIP001  {
//STRIP001      // und jetzt den Contents-Stream
//STRIP001      pImp->pContents->Seek( 0L );
//STRIP001      pImp->pContents->SetBufferSize( SW3_BSR_CONTENTS );
//STRIP001      pImp->pStrm = pImp->pContents;
//STRIP001      pImp->GetSectionList( rSectionList, aBookmarks );
//STRIP001      pImp->pStrm = NULL;
//STRIP001      pImp->CheckIoError( pImp->pContents );
//STRIP001      pImp->pContents->SetBufferSize( 0 );
//STRIP001  }
//STRIP001
//STRIP001  nRes = pImp->nRes;
//STRIP001  if( nRes )
//STRIP001      nRes |= ERRCODE_CLASS_READ;
//STRIP001 //   else if( pImp->nWarn )
//STRIP001 //       nRes = pImp->nWarn | ERRCODE_CLASS_READ;
//STRIP001
//STRIP001  pImp->CloseStreamsForScan();
//STRIP001
//STRIP001  return nRes;
//STRIP001 }

//STRIP001 ULONG Sw3Io::GetMacroTable( SvStorage *pStor, SvxMacroTableDtor& rMacroTbl )
//STRIP001 {
//STRIP001  ULONG nRes = pImp->OpenStreamsForScan( pStor, FALSE );
//STRIP001  if( nRes )
//STRIP001      return nRes;
//STRIP001
//STRIP001  pImp->pContents->Seek( 0L );
//STRIP001  pImp->pContents->SetBufferSize( SW3_BSR_CONTENTS );
//STRIP001  pImp->pStrm = pImp->pContents;
//STRIP001  pImp->GetMacroTable( rMacroTbl );
//STRIP001  pImp->pStrm = NULL;
//STRIP001  pImp->CheckIoError( pImp->pContents );
//STRIP001  pImp->pContents->SetBufferSize( 0 );
//STRIP001
//STRIP001  nRes = pImp->nRes;
//STRIP001  if( nRes )
//STRIP001      nRes |= ERRCODE_CLASS_READ;
//STRIP001 //   else if( pImp->nWarn )
//STRIP001 //       nRes = pImp->nWarn | ERRCODE_CLASS_READ;
//STRIP001
//STRIP001  pImp->CloseStreamsForScan();
//STRIP001
//STRIP001  return nRes;
//STRIP001 }

}
