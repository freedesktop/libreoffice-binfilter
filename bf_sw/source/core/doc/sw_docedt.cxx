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

#include <string.h>         // fuer strchr()
#include <hintids.hxx>

#include <com/sun/star/i18n/WordType.hdl>
#include <unotools/charclass.hxx>

#include <fmtanchr.hxx>
#include <fmtcntnt.hxx>
#include <txtftn.hxx>

#include <horiornt.hxx>

#include <doc.hxx>
#include <docary.hxx>
#include <mvsave.hxx>       // Strukturen zum Sichern beim Move/Delete
#include <ndtxt.hxx>
#include <redline.hxx>
#include <section.hxx>
#include "comcore.hrc"
#include <viscrs.hxx>

namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::linguistic2;
using namespace ::rtl;
using namespace ::com::sun::star::i18n;

#define S2U(rString) OUString::createFromAscii(rString)

SV_IMPL_PTRARR( SaveBookmarks, SaveBookmark* )

// loesche und verschiebe alle "Fly's am Absatz", die in der SSelection
// liegen. Steht am SPoint ein Fly, wird dieser auf den Mark verschoben.

/*N*/ void DelFlyInRange( const SwNodeIndex& rMkNdIdx,
/*N*/                   const SwNodeIndex& rPtNdIdx )
/*N*/ {
/*N*/   const sal_Bool bDelFwrd = rMkNdIdx.GetIndex() <= rPtNdIdx.GetIndex();
/*N*/
/*N*/   SwDoc* pDoc = rMkNdIdx.GetNode().GetDoc();
/*N*/   SwSpzFrmFmts& rTbl = *pDoc->GetSpzFrmFmts();
/*N*/   const SwPosition* pAPos;
/*N*/   for ( sal_uInt16 i = rTbl.Count(); i; )
/*N*/   {
/*N*/       SwFrmFmt *pFmt = rTbl[--i];
/*N*/       const SwFmtAnchor &rAnch = pFmt->GetAnchor();
/*N*/       if( ( rAnch.GetAnchorId() == FLY_AT_CNTNT ||
/*N*/             rAnch.GetAnchorId() == FLY_AUTO_CNTNT ) &&
/*N*/           0 != ( pAPos = rAnch.GetCntntAnchor() ) &&
/*N*/           ( bDelFwrd
/*N*/               ? rMkNdIdx < pAPos->nNode && pAPos->nNode <= rPtNdIdx
/*N*/               : rPtNdIdx <= pAPos->nNode && pAPos->nNode < rMkNdIdx ))
/*N*/       {
/*N*/           OSL_ENSURE( rAnch.GetAnchorId() != FLY_AUTO_CNTNT, "FLY-AUTO-Baustelle!" );
/*N*/           // nur den Anker verchieben ??
/*N*/           if( rPtNdIdx == pAPos->nNode )
/*N*/           {
/*?*/               SwFmtAnchor aAnch( pFmt->GetAnchor() );
/*?*/               SwPosition aPos( rMkNdIdx );
/*?*/               aAnch.SetAnchor( &aPos );
/*?*/               pFmt->SetAttr( aAnch );
/*N*/           }
/*N*/           else
/*N*/           {
/*N*/               // wird der Fly geloescht muss auch im seinem Inhalt alle
/*N*/               // Flys geloescht werden !!
/*N*/               const SwFmtCntnt &rCntnt = pFmt->GetCntnt();
/*N*/               if( rCntnt.GetCntntIdx() )
/*N*/               {
/*N*/                   DelFlyInRange( *rCntnt.GetCntntIdx(),
/*N*/                                   SwNodeIndex( *rCntnt.GetCntntIdx()->
/*N*/                                           GetNode().EndOfSectionNode() ));
/*N*/                   // Position kann sich verschoben haben !
/*N*/                   if( i > rTbl.Count() )
/*?*/                       i = rTbl.Count();
/*N*/                   else if( pFmt != rTbl[i] )
/*?*/                       i = rTbl.GetPos( pFmt );
/*N*/               }
/*N*/
/*N*/               pDoc->DelLayoutFmt( pFmt );
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/ }


/*N*/ _SaveRedlEndPosForRestore::_SaveRedlEndPosForRestore( const SwNodeIndex& rInsIdx )
/*N*/   : pSavArr( 0 ), pSavIdx( 0 )
/*N*/ {
/*N*/   SwNode& rNd = rInsIdx.GetNode();
/*N*/   SwDoc* pDest = rNd.GetDoc();
/*N*/   if( pDest->GetRedlineTbl().Count() )
/*N*/   {
/*N*/       sal_uInt16 nFndPos;
/*N*/       const SwPosition* pEnd;
/*N*/       SwPosition aSrcPos( rInsIdx, SwIndex( rNd.GetCntntNode(), 0 ));
/*N*/       const SwRedline* pRedl = pDest->GetRedline( aSrcPos, &nFndPos );
/*N*/       while( nFndPos-- && *( pEnd = ( pRedl =
/*N*/           pDest->GetRedlineTbl()[ nFndPos ] )->End() ) == aSrcPos &&
/*N*/           *pRedl->Start() != aSrcPos )
/*N*/       {
/*N*/           if( !pSavArr )
/*N*/           {
/*N*/               pSavArr = new SvPtrarr( 2, 2 );
/*N*/               pSavIdx = new SwNodeIndex( rInsIdx, -1 );
/*N*/           }
/*N*/           void* p = (void*)pEnd;
/*N*/           pSavArr->Insert( p, pSavArr->Count() );
/*N*/       }
/*N*/   }
/*N*/ }

/*N*/ _SaveRedlEndPosForRestore::~_SaveRedlEndPosForRestore()
/*N*/ {
/*N*/   if( pSavArr )
/*N*/   {
/*N*/       delete pSavArr;
/*N*/       delete pSavIdx;
/*N*/   }
/*N*/ }

/*N*/ void _SaveRedlEndPosForRestore::_Restore()
/*N*/ {
/*N*/   (*pSavIdx)++;
/*N*/   SwPosition aPos( *pSavIdx, SwIndex( pSavIdx->GetNode().GetCntntNode(), 0 ));
/*N*/   for( sal_uInt16 n = pSavArr->Count(); n; )
/*N*/       *((SwPosition*)pSavArr->GetObject( --n )) = aPos;
/*N*/ }


// ------------------------------------------------------------------------

// Loeschen einer vollstaendigen Section des NodesArray.
// Der uebergebene Node steht irgendwo in der gewuenschten Section
/*N*/ void SwDoc::DeleteSection( SwNode *pNode )
/*N*/ {
/*N*/   OSL_ENSURE( pNode, "Kein Node uebergeben." );
/*N*/   SwStartNode* pSttNd = pNode->IsStartNode() ? (SwStartNode*)pNode
/*N*/                                              : pNode->StartOfSectionNode();
/*N*/   SwNodeIndex aSttIdx( *pSttNd ), aEndIdx( *pNode->EndOfSectionNode() );
/*N*/
/*N*/   // dann loesche mal alle Fly's, text::Bookmarks, ...
/*N*/   DelFlyInRange( aSttIdx, aEndIdx );
/*N*/   DeleteRedline( *pSttNd );
/*N*/   _DelBookmarks( aSttIdx, aEndIdx );
/*N*/
/*N*/   {
/*N*/       // alle Crsr/StkCrsr/UnoCrsr aus dem Loeschbereich verschieben
/*N*/       SwNodeIndex aMvStt( aSttIdx, 1 );
/*N*/       CorrAbs( aMvStt, aEndIdx, SwPosition( aSttIdx ), sal_True );
/*N*/   }
/*N*/
/*N*/   GetNodes().DelNodes( aSttIdx, aEndIdx.GetIndex() - aSttIdx.GetIndex() + 1 );
/*N*/ }

/*************************************************************************
|*                SwDoc::Insert(char)
|*    Beschreibung      Zeichen einfuegen
*************************************************************************/

/*N*/ sal_Bool SwDoc::Insert( const SwPaM &rRg, sal_Unicode c )
/*N*/ {
/*N*/   const SwPosition & rPos = *rRg.GetPoint();
/*N*/
/*N*/   SwTxtNode *pNode = rPos.nNode.GetNode().GetTxtNode();
/*N*/   if(!pNode)
/*?*/       return sal_False;
/*N*/
/*N*/   SwDataChanged aTmp( rRg, 0 );
/*N*/
/*N*/   pNode->Insert( c, rPos.nContent );
/*N*/
/*N*/   SetModified();
/*N*/   return sal_True;
/*N*/ }


/*************************************************************************
|*                SwDoc::Overwrite(char)
|*    Beschreibung      Zeichen ueberschreiben
*************************************************************************/

/* #107318# Convert list of ranges of whichIds to a corresponding list
    of whichIds*/

/*N*/ void lcl_GetJoinFlags( SwPaM& rPam, sal_Bool& rJoinTxt, sal_Bool& rJoinPrev )
/*N*/ {
/*N*/   if( rPam.GetPoint()->nNode != rPam.GetMark()->nNode )
/*N*/   {
/*N*/       const SwPosition* pStt = rPam.Start(), *pEnd = rPam.End();
/*N*/       SwTxtNode* pTxtNd = pStt->nNode.GetNode().GetTxtNode();
/*N*/       rJoinTxt = (0 != pTxtNd) && pEnd->nNode.GetNode().IsTxtNode();
/*N*/
/*N*/       if( rJoinTxt && pStt == rPam.GetPoint() &&
/*N*/           0 != ( pTxtNd = pEnd->nNode.GetNode().GetTxtNode() ) &&
/*N*/           pTxtNd->GetTxt().Len() == pEnd->nContent.GetIndex() )
/*N*/       {
/*?*/           rPam.Exchange();
/*?*/           rJoinPrev = sal_False;
/*N*/       }
/*N*/       else
/*N*/           rJoinPrev = rJoinTxt && rPam.GetPoint() == pStt;
/*N*/   }
/*N*/   else
/*N*/       rJoinTxt = sal_False, rJoinPrev = sal_False;
/*N*/ }

/*N*/ void lcl_JoinText( SwPaM& rPam, sal_Bool bJoinPrev )
/*N*/ {
/*N*/   SwNodeIndex aIdx( rPam.GetPoint()->nNode );
/*N*/   SwTxtNode *pTxtNd = aIdx.GetNode().GetTxtNode();
/*N*/   SwNodeIndex aOldIdx( aIdx );
/*N*/   SwTxtNode *pOldTxtNd = pTxtNd;
/*N*/
/*N*/   if( pTxtNd && pTxtNd->CanJoinNext( &aIdx ) )
/*N*/   {
/*N*/       SwDoc* pDoc = rPam.GetDoc();
/*N*/       if( bJoinPrev )
/*N*/       {
/*N*/           {
/*N*/
/*N*/               /* PageBreaks, PageDesc, ColumnBreaks */
/*N*/               // Sollte an der Logik zum Kopieren der PageBreak's ...
/*N*/               // etwas geaendert werden, muss es auch im SwUndoDelete
/*N*/               // geandert werden. Dort wird sich das AUTO-PageBreak
/*N*/               // aus dem GetMarkNode kopiert.!!!
/*N*/
/*N*/               /* Der GetMarkNode */
/*N*/               if( ( pTxtNd = aIdx.GetNode().GetTxtNode())->GetpSwAttrSet() )
/*N*/               {
/*?*/                   const SfxPoolItem* pItem;
/*?*/                   if( SFX_ITEM_SET == pTxtNd->GetpSwAttrSet()->GetItemState(
/*?*/                       RES_BREAK, sal_False, &pItem ) )
/*?*/                       pTxtNd->ResetAttr( RES_BREAK );
/*?*/                   if( pTxtNd->GetpSwAttrSet() &&
/*?*/                       SFX_ITEM_SET == pTxtNd->GetpSwAttrSet()->GetItemState(
/*?*/                       RES_PAGEDESC, sal_False, &pItem ) )
/*?*/                       pTxtNd->ResetAttr( RES_PAGEDESC );
/*N*/               }
/*N*/
/*N*/               /* Der PointNode */
/*N*/               if( pOldTxtNd->GetpSwAttrSet() )
/*N*/               {
/*?*/                   const SfxPoolItem* pItem;
/*?*/                   SfxItemSet aSet( pDoc->GetAttrPool(), aBreakSetRange );
/*?*/                   SfxItemSet* pSet = pOldTxtNd->GetpSwAttrSet();
/*?*/                   if( SFX_ITEM_SET == pSet->GetItemState( RES_BREAK,
/*?*/                       sal_False, &pItem ) )
/*?*/                       aSet.Put( *pItem );
/*?*/                   if( SFX_ITEM_SET == pSet->GetItemState( RES_PAGEDESC,
/*?*/                       sal_False, &pItem ) )
/*?*/                       aSet.Put( *pItem );
/*?*/                   if( aSet.Count() )
/*?*/                       pTxtNd->SwCntntNode::SetAttr( aSet );
/*N*/               }
/*N*/               pOldTxtNd->FmtToTxtAttr( pTxtNd );
/*N*/
/*N*/               SvULongs aBkmkArr( 15, 15 );
/*N*/               ::binfilter::_SaveCntntIdx( pDoc, aOldIdx.GetIndex(),
/*N*/                                   pOldTxtNd->Len(), aBkmkArr );
/*N*/
/*N*/               SwIndex aAlphaIdx(pTxtNd);
/*N*/               pOldTxtNd->Cut( pTxtNd, aAlphaIdx, SwIndex(pOldTxtNd),
/*N*/                                   pOldTxtNd->Len() );
/*N*/               SwPosition aAlphaPos( aIdx, aAlphaIdx );
/*N*/               pDoc->CorrRel( rPam.GetPoint()->nNode, aAlphaPos, 0, sal_True );
/*N*/
/*N*/               // falls der uebergebene PaM nicht im Crsr-Ring steht,
/*N*/               // gesondert behandeln (z.B. Aufruf aus dem Auto-Format)
/*N*/               if( pOldTxtNd == rPam.GetBound( sal_True ).nContent.GetIdxReg() )
/*?*/                   rPam.GetBound( sal_True ) = aAlphaPos;
/*N*/               if( pOldTxtNd == rPam.GetBound( sal_False ).nContent.GetIdxReg() )
/*?*/                   rPam.GetBound( sal_False ) = aAlphaPos;
/*N*/           }
/*N*/           // jetzt nur noch den Node loeschen
/*N*/           pDoc->GetNodes().Delete( aOldIdx, 1 );
/*N*/       }
/*N*/   }
/*N*/ }

/*N*/ sal_Bool SwDoc::DeleteAndJoin( SwPaM & rPam )
/*N*/ {
/*N*/   sal_Bool bJoinTxt, bJoinPrev;
/*N*/   lcl_GetJoinFlags( rPam, bJoinTxt, bJoinPrev );
/*N*/
/*N*/   {
/*N*/       // dann eine Kopie vom Cursor erzeugen um alle Pams aus den
/*N*/       // anderen Sichten aus dem Loeschbereich zu verschieben
/*N*/       // ABER NICHT SICH SELBST !!
/*N*/       SwPaM aDelPam( *rPam.GetMark(), *rPam.GetPoint() );
/*N*/       ::binfilter::PaMCorrAbs( aDelPam, *aDelPam.GetPoint() );
/*N*/
/*N*/       if( !Delete( aDelPam ) )
/*N*/           return sal_False;
/*N*/
/*N*/       *rPam.GetPoint() = *aDelPam.GetPoint();
/*N*/   }
/*N*/
/*N*/   if( bJoinTxt )
/*N*/       lcl_JoinText( rPam, bJoinPrev );
/*N*/
/*N*/   return sal_True;
/*N*/ }

/*N*/ sal_Bool SwDoc::Delete( SwPaM & rPam )
/*N*/ {
/*N*/   SwPosition *pStt = (SwPosition*)rPam.Start(), *pEnd = (SwPosition*)rPam.End();
/*N*/
/*N*/   if( !rPam.HasMark() || *pStt >= *pEnd )
/*N*/       return sal_False;
/*N*/
/*N*/   {
/*N*/       // loesche alle leeren TextHints an der Mark-Position
/*N*/       SwTxtNode* pTxtNd = rPam.GetMark()->nNode.GetNode().GetTxtNode();
/*N*/       SwpHints* pHts;
/*N*/       if( pTxtNd &&  0 != ( pHts = pTxtNd->GetpSwpHints()) && pHts->Count() )
/*N*/       {
/*N*/           const xub_StrLen *pEndIdx;
/*N*/           xub_StrLen nMkCntPos = rPam.GetMark()->nContent.GetIndex();
/*N*/           for( sal_uInt16 n = pHts->Count(); n; )
/*N*/           {
/*N*/               const SwTxtAttr* pAttr = (*pHts)[ --n ];
/*N*/               if( nMkCntPos > *pAttr->GetStart() )
/*N*/                   break;
/*N*/
/*N*/               if( nMkCntPos == *pAttr->GetStart() &&
/*N*/                   0 != (pEndIdx = pAttr->GetEnd()) &&
/*N*/                   *pEndIdx == *pAttr->GetStart() )
/*?*/                   pTxtNd->DestroyAttr( pHts->Cut( n ) );
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/
/*N*/   {
/*N*/       // Bug 26675:   DataChanged vorm loeschen verschicken, dann bekommt
/*N*/       //          man noch mit, welche Objecte sich im Bereich befinden.
/*N*/       //          Danach koennen sie vor/hinter der Position befinden.
/*N*/       SwDataChanged aTmp( rPam, 0 );
/*N*/   }
/*N*/
/*N*/
/*N*/
/*?*/   if( !IsIgnoreRedline() && GetRedlineTbl().Count() )
/*?*/       DeleteRedline( rPam );
/*?*/
/*?*/   // loesche und verschiebe erstmal alle "Fly's am Absatz", die in der
/*?*/   // SSelection liegen
/*?*/   DelFlyInRange( rPam.GetMark()->nNode, rPam.GetPoint()->nNode );
/*?*/   _DelBookmarks( pStt->nNode, pEnd->nNode, 0,
/*?*/                   &pStt->nContent, &pEnd->nContent );
/*?*/
/*?*/   SwNodeIndex aSttIdx( pStt->nNode );
/*?*/   SwCntntNode * pCNd = aSttIdx.GetNode().GetCntntNode();
/*?*/
/*?*/   do {        // middle checked loop!
/*?*/       if( pCNd )
/*?*/       {
/*?*/           if( pCNd->GetTxtNode() )
/*?*/           {
/*?*/               // verschiebe jetzt noch den Inhalt in den neuen Node
/*?*/               sal_Bool bOneNd = pStt->nNode == pEnd->nNode;
/*?*/               xub_StrLen nLen = ( bOneNd ? pEnd->nContent.GetIndex()
/*?*/                                          : pCNd->Len() )
/*?*/                                       - pStt->nContent.GetIndex();
/*?*/
/*?*/               // falls schon leer, dann nicht noch aufrufen
/*?*/               if( nLen )
/*?*/                   ((SwTxtNode*)pCNd)->Erase( pStt->nContent, nLen );
/*?*/
/*?*/               if( bOneNd )        // das wars schon
/*?*/                   break;
/*?*/
/*?*/               aSttIdx++;
/*?*/           }
/*?*/           else
/*?*/           {
/*?*/               // damit beim loeschen keine Indizies mehr angemeldet sind,
/*?*/               // wird hier der SwPaM aus dem Content entfernt !!
/*?*/               pStt->nContent.Assign( 0, 0 );
/*?*/           }
/*?*/       }
/*?*/
/*?*/       sal_uInt32 nEnde = pEnd->nNode.GetIndex();
/*?*/       pCNd = pEnd->nNode.GetNode().GetCntntNode();
/*?*/       if( pCNd )
/*?*/       {
/*?*/           if( pCNd->GetTxtNode() )
/*?*/           {
/*?*/               // falls schon leer, dann nicht noch aufrufen
/*?*/               if( pEnd->nContent.GetIndex() )
/*?*/               {
/*?*/                   SwIndex aIdx( pCNd, 0 );
/*?*/                   ((SwTxtNode*)pCNd)->Erase( aIdx, pEnd->nContent.GetIndex() );
/*?*/               }
/*?*/               nEnde--;
/*?*/           }
/*?*/           else
/*?*/           {
/*?*/               // damit beim Loeschen keine Indizies mehr angemeldet sind,
/*?*/               // wird hier der SwPaM aus dem Content entfernt !!
/*?*/               pEnd->nContent.Assign( 0, 0 );
/*?*/               nEnde--;
/*?*/           }
/*?*/       }
/*?*/
/*?*/       nEnde++;
/*?*/       if( aSttIdx != nEnde )
/*?*/       {
/*?*/           // loesche jetzt die Nodes in das NodesArary
/*?*/           GetNodes().Delete( aSttIdx, nEnde - aSttIdx.GetIndex() );
/*?*/       }
/*?*/
/*?*/       // falls der Node geloescht wurde, in dem der Cursor stand, so
/*?*/       // muss der Content im akt. Content angemeldet werden !!!
/*?*/       pStt->nContent.Assign( pStt->nNode.GetNode().GetCntntNode(),
/*?*/                               pStt->nContent.GetIndex() );
/*?*/
/*?*/       // der PaM wird korrigiert, denn falls ueber Nodegrenzen geloescht
/*?*/       // wurde, so stehen sie in unterschieden Nodes. Auch die Selektion
/*?*/       // wird aufgehoben !
/*?*/       *pEnd = *pStt;
/*?*/       rPam.DeleteMark();
/*?*/
/*?*/   } while( sal_False );
/*?*/
/*?*/   SetModified();
/*?*/
/*?*/   return sal_True;
/*N*/ }

/*N*/ sal_Bool SwDoc::DelFullPara( SwPaM& rPam )
/*N*/ {
/*N*/   const SwPosition &rStt = *rPam.Start(), &rEnd = *rPam.End();
/*N*/   const SwNode* pNd = &rStt.nNode.GetNode();
/*N*/   sal_uInt32 nSectDiff = pNd->FindStartNode()->EndOfSectionIndex() -
/*N*/                       pNd->StartOfSectionIndex();
/*N*/   sal_uInt32 nNodeDiff = rEnd.nNode.GetIndex() - rStt.nNode.GetIndex();
/*N*/
/*N*/       if ( nSectDiff-2 <= nNodeDiff || IsRedlineOn() ||
/*N*/        /* #i9185# Prevent getting the node after the end node (see below) */
/*N*/       rEnd.nNode.GetIndex() + 1 == aNodes.Count() )
/*N*/       return sal_False;
/*N*/
    /* #i9185# This whould lead to a segmentation fault if not catched
       above. */
/*N*/   ULONG nNextNd = rEnd.nNode.GetIndex() + 1;
/*N*/   SwTableNode* pTblNd = aNodes[ nNextNd ]->GetTableNode();
/*N*/
/*N*/   if( pTblNd && pNd->IsCntntNode() )
/*N*/   {
/*?*/       SwFrmFmt* pTableFmt = pTblNd->GetTable().GetFrmFmt();
/*?*/       {
/*?*/           const SfxPoolItem *pItem;
/*?*/           const SfxItemSet* pSet = ((SwCntntNode*)pNd)->GetpSwAttrSet();
/*?*/           if( pSet && SFX_ITEM_SET == pSet->GetItemState( RES_PAGEDESC,
/*?*/               sal_False, &pItem ) )
/*?*/           {
/*?*/               pTableFmt->SetAttr( *pItem );
/*?*/           }
/*?*/
/*?*/           if( pSet && SFX_ITEM_SET == pSet->GetItemState( RES_BREAK,
/*?*/               sal_False, &pItem ) )
/*?*/           {
/*?*/               pTableFmt->SetAttr( *pItem );
/*?*/           }
/*?*/       }
/*N*/   }
/*N*/
/*N*/   {
/*N*/       SwNodeRange aRg( rStt.nNode, rEnd.nNode );
/*N*/       if( rPam.GetPoint() != &rEnd )
/*?*/           rPam.Exchange();
/*N*/
/*N*/       // versuche hinters Ende zu verschieben
/*N*/       if( !rPam.Move( fnMoveForward, fnGoNode ) )
/*N*/       {
/*N*/           // na gut, dann an den Anfang
/*N*/           rPam.Exchange();
/*N*/           if( !rPam.Move( fnMoveBackward, fnGoNode ))
/*N*/           {
/*N*/               OSL_FAIL( "kein Node mehr vorhanden" );
/*N*/               return sal_False;
/*N*/           }
/*N*/       }
/*N*/           // text::Bookmarks usw. verschieben
/*N*/       CorrAbs( aRg.aStart, aRg.aEnd, *rPam.GetPoint(), sal_True );
/*N*/
/*N*/           // was ist mit Fly's ??
/*N*/       {
/*N*/           // stehen noch FlyFrames rum, loesche auch diese
/*N*/           const SwPosition* pAPos;
/*N*/           for( sal_uInt16 n = 0; n < GetSpzFrmFmts()->Count(); ++n )
/*N*/           {
/*N*/               SwFrmFmt* pFly = (*GetSpzFrmFmts())[n];
/*N*/               const SwFmtAnchor* pAnchor = &pFly->GetAnchor();
/*N*/               if( ( FLY_AT_CNTNT == pAnchor->GetAnchorId() ||
/*N*/                     FLY_AUTO_CNTNT == pAnchor->GetAnchorId() ) &&
/*N*/                   0 != ( pAPos = pAnchor->GetCntntAnchor() ) &&
/*N*/                   aRg.aStart <= pAPos->nNode && pAPos->nNode <= aRg.aEnd )
/*N*/               {
/*?*/                   DelLayoutFmt( pFly );
/*?*/                   --n;
/*N*/               }
/*N*/           }
/*N*/       }
/*N*/
/*N*/       rPam.GetBound( TRUE ).nContent.Assign( 0, 0 );
/*N*/       rPam.GetBound( FALSE ).nContent.Assign( 0, 0 );
/*N*/       GetNodes().Delete( aRg.aStart, nNodeDiff+1 );
/*N*/   }
/*N*/   rPam.DeleteMark();
/*N*/   SetModified();
/*N*/
/*N*/   return sal_True;
/*N*/ }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
