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

#include <bookmrk.hxx>

#include <horiornt.hxx>

#include <doc.hxx>
#include <txatbase.hxx>
#include <ndhints.hxx>
#include <ndtxt.hxx>
#include <unocrsr.hxx>
#include <docary.hxx>
#include <fmthbsh.hxx>
#include <tox.hxx>
#include <unoclbck.hxx>

#include <osl/diagnose.h>

#include <unoobj.hxx>
#include <unoredline.hxx>
#include <fmtanchr.hxx>
#include <unoidx.hxx>
#include <redline.hxx>
#include <crsskip.hxx>
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;

using rtl::OUString;

/******************************************************************
 *  SwXTextPortionEnumeration
 ******************************************************************/

const uno::Sequence< sal_Int8 > & SwXTextPortionEnumeration::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::binfilter::CreateUnoTunnelId();
    return aSeq;
}

sal_Int64 SAL_CALL SwXTextPortionEnumeration::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return 0;
}

OUString SwXTextPortionEnumeration::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextPortionEnumeration");
}

BOOL SwXTextPortionEnumeration::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    return C2U("com.sun.star.text.TextPortionEnumeration") == rServiceName;
}

Sequence< OUString > SwXTextPortionEnumeration::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextPortionEnumeration");
    return aRet;
}

SwXTextPortionEnumeration::SwXTextPortionEnumeration(
    SwPaM& rParaCrsr,
    uno::Reference< XText >  xParentText,
    sal_Int32 nStart,
    sal_Int32 nEnd
    ) :
    xParent(xParentText),
    bAtEnd(sal_False),
    bFirstPortion(sal_True),
    nStartPos(nStart),
    nEndPos(nEnd)
{
    SwUnoCrsr* pUnoCrsr = rParaCrsr.GetDoc()->CreateUnoCrsr(*rParaCrsr.GetPoint(), sal_False);
    pUnoCrsr->Add(this);

    DBG_ASSERT(nEnd == -1 || (nStart <= nEnd &&
        nEnd <= pUnoCrsr->Start()->nNode.GetNode().GetTxtNode()->GetTxt().Len()),
            "start or end value invalid!");
    //alle Rahmen, Grafiken und OLEs suchen, die an diesem Absatz
    // AM ZEICHEN gebunden sind
    ::binfilter::CollectFrameAtNode( *this, pUnoCrsr->GetPoint()->nNode,
                            aFrameArr, TRUE );
    CreatePortions();
}

SwXTextPortionEnumeration::~SwXTextPortionEnumeration()
{
    for(sal_uInt16 nFrame = aFrameArr.Count(); nFrame; )
        delete aFrameArr.GetObject( --nFrame );
    aFrameArr.Remove(0, aFrameArr.Count());

    if( aPortionArr.Count() )
        aPortionArr.DeleteAndDestroy(0, aPortionArr.Count() );

    SwUnoCrsr* pUnoCrsr = GetCrsr();
    delete pUnoCrsr;
}

sal_Bool SwXTextPortionEnumeration::hasMoreElements(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    return aPortionArr.Count() > 0;
}

uno::Any SwXTextPortionEnumeration::nextElement(void)
    throw( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    if(!aPortionArr.Count())
        throw container::NoSuchElementException();
    XTextRangeRefPtr pPortion = aPortionArr.GetObject(0);
    Any aRet(pPortion, ::getCppuType((uno::Reference<XTextRange>*)0));
    aPortionArr.Remove(0);
    delete pPortion;
    return aRet;
}

void lcl_InsertRefMarkPortion(
    XTextRangeArr& rArr, SwUnoCrsr* pUnoCrsr, Reference<XText>& rParent, SwTxtAttr* pAttr, BOOL bEnd)
{
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    SwFmtRefMark& rRefMark = ((SwFmtRefMark&)pAttr->GetAttr());
    Reference<XTextContent> xContent = ((SwUnoCallBack*)pDoc->GetUnoCallBack())->GetRefMark(rRefMark);
    if(!xContent.is())
        xContent = new SwXReferenceMark(pDoc, &rRefMark);

    SwXTextPortion* pPortion;
    if(!bEnd)
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(pUnoCrsr, rParent, PORTION_REFMARK_START)),
            rArr.Count());
        pPortion->SetRefMark(xContent);
        pPortion->SetCollapsed(pAttr->GetEnd() ? FALSE : TRUE);
    }
    else
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(pUnoCrsr, rParent, PORTION_REFMARK_END)),
            rArr.Count());
        pPortion->SetRefMark(xContent);
    }
}
//-----------------------------------------------------------------------------
void lcl_InsertRubyPortion( XTextRangeArr& rArr, SwUnoCrsr* pUnoCrsr,
                        Reference<XText>& rParent, SwTxtAttr* pAttr, BOOL bEnd)
{
    SwXRubyPortion* pPortion;
    rArr.Insert(
        new Reference< XTextRange >(pPortion = new SwXRubyPortion(*pUnoCrsr, *(SwTxtRuby*)pAttr, rParent,
            bEnd)),
        rArr.Count());
    pPortion->SetCollapsed(pAttr->GetEnd() ? FALSE : TRUE);
}
//-----------------------------------------------------------------------------
void lcl_InsertTOXMarkPortion(
    XTextRangeArr& rArr, SwUnoCrsr* pUnoCrsr, Reference<XText>& rParent, SwTxtAttr* pAttr, BOOL bEnd)
{
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    SwTOXMark& rTOXMark = ((SwTOXMark&)pAttr->GetAttr());

    Reference<XTextContent> xContent =
        ((SwUnoCallBack*)pDoc->GetUnoCallBack())->GetTOXMark(rTOXMark);
    if(!xContent.is())
        xContent = new SwXDocumentIndexMark(rTOXMark.GetTOXType(), &rTOXMark, pDoc);

    SwXTextPortion* pPortion;
    if(!bEnd)
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(pUnoCrsr, rParent, PORTION_TOXMARK_START)),
            rArr.Count());
        pPortion->SetTOXMark(xContent);
        pPortion->SetCollapsed(pAttr->GetEnd() ? FALSE : TRUE);
    }
    if(bEnd)
    {
        rArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXTextPortion(pUnoCrsr, rParent, PORTION_TOXMARK_END)),
            rArr.Count());
        pPortion->SetTOXMark(xContent);
    }
}
//-----------------------------------------------------------------------------
#define BKM_TYPE_START          0
#define BKM_TYPE_END            1
#define BKM_TYPE_START_END      2
struct SwXBookmarkPortion_Impl
{
    SwXBookmark*                pBookmark;
    Reference<XTextContent>     xBookmark;
    BYTE                        nBkmType;
    ULONG                       nIndex;

    SwXBookmarkPortion_Impl( SwXBookmark* pXMark, BYTE nType, ULONG nIdx) :
        pBookmark(pXMark),
        xBookmark(pXMark),
        nBkmType(nType),
        nIndex(nIdx){}

    // compare by nIndex
    BOOL operator < (const SwXBookmarkPortion_Impl &rCmp) const;
    BOOL operator ==(const SwXBookmarkPortion_Impl &rCmp) const;

};
BOOL SwXBookmarkPortion_Impl::operator ==(const SwXBookmarkPortion_Impl &rCmp) const
{
    return nIndex == rCmp.nIndex &&
        pBookmark == rCmp.pBookmark &&
            nBkmType == rCmp.nBkmType;
}

BOOL SwXBookmarkPortion_Impl::operator < (const SwXBookmarkPortion_Impl &rCmp) const
{
    return nIndex < rCmp.nIndex;
}

typedef SwXBookmarkPortion_Impl* SwXBookmarkPortion_ImplPtr;
SV_DECL_PTRARR_SORT(SwXBookmarkPortionArr, SwXBookmarkPortion_ImplPtr, 0, 2)
SV_IMPL_OP_PTRARR_SORT(SwXBookmarkPortionArr, SwXBookmarkPortion_ImplPtr)
//-----------------------------------------------------------------------------
void lcl_ExportBookmark(
    SwXBookmarkPortionArr& rBkmArr, ULONG nIndex,
    SwUnoCrsr* pUnoCrsr, Reference<XText> & rParent, XTextRangeArr& rPortionArr)
{
    SwXBookmarkPortion_ImplPtr pPtr;
    while(rBkmArr.Count() && nIndex == (pPtr = rBkmArr.GetObject(0))->nIndex)
    {
        SwXTextPortion* pPortion;
        if(BKM_TYPE_START == pPtr->nBkmType || BKM_TYPE_START_END == pPtr->nBkmType)
        {
            rPortionArr.Insert(
                new Reference< XTextRange >(pPortion = new SwXTextPortion(pUnoCrsr, rParent, PORTION_BOOKMARK_START)),
                rPortionArr.Count());
            pPortion->SetBookmark(pPtr->xBookmark);
            pPortion->SetCollapsed(BKM_TYPE_START_END == pPtr->nBkmType ? TRUE : FALSE);

        }
        if(BKM_TYPE_END == pPtr->nBkmType)
        {
            rPortionArr.Insert(
                new Reference< XTextRange >(pPortion = new SwXTextPortion(pUnoCrsr, rParent, PORTION_BOOKMARK_END)),
                rPortionArr.Count());
            pPortion->SetBookmark(pPtr->xBookmark);
        }
        rBkmArr.Remove((USHORT)0);
        delete pPtr;
    }
}

//-----------------------------------------------------------------------------
#define REDLINE_PORTION_START_REMOVE 0//removed redlines are visible
#define REDLINE_PORTION_END_REMOVE   1//removed redlines are visible
#define REDLINE_PORTION_REMOVE       2//removed redlines are NOT visible
#define REDLINE_PORTION_INSERT_START 3
#define REDLINE_PORTION_INSERT_END   4

struct SwXRedlinePortion_Impl
{
    const SwRedline*    pRedline;
    sal_Bool            bStart;

    SwXRedlinePortion_Impl(const SwRedline* pRed, sal_Bool bIsStart) :
        pRedline(pRed),
        bStart(bIsStart)
        {}

    USHORT GetIndexPos()
    {
        const SwPosition* pOwnPos = bStart ?
                    pRedline->Start() : pRedline->End();
        return pOwnPos->nContent.GetIndex();
    }
    // compare by Position
    BOOL operator < (const SwXRedlinePortion_Impl &rCmp) const
        {   const SwPosition* pOwnPos = bStart ?
                    pRedline->Start() : pRedline->End();
            const SwPosition* pCmpPos = rCmp.bStart ?
                rCmp.pRedline->Start() : rCmp.pRedline->End();
            return *pOwnPos <= *pCmpPos;
        }

    BOOL operator ==(const SwXRedlinePortion_Impl &rCmp) const
        {return pRedline == rCmp.pRedline &&
                    bStart == rCmp.bStart;}
};
typedef SwXRedlinePortion_Impl* SwXRedlinePortion_ImplPtr;
SV_DECL_PTRARR_SORT(SwXRedlinePortionArr, SwXRedlinePortion_ImplPtr, 0, 2)
SV_IMPL_OP_PTRARR_SORT(SwXRedlinePortionArr, SwXRedlinePortion_ImplPtr)

//-----------------------------------------------------------------------------
Reference<XTextRange> lcl_ExportHints(SwpHints* pHints,
                                XTextRangeArr& rPortionArr,
                                SwUnoCrsr* pUnoCrsr,
                                Reference<XText> & rParent,
                                const xub_StrLen nCurrentIndex,
                                SwTextPortionType& ePortionType,
                                const xub_StrLen& nFirstFrameIndex,
                                SwXBookmarkPortionArr& aBkmArr,
                                SwXRedlinePortionArr& aRedArr,
                                sal_Int32 nEndPos )
{
    Reference<XTextRange> xRef;
    SwDoc* pDoc = pUnoCrsr->GetDoc();
    sal_Bool bAttrFound = sal_False;
    //search for special text attributes - first some ends
    sal_uInt16 nEndIndex = 0;
    sal_uInt16 nNextEnd = 0;
    while(nEndIndex < pHints->GetEndCount() &&
        (!pHints->GetEnd(nEndIndex)->GetEnd() ||
        nCurrentIndex >= (nNextEnd = (*pHints->GetEnd(nEndIndex)->GetEnd()))))
    {
        if(pHints->GetEnd(nEndIndex)->GetEnd())
        {
            SwTxtAttr* pAttr = pHints->GetEnd(nEndIndex);
            USHORT nAttrWhich = pAttr->Which();
            if(nNextEnd == nCurrentIndex &&
                ( RES_TXTATR_TOXMARK == nAttrWhich ||
                    RES_TXTATR_REFMARK == nAttrWhich ||
                        RES_TXTATR_CJK_RUBY == nAttrWhich))
            {
                switch( nAttrWhich )
                {
                    case RES_TXTATR_TOXMARK:
                        lcl_InsertTOXMarkPortion(
                            rPortionArr, pUnoCrsr, rParent, pAttr, TRUE);
                        ePortionType = PORTION_TEXT;
                    break;
                    case RES_TXTATR_REFMARK:
                        lcl_InsertRefMarkPortion(
                            rPortionArr, pUnoCrsr, rParent, pAttr, TRUE);
                        ePortionType = PORTION_TEXT;
                    break;
                    case RES_TXTATR_CJK_RUBY:
                        lcl_InsertRubyPortion(
                            rPortionArr, pUnoCrsr, rParent, pAttr, TRUE);
                        ePortionType = PORTION_TEXT;
                    break;
                }
            }
        }
        nEndIndex++;
    }
    //then som starts
    sal_uInt16 nStartIndex = 0;
    sal_uInt16 nNextStart = 0;
    while(nStartIndex < pHints->GetStartCount() &&
        nCurrentIndex >= (nNextStart = (*pHints->GetStart(nStartIndex)->GetStart())))
    {
        SwTxtAttr* pAttr = pHints->GetStart(nStartIndex);
        USHORT nAttrWhich = pAttr->Which();
        if(nNextStart == nCurrentIndex &&
            (!pAttr->GetEnd() ||
                RES_TXTATR_TOXMARK == nAttrWhich ||
                    RES_TXTATR_REFMARK == nAttrWhich||
                        RES_TXTATR_CJK_RUBY == nAttrWhich))
        {
            switch( nAttrWhich )
            {
                case RES_TXTATR_FIELD:
                    pUnoCrsr->Right(1,CRSR_SKIP_CHARS);
                    bAttrFound = sal_True;
                    ePortionType = PORTION_FIELD;
                break;
                case RES_TXTATR_FLYCNT   :
                    pUnoCrsr->Right(1,CRSR_SKIP_CHARS);
                    pUnoCrsr->Exchange();
                    bAttrFound = sal_True;
                    ePortionType = PORTION_FRAME;
                break;
                case RES_TXTATR_FTN      :
                {
                    pUnoCrsr->Right(1,CRSR_SKIP_CHARS);
                    SwXTextPortion* pPortion;
                    xRef =  pPortion = new SwXTextPortion(pUnoCrsr, rParent, PORTION_FOOTNOTE);
                    Reference<XTextContent> xContent =
                        Reference<XTextContent>(
                        SwXFootnotes::GetObject(*pDoc, pAttr->SwTxtAttr::GetFtn()),
                        UNO_QUERY);
                    pPortion->SetFootnote(xContent);
                    bAttrFound = sal_True;
                    ePortionType = PORTION_TEXT;
                }
                break;
                case RES_TXTATR_SOFTHYPH :
                {
                    SwXTextPortion* pPortion;
                    rPortionArr.Insert(
                        new Reference< XTextRange >(
                            pPortion = new SwXTextPortion(
                                pUnoCrsr, rParent, PORTION_CONTROL_CHAR)),
                        rPortionArr.Count());
                    pPortion->SetControlChar(3);
                    ePortionType = PORTION_TEXT;
                }
                break;
                case RES_TXTATR_HARDBLANK:
                {
                    ePortionType = PORTION_CONTROL_CHAR;
                    SwXTextPortion* pPortion;
                    rPortionArr.Insert(
                        new Reference< XTextRange >(
                            pPortion = new SwXTextPortion(
                                pUnoCrsr, rParent, PORTION_CONTROL_CHAR)),
                        rPortionArr.Count());
                    const SwFmtHardBlank& rFmt = pAttr->GetHardBlank();
                    if(rFmt.GetChar() == '-')
                        pPortion->SetControlChar(2);//HARD_HYPHEN
                    else
                        pPortion->SetControlChar(4);//HARD_SPACE
                    ePortionType = PORTION_TEXT;
                }
                break;
                case RES_TXTATR_TOXMARK:
                    lcl_InsertTOXMarkPortion(
                        rPortionArr, pUnoCrsr, rParent, pAttr, FALSE);
                    ePortionType = PORTION_TEXT;
                break;
                case RES_TXTATR_REFMARK:
                    if(!pAttr->GetEnd())
                    {
                        pUnoCrsr->Right(1,CRSR_SKIP_CHARS);
                        bAttrFound = sal_True;
                    }
                    lcl_InsertRefMarkPortion(
                        rPortionArr, pUnoCrsr, rParent, pAttr, FALSE);
                    ePortionType = PORTION_TEXT;
                    if(!pAttr->GetEnd())
                    {
                        if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                                pUnoCrsr->Exchange();
                        pUnoCrsr->DeleteMark();
                    }
                break;
                case RES_TXTATR_CJK_RUBY:
                    if(pAttr->GetEnd())
                    {
                        lcl_InsertRubyPortion(
                            rPortionArr, pUnoCrsr, rParent, pAttr, FALSE);
                        ePortionType = PORTION_TEXT;
                    }
                break;
                default:
                    OSL_FAIL("was fuer ein Attribut?");
            }

        }
        nStartIndex++;
    }

    if(!bAttrFound)
    {
        // search for attribute changes behind the current cursor position
        // break up at frames, bookmarks, redlines

        nStartIndex = 0;
        nNextStart = 0;
        while(nStartIndex < pHints->GetStartCount() &&
            nCurrentIndex >= (nNextStart = (*pHints->GetStart(nStartIndex)->GetStart())))
            nStartIndex++;

        nEndIndex = 0;
        nNextEnd = 0;
        while(nEndIndex < pHints->GetEndCount() &&
            nCurrentIndex >= (nNextEnd = (*pHints->GetEnd(nEndIndex)->GetAnyEnd())))
            nEndIndex++;
        //nMovePos legt die neue EndPosition fest
        sal_uInt16 nMovePos = nNextStart > nCurrentIndex && nNextStart < nNextEnd ? nNextStart : nNextEnd;
        if (nMovePos <= nCurrentIndex)
            nMovePos = pUnoCrsr->GetCntntNode()->Len();

        if(nEndPos > 0 && nMovePos > nEndPos)
            nMovePos = (USHORT)nEndPos;

        if(aBkmArr.Count() && aBkmArr.GetObject(0)->nIndex < nMovePos)
        {
            DBG_ASSERT(aBkmArr.GetObject(0)->nIndex > nCurrentIndex,
                "forgotten bookmark(s)");
            nMovePos = aBkmArr.GetObject(0)->nIndex;
        }
        // break up portions for redlines
        if (aRedArr.Count() && aRedArr.GetObject(0)->GetIndexPos() < nMovePos)
        {
            nMovePos = aRedArr.GetObject(0)->GetIndexPos();
        }
        // break up if the destination is behind a frame
        if(nFirstFrameIndex != STRING_MAXLEN && nMovePos > nFirstFrameIndex)
            nMovePos = nFirstFrameIndex;

        if(nMovePos > nCurrentIndex)
//          pUnoCrsr->Right(nMovePos - nCurrentIndex);
            pUnoCrsr->GetPoint()->nContent = nMovePos;
        else
        {
            // ensure proper exit: move to paragraph end
            // (this should not be necessary any more; we assert it only
            //  happens when the above would move to the end of the
            //  paragraph anyway)
            DBG_ASSERT(nMovePos == pUnoCrsr->GetCntntNode()->Len()||
            (nEndPos > 0 && nMovePos == nEndPos),
                       "may only happen at paragraph end");
            pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
        }
    }
    return xRef;
}
//-----------------------------------------------------------------------------
void lcl_FillBookmarkArray(SwDoc& rDoc,SwUnoCrsr& rUnoCrsr, SwXBookmarkPortionArr& rBkmArr )
{
    const SwBookmarks& rMarks = rDoc.GetBookmarks();
    sal_uInt16 nArrLen = rMarks.Count();
    if ( nArrLen > 0 )
    {
        const SwNodeIndex nOwnNode = rUnoCrsr.GetPoint()->nNode;
        //search for all bookmarks that start or end in this paragraph
        for( sal_uInt16 n = 0; n < nArrLen; ++n )
        {
            SwBookmark* pMark = rMarks.GetObject( n );
            if( !pMark->IsBookMark() )
                continue;

            const SwPosition& rPos1 = pMark->GetPos();
            const SwPosition* pPos2 = pMark->GetOtherPos();
            BOOL bBackward = pPos2 ? rPos1 > *pPos2: FALSE;
            if(rPos1.nNode == nOwnNode)
            {
                BYTE nType = bBackward ? BKM_TYPE_END : BKM_TYPE_START;
                if(!pPos2)
                {
                    nType = BKM_TYPE_START_END;
                }
                SwXBookmarkPortion_ImplPtr pBkmPtr = new SwXBookmarkPortion_Impl(
                    SwXBookmarks::GetObject( *pMark, &rDoc ), nType, rPos1.nContent.GetIndex() );

                rBkmArr.Insert(pBkmPtr);

            }
            if(pPos2 && pPos2->nNode == nOwnNode)
            {
                BYTE nType = bBackward ? BKM_TYPE_START : BKM_TYPE_END;
                SwXBookmarkPortion_ImplPtr pBkmPtr = new SwXBookmarkPortion_Impl(
                        SwXBookmarks::GetObject( *pMark, &rDoc ), nType, pPos2->nContent.GetIndex() );
                rBkmArr.Insert(pBkmPtr);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void lcl_FillRedlineArray(SwDoc& rDoc,SwUnoCrsr& rUnoCrsr, SwXRedlinePortionArr& rRedArr )
{
    const SwRedlineTbl& rRedTbl = rDoc.GetRedlineTbl();
    USHORT nRedTblCount = rRedTbl.Count();

    if ( nRedTblCount > 0 )
    {
        const SwPosition* pStart = rUnoCrsr.GetPoint();
        const SwNodeIndex nOwnNode = pStart->nNode;
        rDoc.GetRedlineMode();

        for(USHORT nRed = 0; nRed < nRedTblCount; nRed++)
        {
            const SwRedline* pRedline = rRedTbl[nRed];
            const SwPosition* pRedStart = pRedline->Start();
            const SwNodeIndex nRedNode = pRedStart->nNode;
            if(nOwnNode == nRedNode)
            {
                SwXRedlinePortion_ImplPtr pToInsert = new SwXRedlinePortion_Impl(pRedline, TRUE);
                rRedArr.Insert(pToInsert);
            }
            if(pRedline->HasMark() && pRedline->End()->nNode == nOwnNode)
            {
                SwXRedlinePortion_ImplPtr pToInsert = new SwXRedlinePortion_Impl(pRedline, FALSE);
                rRedArr.Insert(pToInsert);
            }
        }
    }
}

void lcl_ExportRedline(
    SwXRedlinePortionArr& rRedlineArr, ULONG nIndex,
    SwUnoCrsr* pUnoCrsr, Reference<XText> & rParent, XTextRangeArr& rPortionArr)
{
    SwXRedlinePortion_ImplPtr pPtr;
    while(rRedlineArr.Count() &&  0 != (pPtr = rRedlineArr.GetObject(0)) &&
        ((pPtr->bStart && (nIndex == pPtr->pRedline->Start()->nContent.GetIndex()))||
            (!pPtr->bStart && (nIndex == pPtr->pRedline->End()->nContent.GetIndex()))))
    {
        SwXTextPortion* pPortion;
        rPortionArr.Insert(
            new Reference< XTextRange >(pPortion = new SwXRedlinePortion(
                        pPtr->pRedline, *pUnoCrsr, rParent,
                        pPtr->bStart)),
            rPortionArr.Count());
        rRedlineArr.Remove((USHORT)0);
        delete pPtr;
    }
}

void lcl_ExportBkmAndRedline(
    SwXBookmarkPortionArr& rBkmArr,
    SwXRedlinePortionArr& rRedlineArr, ULONG nIndex,
    SwUnoCrsr* pUnoCrsr, Reference<XText> & rParent, XTextRangeArr& rPortionArr)
{
    if (rBkmArr.Count())
        lcl_ExportBookmark(rBkmArr, nIndex, pUnoCrsr, rParent, rPortionArr);

    if (rRedlineArr.Count())
        lcl_ExportRedline(rRedlineArr, nIndex, pUnoCrsr, rParent, rPortionArr);
}
//-----------------------------------------------------------------------------
sal_Int32 lcl_GetNextIndex(SwXBookmarkPortionArr& rBkmArr, SwXRedlinePortionArr& rRedlineArr)
{
    sal_Int32 nRet = -1;
    if(rBkmArr.Count())
    {
        SwXBookmarkPortion_ImplPtr pPtr = rBkmArr.GetObject(0);
        nRet = pPtr->nIndex;
    }
    if(rRedlineArr.Count())
    {
        SwXRedlinePortion_ImplPtr pPtr = rRedlineArr.GetObject(0);
        USHORT nTmp = pPtr->GetIndexPos();
        if(nRet < 0 || nTmp < nRet)
            nRet = nTmp;
    }
    return nRet;
};
//-----------------------------------------------------------------------------
void SwXTextPortionEnumeration::CreatePortions()
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    // set the start if a selection should be exported
    if(nStartPos > 0 && pUnoCrsr->Start()->nContent.GetIndex() != nStartPos)
    {
        if(pUnoCrsr->HasMark())
            pUnoCrsr->DeleteMark();
        DBG_ASSERT(pUnoCrsr->Start()->nNode.GetNode().GetTxtNode() &&
            nStartPos <= pUnoCrsr->Start()->nNode.GetNode().GetTxtNode()->GetTxt().Len(),
                "Incorrect start position"  );
        pUnoCrsr->Right((xub_StrLen)nStartPos,CRSR_SKIP_CHARS);
    }
    if(pUnoCrsr /*&& !bAtEnd*/)
    {
        SwXBookmarkPortionArr aBkmArr;
        SwXRedlinePortionArr aRedArr;

        SwDoc* pDoc = pUnoCrsr->GetDoc();
        lcl_FillRedlineArray(*pDoc, *pUnoCrsr, aRedArr);
        lcl_FillBookmarkArray(*pDoc, *pUnoCrsr, aBkmArr );

        while(!bAtEnd)
        {
            if(pUnoCrsr->HasMark())
            {
                if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                    pUnoCrsr->Exchange();
                pUnoCrsr->DeleteMark();
            }
            SwNode* pNode = pUnoCrsr->GetNode();
            SwCntntNode *pCNd = pNode->GetCntntNode();
            if(!bFirstPortion   && pCNd &&
                    pUnoCrsr->GetPoint()->nContent == pCNd->Len())
            {
                //hier sollte man nie ankommen!
                bAtEnd = sal_True;
            }
            else
            {
                if(ND_TEXTNODE == pNode->GetNodeType())
                {
                    SwTxtNode* pTxtNode = (SwTxtNode*)pNode;
                    SwpHints* pHints = pTxtNode->GetpSwpHints();
                    SwTextPortionType ePortionType = PORTION_TEXT;
                    xub_StrLen nCurrentIndex = pUnoCrsr->GetPoint()->nContent.GetIndex();
                    xub_StrLen nFirstFrameIndex = STRING_MAXLEN;
                    uno::Reference< XTextRange >  xRef;
                    if(!pCNd->Len())
                    {
                        lcl_ExportBkmAndRedline(aBkmArr, aRedArr, 0, pUnoCrsr, xParent, aPortionArr);
                        // the paragraph is empty
                        xRef = new SwXTextPortion(pUnoCrsr, xParent, ePortionType);
                        // are there any frames?
                        while(aFrameArr.Count())
                        {
                            SwDepend* pCurDepend = aFrameArr.GetObject(0);
                            if(pCurDepend->GetRegisteredIn())
                            {
                                //the previously created portion has to be inserted here
                                aPortionArr.Insert(new Reference<XTextRange>(xRef), aPortionArr.Count());
                                xRef = new SwXTextPortion(pUnoCrsr, xParent,
                                    *(SwFrmFmt*)pCurDepend->GetRegisteredIn());
                            }
                            delete pCurDepend;
                            aFrameArr.Remove(0);
                        }
                    }
                    else
                    {
                        //falls schon Rahmen entsorgt wurden, dann raus hier
                        for(sal_uInt16 nFrame = aFrameArr.Count(); nFrame; nFrame--)
                        {
                            SwDepend* pCurDepend = aFrameArr.GetObject(nFrame - 1);
                            if(!pCurDepend->GetRegisteredIn())
                            {
                                delete pCurDepend;
                                aFrameArr.Remove(nFrame - 1);
                            }
                        }

                        //zunaechst den ersten Frame im aFrameArr finden (bezogen auf die Position im Absatz)
                        SwDepend* pFirstFrameDepend = 0;
                        //Eintraege im aFrameArr sind sortiert!
                        if(aFrameArr.Count())
                        {
                            SwDepend* pCurDepend = aFrameArr.GetObject(0);
                            SwFrmFmt* pFormat = (SwFrmFmt*)pCurDepend->GetRegisteredIn();
                            const SwFmtAnchor& rAnchor = pFormat->GetAnchor();
                            const SwPosition* pAnchorPos = rAnchor.GetCntntAnchor();
                            pFirstFrameDepend = pCurDepend;
                            nFirstFrameIndex = pAnchorPos->nContent.GetIndex();
                            if(nEndPos > 0 && nFirstFrameIndex >= nEndPos)
                                nFirstFrameIndex = USHRT_MAX;
                        }

                        SwXTextCursor::SelectPam(*pUnoCrsr, sal_True);

                        //ist hier schon ein Rahmen faellig?
                        if(nCurrentIndex == nFirstFrameIndex)
                        {
                            xRef = new SwXTextPortion(pUnoCrsr, xParent,
                                *(SwFrmFmt*)pFirstFrameDepend->GetRegisteredIn());
                            SwDepend* pCurDepend = aFrameArr.GetObject(0);
                            delete pCurDepend;
                            aFrameArr.Remove(0);
                        }
                    }
                    if(!xRef.is())
                    {
                        lcl_ExportBkmAndRedline(aBkmArr, aRedArr, nCurrentIndex, pUnoCrsr, xParent, aPortionArr);
                        if(pHints)
                        {
                            xRef = lcl_ExportHints(pHints,
                                aPortionArr,
                                pUnoCrsr,
                                xParent,
                                nCurrentIndex,
                                ePortionType,
                                nFirstFrameIndex,
                                aBkmArr,
                                aRedArr,
                                nEndPos);

                        }
                        else if(USHRT_MAX != nFirstFrameIndex)
                        {
                            pUnoCrsr->Right(nFirstFrameIndex - nCurrentIndex,CRSR_SKIP_CHARS);
                        }
                        else
                        {
                            sal_Int32 nNextIndex = lcl_GetNextIndex(aBkmArr, aRedArr);
                            if(nEndPos > 0 && (nNextIndex > nEndPos || nNextIndex < 0))
                            {
                                nNextIndex = nEndPos;
                                bAtEnd = sal_True;
                            }
                            if(nNextIndex < 0)
                                pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
                            else
                            {
                                DBG_ASSERT(nNextIndex > nCurrentIndex, "wrong move index");
                                pUnoCrsr->Right(nNextIndex - nCurrentIndex,CRSR_SKIP_CHARS);
                            }
                        }
                    }
                    if(!xRef.is() && pUnoCrsr->HasMark() )
                        xRef = new SwXTextPortion(pUnoCrsr, xParent, ePortionType);
                    if(xRef.is())
                        aPortionArr.Insert(new Reference<XTextRange>(xRef), aPortionArr.Count());
                }
                else
                {
                    OSL_FAIL("kein TextNode - was nun?");
                }
            }
            if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                    pUnoCrsr->Exchange();

            // Absatzende ?
            pNode = pUnoCrsr->GetNode();
            pCNd = pNode->GetCntntNode();
            sal_Int32 nLocalEnd = nEndPos > 0 ? nEndPos : pCNd->Len();
            if( pCNd && pUnoCrsr->GetPoint()->nContent == (xub_StrLen)nLocalEnd)
            {
                bAtEnd = sal_True;
                lcl_ExportBkmAndRedline(aBkmArr, aRedArr, nLocalEnd,
                                            pUnoCrsr, xParent, aPortionArr);
                if(ND_TEXTNODE == pNode->GetNodeType())
                {
                    SwTxtNode* pTxtNode = (SwTxtNode*)pNode;
                    SwpHints* pHints = pTxtNode->GetpSwpHints();
                    if(pHints)
                    {
                        SwTextPortionType ePortionType = PORTION_TEXT;
                        Reference<XTextRange> xRef = lcl_ExportHints(pHints,
                            aPortionArr,
                            pUnoCrsr,
                            xParent,
                            nLocalEnd,
                            ePortionType,
                            STRING_MAXLEN,
                            aBkmArr,
                            aRedArr,
                            nEndPos);
                        if(xRef.is())
                            aPortionArr.Insert(new Reference<XTextRange>(xRef), aPortionArr.Count());
                    }
                }
                while(aFrameArr.Count())
                {
                    SwDepend* pCurDepend = aFrameArr.GetObject(0);
                    if(pCurDepend->GetRegisteredIn())
                    {
                        Reference<XTextRange> xRef = new SwXTextPortion(pUnoCrsr, xParent,
                            *(SwFrmFmt*)pCurDepend->GetRegisteredIn());
                        aPortionArr.Insert(new Reference<XTextRange>(xRef), aPortionArr.Count());
                    }
                    delete pCurDepend;
                    aFrameArr.Remove(0);
                }

            }
        }
    }
}

void    SwXTextPortionEnumeration::Modify( SfxPoolItem *pOld, SfxPoolItem *pNew)
{
    ClientModify(this, pOld, pNew);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
