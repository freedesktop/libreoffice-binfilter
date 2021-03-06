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

#include <hintids.hxx>

#include <bf_svx/lspcitem.hxx>
#include <bf_svx/adjitem.hxx>
#include <bf_svx/lrspitem.hxx>
#include <bf_svx/pgrditem.hxx>
#include <vcl/window.hxx>
#include <vcl/svapp.hxx>

#include <horiornt.hxx>

#include <doc.hxx>
#include <viewsh.hxx>   // ViewShell
#include <viewopt.hxx>
#include <pagefrm.hxx>  // SwPageFrm
#include <paratr.hxx>
#include <porrst.hxx>
#include <inftxt.hxx>
#include <tgrditem.hxx>
#include <pagedesc.hxx> // SwPageDesc
#include <frmatr.hxx>
#include <atrhndl.hxx>
namespace binfilter {

/*************************************************************************
 *                      class SwBreakPortion
 *************************************************************************/
/*N*/ SwBreakPortion::SwBreakPortion( const SwLinePortion &rPortion )
/*N*/     : SwLinePortion( rPortion ), nRestWidth( 0 )
/*N*/ {
/*N*/   nLineLength = 1;
/*N*/   SetWhichPor( POR_BRK );
/*N*/ }


/*N*/ SwLinePortion *SwBreakPortion::Compress()
/*N*/ { return (GetPortion() && GetPortion()->InTxtGrp() ? 0 : this); }


/*************************************************************************
 *                 virtual SwBreakPortion::Format()
 *************************************************************************/

/*N*/ sal_Bool SwBreakPortion::Format( SwTxtFormatInfo &rInf )
/*N*/ {
/*N*/     nRestWidth = (USHORT)(rInf.Width() - rInf.X());
/*N*/   register const SwLinePortion *pRoot = rInf.GetRoot();
/*N*/   Width( 0 );
/*N*/   Height( pRoot->Height() );
/*N*/   SetAscent( pRoot->GetAscent() );
/*N*/   if ( rInf.GetIdx()+1 == rInf.GetTxt().Len() )
/*N*/       rInf.SetNewLine( sal_True );
/*N*/   return sal_True;
/*N*/ }

/*************************************************************************
 *              virtual SwBreakPortion::HandlePortion()
 *************************************************************************/



/*N*/ SwKernPortion::SwKernPortion( SwLinePortion &rPortion, short nKrn,
/*N*/                               sal_Bool bBG, sal_Bool bGK ) :
/*N*/     nKern( nKrn ), bBackground( bBG ), bGridKern( bGK )
/*N*/ {
/*N*/   Height( rPortion.Height() );
/*N*/   SetAscent( rPortion.GetAscent() );
/*N*/   nLineLength = 0;
/*N*/   SetWhichPor( POR_KERN );
/*N*/   if( nKern > 0 )
/*N*/       Width( nKern );
/*N*/   rPortion.Insert( this );
/*N*/ }


/*N*/ void SwKernPortion::FormatEOL( SwTxtFormatInfo &rInf )
/*N*/ {
/*N*/     if ( bGridKern )
/*N*/         return;
/*N*/
/*N*/     if( rInf.GetLast() == this )
/*N*/       rInf.SetLast( FindPrevPortion( rInf.GetRoot() ) );
/*N*/   if( nKern < 0 )
/*N*/       Width( -nKern );
/*N*/   else
/*N*/       Width( 0 );
/*N*/   rInf.GetLast()->FormatEOL( rInf );
/*N*/ }

SwArrowPortion::SwArrowPortion( const SwLinePortion & /*rPortion*/ ) :
    bLeft( sal_True )
{
    DBG_BF_ASSERT(0, "STRIP");  // CONSTRUCTOR
}

/*N*/ SwLinePortion *SwArrowPortion::Compress() { return this; }

/*N*/ SwTwips SwTxtFrm::EmptyHeight() const
/*N*/ {
/*N*/     OSL_ENSURE( ! IsVertical() || ! IsSwapped(),"SwTxtFrm::EmptyHeight with swapped frame" );
/*N*/
/*N*/     SwFont *pFnt;
/*N*/   const SwTxtNode& rTxtNode = *GetTxtNode();
/*N*/   if ( rTxtNode.HasSwAttrSet() )
/*N*/   {
/*N*/       const SwAttrSet *pAttrSet = &( rTxtNode.GetSwAttrSet() );
/*N*/         pFnt = new SwFont( pAttrSet, GetTxtNode()->GetDoc() );
/*N*/   }
/*N*/   else
/*N*/   {
/*N*/       SwFontAccess aFontAccess( &rTxtNode.GetAnyFmtColl(), NULL);
/*N*/         pFnt = new SwFont( *aFontAccess.Get()->GetFont() );
/*N*/       pFnt->ChkMagic( NULL, pFnt->GetActual() );
/*N*/   }
/*N*/
/*N*/     if ( IsVertical() )
/*?*/         pFnt->SetVertical( 2700 );
/*N*/
/*N*/   OutputDevice *pOut = NULL;
/*N*/   pOut = &rTxtNode.GetDoc()->GetRefDev();
/*N*/
/*N*/   SwTwips nRet;
/*N*/   if( !pOut )
/*?*/         nRet = IsVertical() ?
/*?*/                Prt().SSize().Width() + 1 :
/*?*/                Prt().SSize().Height() + 1;
/*N*/   else
/*N*/   {
/*N*/       pFnt->SetFntChg( sal_True );
/*N*/       pFnt->ChgPhysFnt( NULL, pOut );
/*N*/       nRet = pFnt->GetHeight( NULL, pOut );
/*N*/   }
/*N*/   delete pFnt;
/*N*/   return nRet;
/*N*/ }

/*************************************************************************
 *                      SwTxtFrm::FormatEmpty()
 *************************************************************************/

/*N*/ sal_Bool SwTxtFrm::FormatEmpty()
/*N*/ {
/*N*/     OSL_ENSURE( ! IsVertical() || ! IsSwapped(),"SwTxtFrm::FormatEmpty with swapped frame" );
/*N*/
/*N*/   if ( HasFollow() || GetTxtNode()->GetpSwpHints() ||
/*N*/       0 != GetTxtNode()->GetNumRule() ||
/*N*/       0 != GetTxtNode()->GetOutlineNum() ||
/*N*/        IsInFtn() || ( HasPara() && GetPara()->IsPrepMustFit() ) )
/*N*/       return sal_False;
/*N*/   const SwAttrSet& aSet = GetTxtNode()->GetSwAttrSet();
/*N*/ #ifdef BIDI
/*N*/     const USHORT nAdjust = aSet.GetAdjust().GetAdjust();
/*N*/     if( ( ( ! IsRightToLeft() && ( SVX_ADJUST_LEFT != nAdjust ) ) ||
/*N*/           (   IsRightToLeft() && ( SVX_ADJUST_RIGHT != nAdjust ) ) ) ||
/*N*/           aSet.GetRegister().GetValue() )
/*N*/ #else
/*N*/   if( SVX_ADJUST_LEFT != aSet.GetAdjust().GetAdjust()
/*N*/       || aSet.GetRegister().GetValue() )
/*N*/ #endif
/*N*/       return sal_False;
/*N*/   const SvxLineSpacingItem &rSpacing = aSet.GetLineSpacing();
/*N*/   if( SVX_LINE_SPACE_MIN == rSpacing.GetLineSpaceRule() ||
/*N*/       SVX_LINE_SPACE_FIX == rSpacing.GetLineSpaceRule() ||
/*N*/       aSet.GetLRSpace().IsAutoFirst() )
/*N*/       return sal_False;
/*N*/   else
/*N*/   {
/*N*/         SwTxtFly aTxtFly( this );
/*N*/       SwRect aRect;
/*N*/       sal_Bool bFirstFlyCheck = 0 != Prt().Height();
/*N*/       if ( bFirstFlyCheck &&
/*N*/            aTxtFly.IsOn() && aTxtFly.IsAnyObj( aRect ) )
/*N*/           return sal_False;
/*N*/       else
/*N*/       {
/*N*/             SwTwips nHeight = EmptyHeight();
/*N*/
/*N*/             if ( GetTxtNode()->GetSwAttrSet().GetParaGrid().GetValue() &&
/*N*/                  IsInDocBody() )
/*N*/             {
/*N*/                 GETGRID( FindPageFrm() )
/*N*/                 if ( pGrid )
/*?*/                     nHeight = pGrid->GetBaseHeight() + pGrid->GetRubyHeight();
/*N*/             }
/*N*/
/*N*/             SWRECTFN( this )
/*N*/             const SwTwips nChg = nHeight - (Prt().*fnRect->fnGetHeight)();
/*N*/
/*N*/             if( !nChg )
/*N*/               SetUndersized( sal_False );
/*N*/           AdjustFrm( nChg );
/*N*/
/*N*/           if( HasBlinkPor() )
/*N*/           {
/*?*/               ClearPara();
/*?*/               ResetBlinkPor();
/*N*/           }
/*N*/           SetCacheIdx( MSHRT_MAX );
/*N*/           if( !IsEmpty() )
/*N*/           {
/*N*/               SetEmpty( sal_True );
/*N*/               SetCompletePaint();
/*N*/           }
/*N*/           if( !bFirstFlyCheck &&
/*N*/                aTxtFly.IsOn() && aTxtFly.IsAnyObj( aRect ) )
/*N*/                return sal_False;
/*N*/           return sal_True;
/*N*/       }
/*N*/   }
/*N*/ }

/*N*/ sal_Bool SwTxtFrm::FillRegister( SwTwips& rRegStart, KSHORT& rRegDiff )
/*N*/ {
/*N*/   const SwFrm *pFrm = this;
/*N*/   rRegDiff = 0;
/*N*/   while( !( ( FRM_BODY | FRM_FLY )
/*N*/          & pFrm->GetType() ) && pFrm->GetUpper() )
/*N*/       pFrm = pFrm->GetUpper();
/*N*/   if( ( FRM_BODY| FRM_FLY ) & pFrm->GetType() )
/*N*/   {
/*N*/         SWRECTFN( pFrm )
/*N*/         rRegStart = (pFrm->*fnRect->fnGetPrtTop)();
/*N*/       pFrm = pFrm->FindPageFrm();
/*N*/       if( pFrm->IsPageFrm() )
/*N*/       {
/*N*/           SwPageDesc* pDesc = ((SwPageFrm*)pFrm)->FindPageDesc();
/*N*/           if( pDesc )
/*N*/           {
/*N*/               rRegDiff = pDesc->GetRegHeight();
/*N*/               if( !rRegDiff )
/*N*/               {
/*N*/                   const SwTxtFmtColl *pFmt = pDesc->GetRegisterFmtColl();
/*N*/                   if( pFmt )
/*N*/                   {
/*N*/                       const SvxLineSpacingItem &rSpace = pFmt->GetLineSpacing();
/*N*/                       if( SVX_LINE_SPACE_FIX == rSpace.GetLineSpaceRule() )
/*N*/                       {
/*?*/                           rRegDiff = rSpace.GetLineHeight();
/*?*/                           pDesc->SetRegHeight( rRegDiff );
/*?*/                           pDesc->SetRegAscent( ( 4 * rRegDiff ) / 5 );
/*N*/                       }
/*N*/                       else
/*N*/                       {
/*N*/                           SwFontAccess aFontAccess( pFmt, NULL );
/*N*/                           SwFont aFnt( *aFontAccess.Get()->GetFont() );
/*N*/                           OutputDevice *pOut = 0;
/*N*/                           if( !GetTxtNode()->GetDoc()->IsBrowseMode() )
/*N*/                                 pOut = &GetTxtNode()->GetDoc()->GetRefDev();
/*N*/                           if( !pOut )
/*?*/                               pOut = GetpApp()->GetDefaultDevice();
/*N*/                           MapMode aOldMap( pOut->GetMapMode() );
/*N*/                           pOut->SetMapMode( MapMode( MAP_TWIP ) );
/*N*/                           aFnt.ChgFnt( NULL, pOut );
/*N*/                           rRegDiff = aFnt.GetHeight( NULL, pOut );
/*N*/                           KSHORT nNettoHeight = rRegDiff;
/*N*/                           switch( rSpace.GetLineSpaceRule() )
/*N*/                           {
/*N*/                               case SVX_LINE_SPACE_AUTO:
/*N*/                               break;
/*N*/                               case SVX_LINE_SPACE_MIN:
/*N*/                               {
/*N*/                                   if( rRegDiff < KSHORT( rSpace.GetLineHeight() ) )
/*N*/                                       rRegDiff = rSpace.GetLineHeight();
/*N*/                                   break;
/*N*/                               }
/*N*/                               default: OSL_ENSURE(
/*N*/                               sal_False, ": unknown LineSpaceRule" );
/*N*/                           }
/*N*/                           switch( rSpace.GetInterLineSpaceRule() )
/*N*/                           {
/*N*/                               case SVX_INTER_LINE_SPACE_OFF:
/*N*/                               break;
/*?*/                               case SVX_INTER_LINE_SPACE_PROP:
/*?*/                               {
/*?*/                                   long nTmp = rSpace.GetPropLineSpace();
/*?*/                                   if( nTmp < 50 )
/*?*/                                       nTmp = nTmp ? 50 : 100;
/*?*/                                   nTmp *= rRegDiff;
/*?*/                                   nTmp /= 100;
/*?*/                                   if( !nTmp )
/*?*/                                       ++nTmp;
/*?*/                                   rRegDiff = (KSHORT)nTmp;
/*?*/                                   nNettoHeight = rRegDiff;
/*?*/                                   break;
/*?*/                               }
/*?*/                               case SVX_INTER_LINE_SPACE_FIX:
/*?*/                               {
/*?*/                                   rRegDiff += rSpace.GetInterLineSpace();
/*?*/                                   nNettoHeight = rRegDiff;
/*?*/                                   break;
/*?*/                               }
/*?*/                               default: OSL_FAIL( ": unknown InterLineSpaceRule" );
/*N*/                           }
/*N*/                           pDesc->SetRegHeight( rRegDiff );
/*N*/                           pDesc->SetRegAscent( rRegDiff - nNettoHeight +
/*N*/                                                aFnt.GetAscent( NULL, pOut ) );
/*N*/                           pOut->SetMapMode( aOldMap );
/*N*/                       }
/*N*/                   }
/*N*/               }
/*N*/                 const long nTmpDiff = pDesc->GetRegAscent() - rRegDiff;
/*N*/                 if ( bVert )
/*?*/                     rRegStart -= nTmpDiff;
/*N*/                 else
/*N*/                     rRegStart += nTmpDiff;
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/   return ( 0 != rRegDiff );
/*N*/ }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
