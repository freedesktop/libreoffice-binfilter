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
#include <numitem.hxx>

#define ITEMID_BRUSH SID_ATTR_BRUSH
#include <brshitem.hxx>
#include <vcl/font.hxx>
#include <svxids.hrc>
#include <numdef.hxx>
#include <vcl/graph.hxx>
#include <vcl/window.hxx>
#include <vcl/svapp.hxx>
#include <unolingu.hxx>
#include <com/sun/star/text/XNumberingFormatter.hpp>
#include <com/sun/star/text/XDefaultNumberingProvider.hpp>
#include <com/sun/star/style/NumberingType.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <comphelper/processfactory.hxx>

#include "unonrule.hxx"
#include <legacysmgr/legacy_binfilters_smgr.hxx>
#include <bf_tools/string.hxx>
namespace binfilter {

#define MM100_TO_TWIP(MM100)    ((MM100*72L+63L)/127L)

#define DEF_WRITER_LSPACE   500     //Standardeinrueckung
#define DEF_DRAW_LSPACE     800     //Standardeinrueckung

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::style;

sal_Int32 SvxNumberType::nRefCount = 0;
::com::sun::star::uno::Reference< ::com::sun::star::text::XNumberingFormatter> SvxNumberType::xFormatter = 0;


/*N*/ SvxNumberType::SvxNumberType(sal_Int16 nType) :
/*N*/   nNumType(nType),
/*N*/   bShowSymbol(sal_True)
/*N*/ {
/*N*/   if(!xFormatter.is())
/*N*/       {
/*N*/       try
/*N*/       {
/*N*/           Reference< XMultiServiceFactory > xMSF = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/           Reference < XInterface > xI = xMSF->createInstance(
/*N*/               ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.text.DefaultNumberingProvider" )) );
/*N*/           Reference<XDefaultNumberingProvider> xRet(xI, UNO_QUERY);
/*N*/           DBG_ASSERT(xRet.is(), "service missing: \"com.sun.star.text.DefaultNumberingProvider\"");
/*N*/           xFormatter = Reference<XNumberingFormatter> (xRet, UNO_QUERY);
/*N*/       }
/*N*/       catch(Exception& )
/*N*/       {
/*N*/       }
/*N*/   }
/*N*/   nRefCount++;
/*N*/ }


/*N*/ SvxNumberType::SvxNumberType(const SvxNumberType& rType) :
/*N*/   nNumType(rType.nNumType),
/*N*/   bShowSymbol(rType.bShowSymbol)
/*N*/ {
/*N*/   nRefCount++;
/*N*/ }


/*N*/ SvxNumberType::~SvxNumberType()
/*N*/ {
/*N*/   if(!--nRefCount)
/*N*/       xFormatter = 0;
/*N*/ }


/*N*/ String SvxNumberType::GetNumStr( ULONG nNo ) const
/*N*/ {
/*N*/     LanguageType eLang = Application::GetSettings().GetLanguage();
/*N*/   Locale aLocale = SvxCreateLocale(eLang);
/*N*/   return GetNumStr( nNo, aLocale );
/*N*/ }


/*N*/ String    SvxNumberType::GetNumStr( ULONG nNo, const Locale& rLocale ) const
/*N*/ {
/*N*/   String aTmpStr;
/*N*/   if(!xFormatter.is())
/*N*/       return aTmpStr;
/*N*/
/*N*/   if(bShowSymbol)
/*N*/   {
/*N*/       switch(nNumType)
/*N*/       {
/*N*/             case NumberingType::CHAR_SPECIAL:
/*N*/             case NumberingType::BITMAP:
/*N*/           break;
/*N*/           default:
/*N*/               {
/*N*/                   //#95525# '0' allowed for ARABIC numberings
/*N*/                   if(NumberingType::ARABIC == nNumType && 0 == nNo )
/*N*/                       aTmpStr = '0';
/*N*/                   else
/*N*/                   {
/*N*/                       Sequence< PropertyValue > aProperties(2);
/*N*/                       PropertyValue* pValues = aProperties.getArray();
/*N*/                       pValues[0].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("NumberingType"));
/*N*/                       pValues[0].Value <<= nNumType;
/*N*/                       pValues[1].Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Value"));
/*N*/                       pValues[1].Value <<= (sal_Int32)nNo;
/*N*/
/*N*/                       try
/*N*/                       {
/*N*/                           aTmpStr = xFormatter->makeNumberingString( aProperties, rLocale );
/*N*/                       }
/*N*/                       catch(Exception&)
/*N*/                       {
/*N*/                       }
/*N*/                   }
/*N*/               }
/*N*/       }
/*N*/   }
/*N*/   return aTmpStr;
/*N*/ }


/*N*/ SvxNumberFormat::SvxNumberFormat(sal_Int16 eType) :
/*N*/   SvxNumberType(eType),
/*N*/   eNumAdjust(SVX_ADJUST_LEFT),
/*N*/   nInclUpperLevels(0),
/*N*/   nStart(1),
/*N*/   cBullet(SVX_DEF_BULLET),
/*N*/   nBulletRelSize(100),
/*N*/   nBulletColor(COL_BLACK),
/*N*/   nFirstLineOffset(0),
/*N*/   nAbsLSpace(0),
/*N*/   nLSpace(0),
/*N*/   nCharTextDistance(0),
/*N*/   pGraphicBrush(0),
/*N*/   eVertOrient(SVX_VERT_NONE),
/*N*/   pBulletFont(0)
/*N*/ {
/*N*/ }


/*N*/ SvxNumberFormat::SvxNumberFormat(const SvxNumberFormat& rFormat) :
/*N*/     SvxNumberType(rFormat),
/*N*/     pGraphicBrush(0),
/*N*/     pBulletFont(0)
/*N*/ {
/*N*/     *this = rFormat;
/*N*/ }


/*N*/ SvxNumberFormat::~SvxNumberFormat()
/*N*/ {
/*N*/   delete pGraphicBrush;
/*N*/   delete pBulletFont;
/*N*/ }


/*N*/ SvxNumberFormat::SvxNumberFormat(SvStream &rStream)
/*N*/ {
/*N*/     USHORT nVersion;
/*N*/   rStream >> nVersion;
/*N*/
/*N*/   USHORT nUSHORT;
/*N*/   rStream >> nUSHORT;
/*N*/   SetNumberingType((sal_Int16)nUSHORT);
/*N*/   rStream >> nUSHORT;
/*N*/   eNumAdjust      = (SvxAdjust)nUSHORT;
/*N*/   rStream >> nUSHORT;
/*N*/   nInclUpperLevels = (BYTE)nUSHORT;
/*N*/   rStream >> nUSHORT;
/*N*/   nStart          = nUSHORT;
/*N*/   rStream >> nUSHORT;
/*N*/   cBullet = nUSHORT;
/*N*/
/*N*/   short nShort;
/*N*/   rStream >> nShort;
/*N*/   nFirstLineOffset        = nShort;
/*N*/   rStream >> nShort;
/*N*/   nAbsLSpace              = nShort;
/*N*/   rStream >> nShort;
/*N*/   nLSpace                 = nShort;
/*N*/
/*N*/   rStream >> nShort;
/*N*/   nCharTextDistance       = nShort;
/*N*/   rtl_TextEncoding eEnc = osl_getThreadTextEncoding();
/*N*/   sPrefix = rStream.ReadUniOrByteString(eEnc);
/*N*/   sSuffix = rStream.ReadUniOrByteString(eEnc);
/*N*/   sCharStyleName = rStream.ReadUniOrByteString(eEnc);
/*N*/   rStream >> nUSHORT;
/*N*/   if(nUSHORT)
/*N*/   {
/*?*/       SvxBrushItem aHelper;
/*?*/       pGraphicBrush = (SvxBrushItem*) aHelper.Create( rStream, BRUSH_GRAPHIC_VERSION );
/*N*/   }
/*N*/   else
/*N*/       pGraphicBrush = 0;
/*N*/
/*N*/   rStream >> nUSHORT;
/*N*/   eVertOrient          = (SvxFrameVertOrient)nUSHORT;
/*N*/
/*N*/   rStream >> nUSHORT;
/*N*/   if(nUSHORT)
/*N*/   {
/*N*/       pBulletFont = new Font;
/*N*/       rStream >> *pBulletFont;
/*N*/         if(!pBulletFont->GetCharSet())
/*N*/             pBulletFont->SetCharSet(rStream.GetStreamCharSet());
/*N*/     }
/*N*/   else
/*N*/       pBulletFont = 0;
/*N*/   rStream >> aGraphicSize;
/*N*/
/*N*/   rStream >> nBulletColor;
/*N*/   rStream >> nUSHORT;
/*N*/   nBulletRelSize = nUSHORT;
/*N*/   rStream >> nUSHORT;
/*N*/   SetShowSymbol((BOOL)nUSHORT);
/*N*/
/*N*/   if( nVersion < NUMITEM_VERSION_03 )
/*N*/       cBullet = ByteString::ConvertToUnicode( cBullet,
/*N*/                           (pBulletFont&&pBulletFont->GetCharSet()) ?  pBulletFont->GetCharSet()
/*N*/                                       : RTL_TEXTENCODING_SYMBOL );
/*N*/     if(pBulletFont)
/*N*/     {
/*N*/         BOOL bConvertBulletFont = rStream.GetVersion() <= SOFFICE_FILEFORMAT_50;
/*N*/         if(bConvertBulletFont)
/*N*/         {
/*N*/
/*N*/             FontToSubsFontConverter pConverter =
/*N*/                         CreateFontToSubsFontConverter(pBulletFont->GetName(),
/*N*/                             FONTTOSUBSFONT_IMPORT|FONTTOSUBSFONT_ONLYOLDSOSYMBOLFONTS);
/*N*/             if(pConverter)
/*N*/             {
/*N*/                 cBullet = ConvertFontToSubsFontChar(pConverter, cBullet);
/*N*/                 String sFontName = GetFontToSubsFontName(pConverter);
/*N*/                 pBulletFont->SetName(sFontName);
/*N*/                 DestroyFontToSubsFontConverter(pConverter);
/*N*/             }
/*N*/         }
/*N*/     }
/*N*/ }


/*N*/ SvxNumberFormat& SvxNumberFormat::operator=( const SvxNumberFormat& rFormat )
/*N*/ {
/*N*/   SetNumberingType(rFormat.GetNumberingType());
/*N*/       eNumAdjust          = rFormat.eNumAdjust ;
/*N*/       nInclUpperLevels    = rFormat.nInclUpperLevels ;
/*N*/       nStart              = rFormat.nStart ;
/*N*/       cBullet             = rFormat.cBullet ;
/*N*/       nFirstLineOffset    = rFormat.nFirstLineOffset;
/*N*/       nAbsLSpace          = rFormat.nAbsLSpace ;
/*N*/       nLSpace             = rFormat.nLSpace ;
/*N*/       nCharTextDistance   = rFormat.nCharTextDistance ;
/*N*/       eVertOrient         = rFormat.eVertOrient ;
/*N*/       sPrefix             = rFormat.sPrefix     ;
/*N*/       sSuffix             = rFormat.sSuffix     ;
/*N*/       aGraphicSize        = rFormat.aGraphicSize  ;
/*N*/       nBulletColor        = rFormat.nBulletColor   ;
/*N*/       nBulletRelSize      = rFormat.nBulletRelSize;
/*N*/       SetShowSymbol(rFormat.IsShowSymbol());
/*N*/       sCharStyleName      = rFormat.sCharStyleName;
/*N*/   DELETEZ(pGraphicBrush);
/*N*/   DELETEZ(pBulletFont);
/*N*/   if(rFormat.pBulletFont)
/*N*/           pBulletFont = new Font(*rFormat.pBulletFont);
/*N*/   return *this;
/*N*/ }


/*N*/ BOOL  SvxNumberFormat::operator==( const SvxNumberFormat& rFormat) const
/*N*/ {
/*N*/   if( GetNumberingType()  != rFormat.GetNumberingType() ||
/*N*/       eNumAdjust          != rFormat.eNumAdjust ||
/*N*/       nInclUpperLevels    != rFormat.nInclUpperLevels ||
/*N*/       nStart              != rFormat.nStart ||
/*N*/       cBullet             != rFormat.cBullet ||
/*N*/       nFirstLineOffset    != rFormat.nFirstLineOffset ||
/*N*/       nAbsLSpace          != rFormat.nAbsLSpace ||
/*N*/       nLSpace             != rFormat.nLSpace ||
/*N*/       nCharTextDistance   != rFormat.nCharTextDistance ||
/*N*/       eVertOrient         != rFormat.eVertOrient ||
/*N*/       sPrefix             != rFormat.sPrefix     ||
/*N*/       sSuffix             != rFormat.sSuffix     ||
/*N*/       aGraphicSize        != rFormat.aGraphicSize  ||
/*N*/       nBulletColor        != rFormat.nBulletColor   ||
/*N*/       nBulletRelSize      != rFormat.nBulletRelSize ||
/*N*/       IsShowSymbol()      != rFormat.IsShowSymbol() ||
/*N*/       sCharStyleName      != rFormat.sCharStyleName
/*N*/       )
/*N*/       return FALSE;
/*N*/   if((pGraphicBrush && !rFormat.pGraphicBrush) ||
/*N*/           (!pGraphicBrush && rFormat.pGraphicBrush) ||
/*N*/               (pGraphicBrush && *pGraphicBrush != *rFormat.pGraphicBrush ))
/*N*/       return FALSE;
/*N*/   if( (pBulletFont && !rFormat.pBulletFont) ||
/*N*/           (!pBulletFont && rFormat.pBulletFont) ||
/*N*/           (pBulletFont && (*pBulletFont != *rFormat.pBulletFont)))
/*N*/       return FALSE;
/*N*/   return TRUE;
/*N*/ }


/*N*/ void SvxNumberFormat::SetGraphicBrush( const SvxBrushItem* pBrushItem,
/*N*/                   const Size* pSize, const SvxFrameVertOrient* pOrient)
/*N*/ {
/*N*/   if(!pBrushItem)
/*N*/   {
/*N*/       delete pGraphicBrush;
/*N*/       pGraphicBrush = 0;
/*N*/   }
/*N*/
/*N*/   if(pOrient)
/*N*/       eVertOrient = *pOrient;
/*N*/   else
/*?*/       eVertOrient = SVX_VERT_NONE;
/*N*/   if(pSize)
/*N*/       aGraphicSize = *pSize;
/*N*/   else
/*?*/       aGraphicSize.Width() = aGraphicSize.Height() = 0;
/*N*/ }




/*N*/ SvxFrameVertOrient    SvxNumberFormat::GetVertOrient() const
/*N*/ {
/*N*/   return eVertOrient;
/*N*/ }


/*N*/ void SvxNumberFormat::SetBulletFont(const Font* pFont)
/*N*/ {
/*N*/   delete pBulletFont;
/*N*/   pBulletFont = pFont ? new Font(*pFont): 0;
/*N*/ }




/*N*/ String SvxNumberFormat::CreateRomanString( ULONG nNo, BOOL bUpper )
/*N*/ {
/*N*/   nNo %= 4000;            // mehr kann nicht dargestellt werden
/*N*/ //        i, ii, iii, iv, v, vi, vii, vii, viii, ix
/*N*/ //                            (Dummy),1000,500,100,50,10,5,1
/*N*/   const char *cRomanArr = bUpper
/*N*/                       ? "MDCLXVI--"   // +2 Dummy-Eintraege !!
/*N*/                       : "mdclxvi--";  // +2 Dummy-Eintraege !!
/*N*/
/*N*/   String sRet;
/*N*/   USHORT nMask = 1000;
/*N*/   while( nMask )
/*N*/   {
/*N*/       BYTE nZahl = BYTE(nNo / nMask);
/*N*/       BYTE nDiff = 1;
/*N*/       nNo %= nMask;
/*N*/
/*N*/       if( 5 < nZahl )
/*N*/       {
/*N*/           if( nZahl < 9 )
/*N*/               sRet += sal_Unicode(*(cRomanArr-1));
/*N*/           ++nDiff;
/*N*/           nZahl -= 5;
/*N*/       }
/*N*/       switch( nZahl )
/*N*/       {
/*N*/       case 3:     { sRet += sal_Unicode(*cRomanArr); }
/*N*/       case 2:     { sRet += sal_Unicode(*cRomanArr); }
/*N*/       case 1:     { sRet += sal_Unicode(*cRomanArr); }
/*N*/                   break;
/*N*/
/*N*/       case 4:     {
/*N*/                       sRet += sal_Unicode(*cRomanArr);
/*N*/                       sRet += sal_Unicode(*(cRomanArr-nDiff));
/*N*/                   }
/*N*/                   break;
/*N*/       case 5:     { sRet += sal_Unicode(*(cRomanArr-nDiff)); }
/*N*/                   break;
/*N*/       }
/*N*/
/*N*/       nMask /= 10;            // zur naechsten Dekade
/*N*/       cRomanArr += 2;
/*N*/   }
/*N*/   return sRet;
/*N*/ }
#ifdef OLD_NUMBER_FORMATTING
/*?*/ void SvxNumberFormat::GetCharStr( ULONG nNo, String& rStr ) const
/*?*/ {
/*?*/   DBG_ASSERT( nNo, "0 ist eine ungueltige Nummer !!" );
/*?*/
/*?*/   const ULONG coDiff = 'Z' - 'A' +1;
/*?*/   char cAdd = (SVX_NUM_CHARS_UPPER_LETTER == eNumType ? 'A' : 'a') - 1;
/*?*/   ULONG nCalc;
/*?*/
/*?*/   do {
/*?*/       nCalc = nNo % coDiff;
/*?*/       if( !nCalc )
/*?*/           nCalc = coDiff;
/*?*/       rStr.Insert( sal_Unicode(cAdd + nCalc ), 0 );
/*?*/       nNo -= nCalc;
/*?*/       if( nNo )
/*?*/           nNo /= coDiff;
/*?*/   } while( nNo );
/*?*/ }
/*?*/
/*?*/ void SvxNumberFormat::GetCharStrN( ULONG nNo, String& rStr ) const
/*?*/ {
/*?*/   DBG_ASSERT( nNo, "0 ist eine ungueltige Nummer !!" );
/*?*/
/*?*/   const ULONG coDiff = 'Z' - 'A' +1;
/*?*/   char cChar = (char)(--nNo % coDiff);
/*?*/   if( SVX_NUM_CHARS_UPPER_LETTER_N == eNumType )
/*?*/       cChar += 'A';
/*?*/   else
/*?*/       cChar += 'a';
/*?*/
/*?*/   rStr.Fill( (USHORT)(nNo / coDiff) + 1, sal_Unicode(cChar) );
/*?*/ }
#endif //OLD_NUMBER_FORMATTING


/*N*/ const String& SvxNumberFormat::GetCharFmtName()const
/*N*/ {
/*N*/   return sCharStyleName;
/*N*/ }


sal_Int32 SvxNumRule::nRefCount = 0;
static SvxNumberFormat* pStdNumFmt = 0;
static SvxNumberFormat* pStdOutlineNumFmt = 0;
/*N*/ SvxNumRule::SvxNumRule(ULONG nFeatures, USHORT nLevels, BOOL bCont, SvxNumRuleType eType) :
/*N*/   nLevelCount(nLevels),
/*N*/   nFeatureFlags(nFeatures),
/*N*/   eNumberingType(eType),
/*N*/   bContinuousNumbering(bCont)
/*N*/ {
/*N*/   ++nRefCount;
/*N*/     LanguageType eLang = Application::GetSettings().GetLanguage();
/*N*/   aLocale = SvxCreateLocale(eLang);
/*N*/   for(USHORT i = 0; i < SVX_MAX_NUM; i++)
/*N*/   {
/*N*/       if(i < nLevels)
/*N*/       {
/*N*/           aFmts[i] = new SvxNumberFormat(SVX_NUM_CHARS_UPPER_LETTER);
/*N*/           //daran wird zwischen writer und draw unterschieden
/*N*/           if(nFeatures & NUM_CONTINUOUS)
/*N*/           {
/*N*/               aFmts[i]->SetLSpace( MM100_TO_TWIP(DEF_WRITER_LSPACE) );
/*N*/               aFmts[i]->SetAbsLSpace( MM100_TO_TWIP(DEF_WRITER_LSPACE * (i+1)) );
/*N*/                 aFmts[i]->SetFirstLineOffset(MM100_TO_TWIP(-DEF_WRITER_LSPACE));
/*N*/           }
/*N*/           else
/*N*/           {
/*N*/               aFmts[i]->SetLSpace( DEF_DRAW_LSPACE );
/*N*/               aFmts[i]->SetAbsLSpace( DEF_DRAW_LSPACE * (i) );
/*N*/           }
/*N*/       }
/*N*/       else
/*N*/           aFmts[i] = 0;
/*N*/       aFmtsSet[i] = FALSE;
/*N*/   }
/*N*/ }


/*N*/ SvxNumRule::SvxNumRule(const SvxNumRule& rCopy)
/*N*/ {
/*N*/   ++nRefCount;
/*N*/   aLocale             = rCopy.aLocale;
/*N*/   nLevelCount          = rCopy.nLevelCount         ;
/*N*/   nFeatureFlags        = rCopy.nFeatureFlags       ;
/*N*/   bContinuousNumbering = rCopy.bContinuousNumbering;
/*N*/   eNumberingType       = rCopy.eNumberingType;
/*N*/   memset( aFmts, 0, sizeof( aFmts ));
/*N*/   for(USHORT i = 0; i < SVX_MAX_NUM; i++)
/*N*/   {
/*N*/       if(rCopy.aFmts[i])
/*N*/           aFmts[i] = new SvxNumberFormat(*rCopy.aFmts[i]);
/*N*/       else
/*N*/           aFmts[i] = 0;
/*N*/       aFmtsSet[i] = rCopy.aFmtsSet[i];
/*N*/   }
/*N*/ }


/*N*/ SvxNumRule::SvxNumRule(SvStream &rStream)
/*N*/ {
/*N*/   ++nRefCount;
/*N*/     LanguageType eLang = Application::GetSettings().GetLanguage();
/*N*/   aLocale = SvxCreateLocale(eLang);
/*N*/   USHORT nVersion;
/*N*/   USHORT nTemp;
/*N*/   rStream >> nVersion;
/*N*/   rStream >> nLevelCount;
/*N*/   rStream >> nTemp;
/*N*/   nFeatureFlags = nTemp;
/*N*/   rStream >> nTemp;
/*N*/   bContinuousNumbering = (BOOL)nTemp;
/*N*/   rStream >> nTemp;
/*N*/   eNumberingType       = (SvxNumRuleType)nTemp;
/*N*/   memset( aFmts, 0, sizeof( aFmts ));
/*N*/
/*N*/   for(USHORT i = 0; i < SVX_MAX_NUM; i++)
/*N*/   {
/*N*/       USHORT nSet;
/*N*/       rStream >> nSet;
/*N*/       if(nSet)
/*N*/           aFmts[i] = new SvxNumberFormat(rStream);
/*N*/       else
/*N*/           aFmts[i] = 0;
/*N*/       aFmtsSet[i] = aFmts[i] ? TRUE : FALSE;
/*N*/   }
/*N*/   if(NUMITEM_VERSION_02 <= nVersion)
/*N*/   {
/*N*/       USHORT nShort;
/*N*/       rStream >> nShort;
/*N*/       nFeatureFlags = nShort;
/*N*/   }
/*N*/ }



/*N*/ SvxNumRule::~SvxNumRule()
/*N*/ {
/*N*/   for(USHORT i = 0; i < SVX_MAX_NUM; i++)
/*N*/       delete aFmts[i];
/*N*/   if(!--nRefCount)
/*N*/   {
/*N*/       DELETEZ(pStdNumFmt);
/*N*/       DELETEZ(pStdOutlineNumFmt);
/*N*/   }
/*N*/ }




/*N*/ int   SvxNumRule::operator==( const SvxNumRule& rCopy) const
/*N*/ {
/*N*/   if(nLevelCount != rCopy.nLevelCount ||
/*N*/       nFeatureFlags != rCopy.nFeatureFlags ||
/*N*/           bContinuousNumbering != rCopy.bContinuousNumbering ||
/*N*/               eNumberingType != rCopy.eNumberingType)
/*N*/           return FALSE;
/*N*/   for(USHORT i = 0; i < nLevelCount; i++)
/*N*/   {
/*N*/       if( aFmtsSet[i] != rCopy.aFmtsSet[i] ||
/*N*/           (!aFmts[i] &&  rCopy.aFmts[i]) ||
/*N*/           (aFmts[i] &&  !rCopy.aFmts[i]) ||
/*N*/           (aFmts[i] && *aFmts[i] !=  *rCopy.aFmts[i] ))
/*N*/           return FALSE;
/*N*/   }
/*N*/   return TRUE;
/*N*/ }


/*N*/ const SvxNumberFormat*    SvxNumRule::Get(USHORT nLevel)const
/*N*/ {
/*N*/   DBG_ASSERT(nLevel < SVX_MAX_NUM, "falsches Level" );
/*N*/   return aFmtsSet[nLevel] ? aFmts[nLevel] : 0;
/*N*/ }


/*N*/ const SvxNumberFormat&    SvxNumRule::GetLevel(USHORT nLevel)const
/*N*/ {
/*N*/   if(!pStdNumFmt)
/*N*/   {
/*N*/       pStdNumFmt = new SvxNumberFormat(SVX_NUM_ARABIC);
/*N*/       pStdOutlineNumFmt = new SvxNumberFormat(SVX_NUM_NUMBER_NONE);
/*N*/   }
/*N*/
/*N*/   DBG_ASSERT(nLevel < SVX_MAX_NUM, "falsches Level" );
/*N*/   return aFmts[nLevel] ?
/*N*/       *aFmts[nLevel] :  eNumberingType == SVX_RULETYPE_NUMBERING ?
/*N*/                                                   *pStdNumFmt : *pStdOutlineNumFmt;
/*N*/ }



/*N*/ void SvxNumRule::SetLevel( USHORT i, const SvxNumberFormat& rNumFmt, BOOL bIsValid )
/*N*/ {
/*N*/   if( !aFmtsSet[i] || !(rNumFmt == *Get( i )) )
/*N*/   {
/*N*/       delete aFmts[ i ];
/*N*/       aFmts[ i ] = new SvxNumberFormat( rNumFmt );
/*N*/       aFmtsSet[i] = bIsValid;
/*N*/   }
/*N*/ }




// Description: changes linked to embedded bitmaps
/*N*/ BOOL SvxNumRule::UnLinkGraphics()
/*N*/ {
/*N*/   BOOL bRet = FALSE;
/*N*/   for(USHORT i = 0; i < GetLevelCount(); i++)
/*N*/   {
/*N*/       SvxNumberFormat aFmt(GetLevel(i));
/*N*/       if((SVX_NUM_BITMAP|LINK_TOKEN) == aFmt.GetNumberingType())
/*N*/           aFmt.SetNumberingType(SVX_NUM_BITMAP);
/*N*/       SetLevel(i, aFmt);
/*N*/   }
/*N*/   return bRet;
/*N*/ }



/*N*/ SvxNumBulletItem::SvxNumBulletItem(SvxNumRule& rRule) :
/*N*/   SfxPoolItem(SID_ATTR_NUMBERING_RULE),
/*N*/   pNumRule(new SvxNumRule(rRule))
/*N*/ {
/*N*/ }

// Das sind ja sehr sinnige Kommentare...
/*N*/ SvxNumBulletItem::SvxNumBulletItem(SvxNumRule& rRule, USHORT nWhich ) :
/*N*/   SfxPoolItem(nWhich),
/*N*/   pNumRule(new SvxNumRule(rRule))
/*N*/ {
/*N*/ }



/*N*/ SvxNumBulletItem::SvxNumBulletItem(const SvxNumBulletItem& rCopy) :
/*N*/   SfxPoolItem(rCopy.Which())
/*N*/ {
/*N*/   pNumRule = new SvxNumRule(*rCopy.pNumRule);
/*N*/ }


/*N*/ SvxNumBulletItem::~SvxNumBulletItem()
/*N*/ {
/*N*/   delete pNumRule;
/*N*/ }



/*N*/ int  SvxNumBulletItem::operator==( const SfxPoolItem& rCopy) const
/*N*/ {
/*N*/   return *pNumRule == *((SvxNumBulletItem&)rCopy).pNumRule;
/*N*/ }


/*N*/ SfxPoolItem*  SvxNumBulletItem::Clone( SfxItemPool* /*pPool*/ ) const
/*N*/ {
/*N*/   return new SvxNumBulletItem(*this);
/*N*/ }


/*N*/ SfxPoolItem*   SvxNumBulletItem::Create(SvStream &rStream, USHORT) const
/*N*/ {
/*N*/   SvxNumRule aRule(rStream);
/*N*/   return new SvxNumBulletItem(aRule, Which() );
/*N*/ }


/*N*/ USHORT    SvxNumBulletItem::GetVersion( USHORT /*nFileVersion*/ ) const
/*N*/ {
/*N*/   return NUMITEM_VERSION_03;
/*N*/ }





/*N*/ bool SvxNumBulletItem::PutValue( const ::com::sun::star::uno::Any& rVal, BYTE /*nMemberId*/ )
/*N*/ {
/*N*/   uno::Reference< container::XIndexReplace > xRule;
/*N*/   if( rVal >>= xRule )
/*N*/   {
/*N*/       try
/*N*/       {
/*N*/           SvxNumRule* pNewRule = new SvxNumRule( SvxGetNumRule( xRule ) );
/*N*/           if( pNewRule->GetLevelCount() != pNumRule->GetLevelCount() ||
/*N*/               pNewRule->GetNumRuleType() != pNumRule->GetNumRuleType() )
/*N*/           {
/*N*/               SvxNumRule* pConverted = SvxConvertNumRule( pNewRule, pNumRule->GetLevelCount(), pNumRule->GetNumRuleType() );
/*N*/               delete pNewRule;
/*N*/               pNewRule = pConverted;
/*N*/           }
/*N*/           delete pNumRule;
/*N*/           pNumRule = pNewRule;
/*N*/           return sal_True;
/*N*/       }
/*N*/       catch(lang::IllegalArgumentException&)
/*N*/       {
/*N*/       }
/*N*/   }
/*N*/   return sal_False;
/*N*/ }



/*N*/ SvxNumRule* SvxConvertNumRule( const SvxNumRule* pRule, USHORT nLevels, SvxNumRuleType eType )
/*N*/ {
/*N*/   const USHORT nSrcLevels = pRule->GetLevelCount();
/*N*/   SvxNumRule* pNewRule = new SvxNumRule( pRule->GetFeatureFlags(), nLevels, pRule->IsContinuousNumbering(), eType );
/*N*/
/*N*/   // move all levels one level up if the destination is a presentation numbering and the source is not
/*N*/   const sal_Bool bConvertUp = pRule->GetNumRuleType() != SVX_RULETYPE_PRESENTATION_NUMBERING &&
/*N*/                                 eType == SVX_RULETYPE_PRESENTATION_NUMBERING;
/*N*/
/*N*/   // move all levels one level down if the source is a presentation numbering and the destination is not
/*N*/   const sal_Bool bConvertDown = pRule->GetNumRuleType() == SVX_RULETYPE_PRESENTATION_NUMBERING &&
/*N*/                                 eType != SVX_RULETYPE_PRESENTATION_NUMBERING;
/*N*/
/*N*/   USHORT nSrcLevel = bConvertDown ? 1 : 0;
/*N*/   USHORT nDstLevel = bConvertUp ? 1 : 0;
/*N*/   for( ; (nDstLevel < nLevels) && (nSrcLevel < nSrcLevels); nSrcLevel++, nDstLevel++ )
/*N*/       pNewRule->SetLevel( nDstLevel, pRule->GetLevel( nSrcLevel ) );
/*N*/
/*N*/   return pNewRule;
/*N*/ }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
