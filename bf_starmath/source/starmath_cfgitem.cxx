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

#include <vcl/svapp.hxx>
#include <sal/macros.h>

#include "cfgitem.hxx"

#include "starmath.hrc"
#include "format.hxx"
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;

using ::rtl::OUString;

#define A2OU(x)        ::rtl::OUString::createFromAscii( x )

static const char* aRootName = "Office.Math";

#define SYMBOL_LIST         "SymbolList"
#define FONT_FORMAT_LIST    "FontFormatList"

SmFntFmtListEntryArr::SmFntFmtListEntryArr( USHORT nInit, BYTE )
    : pData (0),
      nFree (nInit),
      nA    (0)
{
    if( nInit )
    {
        pData = (SmFntFmtListEntry*)(rtl_allocateMemory(sizeof(SmFntFmtListEntry) * nInit));
        DBG_ASSERT( pData, "CTOR, allocate");
    }
}

void SmFntFmtListEntryArr::_destroy()
{
    if(pData)
    {
        SmFntFmtListEntry* pTmp=pData;
        for(USHORT n=0; n < nA; n++,pTmp++ )
        {
            pTmp->~SmFntFmtListEntry();
        }
        rtl_freeMemory(pData);
        pData = 0;
    }
}

void SmFntFmtListEntryArr::_resize (size_t n)
{
    USHORT nL = ((n < USHRT_MAX) ? USHORT(n) : USHRT_MAX);
    SmFntFmtListEntry* pE = (SmFntFmtListEntry*)(rtl_reallocateMemory (pData, sizeof(SmFntFmtListEntry) * nL));
    if ((pE != 0) || (nL == 0))
    {
        pData = pE;
        nFree = nL - nA;
    }
}

void SmFntFmtListEntryArr::Insert( const SmFntFmtListEntry &aE, USHORT nP )
{
    if (nFree < 1)
        _resize (nA + ((nA > 1) ? nA : 1));
    if( pData && nP < nA )
        memmove( pData+nP+1, pData+nP, (nA-nP) * sizeof( SmFntFmtListEntry ));
    SmFntFmtListEntry* pTmp = pData+nP;
    new( (DummyType*) pTmp ) SmFntFmtListEntry( (SmFntFmtListEntry&)aE );
    ++nA; --nFree;
}

/////////////////////////////////////////////////////////////////


/*N*/ static Sequence< OUString > lcl_GetFontPropertyNames()
/*N*/ {
/*N*/   static const char * aPropNames[] =
/*N*/   {
/*N*/         "Name",
/*N*/         "CharSet",
/*N*/         "Family",
/*N*/         "Pitch",
/*N*/         "Weight",
/*N*/         "Italic",
/*N*/       0
/*N*/   };
/*N*/
/*N*/   const char** ppPropName = aPropNames;
/*N*/
/*N*/     Sequence< OUString > aNames( 6 );
/*N*/   OUString *pNames = aNames.getArray();
/*N*/   for( INT32 i = 0; *ppPropName;  ++i, ++ppPropName )
/*N*/   {
/*N*/       pNames[i] = A2OU( *ppPropName );
/*N*/   }
/*N*/   return aNames;
/*N*/ }

/////////////////////////////////////////////////////////////////


/*N*/ static Sequence< OUString > lcl_GetSymbolPropertyNames()
/*N*/ {
/*N*/   static const char * aPropNames[] =
/*N*/   {
/*N*/         "Char",
/*N*/         "Set",
/*N*/         "Predefined",
/*N*/         "FontFormatId",
/*N*/       0
/*N*/   };
/*N*/
/*N*/   const char** ppPropName = aPropNames;
/*N*/
/*N*/     Sequence< OUString > aNames( 4 );
/*N*/   OUString *pNames = aNames.getArray();
/*N*/   for( INT32 i = 0; *ppPropName;  ++i, ++ppPropName )
/*N*/   {
/*N*/       pNames[i] = A2OU( *ppPropName );
/*N*/   }
/*N*/   return aNames;
/*N*/ }

/////////////////////////////////////////////////////////////////

/*N*/ static const char * aMathPropNames[] =
/*N*/ {
/*N*/     "Print/Title",
/*N*/     "Print/FormulaText",
/*N*/     "Print/Frame",
/*N*/     "Print/Size",
/*N*/     "Print/ZoomFactor",
/*N*/     "Misc/IgnoreSpacesRight",
/*N*/     "View/ToolboxVisible",
/*N*/     "View/AutoRedraw",
/*N*/     "View/FormulaCursor"
/*N*/ };


//! Beware of order according to *_BEGIN *_END defines in format.hxx !
//! see respective load/save routines here
/*N*/ static const char * aFormatPropNames[] =
/*N*/ {
/*N*/     "StandardFormat/Textmode",
/*N*/     "StandardFormat/ScaleNormalBracket",
/*N*/     "StandardFormat/HorizontalAlignment",
/*N*/     "StandardFormat/BaseSize",
/*N*/     "StandardFormat/TextSize",
/*N*/     "StandardFormat/IndexSize",
/*N*/     "StandardFormat/FunctionSize",
/*N*/     "StandardFormat/OperatorSize",
/*N*/     "StandardFormat/LimitsSize",
/*N*/     "StandardFormat/Distance/Horizontal",
/*N*/     "StandardFormat/Distance/Vertical",
/*N*/     "StandardFormat/Distance/Root",
/*N*/     "StandardFormat/Distance/SuperScript",
/*N*/     "StandardFormat/Distance/SubScript",
/*N*/     "StandardFormat/Distance/Numerator",
/*N*/     "StandardFormat/Distance/Denominator",
/*N*/     "StandardFormat/Distance/Fraction",
/*N*/     "StandardFormat/Distance/StrokeWidth",
/*N*/     "StandardFormat/Distance/UpperLimit",
/*N*/     "StandardFormat/Distance/LowerLimit",
/*N*/     "StandardFormat/Distance/BracketSize",
/*N*/     "StandardFormat/Distance/BracketSpace",
/*N*/     "StandardFormat/Distance/MatrixRow",
/*N*/     "StandardFormat/Distance/MatrixColumn",
/*N*/     "StandardFormat/Distance/OrnamentSize",
/*N*/     "StandardFormat/Distance/OrnamentSpace",
/*N*/     "StandardFormat/Distance/OperatorSize",
/*N*/     "StandardFormat/Distance/OperatorSpace",
/*N*/     "StandardFormat/Distance/LeftSpace",
/*N*/     "StandardFormat/Distance/RightSpace",
/*N*/     "StandardFormat/Distance/TopSpace",
/*N*/     "StandardFormat/Distance/BottomSpace",
/*N*/     "StandardFormat/Distance/NormalBracketSize",
/*N*/     "StandardFormat/VariableFont",
/*N*/     "StandardFormat/FunctionFont",
/*N*/     "StandardFormat/NumberFont",
/*N*/     "StandardFormat/TextFont",
/*N*/     "StandardFormat/SerifFont",
/*N*/     "StandardFormat/SansFont",
/*N*/     "StandardFormat/FixedFont"
/*N*/ };


/*N*/ static Sequence< OUString > lcl_GetPropertyNames(
/*N*/         const char * aPropNames[], USHORT nCount )
/*N*/ {
/*N*/
/*N*/   const char** ppPropName = aPropNames;
/*N*/
/*N*/     Sequence< OUString > aNames( nCount );
/*N*/   OUString *pNames = aNames.getArray();
/*N*/     for (INT32 i = 0;  i < nCount;  ++i, ++ppPropName)
/*N*/   {
/*N*/       pNames[i] = A2OU( *ppPropName );
/*N*/   }
/*N*/   return aNames;
/*N*/ }

/////////////////////////////////////////////////////////////////

/*N*/ Sequence< OUString > SmMathConfigItem::GetFormatPropertyNames()
/*N*/ {
/*N*/     USHORT nCnt = SAL_N_ELEMENTS(aFormatPropNames);
/*N*/     return lcl_GetPropertyNames( aFormatPropNames, nCnt );
/*N*/ }


/*N*/ Sequence< OUString > SmMathConfigItem::GetOtherPropertyNames()
/*N*/ {
/*N*/     USHORT nCnt = SAL_N_ELEMENTS(aMathPropNames);
/*N*/     return lcl_GetPropertyNames( aMathPropNames, nCnt );
/*N*/ }

/////////////////////////////////////////////////////////////////

/*N*/ struct SmCfgOther
/*N*/ {
/*N*/     SmPrintSize     ePrintSize;
/*N*/     USHORT          nPrintZoomFactor;
/*N*/     BOOL            bPrintTitle;
/*N*/     BOOL            bPrintFormulaText;
/*N*/     BOOL            bPrintFrame;
/*N*/     BOOL            bIgnoreSpacesRight;
/*N*/     BOOL            bToolboxVisible;
/*N*/     BOOL            bAutoRedraw;
/*N*/     BOOL            bFormulaCursor;
/*N*/
/*N*/     SmCfgOther();
/*N*/ };


/*N*/ SmCfgOther::SmCfgOther()
/*N*/ {
/*N*/     ePrintSize          = PRINT_SIZE_NORMAL;
/*N*/     nPrintZoomFactor    = 100;
/*N*/     bPrintTitle         = bPrintFormulaText   =
/*N*/     bPrintFrame         = bIgnoreSpacesRight  =
/*N*/     bToolboxVisible     = bAutoRedraw         =
/*N*/     bFormulaCursor      = TRUE;
/*N*/ }

/////////////////////////////////////////////////////////////////


/*N*/ SmFontFormat::SmFontFormat()
/*N*/ {
/*N*/     aName.AssignAscii( FONTNAME_MATH );
/*N*/     nCharSet    = RTL_TEXTENCODING_UNICODE;
/*N*/     nFamily     = FAMILY_DONTKNOW;
/*N*/     nPitch      = PITCH_DONTKNOW;
/*N*/     nWeight     = WEIGHT_DONTKNOW;
/*N*/     nItalic     = ITALIC_NONE;
/*N*/ }


/*N*/ const Font SmFontFormat::GetFont() const
/*N*/ {
/*N*/     Font aRes;
/*N*/     aRes.SetName( aName );
/*N*/     aRes.SetCharSet( (rtl_TextEncoding) nCharSet );
/*N*/     aRes.SetFamily( (FontFamily) nFamily );
/*N*/     aRes.SetPitch( (FontPitch) nPitch );
/*N*/     aRes.SetWeight( (FontWeight) nWeight );
/*N*/     aRes.SetItalic( (FontItalic) nItalic );
/*N*/     return aRes;
/*N*/ }




/////////////////////////////////////////////////////////////////

/*N*/ SmFntFmtListEntry::SmFntFmtListEntry( const String &rId, const SmFontFormat &rFntFmt ) :
/*N*/     aId     (rId),
/*N*/     aFntFmt (rFntFmt)
/*N*/ {
/*N*/ }


/*N*/ SmFontFormatList::SmFontFormatList()
/*N*/ {
/*N*/     bModified = FALSE;
/*N*/ }


/*N*/ void SmFontFormatList::AddFontFormat( const String &rFntFmtId,
/*N*/         const SmFontFormat &rFntFmt )
/*N*/ {
/*N*/     const SmFontFormat *pFntFmt = GetFontFormat( rFntFmtId );
/*N*/     DBG_ASSERT( !pFntFmt, "FontFormatId already exists" );
/*N*/     if (!pFntFmt)
/*N*/     {
/*N*/         SmFntFmtListEntry aEntry( rFntFmtId, rFntFmt );
/*N*/         aEntries.Insert( aEntry, aEntries.Count() );
/*N*/         SetModified( TRUE );
/*N*/     }
/*N*/ }


/*N*/ const SmFontFormat * SmFontFormatList::GetFontFormat( const String &rFntFmtId ) const
/*N*/ {
/*N*/     SmFontFormat *pRes = 0;
/*N*/
/*N*/     USHORT nCnt = aEntries.Count();
/*N*/     USHORT i;
/*N*/     for (i = 0;  i < nCnt  &&  !pRes;  ++i)
/*N*/     {
/*N*/         if (aEntries[i].aId == rFntFmtId)
/*N*/             pRes = &aEntries[i].aFntFmt;
/*N*/     }
/*N*/
/*N*/     return pRes;
/*N*/ }


/////////////////////////////////////////////////////////////////

/*N*/ SmMathConfig::SmMathConfig()
/*N*/ {
/*N*/     pFormat         = 0;
/*N*/     pOther          = 0;
/*N*/     pFontFormatList = 0;
/*N*/   pSymbols        = 0;
/*N*/   nSymbolCount    = 0;
/*N*/
/*N*/     bIsOtherModified = bIsFormatModified = FALSE;
/*N*/
/*N*/     aSaveTimer.SetTimeout( 3000 );
/*N*/   aSaveTimer.SetTimeoutHdl( LINK( this, SmMathConfig, TimeOut ) );
/*N*/ }


/*N*/ SmMathConfig::~SmMathConfig()
/*N*/ {
/*N*/     Save();
/*N*/     delete pFormat;
/*N*/     delete pOther;
/*N*/     delete pFontFormatList;
/*N*/     delete [] pSymbols;
/*N*/ }


/*N*/ void SmMathConfig::SetOtherModified( BOOL bVal )
/*N*/ {
/*N*/     if ((bIsOtherModified = bVal))
/*?*/       aSaveTimer.Start();
/*N*/ }


/*N*/ void SmMathConfig::SetFormatModified( BOOL bVal )
/*N*/ {
/*N*/     if ((bIsFormatModified = bVal))
/*?*/       aSaveTimer.Start();
/*N*/ }


/*N*/ SmSym SmMathConfig::ReadSymbol( SmMathConfigItem &rCfg,
/*N*/                       const ::rtl::OUString &rSymbolName,
/*N*/                       const ::rtl::OUString &rBaseNode ) const
/*N*/ {
/*N*/   SmSym aRes;
/*N*/
/*N*/   Sequence< OUString > aNames = lcl_GetSymbolPropertyNames();
/*N*/   INT32 nProps = aNames.getLength();
/*N*/
/*N*/   OUString aDelim( OUString::valueOf( (sal_Unicode) '/' ) );
/*N*/   OUString *pName = aNames.getArray();
/*N*/   for (INT32 i = 0;  i < nProps;  ++i)
/*N*/   {
/*N*/       OUString &rName = pName[i];
/*N*/       OUString aTmp( rName );
/*N*/       rName = rBaseNode;
/*N*/       rName += aDelim;
/*N*/       rName += rSymbolName;
/*N*/       rName += aDelim;
/*N*/       rName += aTmp;
/*N*/   }
/*N*/
/*N*/   const Sequence< Any > aValues = rCfg.GetProperties( aNames );
/*N*/
/*N*/   if (nProps  &&  aValues.getLength() == nProps)
/*N*/   {
/*N*/       const Any * pValue = aValues.getConstArray();
/*N*/         Font        aFont;
/*N*/         sal_Unicode cChar(0);
/*N*/         String      aSet;
/*N*/         BOOL        bPredefined(FALSE);
/*N*/
/*N*/         OUString    aTmpStr;
/*N*/         INT32       nTmp32 = 0;
/*N*/         BOOL        bTmp = FALSE;
/*N*/
/*N*/         BOOL bOK = TRUE;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= nTmp32))
/*N*/             cChar = (sal_Unicode) nTmp32;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/         ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= aTmpStr))
/*N*/             aSet = aTmpStr;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/       ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= bTmp))
/*N*/             bPredefined = bTmp;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/       ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= aTmpStr))
/*N*/         {
/*N*/             const SmFontFormat *pFntFmt = GetFontFormatList().GetFontFormat( aTmpStr );
/*N*/             DBG_ASSERT( pFntFmt, "unknown FontFormat" );
/*N*/             if (pFntFmt)
/*N*/                 aFont = pFntFmt->GetFont();
/*N*/         }
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/         ++pValue;
/*N*/
/*N*/         if (bOK)
/*N*/         {
/*N*/             String aUiName( rSymbolName );
/*N*/             String aUiSetName( aSet );
/*N*/             if (bPredefined)
/*N*/             {
/*N*/                 String aTmp;
/*N*/                 aTmp = GetUiSymbolName( rSymbolName );
/*N*/                 DBG_ASSERT( aTmp.Len(), "localized symbol-name not found" );
/*N*/                 if (aTmp.Len())
/*N*/                     aUiName = aTmp;
/*N*/                 aTmp = GetUiSymbolSetName( aSet );
/*N*/                 DBG_ASSERT( aTmp.Len(), "localized symbolset-name not found" );
/*N*/                 if (aTmp.Len())
/*N*/                     aUiSetName = aTmp;
/*N*/             }
/*N*/
/*N*/             aRes = SmSym( aUiName, aFont, cChar, aUiSetName, bPredefined );
/*N*/             if (aUiName != String(rSymbolName))
/*?*/                 aRes.SetExportName( rSymbolName );
/*N*/         }
/*N*/         else
/*N*/         {
/*N*/             OSL_FAIL( "symbol read error" );
/*N*/         }
/*N*/   }
/*N*/
/*N*/   return aRes;
/*N*/ }


/*N*/ void SmMathConfig::LoadSymbols()
/*N*/ {
/*N*/   SmMathConfigItem aCfg( String::CreateFromAscii( aRootName ));
/*N*/
/*N*/   Sequence< OUString > aNodes( aCfg.GetNodeNames( A2OU( SYMBOL_LIST ) ) );
/*N*/   const OUString *pNode = aNodes.getConstArray();
/*N*/   INT32 nNodes = aNodes.getLength();
/*N*/
/*N*/   if (pSymbols)
/*?*/         delete [] pSymbols;
/*N*/   pSymbols = nNodes ? new SmSym[ nNodes ] : 0;
/*N*/     nSymbolCount = (USHORT) nNodes;
/*N*/
/*N*/   for (INT32 i = 0;  i < nNodes;  ++i)
/*N*/   {
/*N*/       pSymbols[i] = ReadSymbol( aCfg, pNode[i], A2OU( SYMBOL_LIST ) );
/*N*/   }
/*N*/ }

/*N*/ USHORT SmMathConfig::GetSymbolCount() const
/*N*/ {
/*N*/   if (!pSymbols)
/*N*/       ((SmMathConfig *) this)->LoadSymbols();
/*N*/   return nSymbolCount;
/*N*/ }


/*N*/ const SmSym * SmMathConfig::GetSymbol( USHORT nIndex ) const
/*N*/ {
/*N*/   SmSym *pRes = 0;
/*N*/   if (!pSymbols)
/*?*/       ((SmMathConfig *) this)->LoadSymbols();
/*N*/   if (nIndex < nSymbolCount)
/*N*/       pRes = &pSymbols[ nIndex ];
/*N*/   return pRes;
/*N*/ }




/*N*/ SmFontFormatList & SmMathConfig::GetFontFormatList()
/*N*/ {
/*N*/     if (!pFontFormatList)
/*N*/     {
/*N*/         LoadFontFormatList();
/*N*/     }
/*N*/     return *pFontFormatList;
/*N*/ }


/*N*/ void SmMathConfig::LoadFontFormatList()
/*N*/ {
/*N*/   if (!pFontFormatList)
/*N*/       pFontFormatList = new SmFontFormatList;
/*N*/
/*N*/   SmMathConfigItem aCfg( String::CreateFromAscii( aRootName ) );
/*N*/
/*N*/   Sequence< OUString > aNodes( aCfg.GetNodeNames( A2OU( FONT_FORMAT_LIST ) ) );
/*N*/   const OUString *pNode = aNodes.getConstArray();
/*N*/   INT32 nNodes = aNodes.getLength();
/*N*/
/*N*/   for (INT32 i = 0;  i < nNodes;  ++i)
/*N*/   {
/*N*/       SmFontFormat aFntFmt( ReadFontFormat( aCfg, pNode[i], A2OU( FONT_FORMAT_LIST ) ) );
/*N*/       if (!pFontFormatList->GetFontFormat( pNode[i] ))
/*N*/       {
/*N*/           DBG_ASSERT( 0 == pFontFormatList->GetFontFormat( pNode[i] ),
/*N*/                     "FontFormat ID already exists" );
/*N*/           pFontFormatList->AddFontFormat( pNode[i], aFntFmt );
/*N*/       }
/*N*/   }
/*N*/   pFontFormatList->SetModified( FALSE );
/*N*/ }


/*N*/ SmFontFormat SmMathConfig::ReadFontFormat( SmMathConfigItem &rCfg,
/*N*/         const OUString &rSymbolName, const OUString &rBaseNode ) const
/*N*/ {
/*N*/     SmFontFormat aRes;
/*N*/
/*N*/     Sequence< OUString > aNames = lcl_GetFontPropertyNames();
/*N*/   INT32 nProps = aNames.getLength();
/*N*/
/*N*/   OUString aDelim( OUString::valueOf( (sal_Unicode) '/' ) );
/*N*/   OUString *pName = aNames.getArray();
/*N*/   for (INT32 i = 0;  i < nProps;  ++i)
/*N*/   {
/*N*/       OUString &rName = pName[i];
/*N*/       OUString aTmp( rName );
/*N*/       rName = rBaseNode;
/*N*/       rName += aDelim;
/*N*/       rName += rSymbolName;
/*N*/       rName += aDelim;
/*N*/       rName += aTmp;
/*N*/   }
/*N*/
/*N*/   const Sequence< Any > aValues = rCfg.GetProperties( aNames );
/*N*/
/*N*/   if (nProps  &&  aValues.getLength() == nProps)
/*N*/   {
/*N*/       const Any * pValue = aValues.getConstArray();
/*N*/
/*N*/         OUString    aTmpStr;
/*N*/         INT16       nTmp16 = 0;
/*N*/
/*N*/         BOOL bOK = TRUE;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= aTmpStr))
/*N*/             aRes.aName = aTmpStr;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/         ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= nTmp16))
/*N*/             aRes.nCharSet = nTmp16; // 6.0 file-format GetSOLoadTextEncoding not needed
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/       ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= nTmp16))
/*N*/             aRes.nFamily = nTmp16;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/       ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= nTmp16))
/*N*/             aRes.nPitch = nTmp16;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/       ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= nTmp16))
/*N*/             aRes.nWeight = nTmp16;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/       ++pValue;
/*N*/         if (pValue->hasValue()  &&  (*pValue >>= nTmp16))
/*N*/             aRes.nItalic = nTmp16;
/*N*/         else
/*N*/             bOK = FALSE;
/*N*/       ++pValue;
/*N*/
/*N*/         DBG_ASSERT( bOK, "read FontFormat failed" );
/*N*/         (void)bOK;
/*N*/   }
/*N*/
/*N*/     return aRes;
/*N*/ }

/*N*/ void SmMathConfig::LoadOther()
/*N*/ {
/*N*/     if (!pOther)
/*N*/         pOther = new SmCfgOther;
/*N*/
/*N*/   SmMathConfigItem aCfg( String::CreateFromAscii( aRootName ));
/*N*/
/*N*/     Sequence< OUString > aNames( aCfg.GetOtherPropertyNames() );
/*N*/     INT32 nProps = aNames.getLength();
/*N*/
/*N*/     Sequence< Any > aValues( aCfg.GetProperties( aNames ) );
/*N*/     if (nProps  &&  aValues.getLength() == nProps)
/*N*/     {
/*N*/         const Any *pValues = aValues.getConstArray();
/*N*/         const Any *pVal = pValues;
/*N*/
/*N*/         INT16   nTmp16( 0 );
/*N*/         BOOL    bTmp( 0 );
/*N*/
/*N*/         // Print/Title
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pOther->bPrintTitle = bTmp;
/*N*/         ++pVal;
/*N*/         // Print/FormulaText
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pOther->bPrintFormulaText = bTmp;
/*N*/         ++pVal;
/*N*/         // Print/Frame
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pOther->bPrintFrame = bTmp;
/*N*/         ++pVal;
/*N*/         // Print/Size
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= nTmp16))
/*N*/             pOther->ePrintSize = (SmPrintSize) nTmp16;
/*N*/         ++pVal;
/*N*/         // Print/ZoomFactor
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= nTmp16))
/*N*/             pOther->nPrintZoomFactor = nTmp16;
/*N*/         ++pVal;
/*N*/         // Misc/IgnoreSpacesRight
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pOther->bIgnoreSpacesRight = bTmp;
/*N*/         ++pVal;
/*N*/         // View/ToolboxVisible
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pOther->bToolboxVisible = bTmp;
/*N*/         ++pVal;
/*N*/         // View/AutoRedraw
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pOther->bAutoRedraw = bTmp;
/*N*/         ++pVal;
/*N*/         // View/FormulaCursor
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pOther->bFormulaCursor = bTmp;
/*N*/         ++pVal;
/*N*/
/*N*/         DBG_ASSERT( pVal - pValues == nProps, "property mismatch" );
/*N*/         SetOtherModified( FALSE );
/*N*/     }
/*N*/ }

/*N*/ void SmMathConfig::LoadFormat()
/*N*/ {
/*N*/     if (!pFormat)
/*N*/         pFormat = new SmFormat;
/*N*/
/*N*/   SmMathConfigItem aCfg( String::CreateFromAscii( aRootName ));
/*N*/
/*N*/     Sequence< OUString > aNames( aCfg.GetFormatPropertyNames() );
/*N*/     INT32 nProps = aNames.getLength();
/*N*/
/*N*/     Sequence< Any > aValues( aCfg.GetProperties( aNames ) );
/*N*/     if (nProps  &&  aValues.getLength() == nProps)
/*N*/     {
/*N*/         const Any *pValues = aValues.getConstArray();
/*N*/         const Any *pVal = pValues;
/*N*/
/*N*/         OUString    aTmpStr;
/*N*/         INT16       nTmp16( 0 );
/*N*/         BOOL        bTmp( 0 );
/*N*/
/*N*/         // StandardFormat/Textmode
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pFormat->SetTextmode( bTmp );
/*N*/         ++pVal;
/*N*/         // StandardFormat/ScaleNormalBracket
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= bTmp))
/*N*/             pFormat->SetScaleNormalBrackets( bTmp );
/*N*/         ++pVal;
/*N*/         // StandardFormat/HorizontalAlignment
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= nTmp16))
/*N*/             pFormat->SetHorAlign( (SmHorAlign) nTmp16 );
/*N*/         ++pVal;
/*N*/         // StandardFormat/BaseSize
/*N*/         if (pVal->hasValue()  &&  (*pVal >>= nTmp16))
/*N*/             pFormat->SetBaseSize( Size(0, SmPtsTo100th_mm( nTmp16 )) );
/*N*/         ++pVal;
/*N*/
/*N*/         USHORT i;
/*N*/         for (i = SIZ_BEGIN;  i <= SIZ_END;  ++i)
/*N*/         {
/*N*/             if (pVal->hasValue()  &&  (*pVal >>= nTmp16))
/*N*/                 pFormat->SetRelSize( i, nTmp16 );
/*N*/             ++pVal;
/*N*/         }
/*N*/
/*N*/         for (i = DIS_BEGIN;  i <= DIS_END;  ++i)
/*N*/         {
/*N*/             if (pVal->hasValue()  &&  (*pVal >>= nTmp16))
/*N*/                 pFormat->SetDistance( i, nTmp16 );
/*N*/             ++pVal;
/*N*/         }
/*N*/
/*N*/         LanguageType nLang = Application::GetSettings().GetUILanguage();
/*N*/         for (i = FNT_BEGIN;  i < FNT_END;  ++i)
/*N*/         {
/*N*/             Font aFnt;
/*N*/             BOOL bUseDefaultFont = TRUE;
/*N*/             if (pVal->hasValue()  &&  (*pVal >>= aTmpStr))
/*N*/             {
/*N*/                 bUseDefaultFont = 0 == aTmpStr.getLength();
/*N*/                 if (bUseDefaultFont)
/*N*/                 {
/*N*/                     aFnt = pFormat->GetFont( i );
/*N*/                     aFnt.SetName( GetDefaultFontName( nLang, i ) );
/*N*/                 }
/*N*/                 else
/*N*/                 {
/*?*/                     const SmFontFormat *pFntFmt = GetFontFormatList().GetFontFormat( aTmpStr );
/*?*/                     DBG_ASSERT( pFntFmt, "unknown FontFormat" );
/*?*/                     if (pFntFmt)
/*?*/                         aFnt = pFntFmt->GetFont();
/*N*/                 }
/*N*/             }
/*N*/             ++pVal;
/*N*/
/*N*/             aFnt.SetSize( pFormat->GetBaseSize() );
/*N*/             pFormat->SetFont( i, aFnt, bUseDefaultFont );
/*N*/         }
/*N*/
/*N*/         DBG_ASSERT( pVal - pValues == nProps, "property mismatch" );
/*N*/         SetFormatModified( FALSE );
/*N*/     }
/*N*/ }

/*N*/ const SmFormat & SmMathConfig::GetStandardFormat() const
/*N*/ {
/*N*/     if (!pFormat)
/*N*/         ((SmMathConfig *) this)->LoadFormat();
/*N*/     return *pFormat;
/*N*/ }




/*N*/ SmPrintSize SmMathConfig::GetPrintSize() const
/*N*/ {
/*N*/     if (!pOther)
/*N*/         ((SmMathConfig *) this)->LoadOther();
/*N*/     return pOther->ePrintSize;
/*N*/ }




/*N*/ USHORT SmMathConfig::GetPrintZoomFactor() const
/*N*/ {
/*N*/     if (!pOther)
/*?*/         ((SmMathConfig *) this)->LoadOther();
/*N*/     return pOther->nPrintZoomFactor;
/*N*/ }






/*N*/ BOOL SmMathConfig::IsPrintTitle() const
/*N*/ {
/*N*/     if (!pOther)
/*?*/         ((SmMathConfig *) this)->LoadOther();
/*N*/     return pOther->bPrintTitle;
/*N*/ }




/*N*/ BOOL SmMathConfig::IsPrintFormulaText() const
/*N*/ {
/*N*/     if (!pOther)
/*?*/         ((SmMathConfig *) this)->LoadOther();
/*N*/     return pOther->bPrintFormulaText;
/*N*/ }




/*N*/ BOOL SmMathConfig::IsPrintFrame() const
/*N*/ {
/*N*/     if (!pOther)
/*?*/         ((SmMathConfig *) this)->LoadOther();
/*N*/     return pOther->bPrintFrame;
/*N*/ }




/*N*/ BOOL SmMathConfig::IsIgnoreSpacesRight() const
/*N*/ {
/*N*/     if (!pOther)
/*?*/         ((SmMathConfig *) this)->LoadOther();
/*N*/     return pOther->bIgnoreSpacesRight;
/*N*/ }


    void SmMathConfigItem::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& /*rPropertyNames*/ ) {}
    void SmMathConfigItem::Commit() {}






/*N*/ BOOL SmMathConfig::IsAutoRedraw() const
/*N*/ {
/*N*/     if (!pOther)
/*?*/         ((SmMathConfig *) this)->LoadOther();
/*N*/     return pOther->bAutoRedraw;
/*N*/ }

IMPL_LINK( SmMathConfig, TimeOut, Timer *, EMPTYARG )
{
    DBG_BF_ASSERT(0, "STRIP");  // IMPL_LINK
    return 0;
}

/////////////////////////////////////////////////////////////////

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
