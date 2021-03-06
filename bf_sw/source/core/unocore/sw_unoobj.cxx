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


#include <rtl/ustrbuf.hxx>
#include <swtypes.hxx>
#include <hintids.hxx>
#include <cmdid.h>
#include <hints.hxx>
#include <bookmrk.hxx>
#include <frmfmt.hxx>

#include <horiornt.hxx>

#include <doc.hxx>

#include <osl/diagnose.h>

#include <ndtxt.hxx>
#include <ndnotxt.hxx>
#include <unocrsrhelper.hxx>
#include <unocrsr.hxx>
#include <rootfrm.hxx>
#include <flyfrm.hxx>
#include <ftnidx.hxx>
#include <bf_so3/linkmgr.hxx>
#include <docary.hxx>
#include <paratr.hxx>
#include <tools/urlobj.hxx>
#include <pam.hxx>
#include <tools/cachestr.hxx>
#include <shellio.hxx>
#include <swerror.h>
#include <swtblfmt.hxx>
#include <fmtruby.hxx>
#include <fmthbsh.hxx>
#include <docsh.hxx>
#include <bf_svtools/style.hxx>
#include <docstyle.hxx>
#include <charfmt.hxx>
#include <txtfld.hxx>
#include <fmtfld.hxx>
#include <fmtpdsc.hxx>
#include <pagedesc.hxx>
#include <poolfmt.hrc>
#include <poolfmt.hxx>
#include <viscrs.hxx>
#include <fchrfmt.hxx>
#include <cntfrm.hxx>
#include <pagefrm.hxx>
#include <doctxm.hxx>
#include <tox.hxx>
#include <bf_sfx2/docfilt.hxx>
#include <bf_sfx2/docfile.hxx>
#include <bf_sfx2/fcontnr.hxx>
#include <fmtrfmrk.hxx>
#include <txtrfmrk.hxx>
#include <unoclbck.hxx>
#include <unoobj.hxx>
#include <unomap.hxx>
#include <unomid.h>
#include <unosett.hxx>
#include <unoprnms.hxx>
#include <unotbl.hxx>
#include <unodraw.hxx>
#include <unocoll.hxx>
#include <unostyle.hxx>
#include <unofield.hxx>
#include <fmtanchr.hxx>
#include <bf_svx/flstitem.hxx>
#include <bf_svtools/ctrltool.hxx>
#include <bf_svtools/eitem.hxx>
#include <txtftn.hxx>
#include <section.hxx>
#include <fmtftn.hxx>

#include <com/sun/star/text/WrapTextMode.hpp>
#include <com/sun/star/text/TextContentAnchorType.hpp>
#include <com/sun/star/style/PageStyleLayout.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <unoidx.hxx>
#include <unoframe.hxx>
#include <fmthdft.hxx>
#include <osl/mutex.hxx>
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
#include <fmtflcnt.hxx>
#define _SVSTDARR_USHORTS
#define _SVSTDARR_USHORTSSORT
#include <bf_svtools/svstdarr.hxx>
#include <bf_svx/brshitem.hxx>
#include <bf_svtools/stritem.hxx>
#include <fmtclds.hxx>
#include <rtl/uuid.h>
#include <dcontact.hxx>
#include <SwStyleNameMapper.hxx>
#include <crsskip.hxx>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <memory>
#include <osl/endian.h>
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::drawing;
using std::auto_ptr;
using namespace ::rtl;

//collectn.cxx
//BOOL lcl_IsNumeric(const String&);
/****************************************************************************
    static methods
****************************************************************************/
::com::sun::star::uno::Sequence< sal_Int8 >  CreateUnoTunnelId()
{
    static osl::Mutex aCreateMutex;
    osl::Guard<osl::Mutex> aGuard( aCreateMutex );
    uno::Sequence< sal_Int8 > aSeq( 16 );
    rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0, sal_True );
    return aSeq;
}
/****************************************************************************
    Hilfsklassen
****************************************************************************/

SwParaSelection::SwParaSelection(SwUnoCrsr* pCrsr) :
    pUnoCrsr(pCrsr)
{
    if(pUnoCrsr->HasMark())
        pUnoCrsr->DeleteMark();
    // steht er schon am Anfang?
    if(pUnoCrsr->GetPoint()->nContent != 0)
        pUnoCrsr->MovePara(fnParaCurr, fnParaStart);
    // oder gleichzeitig am Ende?
    if(pUnoCrsr->GetPoint()->nContent != pUnoCrsr->GetCntntNode()->Len())
    {
        pUnoCrsr->SetMark();
        pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
    }
}

SwParaSelection::~SwParaSelection()
{
    if(pUnoCrsr->GetPoint()->nContent != 0)
    {
        pUnoCrsr->DeleteMark();
        pUnoCrsr->MovePara(fnParaCurr, fnParaStart);
    }
}

SwUnoInternalPaM::SwUnoInternalPaM(SwDoc& rDoc) :
    SwPaM(rDoc.GetNodes())
{
}
SwUnoInternalPaM::~SwUnoInternalPaM()
{
    while( GetNext() != this)
    {
        delete GetNext();
    }
}

SwUnoInternalPaM&   SwUnoInternalPaM::operator=(const SwPaM& rPaM)
{
    const SwPaM* pTmp = &rPaM;
    *GetPoint() = *rPaM.GetPoint();
    if(rPaM.HasMark())
    {
        SetMark();
        *GetMark() = *rPaM.GetMark();
    }
    else
        DeleteMark();
    while(&rPaM != (pTmp = (const SwPaM*)pTmp->GetNext()))
    {
        if(pTmp->HasMark())
            new SwPaM(*pTmp->GetMark(), *pTmp->GetPoint(), this);
        else
            new SwPaM(*pTmp->GetPoint(), this);
    }
    return *this;
}

/****************************************************************************
    ActionContext
****************************************************************************/
UnoActionContext::UnoActionContext(SwDoc* pDc) :
    pDoc(pDc)
{
}

UnoActionContext::~UnoActionContext()
{
}

/****************************************************************************
    ActionRemoveContext
****************************************************************************/
UnoActionRemoveContext::UnoActionRemoveContext(SwDoc* pDc) :
    pDoc(pDc)
{
}

UnoActionRemoveContext::~UnoActionRemoveContext()
{
}

void SwXTextCursor::SelectPam(SwPaM& rCrsr, sal_Bool bExpand)
{
    if(bExpand)
    {
        if(!rCrsr.HasMark())
            rCrsr.SetMark();
    }
    else if(rCrsr.HasMark())
        rCrsr.DeleteMark();

}

void SwXTextCursor::getTextFromPam(SwPaM& aCrsr, OUString& rBuffer)
{
    if(!aCrsr.HasMark())
        return;
    SvCacheStream aStream( 20480 );
#ifdef OSL_BIGENDIAN
    aStream.SetNumberFormatInt( NUMBERFORMAT_INT_BIGENDIAN );
#else
    aStream.SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
#endif
    WriterRef xWrt = GetASCWriter(C2S(FILTER_TEXT_DLG));
    if( xWrt.Is() )
    {
        SwWriter aWriter( aStream, aCrsr );
        xWrt->bASCII_NoLastLineEnd = sal_True;
        SwAsciiOptions aOpt = xWrt->GetAsciiOptions();
        aOpt.SetCharSet( RTL_TEXTENCODING_UNICODE );
        xWrt->SetAsciiOptions( aOpt );
        xWrt->bUCS2_WithStartChar = FALSE;

        long lLen;
        if( !IsError( aWriter.Write( xWrt ) ) &&
                STRING_MAXLEN > (( lLen  = aStream.GetSize() )
                / sizeof( sal_Unicode )) + 1 )
        {
            aStream << (sal_Unicode)'\0';

            String sBuf;
            const sal_Unicode *p = (sal_Unicode*)aStream.GetBuffer();
            if( p )
                sBuf = p;
            else
            {
                long lUniLen = (lLen / sizeof( sal_Unicode ));
                sal_Unicode* pStrBuf = sBuf.AllocBuffer( xub_StrLen(
                lUniLen + 1));
                aStream.Seek( 0 );
                aStream.ResetError();
                aStream.Read( pStrBuf, lLen );
                pStrBuf[ lUniLen ] = '\0';
            }
            rBuffer = OUString( sBuf );
        }
    }
}

void lcl_setCharStyle(SwDoc* pDoc, const uno::Any aValue, SfxItemSet& rSet)
     throw (lang::IllegalArgumentException)
{
    SwDocShell* pDocSh = pDoc->GetDocShell();
    if(pDocSh)
    {
        OUString uStyle;
        aValue >>= uStyle;
        String sStyle;
        SwStyleNameMapper::FillUIName(uStyle, sStyle, GET_POOLID_CHRFMT, sal_True );
        SwDocStyleSheet* pStyle =
            (SwDocStyleSheet*)pDocSh->GetStyleSheetPool()->Find(sStyle, SFX_STYLE_FAMILY_CHAR);
        if(pStyle)
        {
            SwFmtCharFmt aFmt(pStyle->GetCharFmt());
            rSet.Put(aFmt);
        }
        else
        {
             throw lang::IllegalArgumentException();
        }

    }
};

void lcl_SetTxtFmtColl(const uno::Any& rAny, SwPaM& rPaM)
    throw (lang::IllegalArgumentException)
{
    SwDoc* pDoc = rPaM.GetDoc();
    SwDocShell* pDocSh = pDoc->GetDocShell();
    if(!pDocSh)
        return;
    OUString uStyle;
    rAny >>= uStyle;
    String sStyle;
    SwStyleNameMapper::FillUIName(uStyle, sStyle, GET_POOLID_TXTCOLL, sal_True );
    SwDocStyleSheet* pStyle =
                    (SwDocStyleSheet*)pDocSh->GetStyleSheetPool()->Find(sStyle, SFX_STYLE_FAMILY_PARA);
    if(pStyle)
    {
        SwTxtFmtColl *pLocal = pStyle->GetCollection();
        UnoActionContext aAction(pDoc);
        FOREACHUNOPAM_START(&rPaM)
            pDoc->SetTxtFmtColl(*PUNOPAM, pLocal);
        FOREACHUNOPAM_END()
    }
    else
    {
        throw lang::IllegalArgumentException();
    }

}

 void lcl_setPageDesc(SwDoc* pDoc, const uno::Any& aValue, SfxItemSet& rSet)
 {
    if(aValue.getValueType() != ::getCppuType((const OUString*)0))
        return;
    SwFmtPageDesc *pNewDesc = 0 ;
    const SfxPoolItem* pItem;
    if(SFX_ITEM_SET == rSet.GetItemState( RES_PAGEDESC, sal_True, &pItem ) )
    {
        pNewDesc = new SwFmtPageDesc(*((SwFmtPageDesc*)pItem));
    }
    if(!pNewDesc)
        pNewDesc = new SwFmtPageDesc();
    OUString uDescName;
    aValue >>= uDescName;
    String sDescName;
    SwStyleNameMapper::FillUIName(uDescName, sDescName, GET_POOLID_PAGEDESC, sal_True );
    if(!pNewDesc->GetPageDesc() || pNewDesc->GetPageDesc()->GetName() != sDescName)
    {
        pDoc->GetPageDescCnt();
        sal_Bool bPut = sal_False;
        if(sDescName.Len())
        {
            SwPageDesc* pPageDesc = ::binfilter::GetPageDescByName_Impl(*pDoc, sDescName);
            if(pPageDesc)
            {
                pPageDesc->Add( pNewDesc );
                bPut = sal_True;
            }
            else
            {
                throw lang::IllegalArgumentException();
            }
        }
        if(!bPut)
        {
            rSet.ClearItem(RES_BREAK);
            rSet.Put(SwFmtPageDesc());
        }
        else
            rSet.Put(*pNewDesc);
    }
    delete pNewDesc;
}

void lcl_SetNodeNumStart( SwPaM& rCrsr, uno::Any aValue )
{
    sal_Int16 nTmp(0);
    aValue >>= nTmp;
    sal_uInt16 nStt = (nTmp < 0 ? USHRT_MAX : (sal_uInt16)nTmp);
    SwDoc* pDoc = rCrsr.GetDoc();
    UnoActionContext aAction(pDoc);

    if( rCrsr.GetNext() == &rCrsr )         // Mehrfachselektion ?
        pDoc->SetNodeNumStart( *rCrsr.GetPoint(), nStt );
}

class SwTextCursorItemSet_Impl
{
    SwDoc *pDoc;
    SfxItemSet* pItemSet;
    sal_uInt16 nWhich;
    sal_Bool bGotAttrs;

public:
    SwTextCursorItemSet_Impl( SwDoc *pD, sal_uInt16 nW ) :
        pDoc( pD ), pItemSet(0), nWhich( nW ), bGotAttrs( sal_False ) {}
    ~SwTextCursorItemSet_Impl()
        {delete pItemSet;}

    SfxItemSet& GetItemSet( SwPaM *pPaM=0 )
    {
        if( !pItemSet )
            pItemSet = new SfxItemSet( pDoc->GetAttrPool(), nWhich, nWhich );
        if( pPaM && !bGotAttrs )
        {
            SwXTextCursor::GetCrsrAttr( *pPaM, *pItemSet );
            bGotAttrs = sal_True;
        }
        return *pItemSet;
    }

    SfxItemSet *GetItemSetPtr() { return pItemSet; }
};

sal_Bool lcl_setCrsrPropertyValue(const SfxItemPropertyMap* pMap,
                                SwPaM& rPam,
                                SwTextCursorItemSet_Impl& rSet,
                                const uno::Any& aValue ) throw (lang::IllegalArgumentException)
{
    sal_Bool bRet = sal_True;
    if(0 ==(pMap->nFlags&PropertyAttribute::MAYBEVOID) &&
        aValue.getValueType() == ::getCppuVoidType())
        bRet = sal_False;
    else
    {
        switch(pMap->nWID)
        {
            case RES_TXTATR_CHARFMT:
                lcl_setCharStyle(rPam.GetDoc(), aValue, rSet.GetItemSet() );
            break;
            case FN_UNO_CHARFMT_SEQUENCE:
            {
                Sequence<OUString> aCharStyles;
                if(aValue >>= aCharStyles)
                {
                    for(sal_Int32 nStyle = 0; nStyle < aCharStyles.getLength(); nStyle++)
                    {
                        Any aStyle;
                        aStyle <<= aCharStyles.getConstArray()[nStyle];
                        //create a local set and apply each format directly
                        SfxItemSet aSet(rPam.GetDoc()->GetAttrPool(), RES_TXTATR_CHARFMT, RES_TXTATR_CHARFMT );
                        lcl_setCharStyle(rPam.GetDoc(), aStyle, aSet );
                        //the first style should replace the current attributes, all other have to be added
                        SwXTextCursor::SetCrsrAttr(rPam, aSet, nStyle ? CRSR_ATTR_MODE_DONTREPLACE : 0);
                    }
                }
                else
                    bRet = sal_False;
            }
            break;
            case FN_UNO_PARA_STYLE :
                lcl_SetTxtFmtColl(aValue, rPam);
            break;
            case FN_UNO_PAGE_STYLE :
            break;
            case FN_UNO_NUM_START_VALUE  :
                lcl_SetNodeNumStart( rPam, aValue );
            break;
            case FN_UNO_NUM_LEVEL  :
            case FN_UNO_IS_NUMBER  :
            {
                SwTxtNode* pTxtNd = rPam.GetNode()->GetTxtNode();
                const SwNumRule* pRule = pTxtNd->GetNumRule();
                // hier wird Multiselektion nicht beruecksichtigt
                if( pRule && pTxtNd->GetNum() )
                {
                    if( FN_UNO_NUM_LEVEL == pMap->nWID)
                    {
                        sal_Int16 nLevel(0);
                        aValue >>= nLevel;
                        sal_Int16 nOldLevel = pTxtNd->GetNum()->GetLevel() & ~NO_NUMLEVEL;
                        if(nLevel < MAXLEVEL && nOldLevel != nLevel)
                        {
                            UnoActionContext aAction(rPam.GetDoc());
                            sal_Bool bDown = nLevel > nOldLevel;
                            sal_Int8 nMove = (sal_Int8)(bDown ? nLevel - nOldLevel : nOldLevel - nLevel);
                            while( nMove-- )
                            {
                                rPam.GetDoc()->NumUpDown( rPam, bDown );
                            }
                        }
                    }
                    else
                    {
                        BOOL bIsNumber = *(sal_Bool*) aValue.getValue();
                        SwNodeNum aNum = *pTxtNd->GetNum();
                        sal_Int16 nOldLevel = aNum.GetLevel() & ~NO_NUMLEVEL;
                        if(!bIsNumber)
                            nOldLevel |= NO_NUMLEVEL;
                        aNum.SetLevel(nOldLevel);
                        pTxtNd->UpdateNum( aNum );

                    }
                }
                //PROPERTY_MAYBEVOID!
            }
            break;
            case FN_NUMBER_NEWSTART :
            break;
            case FN_UNO_NUM_RULES:
                SwUnoCursorHelper::setNumberingProperty(aValue, rPam);
            break;
            case RES_PARATR_DROP:
            {
                if( MID_DROPCAP_CHAR_STYLE_NAME == pMap->nMemberId)
                {
                    OUString uStyle;
                    if(aValue >>= uStyle)
                    {
                        SfxItemSet& rItemSet = rSet.GetItemSet( &rPam );
                        String sStyle;
                        SwStyleNameMapper::FillUIName(uStyle, sStyle, GET_POOLID_CHRFMT, sal_True );
                        SwDoc* pDoc = rPam.GetDoc();
                        //default character style mustn't be set as default format
                        SwDocStyleSheet* pStyle =
                            (SwDocStyleSheet*)pDoc->GetDocShell()->GetStyleSheetPool()->Find(sStyle, SFX_STYLE_FAMILY_CHAR);
                        SwFmtDrop* pDrop = 0;
                        if(pStyle &&
                                ((SwDocStyleSheet*)pStyle)->GetCharFmt() != pDoc->GetDfltCharFmt())
                        {
                            const SfxPoolItem* pItem;
                            if(SFX_ITEM_SET == rItemSet.GetItemState( RES_PARATR_DROP, sal_True, &pItem ) )
                                pDrop = new SwFmtDrop(*((SwFmtDrop*)pItem));
                            if(!pDrop)
                                pDrop = new SwFmtDrop();
                            SwDocStyleSheet aStyle( *(SwDocStyleSheet*)pStyle );
                            pDrop->SetCharFmt(aStyle.GetCharFmt());
                        }
                        else
                             throw lang::IllegalArgumentException();
                        rItemSet.Put(*pDrop);
                        delete pDrop;
                    }
                    else
                        throw lang::IllegalArgumentException();
                }
                else
                    bRet = sal_False;
            }
            break;
            case RES_TXTATR_CJK_RUBY:
                if(MID_RUBY_CHARSTYLE == pMap->nMemberId )
                {
                    OUString sTmp;
                    if(aValue >>= sTmp)
                    {
                        SfxItemSet& rItemSet = rSet.GetItemSet( &rPam );
                        SwFmtRuby* pRuby = 0;
                        const SfxPoolItem* pItem;
                        if(SFX_ITEM_SET == rItemSet.GetItemState( RES_TXTATR_CJK_RUBY, sal_True, &pItem ) )
                            pRuby = new SwFmtRuby(*((SwFmtRuby*)pItem));
                        if(!pRuby)
                            pRuby = new SwFmtRuby(aEmptyStr);
                        String sStyle;
                        SwStyleNameMapper::FillUIName(sTmp, sStyle, GET_POOLID_CHRFMT, sal_True );
                         pRuby->SetCharFmtName( sStyle );
                        pRuby->SetCharFmtId( 0 );
                        if(sStyle.Len() > 0)
                        {
                            sal_uInt16 nId = SwStyleNameMapper::GetPoolIdFromUIName( sStyle, GET_POOLID_CHRFMT );
                            pRuby->SetCharFmtId(nId);
                        }
                        rItemSet.Put(*pRuby);
                        delete pRuby;
                    }
                    else
                        throw lang::IllegalArgumentException();
                    bRet = sal_True;
                }
                else
                    bRet = sal_False;
            break;
            case RES_PAGEDESC      :
            if(MID_PAGEDESC_PAGEDESCNAME == pMap->nMemberId )
            {
                SfxItemSet& rItemSet = rSet.GetItemSet( &rPam );
                lcl_setPageDesc(rPam.GetDoc(), aValue, rItemSet);
                break;
            }
            //hier kein break
            default: bRet = sal_False;
        }
    }
return bRet;
}

SwFmtColl* SwXTextCursor::GetCurTxtFmtColl(SwPaM& rPam, BOOL bConditional)
{
    static const sal_uInt16 nMaxLookup = 1000;
    SwFmtColl *pFmt = 0;

    sal_Bool bError = sal_False;
    FOREACHUNOPAM_START(&rPam)

        sal_uInt32 nSttNd = PUNOPAM->GetMark()->nNode.GetIndex(),
              nEndNd = PUNOPAM->GetPoint()->nNode.GetIndex();
        xub_StrLen nSttCnt = PUNOPAM->GetMark()->nContent.GetIndex(),
                nEndCnt = PUNOPAM->GetPoint()->nContent.GetIndex();

        if( nSttNd > nEndNd || ( nSttNd == nEndNd && nSttCnt > nEndCnt ))
        {
            sal_uInt32 nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
            nTmp = nSttCnt; nSttCnt = nEndCnt; nEndCnt = (sal_uInt16)nTmp;
        }

        if( nEndNd - nSttNd >= nMaxLookup )
        {
            pFmt = 0;
            break;
        }

        const SwNodes& rNds = rPam.GetDoc()->GetNodes();
        for( ULONG n = nSttNd; n <= nEndNd; ++n )
        {
            const SwTxtNode* pNd = rNds[ n ]->GetTxtNode();
            if( pNd )
            {
                SwFmtColl* pNdFmt = bConditional ? pNd->GetFmtColl()
                                                    : &pNd->GetAnyFmtColl();
                if( !pFmt )
                    pFmt = pNdFmt;
                else if( pFmt != pNdFmt )
                {
                    bError = sal_True;
                    break;
                }
            }
        }
        if(bError)
            break;
    FOREACHUNOPAM_END()
    return bError ? 0 : pFmt;
}

 SwPageDesc*    GetPageDescByName_Impl(SwDoc& rDoc, const String& rName)
 {
    SwPageDesc* pRet = 0;
    sal_uInt16 nDCount = rDoc.GetPageDescCnt();
    sal_uInt16 i=0;
    for( i = 0; i < nDCount; i++ )
    {
        SwPageDesc* pDsc = &rDoc._GetPageDesc( i );
        if(pDsc->GetName() == rName)
        {
            pRet = pDsc;
            break;
        }
    }
    if(!pRet)
    {
        for(i = RC_POOLPAGEDESC_BEGIN; i <= STR_POOLPAGE_ENDNOTE; ++i)
        {
            const String aFmtName(SW_RESSTR(i));
            if(aFmtName == rName)
            {
                pRet = rDoc.GetPageDescFromPool( RES_POOLPAGE_BEGIN + i - RC_POOLPAGEDESC_BEGIN );
                break;
            }
        }
    }

    return pRet;
 }

/******************************************************************************
 ******************************************************************************
 ******************************************************************************
 * Taskforce ONE51
 ******************************************************************************/

/******************************************************************
 * SwXTextCursor
 ******************************************************************/

OUString SwXTextCursor::getImplementationName(void) throw( RuntimeException )
{
    return C2U("SwXTextCursor");
}

BOOL SwXTextCursor::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
    String sServiceName(rServiceName);
    return sServiceName.EqualsAscii("com.sun.star.text.TextCursor") ||
         sServiceName.EqualsAscii("com.sun.star.style.CharacterProperties")||
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesAsian")||
        sServiceName.EqualsAscii("com.sun.star.style.CharacterPropertiesComplex")||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphProperties") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesAsian") ||
        sServiceName.EqualsAscii("com.sun.star.style.ParagraphPropertiesComplex") ||
        sServiceName.EqualsAscii("com.sun.star.text.TextSortable");
}

Sequence< OUString > SwXTextCursor::getSupportedServiceNames(void) throw( RuntimeException )
{
    Sequence< OUString > aRet(8);
    OUString* pArray = aRet.getArray();
    pArray[0] = C2U("com.sun.star.text.TextCursor");
     pArray[1] = C2U("com.sun.star.style.CharacterProperties");
    pArray[2] = C2U("com.sun.star.style.CharacterPropertiesAsian");
    pArray[3] = C2U("com.sun.star.style.CharacterPropertiesComplex");
    pArray[4] = C2U("com.sun.star.style.ParagraphProperties");
    pArray[5] = C2U("com.sun.star.style.ParagraphPropertiesAsian");
    pArray[6] = C2U("com.sun.star.style.ParagraphPropertiesComplex");
    pArray[7] = C2U("com.sun.star.text.TextSortable");
    return aRet;
}

SwXTextCursor::SwXTextCursor(uno::Reference< XText >  xParent, const SwPosition& rPos,
                    CursorType eSet, SwDoc* pDoc, const SwPosition* pMark) :
    aLstnrCntnr(( util::XSortable*)this),
    aPropSet(aSwMapProvider.GetPropertyMap(PROPERTY_MAP_TEXT_CURSOR)),
    xParentText(xParent),
    pLastSortOptions(0),
    eType(eSet)
{
    SwUnoCrsr* pUnoCrsr = pDoc->CreateUnoCrsr(rPos, sal_False);
    if(pMark)
    {
        pUnoCrsr->SetMark();
        *pUnoCrsr->GetMark() = *pMark;
    }
    pUnoCrsr->Add(this);
}

SwXTextCursor::~SwXTextCursor()
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    delete pUnoCrsr;
    delete pLastSortOptions;
}

void SwXTextCursor::DeleteAndInsert(const String& rText)
{
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        // Start/EndAction
        SwDoc* pDoc = pUnoCrsr->GetDoc();
        UnoActionContext aAction(pDoc);
        xub_StrLen nTxtLen = rText.Len();
        SwCursor *_pStartCrsr = pUnoCrsr;
        do
        {
            if(_pStartCrsr->HasMark())
            {
                pDoc->DeleteAndJoin(*_pStartCrsr);
            }
            if(nTxtLen)
            {
                //OPT: GetSystemCharSet
                if( !pDoc->Insert(*_pStartCrsr, rText) )
                {
                    OSL_FAIL( "Doc->Insert(Str) failed." );
                }
                SwXTextCursor::SelectPam(*pUnoCrsr, sal_True);
                _pStartCrsr->Left(rText.Len(), CRSR_SKIP_CHARS);
            }
        } while( (_pStartCrsr=(SwCursor*)_pStartCrsr->GetNext()) != pUnoCrsr );
    }
}

const uno::Sequence< sal_Int8 > & SwXTextCursor::getUnoTunnelId()
{
    static uno::Sequence< sal_Int8 > aSeq = ::binfilter::CreateUnoTunnelId();
    return aSeq;
}

sal_Int64 SAL_CALL SwXTextCursor::getSomething( const uno::Sequence< sal_Int8 >& rId )
    throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
                                        rId.getConstArray(), 16 ) )
    {
            return (sal_Int64)this;
    }
    return OTextCursorHelper::getSomething(rId);
}

void SwXTextCursor::collapseToStart(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(pUnoCrsr->HasMark())
        {
            if(*pUnoCrsr->GetPoint() > *pUnoCrsr->GetMark())
                pUnoCrsr->Exchange();
            pUnoCrsr->DeleteMark();
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}

void SwXTextCursor::collapseToEnd(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(pUnoCrsr->HasMark())
        {
            if(*pUnoCrsr->GetPoint() < *pUnoCrsr->GetMark())
                pUnoCrsr->Exchange();
            pUnoCrsr->DeleteMark();
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}

sal_Bool SwXTextCursor::isCollapsed(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_True;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr && pUnoCrsr->GetMark())
    {
        bRet = (*pUnoCrsr->GetPoint() == *pUnoCrsr->GetMark());
    }
    return bRet;
}

sal_Bool SwXTextCursor::goLeft(sal_Int16 nCount, sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->Left( nCount, CRSR_SKIP_CHARS);
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

sal_Bool SwXTextCursor::goRight(sal_Int16 nCount, sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->Right(nCount, CRSR_SKIP_CHARS);
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

void SwXTextCursor::gotoStart(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        if(eType == CURSOR_BODY)
        {
            pUnoCrsr->Move( fnMoveBackward, fnGoDoc );
            //check, that the cursor is not in a table
            SwTableNode* pTblNode = pUnoCrsr->GetNode()->FindTableNode();
            SwCntntNode* pCont = 0;
            while( pTblNode )
            {
                pUnoCrsr->GetPoint()->nNode = *pTblNode->EndOfSectionNode();
                pCont = GetDoc()->GetNodes().GoNext(&pUnoCrsr->GetPoint()->nNode);
                pTblNode = pCont->FindTableNode();
            }
            if(pCont)
                pUnoCrsr->GetPoint()->nContent.Assign(pCont, 0);
            const SwStartNode* pTmp = pUnoCrsr->GetNode()->FindStartNode();
            if(pTmp->IsSectionNode())
            {
                SwSectionNode* pSectionStartNode = (SwSectionNode*)pTmp;
                if(pSectionStartNode->GetSection().IsHiddenFlag())
                {
                    pCont = GetDoc()->GetNodes().GoNextSection(
                                &pUnoCrsr->GetPoint()->nNode, sal_True, sal_False);
                    if(pCont)
                        pUnoCrsr->GetPoint()->nContent.Assign(pCont, 0);
                }
            }
        }
        else if(eType == CURSOR_FRAME ||
                eType == CURSOR_TBLTEXT ||
                eType == CURSOR_HEADER ||
                eType == CURSOR_FOOTER ||
                eType == CURSOR_FOOTNOTE||
                eType == CURSOR_REDLINE)
        {
            pUnoCrsr->MoveSection( fnSectionCurr, fnSectionStart);
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}

void SwXTextCursor::gotoEnd(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        if(eType == CURSOR_BODY)
            pUnoCrsr->Move( fnMoveForward, fnGoDoc );
        else if(eType == CURSOR_FRAME ||
                eType == CURSOR_TBLTEXT ||
                eType == CURSOR_HEADER ||
                eType == CURSOR_FOOTER ||
                eType == CURSOR_FOOTNOTE||
                eType == CURSOR_REDLINE)
        {
            pUnoCrsr->MoveSection( fnSectionCurr, fnSectionEnd);
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
}

void SwXTextCursor::gotoRange(const uno::Reference< XTextRange > & xRange, sal_Bool bExpand )
                                throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pOwnCursor = GetCrsr();
    if(!pOwnCursor || !xRange.is())
    {
        throw uno::RuntimeException();
    }

    uno::Reference<lang::XUnoTunnel> xRangeTunnel( xRange, uno::UNO_QUERY);
    SwXTextRange* pRange = 0;
    OTextCursorHelper* pCursor = 0;
    if(xRangeTunnel.is())
    {
        pRange = (SwXTextRange*)xRangeTunnel->getSomething(
                                SwXTextRange::getUnoTunnelId());
        pCursor = (OTextCursorHelper*)xRangeTunnel->getSomething(
                                OTextCursorHelper::getUnoTunnelId());
    }

    SwStartNodeType eSearchNodeType = SwNormalStartNode;
    switch(eType)
    {
        case CURSOR_FRAME:      eSearchNodeType = SwFlyStartNode;       break;
        case CURSOR_TBLTEXT:    eSearchNodeType = SwTableBoxStartNode;  break;
        case CURSOR_FOOTNOTE:   eSearchNodeType = SwFootnoteStartNode;  break;
        case CURSOR_HEADER:     eSearchNodeType = SwHeaderStartNode;    break;
        case CURSOR_FOOTER:     eSearchNodeType = SwFooterStartNode;    break;
        default: break;
    }
    const SwStartNode* pOwnStartNode = pOwnCursor->GetNode()->
                                            FindSttNodeByType(eSearchNodeType);

    const SwNode* pSrcNode = 0;
    if(pCursor && pCursor->GetPaM())
    {
        pSrcNode = pCursor->GetPaM()->GetNode();
    }
    else if(pRange && pRange->GetBookmark())
    {
        SwBookmark* pBkm = pRange->GetBookmark();
        pSrcNode = &pBkm->GetPos().nNode.GetNode();
    }
    const SwStartNode* pTmp = pSrcNode ? pSrcNode->FindSttNodeByType(eSearchNodeType) : 0;

    //SectionNodes ueberspringen
    while(pTmp && pTmp->IsSectionNode())
    {
        pTmp = pTmp->FindStartNode();
    }
    while(pOwnStartNode && pOwnStartNode->IsSectionNode())
    {
        pOwnStartNode = pOwnStartNode->FindStartNode();
    }
    if(pOwnStartNode != pTmp)
    {
        throw uno::RuntimeException();
    }

    //jetzt muss die Selektion erweitert werden
    if(bExpand)
    {
        // der Cursor soll alles einschliessen, was bisher von ihm und dem uebergebenen
        // Range eingeschlossen wurde
        SwPosition aOwnLeft(*pOwnCursor->GetPoint());
        SwPosition aOwnRight(pOwnCursor->HasMark() ? *pOwnCursor->GetMark() : aOwnLeft);
        if(aOwnRight < aOwnLeft)
        {
            SwPosition aTmp = aOwnLeft;
            aOwnLeft = aOwnRight;
            aOwnRight = aTmp;
        }
        SwPosition* pParamLeft;
        SwPosition* pParamRight;
        if(pCursor)
        {
            const SwPaM* pLclTmp = pCursor->GetPaM();
            pParamLeft = new SwPosition(*pLclTmp->GetPoint());
            pParamRight = new SwPosition(pLclTmp->HasMark() ? *pLclTmp->GetMark() : *pParamLeft);
        }
        else
        {
            SwBookmark* pBkm = pRange->GetBookmark();
            pParamLeft = new SwPosition(pBkm->GetPos());
            pParamRight = new SwPosition(pBkm->GetOtherPos() ? *pBkm->GetOtherPos() : *pParamLeft);
        }
        if(*pParamRight < *pParamLeft)
        {
            SwPosition* pLclTmp = pParamLeft;
            pParamLeft = pParamRight;
            pParamRight = pLclTmp;
        }
        // jetzt sind vier SwPositions da, zwei davon werden gebraucht, also welche?
        if(aOwnRight < *pParamRight)
            *pOwnCursor->GetPoint() = aOwnRight;
        else
            *pOwnCursor->GetPoint() = *pParamRight;
        pOwnCursor->SetMark();
        if(aOwnLeft < *pParamLeft)
            *pOwnCursor->GetMark() = *pParamLeft;
        else
            *pOwnCursor->GetMark() = aOwnLeft;
        delete pParamLeft;
        delete pParamRight;
    }
    else
    {
        //der Cursor soll dem uebergebenen Range entsprechen
        if(pCursor)
        {
            const SwPaM* pLclTmp = pCursor->GetPaM();
            *pOwnCursor->GetPoint() = *pLclTmp->GetPoint();
            if(pLclTmp->HasMark())
            {
                pOwnCursor->SetMark();
                *pOwnCursor->GetMark() = *pLclTmp->GetMark();
            }
            else
                pOwnCursor->DeleteMark();
        }
        else
        {
            SwBookmark* pBkm = pRange->GetBookmark();
            *pOwnCursor->GetPoint() = pBkm->GetPos();
            if(pBkm->GetOtherPos())
            {
                pOwnCursor->SetMark();
                *pOwnCursor->GetMark() = *pBkm->GetOtherPos();
            }
            else
                pOwnCursor->DeleteMark();
        }
    }
}

sal_Bool SwXTextCursor::isStartOfWord(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        bRet = sal_False;
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

sal_Bool SwXTextCursor::isEndOfWord(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        bRet = sal_False;
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

sal_Bool SwXTextCursor::gotoNextWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    //Probleme gibt's noch mit einem Absatzanfang, an dem kein Wort beginnt.
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        //Absatzende?
        if(pUnoCrsr->GetCntntNode() &&
                pUnoCrsr->GetPoint()->nContent == pUnoCrsr->GetCntntNode()->Len())
                bRet = pUnoCrsr->Right(1, CRSR_SKIP_CHARS);
        else
        {
            bRet = pUnoCrsr->MovePara(fnParaNext, fnParaStart);
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

sal_Bool SwXTextCursor::gotoPreviousWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    // hier machen Leerzeichen am Absatzanfang Probleme
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        //Absatzanfang ?
        if(pUnoCrsr->GetPoint()->nContent == 0)
            bRet = pUnoCrsr->Left(1, CRSR_SKIP_CHARS);
        else
        {
            bRet = sal_False;
            if( pUnoCrsr->GetPoint()->nContent == 0 )
                pUnoCrsr->Left(1, CRSR_SKIP_CHARS);
        }
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

sal_Bool SwXTextCursor::gotoEndOfWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = sal_False;
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

sal_Bool SwXTextCursor::gotoStartOfWord(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    sal_Bool bRet = FALSE;
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = sal_False;
    }
    else
    {
        throw uno::RuntimeException();
    }
    return bRet;
}

sal_Bool SwXTextCursor::isStartOfSentence(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        // start of paragraph?
        bRet = pUnoCrsr->GetPoint()->nContent == 0;
        // with mark ->no sentence start
        if(!bRet && !pUnoCrsr->HasMark())
        {
            SwCursor aCrsr(*pUnoCrsr->GetPoint());
            aCrsr.Right(1, CRSR_SKIP_CHARS);
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::isEndOfSentence(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        //start of paragraph?
        bRet = pUnoCrsr->GetCntntNode() &&
                pUnoCrsr->GetPoint()->nContent == pUnoCrsr->GetCntntNode()->Len();
        // with mark->no sentence end
        if(!bRet && !pUnoCrsr->HasMark())
        {
            SwCursor aCrsr(*pUnoCrsr->GetPoint());
            aCrsr.Left( 1, CRSR_SKIP_CHARS);
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoNextSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        BOOL bWasEOS = isEndOfSentence();
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->MovePara(fnParaNext, fnParaStart);

        if ( bWasEOS )
            bRet = sal_False;
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoPreviousSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        if(0 != (bRet = pUnoCrsr->MovePara(fnParaPrev, fnParaStart)))
        {
            pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
        }
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoStartOfSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = SwUnoCursorHelper::IsStartOfPara(*pUnoCrsr);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoEndOfSentence(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        // bRet is true if the MovePara() succeeded while the end of the para is
        // not reached already
        sal_Bool bAlreadyParaEnd = SwUnoCursorHelper::IsEndOfPara(*pUnoCrsr);
        bRet = !bAlreadyParaEnd && (pUnoCrsr->MovePara(fnParaCurr, fnParaEnd));

    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::isStartOfParagraph(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        bRet = SwUnoCursorHelper::IsStartOfPara(*pUnoCrsr);
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::isEndOfParagraph(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        bRet = SwUnoCursorHelper::IsEndOfPara(*pUnoCrsr);
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoStartOfParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr )
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = SwUnoCursorHelper::IsStartOfPara(*pUnoCrsr);
        if(!bRet)
            bRet = pUnoCrsr->MovePara(fnParaCurr, fnParaStart);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoEndOfParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = SwUnoCursorHelper::IsEndOfPara(*pUnoCrsr);
        if(!bRet)
            bRet = pUnoCrsr->MovePara(fnParaCurr, fnParaEnd);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoNextParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->MovePara(fnParaNext, fnParaStart);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

sal_Bool SwXTextCursor::gotoPreviousParagraph(sal_Bool Expand) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    sal_Bool bRet = sal_False;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        SwXTextCursor::SelectPam(*pUnoCrsr, Expand);
        bRet = pUnoCrsr->MovePara(fnParaPrev, fnParaStart);
    }
    else
        throw uno::RuntimeException();
    return bRet;
}

uno::Reference< XText >  SwXTextCursor::getText(void) throw( uno::RuntimeException )
{
    return xParentText;
}

uno::Reference< XTextRange >  SwXTextCursor::getStart(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    uno::Reference< XTextRange >  xRet;
    SwUnoCrsr* pUnoCrsr = ((SwXTextCursor*)this)->GetCrsr();
    if( pUnoCrsr)
    {
        SwPaM aPam(*pUnoCrsr->Start());
        uno::Reference< XText >  xParent = getText();
        xRet = new SwXTextRange(aPam, xParent);
    }
    else
        throw uno::RuntimeException();
    return xRet;
}

uno::Reference< XTextRange >  SwXTextCursor::getEnd(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    uno::Reference< XTextRange >  xRet;
    SwUnoCrsr* pUnoCrsr = ((SwXTextCursor*)this)->GetCrsr();
    if( pUnoCrsr)
    {
        SwPaM aPam(*pUnoCrsr->End());
        uno::Reference< XText >  xParent = getText();
        xRet = new SwXTextRange(aPam, xParent);
    }
    else
        throw uno::RuntimeException();
    return xRet;
}

OUString SwXTextCursor::getString(void) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    OUString aTxt;
    SwUnoCrsr* pUnoCrsr = ((SwXTextCursor*)this)->GetCrsr();
    if( pUnoCrsr)
    {
            //Text ueber mehrere Absaetze
            SwXTextCursor::getTextFromPam(*pUnoCrsr, aTxt);
    }
    else
        throw uno::RuntimeException();
    return aTxt;
}

void SwXTextCursor::setString(const OUString& aString) throw( uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(!pUnoCrsr)
        throw uno::RuntimeException();

    DeleteAndInsert(aString);
}

Any SwXTextCursor::GetPropertyValue(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet,
    const OUString& rPropertyName)
        throw( UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    Any aAny;
    const SfxItemPropertyMap*   pMap = SfxItemPropertyMap::GetByName(
                                rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        PropertyState eTemp;
        BOOL bDone = SwUnoCursorHelper::getCrsrPropertyValue( pMap, rPaM, &aAny, eTemp );
        if(!bDone)
        {
            SfxItemSet aSet(rPaM.GetDoc()->GetAttrPool(),
                RES_CHRATR_BEGIN,   RES_PARATR_NUMRULE,
                RES_UNKNOWNATR_CONTAINER, RES_UNKNOWNATR_CONTAINER,
                RES_TXTATR_UNKNOWN_CONTAINER, RES_TXTATR_UNKNOWN_CONTAINER,
                RES_FILL_ORDER,     RES_FRMATR_END -1,
                0L);
            SwXTextCursor::GetCrsrAttr(rPaM, aSet);

            aAny = rPropSet.getPropertyValue(*pMap, aSet);
        }
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );

    return aAny;
}

void SwXTextCursor::SetPropertyValue(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet, const OUString& rPropertyName,
    const Any& aValue, const SfxItemPropertyMap* _pMap, USHORT nAttrMode)
        throw (UnknownPropertyException, PropertyVetoException,
            IllegalArgumentException, WrappedTargetException, RuntimeException)
{
    Any aAny;

    SwDoc* pDoc = rPaM.GetDoc();
    const SfxItemPropertyMap*   pMap = _pMap ? _pMap : SfxItemPropertyMap::GetByName(
                            rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        if ( pMap->nFlags & PropertyAttribute::READONLY)
            throw PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
        SwTextCursorItemSet_Impl aSet(pDoc, pMap->nWID );
        if(!lcl_setCrsrPropertyValue( pMap, rPaM, aSet, aValue ))
            rPropSet.setPropertyValue(*pMap, aValue, aSet.GetItemSet( &rPaM ) );
        if( aSet.GetItemSetPtr() )
            SwXTextCursor::SetCrsrAttr(rPaM, aSet.GetItemSet(), nAttrMode );
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
}

Sequence< PropertyState > SwXTextCursor::GetPropertyStates(
            SwPaM& rPaM, SfxItemPropertySet& rPropSet,
            const Sequence< OUString >& PropertyNames,
            SwGetPropertyStatesCaller eCaller )
            throw(UnknownPropertyException, RuntimeException)
{
    const OUString* pNames = PropertyNames.getConstArray();
    Sequence< PropertyState > aRet ( PropertyNames.getLength() );
    PropertyState* pStates = aRet.getArray();

    SfxItemSet *pSet = 0, *pSetParent = 0;
    const SfxItemPropertyMap* pSaveMap, *pMap = rPropSet.getPropertyMap();
    for( INT32 i = 0, nEnd = PropertyNames.getLength(); i < nEnd; i++ )
    {
        pSaveMap = pMap;
        pMap = SfxItemPropertyMap::GetByName( pMap, pNames[i] );
        if(!pMap)
        {
            if(pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)) ||
               pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
            {
                pStates[i] = beans::PropertyState_DEFAULT_VALUE;
                pMap = pSaveMap;
                continue;
            }
            else
                throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( 0 ) );
        }
        if (eCaller == SW_PROPERTY_STATE_CALLER_SWX_TEXT_PORTION &&
            pMap->nWID < FN_UNO_RANGE_BEGIN &&
            pMap->nWID > FN_UNO_RANGE_END  &&
            pMap->nWID < RES_CHRATR_BEGIN &&
            pMap->nWID > RES_TXTATR_END )
            pStates[i] = beans::PropertyState_DEFAULT_VALUE;
        else
        {
            if ( pMap->nWID >= FN_UNO_RANGE_BEGIN &&
                 pMap->nWID <= FN_UNO_RANGE_END )
                SwUnoCursorHelper::getCrsrPropertyValue(pMap, rPaM, 0, pStates[i] );
            else
            {
                if( !pSet )
                {
                    switch ( eCaller )
                    {
                        case SW_PROPERTY_STATE_CALLER_SWX_TEXT_PORTION:
                            pSet = new SfxItemSet( rPaM.GetDoc()->GetAttrPool(),
                                    RES_CHRATR_BEGIN,   RES_TXTATR_END );
                        break;
                        case SW_PROPERTY_STATE_CALLER_SINGLE_VALUE_ONLY:
                            pSet = new SfxItemSet( rPaM.GetDoc()->GetAttrPool(),
                                    pMap->nWID, pMap->nWID );
                        break;
                        default:
                            pSet = new SfxItemSet( rPaM.GetDoc()->GetAttrPool(),
                                RES_CHRATR_BEGIN,   RES_PARATR_NUMRULE,
                                RES_FILL_ORDER,     RES_FRMATR_END -1,
                                RES_UNKNOWNATR_CONTAINER, RES_UNKNOWNATR_CONTAINER,
                                RES_TXTATR_UNKNOWN_CONTAINER, RES_TXTATR_UNKNOWN_CONTAINER,
                                0L );
                    }
                    SwXTextCursor::GetCrsrAttr( rPaM, *pSet, FALSE );
                }

                if( pSet->Count() )
                    pStates[i] = rPropSet.getPropertyState( *pMap,*pSet );
                else
                    pStates[i] = PropertyState_DEFAULT_VALUE;

                //try again to find out if a value has been inherited
                if( beans::PropertyState_DIRECT_VALUE == pStates[i] )
                {
                    if( !pSetParent )
                    {
                        pSetParent = pSet->Clone( FALSE );
                        SwXTextCursor::GetCrsrAttr( rPaM, *pSetParent, sal_True );
                    }

                    if( (pSetParent)->Count() )
                        pStates[i] = rPropSet.getPropertyState( *pMap, *pSetParent );
                    else
                        pStates[i] = PropertyState_DEFAULT_VALUE;
                }
            }
        }
        pMap++;
    }
    delete pSet;
    delete pSetParent;
    return aRet;
}

PropertyState SwXTextCursor::GetPropertyState(
    SwPaM& rPaM, SfxItemPropertySet& rPropSet, const OUString& rPropertyName)
                        throw(UnknownPropertyException, RuntimeException)
{
    Sequence < OUString > aStrings ( 1 );
    aStrings[0] = rPropertyName;
    Sequence < PropertyState > aSeq = GetPropertyStates( rPaM, rPropSet, aStrings, SW_PROPERTY_STATE_CALLER_SINGLE_VALUE_ONLY );
    return aSeq[0];
}

void lcl_SelectParaAndReset ( SwPaM &rPaM, SwDoc* pDoc, const SvUShortsSort* pWhichIds = 0 )
{
    // if we are reseting paragraph attributes, we need to select the full paragraph first
    SwPosition aStart = *rPaM.Start();
    SwPosition aEnd = *rPaM.End();
    auto_ptr < SwUnoCrsr > pTemp ( pDoc->CreateUnoCrsr(aStart, FALSE) );
    if(!SwUnoCursorHelper::IsStartOfPara(*pTemp))
        pTemp->MovePara(fnParaCurr, fnParaStart);
    pTemp->SetMark();
    *pTemp->GetPoint() = aEnd;
    SwXTextCursor::SelectPam(*pTemp, sal_True);
    if(!SwUnoCursorHelper::IsEndOfPara(*pTemp))
        pTemp->MovePara(fnParaCurr, fnParaEnd);
    pDoc->ResetAttr(*pTemp, sal_True, pWhichIds);
}
void SwXTextCursor::SetPropertyToDefault(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet,
    const OUString& rPropertyName)
    throw(UnknownPropertyException, RuntimeException)
{
    SolarMutexGuard aGuard;
    SwDoc* pDoc = rPaM.GetDoc();
    const SfxItemPropertyMap*   pMap = SfxItemPropertyMap::GetByName(
                            rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        if ( pMap->nFlags & PropertyAttribute::READONLY)
            throw PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
        if(pMap->nWID < RES_FRMATR_END)
        {
            SvUShortsSort aWhichIds;
            aWhichIds.Insert(pMap->nWID);
            if(pMap->nWID < RES_PARATR_BEGIN)
                pDoc->ResetAttr(rPaM, sal_True, &aWhichIds);
            else
                lcl_SelectParaAndReset ( rPaM, pDoc, &aWhichIds );
        }
        else
            SwUnoCursorHelper::resetCrsrPropertyValue(pMap, rPaM);
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
}

Any SwXTextCursor::GetPropertyDefault(
    SwPaM& rPaM, const SfxItemPropertySet& rPropSet,
    const OUString& rPropertyName)
    throw( UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    Any aRet;
    SwDoc* pDoc = rPaM.GetDoc();
    const SfxItemPropertyMap*   pMap = SfxItemPropertyMap::GetByName(
                            rPropSet.getPropertyMap(), rPropertyName);
    if(pMap)
    {
        if(pMap->nWID < RES_FRMATR_END)
        {
            const SfxPoolItem& rDefItem = pDoc->GetAttrPool().GetDefaultItem(pMap->nWID);
            rDefItem.QueryValue(aRet, pMap->nMemberId);
        }
    }
    else
        throw UnknownPropertyException(OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + rPropertyName, static_cast < cppu::OWeakObject * > ( 0 ) );
    return aRet;
}

uno::Reference< beans::XPropertySetInfo >  SwXTextCursor::getPropertySetInfo(void) throw( uno::RuntimeException )
{
    static uno::Reference< beans::XPropertySetInfo >  xRef;
    if(!xRef.is())
    {
        static SfxItemPropertyMap aCrsrExtMap_Impl[] =
        {
            { SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT), FN_SKIP_HIDDEN_TEXT, &::getBooleanCppuType(), PROPERTY_NONE,     0},
            { SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT), FN_SKIP_PROTECTED_TEXT, &::getBooleanCppuType(), PROPERTY_NONE,     0},
            {0,0,0,0,0,0}
        };
        uno::Reference< beans::XPropertySetInfo >  xInfo = aPropSet.getPropertySetInfo();
        // PropertySetInfo verlaengern!
        const uno::Sequence<beans::Property> aPropSeq = xInfo->getProperties();
        xRef = new SfxExtItemPropertySetInfo(
            aCrsrExtMap_Impl,
            aPropSeq );
    }
    return xRef;
}

void SwXTextCursor::setPropertyValue(const OUString& rPropertyName, const uno::Any& aValue)
        throw( beans::UnknownPropertyException, beans::PropertyVetoException,
             lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;
    uno::Any aAny;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)))
        {
            sal_Bool bSet = *(sal_Bool*)aValue.getValue();
            pUnoCrsr->SetSkipOverHiddenSections(bSet);
        }
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
        {
            sal_Bool bSet = *(sal_Bool*)aValue.getValue();
            pUnoCrsr->SetSkipOverProtectSections(bSet);
        }
        else
            SetPropertyValue(*pUnoCrsr, aPropSet, rPropertyName, aValue);
    }
    else
        throw uno::RuntimeException();

}

Any SwXTextCursor::getPropertyValue(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    uno::Any aAny;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)))
        {
            BOOL bSet = pUnoCrsr->IsSkipOverHiddenSections();
            aAny.setValue(&bSet, ::getBooleanCppuType());
        }
        else if(rPropertyName.equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
        {
            BOOL bSet = pUnoCrsr->IsSkipOverProtectSections();
            aAny.setValue(&bSet, ::getBooleanCppuType());
        }
        else
            aAny = GetPropertyValue(*pUnoCrsr, aPropSet, rPropertyName);
    }
    else
        throw uno::RuntimeException();
    return aAny;

}

void SwXTextCursor::addPropertyChangeListener(
        const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}

void SwXTextCursor::removePropertyChangeListener(
        const OUString& /*PropertyName*/, const uno::Reference< beans::XPropertyChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}

void SwXTextCursor::addVetoableChangeListener(
        const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}

void SwXTextCursor::removeVetoableChangeListener(
        const OUString& /*PropertyName*/, const uno::Reference< beans::XVetoableChangeListener > & /*aListener*/)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    DBG_WARNING("not implemented");
}

beans::PropertyState SwXTextCursor::getPropertyState(const OUString& rPropertyName)
                        throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    beans::PropertyState eRet = beans::PropertyState_DEFAULT_VALUE;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
    {
        eRet = GetPropertyState(*pUnoCrsr, aPropSet, rPropertyName);
    }
    else
        throw RuntimeException();
    return eRet;
}

uno::Sequence< beans::PropertyState > SwXTextCursor::getPropertyStates(
            const uno::Sequence< OUString >& PropertyNames)
            throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(!pUnoCrsr)
        throw RuntimeException();
    return GetPropertyStates(*pUnoCrsr, aPropSet, PropertyNames);
}

void SwXTextCursor::setPropertyToDefault(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, uno::RuntimeException )
{
    Sequence < OUString > aSequence ( &rPropertyName, 1 );
    setPropertiesToDefault ( aSequence );
}

uno::Any SwXTextCursor::getPropertyDefault(const OUString& rPropertyName)
    throw( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException )
{
    const Sequence < OUString > aSequence ( &rPropertyName, 1 );
    return getPropertyDefaults ( aSequence ).getConstArray()[0];
}

void SAL_CALL SwXTextCursor::setAllPropertiesToDefault()
    throw (RuntimeException)
{
    SolarMutexGuard aGuard;
    SwUnoCrsr* pUnoCrsr = GetCrsr();
    if(pUnoCrsr)
        lcl_SelectParaAndReset ( *pUnoCrsr, pUnoCrsr->GetDoc());
    else
        throw uno::RuntimeException();
}
void SAL_CALL SwXTextCursor::setPropertiesToDefault( const Sequence< OUString >& aPropertyNames )
    throw (UnknownPropertyException, RuntimeException)
{
    SolarMutexGuard aGuard;
    sal_Int32 nCount = aPropertyNames.getLength();
    if ( nCount )
    {
        SwUnoCrsr* pUnoCrsr = GetCrsr();
        if(pUnoCrsr)
        {
            SwDoc* pDoc = pUnoCrsr->GetDoc();
            const SfxItemPropertyMap* pMap = aPropSet.getPropertyMap(), *pSaveMap;
            const OUString * pNames = aPropertyNames.getConstArray();
            SvUShortsSort aWhichIds, aParaWhichIds;
            for ( sal_Int32 i = 0; i < nCount; i++ )
            {
                pSaveMap = pMap;
                pMap = SfxItemPropertyMap::GetByName( pMap, pNames[i]);
                if(!pMap)
                {
                    if(pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)) ||
                       pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
                    {
                        pMap = pSaveMap;
                        continue;
                    }
                    else
                        throw UnknownPropertyException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( 0 ) );
                }
                if ( pMap->nFlags & PropertyAttribute::READONLY)
                    throw PropertyVetoException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Property is read-only: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( 0 ) );

                if( pMap->nWID < RES_FRMATR_END)
                {
                    if(pMap->nWID < RES_PARATR_BEGIN)
                        aWhichIds.Insert(pMap->nWID);
                    else
                        aParaWhichIds.Insert (pMap->nWID);
                }
                else if ( pMap->nWID == FN_UNO_NUM_START_VALUE )
                    SwUnoCursorHelper::resetCrsrPropertyValue(pMap, *pUnoCrsr);
            }

            if ( aParaWhichIds.Count() )
                lcl_SelectParaAndReset ( *pUnoCrsr, pDoc, &aParaWhichIds );
            if (aWhichIds.Count() )
                pDoc->ResetAttr(*pUnoCrsr, sal_True, &aWhichIds);
        }
        else
            throw uno::RuntimeException();
    }
}
Sequence< Any > SAL_CALL SwXTextCursor::getPropertyDefaults( const Sequence< OUString >& aPropertyNames )
    throw (UnknownPropertyException, WrappedTargetException, RuntimeException)
{
    SolarMutexGuard aGuard;
    sal_Int32 nCount = aPropertyNames.getLength();
    Sequence < Any > aRet ( nCount );
    if ( nCount )
    {
        SwUnoCrsr* pUnoCrsr = GetCrsr();
        if (pUnoCrsr)
        {
            SwDoc* pDoc = pUnoCrsr->GetDoc();
            const SfxItemPropertyMap *pSaveMap, *pMap = aPropSet.getPropertyMap();
            const OUString *pNames = aPropertyNames.getConstArray();
            Any *pAny = aRet.getArray();
            for ( sal_Int32 i = 0; i < nCount; i++)
            {
                pSaveMap = pMap;
                pMap = SfxItemPropertyMap::GetByName( pMap, pNames[i]);
                if(!pMap)
                {
                    if(pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_HIDDEN_TEXT)) ||
                       pNames[i].equalsAsciiL( SW_PROP_NAME(UNO_NAME_IS_SKIP_PROTECTED_TEXT)))
                    {
                        pMap = pSaveMap;
                        continue;
                    }
                    else
                        throw UnknownPropertyException ( OUString ( RTL_CONSTASCII_USTRINGPARAM ( "Unknown property: " ) ) + pNames[i], static_cast < cppu::OWeakObject * > ( 0 ) );
                }
                if(pMap->nWID < RES_FRMATR_END)
                {
                    const SfxPoolItem& rDefItem = pDoc->GetAttrPool().GetDefaultItem(pMap->nWID);
                    rDefItem.QueryValue(pAny[i], pMap->nMemberId);
                }
            }
        }
        else
            throw UnknownPropertyException();
    }
    return aRet;
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
