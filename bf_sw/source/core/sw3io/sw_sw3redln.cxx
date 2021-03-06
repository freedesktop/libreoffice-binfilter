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

#ifndef _DOC_HXX //autogen

#include <horiornt.hxx>

#include <doc.hxx>
#endif

#include <osl/diagnose.h>

#include <redline.hxx>


#include "sw3marks.hxx"
#include "sw3imp.hxx"
namespace binfilter {


// lokaler Record in SWG_REDLINES
#define SWG_REDLINE_LCL 'R'

// lokaler Record in SWG_REDLINE
#define SWG_REDLINEDATA_LCL 'D'

// REDLINE:
// BYTE     Flags
//          0x10 - visisble Flags
// UINT16   Anzahl REDLINEDATA
// REDLINEDTA*
//
// REDLINEDATA:
// BYTE     Flags
// BYTE     Redline-Typ
// UINT16   String-Pool-Index des Autors
// UINT32   Datum
// UINT32   Uhrzeit
// String   Kommentar

/*N*/ void Sw3IoImp::InRedline() //SW50.SDW
/*N*/ {
/*N*/   OpenRec( SWG_REDLINE_LCL );
/*N*/
/*N*/   BYTE cFlags = OpenFlagRec();
/*N*/
/*N*/   UINT16 nCount;
/*N*/   *pStrm >> nCount;
/*N*/
/*N*/   CloseFlagRec();
/*N*/
/*N*/   SwRedlineData *pData = 0;
/*N*/   for( USHORT i=0; i<nCount; i++ )
/*N*/   {
/*N*/       OpenRec( SWG_REDLINEDATA_LCL );
/*N*/
/*N*/       OpenFlagRec();
/*N*/
/*N*/       BYTE cType;
/*N*/       UINT16 nStrIdx;
/*N*/
/*N*/       *pStrm  >> cType
/*N*/               >> nStrIdx;
/*N*/       CloseFlagRec();
/*N*/
/*N*/       UINT32 nDate2, nTime2;
/*N*/       String aComment;
/*N*/       *pStrm  >> nDate2
/*N*/               >> nTime2;
/*N*/       InString( *pStrm, aComment );
/*N*/
/*N*/       // Das oberste Element des Stack wurde als letztes geschrieben.
/*N*/       USHORT nAuthorIdx;
/*N*/       if( bNormal && !bInsert )
/*N*/           nAuthorIdx = pDoc->InsertRedlineAuthor(aStringPool.Find(nStrIdx));
/*N*/       else
/*N*/           nAuthorIdx = 0;
/*N*/
/*N*/       pData = new SwRedlineData( (SwRedlineType)cType, nAuthorIdx,
/*N*/                                  DateTime( nDate2, nTime2 ), aComment,
/*N*/                                  pData );
/*N*/
/*N*/       CloseRec( SWG_REDLINEDATA_LCL );
/*N*/   }
/*N*/
/*N*/   // Der PaM ist erstmal egal und wird erst spaeter gesetzt
/*N*/   SwPosition aDummyPos( pDoc->GetNodes().GetEndOfExtras() );
/*N*/   SwRedline *pRedline =
/*N*/       new SwRedline( pData, aDummyPos, (cFlags & 0x10) != 0,
/*N*/                      (cFlags & 0x20) != 0, (cFlags & 0x40) != 0 );
/*N*/
/*N*/   // Weil der PaM noch nicht gueltig ist, merken wir uns die Redline
/*N*/   // erstmal so und fuegen sie erst spaeter in das Dokument ein.
/*N*/   if( !pRedlines )
/*N*/       pRedlines = new Sw3Redlines;
/*N*/   pRedlines->Insert( pRedline, pRedlines->Count() );
/*N*/
/*N*/   CloseRec( SWG_REDLINE_LCL );
/*N*/ }

// REDLINES:
// REDLINE*

/*N*/ void Sw3IoImp::InRedlines() //SW50.SDW
/*N*/ {
/*N*/   if( pRedlines )
/*N*/   {
/*N*/       delete pRedlines;
/*N*/       pRedlines = 0;
/*N*/   }
/*N*/
/*N*/   OpenRec( SWG_REDLINES );
/*N*/
/*N*/   while( BytesLeft() )
/*N*/       InRedline();
/*N*/
/*N*/   CloseRec( SWG_REDLINES );
/*N*/ }

// NODEREDLINE:
// BYTE     Flags
//          0x01 - End-Position einer Redline, sonst Start-Positiom
//          0x02 - Redline besitzt Section
// UINT16   Id der Redline (Array-Position)
// UINT16   Content-Position bzw. USHRT_MAX, wenn die Psoition dem zu
//          einem Start-Node gehoerenden ENd-Node bezeichnet.
// CONTENT? Section (optional)

/*N*/ void Sw3IoImp::InNodeRedline( const SwNodeIndex& rNodeIdx, INT32& nCntntOff, //SW50.SDW
/*N*/                             BYTE nInsFirstPara )
/*N*/ {
/*N*/   OpenRec( SWG_NODEREDLINE );
/*N*/
/*N*/   BYTE cFlags = OpenFlagRec();
/*N*/   UINT16 nId, nNodeOff;
/*N*/   *pStrm >> nId >> nNodeOff;
/*N*/   CloseFlagRec();
/*N*/
/*N*/   OSL_ENSURE( pRedlines && nId < pRedlines->Count(), "Ungueltige Redline-Id" );
/*N*/   if( !pRedlines || nId >= pRedlines->Count() )
/*N*/   {
/*N*/       Error();
/*N*/       CloseRec( SWG_NODEREDLINE );
/*N*/       return;
/*N*/   }
/*N*/
/*N*/   SwRedline *pRedline = (*pRedlines)[nId];
/*N*/   SwRedlineType eType = pRedline ? pRedline->GetType() : REDLINE_FLAG_MASK;
/*N*/   if( REDLINE_INSERT != eType && REDLINE_DELETE != eType &&
/*N*/       REDLINE_FORMAT != eType )
/*N*/   {
/*N*/       // Unbekannter oder noch nicht implementierter Redline-Typ:
/*N*/       // Da ist es besser, wir ignorieren sie, denn wir koennen ja
/*N*/       // sowieso nichts damit anfangen.
/*N*/       OSL_ENSURE( !pRedline, "nicht implementierter Redline-Typ" );
/*N*/       Warning();
/*N*/       CloseRec( SWG_NODEREDLINE );
/*N*/       return;
/*N*/   }
/*N*/
/*N*/   // Position setzen
/*N*/   SwPosition *pPos;
/*N*/   if( 0x10 & cFlags )
/*N*/   {
/*N*/       // bei End-Positionen Mark setzen
/*N*/       pRedline->SetMark();
/*N*/       pPos = pRedline->GetMark();
/*N*/   }
/*N*/   else
/*N*/   {
/*N*/       // sonst Point
/*N*/       pPos = pRedline->GetPoint();
/*N*/   }
/*N*/
/*N*/   SwNode& rNode = rNodeIdx.GetNode();
/*N*/
/*N*/   // Beim Einfuegen in den ersten Absatz sollten wir eine Content-Node
/*N*/   // haben, in den wir an der uebergeben Position einfuegen.
/*N*/   if( nInsFirstPara )
/*N*/   {
/*N*/       OSL_ENSURE( rNode.IsCntntNode(), "Content-Node beim Einf. erwartet" );
/*N*/       OSL_ENSURE( USHRT_MAX==nNodeOff, "End-Node-Position beim Einf. erwartet" );
/*N*/       nNodeOff = 0;   // Im IsCntntNode-Zweig gibts so die richtige Pos.
/*N*/   }
/*N*/
/*N*/   sal_Bool bInvalid = sal_False;
/*N*/   // Positionen koennen auch auf Start- oder End-Nodes fallen.
/*N*/   if( rNode.IsCntntNode() )
/*N*/   {
/*N*/       pPos->nNode = rNodeIdx;
/*N*/       xub_StrLen nLen = rNode.GetCntntNode()->Len();
/*N*/       xub_StrLen nIdx = static_cast< xub_StrLen >( nNodeOff+nCntntOff );
/*N*/       OSL_ENSURE( nIdx<=nLen, "ungueltiger Cntnt-Offset" );
/*N*/       if( nIdx > nLen )
/*N*/       {
/*N*/           bInvalid = sal_True;
/*N*/       }
/*N*/       else
/*N*/       {
/*N*/           pPos->nContent.Assign( rNode.GetCntntNode(), nIdx );
/*N*/       }
/*N*/   }
/*N*/   else
/*N*/   {
/*N*/       OSL_ENSURE( rNode.IsStartNode(), "Start-Node erwartet" );
/*N*/       if( USHRT_MAX==nNodeOff )
/*N*/       {
/*N*/           pPos->nNode = rNode.EndOfSectionIndex();
/*N*/       }
/*N*/       else
/*N*/       {
/*N*/           pPos->nNode = rNodeIdx;
/*N*/
/*N*/           OSL_ENSURE( 0==nNodeOff,
/*N*/                   "ungeueltige Content-Position fuer Start-Node" );
/*N*/           if( 0 != nNodeOff )
/*N*/               Warning();
/*N*/       }
/*N*/       pPos->nContent.Assign( 0, 0 );
/*N*/   }
/*N*/
/*N*/   if( cFlags & 0x20 )
/*N*/   {
/*N*/       if( (bNormal && bInsert) || bPageDescs )
/*N*/       {
/*N*/           if( REDLINE_INSERT == eType )
/*N*/           {
/*N*/               // Ein einefuegter Text wird nicht angezeigt. Dann muss der
/*N*/               // eingefuegt Text an der entsprechenden Position eingfuegt
/*N*/               // werden.
/*N*/               if( !nInsFirstPara && !pPos->nNode.GetNode().IsCntntNode() &&
/*N*/                   pRedline->IsDelLastPara() )
/*N*/               {
/*N*/                   // Wenn das DelLastPara-Flag gsetzt ist, wird
/*N*/                   // versucht in den Absatz davor einzufuegen, denn
/*N*/                   // der soll ja geloescht werden.
/*N*/                   pPos->nNode--;
/*N*/                   SwCntntNode *pCNd = pPos->nNode.GetNode().GetCntntNode();
/*N*/                   OSL_ENSURE( pCNd, "Kein Content-Node trotz DelLastPara" );
/*N*/                   if( pCNd )
/*N*/                       pPos->nContent.Assign( pCNd, pCNd->Len() );
/*N*/                   else
/*N*/                       pPos->nNode++;
/*N*/               }
/*N*/               const SwNode& rNewNode = pPos->nNode.GetNode();
/*N*/               if( rNewNode.IsCntntNode() )
/*N*/               {
/*N*/                   xub_StrLen nCntntIdx = pPos->nContent.GetIndex();
/*N*/
/*N*/                   InContents( pPos->nNode, nCntntIdx );
/*N*/
/*N*/                   // Den Offset noch an die Verschiebung der
/*N*/                   // Content-Position anpassen.
/*N*/                   nCntntOff += pPos->nContent.GetIndex();
/*N*/                   nCntntOff -= nCntntIdx;
/*N*/               }
/*N*/               else
/*N*/               {
/*N*/                   InContents( pPos->nNode, 0, FALSE, 0, TRUE );
/*N*/               }
/*N*/           }
/*N*/           else
/*N*/               SkipRec();
/*N*/       }
/*N*/       else
/*N*/       {
/*N*/           SwNodeIndex aStart( pDoc->GetNodes().GetEndOfRedlines() );
/*N*/           SwStartNode* pSttNd =
/*N*/               pDoc->GetNodes().MakeEmptySection( aStart,SwNormalStartNode );
/*N*/           aStart = *pSttNd->EndOfSectionNode();
/*N*/           InContents( aStart, 0, FALSE );
/*N*/           aStart = pSttNd->GetIndex();
/*N*/           if( pSttNd->EndOfSectionIndex() - aStart.GetIndex() == 1 )
/*N*/               bInvalid = sal_True;
/*N*/           OSL_ENSURE( !bInvalid, "empty redline section imported" );
/*N*/       }
/*N*/   }
/*N*/
/*N*/   const SwRedlineTbl& rRedlineTbl = pDoc->GetRedlineTbl();
/*N*/   if( !bInvalid )
/*N*/   {
/*N*/       // Check for nested redlines.
/*N*/       sal_Bool bHasMark = pRedline->HasMark() &&
/*N*/                           *pRedline->GetPoint() != *pRedline->GetMark();
/*N*/       const SwPosition *pStt = bHasMark ? pRedline->Start() : 0;
/*N*/       const SwPosition *pEnd = bHasMark ? pRedline->End() : 0;
/*N*/       sal_uInt16 nCount = rRedlineTbl.Count();
/*N*/       for( sal_uInt16 i=0; !bInvalid && i<nCount; i++ )
/*N*/       {
/*N*/           const SwRedline *pTest = rRedlineTbl[i];
/*N*/           if( pTest != pRedline )
/*N*/           {
/*N*/
/*N*/               // Is the ineew position is inside an existing
/*N*/               // redline then it is invalid.
/*N*/               const SwPosition *pTestStt = pTest->Start();
/*N*/               const SwPosition *pTestEnd = pTest->HasMark() ? pTest->End() : 0;
/*N*/               bInvalid = pTestEnd != 0 && *pPos > *pTestStt && *pPos < *pTestEnd;
/*N*/
/*N*/               // If the start or end of and existing redline is inside the
/*N*/               // redline then the new redline is invalid as well;
/*N*/               if( !bInvalid && bHasMark )
/*N*/                   bInvalid = *pTestStt > *pStt && *pTestStt < *pEnd;
/*N*/               if( !bInvalid && bHasMark && pTestEnd != 0 )
/*N*/                   bInvalid = *pTestEnd > *pStt && *pTestEnd < *pEnd;
/*N*/
/*N*/               OSL_ENSURE( !bInvalid, "overlapping redline" );
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/
/*N*/   if( bNormal && !bInsert )
/*N*/   {
/*N*/       if( (0x10 & cFlags) == 0 )
/*N*/       {
/*N*/           // Die Redline darf jetzt eingefuegt werden. Das Redline-Objekt
/*N*/           // geht in den Besitz des Dokuments ueber.
/*N*/
/*N*/           if( !bInvalid )
/*N*/               bInvalid = !const_cast< SwRedlineTbl& >(rRedlineTbl).Insert(
/*N*/                   pRedline, (BOOL)FALSE );
/*N*/       }
/*N*/       else if (bInvalid )
/*N*/       {
/*N*/           // The redline is is inserted already and has to be removed from the
/*N*/           // table;
/*N*/
/*N*/           sal_uInt16 nPos = rRedlineTbl.GetPos( pRedline );
/*N*/           OSL_ENSURE( USHRT_MAX != nPos, "inserted invalid redline not found" );
/*N*/           if( USHRT_MAX != nPos )
/*N*/               const_cast< SwRedlineTbl& >(rRedlineTbl).Remove( nPos );
/*N*/       }
/*N*/
/*N*/       if( bInvalid )
/*N*/       {
/*N*/           // JP 18.5.2001: Bug 87222 - then delete this invalid redline
/*N*/           delete pRedline;
/*N*/           pRedlines->Replace( 0, nId );
/*N*/           Warning();
/*N*/       }
/*N*/   }
/*N*/
/*N*/   CloseRec( SWG_NODEREDLINE );
/*N*/ }

/*N*/ namespace
/*N*/ {
/*N*/   void CorrRedline( SwRedline& rRedline,
/*N*/                     const SwPosition& rOldPos,
/*N*/                     const SwPosition& rNewPos )
/*N*/   {
/*N*/       sal_uInt32 nOldNodeIdx = rOldPos.nNode.GetIndex();
/*N*/       if( rRedline.GetPoint()->nNode.GetIndex() == nOldNodeIdx )
/*N*/       {
/*N*/           xub_StrLen nOffset = rRedline.GetPoint()->nContent.GetIndex() -
/*N*/                                rOldPos.nContent.GetIndex();
/*N*/           rRedline.GetPoint()->nNode = rNewPos.nNode;
/*N*/           rRedline.GetPoint()->nContent.Assign(
/*N*/                               rNewPos.nNode.GetNode().GetCntntNode(),
/*N*/                               rNewPos.nContent.GetIndex() + nOffset );
/*N*/       }
/*N*/       if( rRedline.GetMark()->nNode.GetIndex() == nOldNodeIdx )
/*N*/       {
/*N*/           xub_StrLen nOffset = rRedline.GetMark()->nContent.GetIndex() -
/*N*/                                rOldPos.nContent.GetIndex();
/*N*/           rRedline.GetMark()->nNode = rNewPos.nNode;
/*N*/           rRedline.GetMark()->nContent.Assign(
/*N*/                               rNewPos.nNode.GetNode().GetCntntNode(),
/*N*/                               rNewPos.nContent.GetIndex() + nOffset );
/*N*/       }
/*N*/   }
/*N*/ }
/*N*/
/*N*/ void Sw3IoImp::CleanupRedlines()
/*N*/ {
/*N*/   if( !pRedlines || !pRedlines->Count() )
/*N*/       return;
/*N*/
/*N*/   // Bisher muss hier nur was beim Einfugen und Laden von Seiten-Vorlagen
/*N*/   // passieren.
/*N*/   if( (bNormal && bInsert) || bPageDescs )
/*N*/   {
/*N*/       // Alle geloeschten-Redlines, die angezeigt werden, muessen noch
/*N*/       // geloescht werden.
/*N*/
/*N*/       // In a first step, delete the redline objects that are not deletions.
/*N*/       sal_uInt16 i=0;
/*N*/       while( i < pRedlines->Count() )
/*N*/       {
/*N*/           SwRedline *pRedline = (*pRedlines)[i];
/*N*/           if( pRedline )
/*N*/           {
/*N*/               if( REDLINE_DELETE != pRedline->GetType() ||
/*N*/                   pRedline->GetContentIdx() != 0 )
/*N*/               {
/*N*/                   delete pRedline;
/*N*/                   pRedlines->Remove( i );
/*N*/               }
/*N*/               else
/*N*/               {
/*N*/                   ++i;
/*N*/               }
/*N*/           }
/*N*/           else
/*N*/           {
/*N*/               pRedlines->Remove( i );
/*N*/           }
/*N*/       }
/*N*/
/*N*/       // now process the remaining redlines
/*N*/       sal_uInt16 nCount = pRedlines->Count();
/*N*/       for( i=0; i<nCount; ++i )
/*N*/       {
/*N*/           SwRedline *pRedline = (*pRedlines)[i];
/*N*/           OSL_ENSURE( REDLINE_DELETE == pRedline->GetType() &&
/*N*/                   !pRedline->GetContentIdx(), "wrong redline type" );
/*N*/           SwRedlineMode eOld = pDoc->GetRedlineMode();
/*N*/           pDoc->SetRedlineMode_intern( eOld & ~(REDLINE_ON | REDLINE_IGNORE) );
/*N*/           {
/*N*/               // If the end node of the redline is deleted (and not joined
/*N*/               // with the next one), then we have to make sure that none of
/*N*/               // the remaining redlines starts in that node.
/*N*/               // The end node of the redline is deleted if the PaM's point
/*N*/               // or if it is the PaM mark, but in this later case, the whole
/*N*/               // last node has to be selected.
/*N*/               const SwPosition& rPointPos = *pRedline->GetPoint();
/*N*/               const SwPosition& rMarkPos = *pRedline->GetMark();
/*N*/               if( rPointPos.nNode.GetNode().IsCntntNode() &&
/*N*/                   rMarkPos.nNode.GetNode().IsCntntNode() &&
/*N*/                   ( rPointPos.nNode.GetIndex() > rMarkPos.nNode.GetIndex() ||
/*N*/                     (rMarkPos.nNode.GetIndex() > rPointPos.nNode.GetIndex() &&
/*N*/                      rMarkPos.nContent.GetIndex() == rMarkPos.nNode.GetNode().GetCntntNode()->Len()) ) )
/*N*/               {
/*N*/                   for( sal_uInt16 j= i+1; j < nCount; ++j )
/*N*/                   {
/*N*/                       SwRedline *pTestRedline = (*pRedlines)[j];
/*N*/                       CorrRedline( *pTestRedline, *pRedline->End(),
/*N*/                                    *pRedline->Start() );
/*N*/                   }
/*N*/               }
/*N*/               pDoc->DeleteAndJoin( *pRedline );
/*N*/               pDoc->SetRedlineMode_intern( eOld );
/*N*/           }
/*N*/           delete pRedline;
/*N*/       }
/*N*/       // Das Array wird zwar auch im CloseStreams geloscht, da wir
/*N*/       // die Objekte aber schon geloescht haben, ist es besser, das
/*N*/       // Array gleich zu loeschen.
/*N*/       pRedlines->Remove( 0, pRedlines->Count() );
/*N*/       delete pRedlines; pRedlines = 0;
/*N*/   }
/*N*/ }


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
