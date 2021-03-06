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

//------------------------------------------------------------------------

#include <tools/debug.hxx>
#include <string.h>
#include <unotools/collatorwrapper.hxx>
#include <unotools/transliterationwrapper.hxx>

#include "rangenam.hxx"
#include "compiler.hxx"
#include "rechead.hxx"
#include "document.hxx"
namespace binfilter {


//========================================================================
// ScRangeData
//========================================================================

// Interner ctor fuer das Suchen nach einem Index

/*N*/ ScRangeData::ScRangeData( USHORT n )
/*N*/          : pCode( NULL ), nIndex( n ), bModified( FALSE )
/*N*/ {}

/*N*/ ScRangeData::ScRangeData( ScDocument* pDok,
/*N*/                         const String& rName,
/*N*/                         const String& rSymbol,
/*N*/                           const ScAddress& rAddress,
/*N*/                         RangeType nType,
/*N*/                         BOOL bEnglish ) :
/*N*/               aName       ( rName ),
/*N*/               pCode       ( NULL ),
/*N*/               aPos        ( rAddress ),
/*N*/               eType       ( nType ),
/*N*/               pDoc        ( pDok ),
/*N*/               nIndex      ( 0 ),
/*N*/               nExportIndex( 0 ),
/*N*/               bModified   ( FALSE )
/*N*/ {
/*N*/   if (rSymbol.Len() > 0)
/*N*/   {
/*N*/       ScCompiler aComp( pDoc, aPos );
/*N*/       aComp.SetCompileEnglish(bEnglish);
/*N*/       pCode = aComp.CompileString( rSymbol );
/*N*/       if( !pCode->GetError() )
/*N*/       {
/*N*/           pCode->Reset();
/*N*/           ScToken* p = pCode->GetNextReference();
/*N*/           if( p )// genau eine Referenz als erstes
/*N*/           {
/*N*/               if( p->GetType() == svSingleRef )
/*N*/                   eType = eType | RT_ABSPOS;
/*N*/               else
/*N*/                   eType = eType | RT_ABSAREA;
/*N*/           }
/*N*/           // ggf. den Fehlercode wg. unvollstaendiger Formel setzen!
/*N*/           // Dies ist fuer die manuelle Eingabe
/*N*/           aComp.CompileTokenArray();
/*N*/           pCode->DelRPN();
/*N*/       }
/*N*/   }
/*N*/ }

/*N*/ ScRangeData::ScRangeData(const ScRangeData& rScRangeData) : DataObject(rScRangeData),
/*N*/   aName   (rScRangeData.aName),
/*N*/   pCode       (rScRangeData.pCode ? rScRangeData.pCode->Clone() : new ScTokenArray),      // echte Kopie erzeugen (nicht copy-ctor)
/*N*/   aPos        (rScRangeData.aPos),
/*N*/   eType       (rScRangeData.eType),
/*N*/   pDoc        (rScRangeData.pDoc),
/*N*/   nIndex      (rScRangeData.nIndex),
/*N*/   bModified   (rScRangeData.bModified)
/*N*/ {}

/*N*/ ScRangeData::~ScRangeData()
/*N*/ {
/*N*/   delete pCode;
/*N*/ }

/*N*/ DataObject* ScRangeData::Clone() const
/*N*/ {
/*N*/   return new ScRangeData(*this);
/*N*/ }

/*N*/ ScRangeData::ScRangeData
/*N*/   ( SvStream& rStream, ScMultipleReadHeader& rHdr, USHORT nVer )
/*N*/          : pCode      ( new ScTokenArray ),
/*N*/            bModified  (FALSE)
/*N*/
/*N*/ {
/*N*/   rHdr.StartEntry();
/*N*/
/*N*/   if( nVer >= SC_NEW_TOKEN_ARRAYS )
/*N*/   {
/*N*/       UINT32 nPos;
/*N*/       BYTE nData;
/*N*/       aName = rStream.ReadUniOrByteString( rStream.GetStreamCharSet() );
/*N*/       rStream >> nPos >> eType >> nIndex >> nData;
/*N*/       if( nData & 0x0F )
/*?*/           rStream.SeekRel( nData & 0x0F );
/*N*/       aPos = ScAddress( nPos );
/*N*/       pCode->Load( rStream, nVer, aPos );
/*N*/   }
/*N*/   else
/*N*/   {
/*N*/       UINT16 nTokLen, r, c, t;
/*N*/       aName = rStream.ReadUniOrByteString( rStream.GetStreamCharSet() );
/*N*/       rStream >> c >> r >> t >> eType >> nIndex >> nTokLen;
/*N*/       aPos.Set( c, r, t );
/*N*/       if( nTokLen )
/*N*/           pCode->Load30( rStream, aPos );
/*N*/   }
/*N*/
/*N*/   rHdr.EndEntry();
/*N*/ }

/*N*/ void ScRangeData::GetSymbol   (String& rSymbol) const
/*N*/ {
/*N*/   ScCompiler aScComp(pDoc, aPos, *pCode);
/*N*/   aScComp.CreateStringFromTokenArray( rSymbol );
/*N*/ }

/*N*/ void ScRangeData::GetEnglishSymbol    (String& rSymbol, BOOL bCompileXML) const
/*N*/ {
/*N*/   ScCompiler aScComp(pDoc, aPos, *pCode);
/*N*/   aScComp.SetCompileEnglish( TRUE );
/*N*/   aScComp.SetCompileXML( bCompileXML );
/*N*/   aScComp.CreateStringFromTokenArray( rSymbol );
/*N*/ }

/*N*/ void ScRangeData::UpdateSymbol(   String& rSymbol, const ScAddress& rPos,
/*N*/                               BOOL bEnglish, BOOL bCompileXML )
/*N*/ {
/*N*/   ScTokenArray* pTemp = pCode->Clone();
/*N*/   ScCompiler aComp( pDoc, rPos, *pTemp );
/*N*/   aComp.SetCompileEnglish( bEnglish );
/*N*/   aComp.SetCompileXML( bCompileXML );
/*N*/   aComp.MoveRelWrap();
/*N*/   aComp.CreateStringFromTokenArray( rSymbol );
/*N*/   delete pTemp;
/*N*/ }

/*N*/ void ScRangeData::UpdateSymbol(   ::rtl::OUStringBuffer& rBuffer, const ScAddress& rPos,
/*N*/                               BOOL bEnglish, BOOL bCompileXML )
/*N*/ {
/*N*/   ScTokenArray* pTemp = pCode->Clone();
/*N*/   ScCompiler aComp( pDoc, rPos, *pTemp );
/*N*/   aComp.SetCompileEnglish( bEnglish );
/*N*/   aComp.SetCompileXML( bCompileXML );
/*N*/   aComp.MoveRelWrap();
/*N*/   aComp.CreateStringFromTokenArray( rBuffer );
/*N*/   delete pTemp;
/*N*/ }

/*N*/ void ScRangeData::UpdateReference(    UpdateRefMode eUpdateRefMode,
/*N*/                                   const ScRange& r,
/*N*/                                   short nDx, short nDy, short nDz )
/*N*/ {
/*N*/   BOOL bChanged = FALSE;
/*N*/
/*N*/   pCode->Reset();
/*N*/   if( pCode->GetNextReference() )
/*N*/   {
/*N*/       ScCompiler aComp( pDoc, aPos, *pCode );
/*N*/       BOOL bRelRef = aComp.UpdateNameReference( eUpdateRefMode, r,
/*N*/                                                   nDx, nDy, nDz,
/*N*/                                                   bChanged);
/*N*/       if (eType&RT_SHARED)
/*N*/       {
/*N*/           if (bRelRef)
/*N*/               eType = eType | RT_SHAREDMOD;
/*N*/           else
/*N*/               eType = eType & ~RT_SHAREDMOD;
/*N*/       }
/*N*/   }
/*N*/
/*N*/   bModified = bChanged;
/*N*/ }







/*N*/ BOOL ScRangeData::IsReference( ScRange& rRange ) const
/*N*/ {
/*N*/   BOOL bIs = FALSE;
/*N*/   if ( eType & ( RT_ABSAREA | RT_REFAREA | RT_ABSPOS ) )
/*N*/       if ( pCode )
/*N*/           return pCode->IsReference( rRange );
/*N*/
/*N*/   return bIs;
/*N*/ }


//  wie beim Uebernehmen von Namen in Excel

/*N*/ void ScRangeData::MakeValidName( String& rName )      // static
/*N*/ {
/*N*/   if (!ScCompiler::HasCharTable())
/*N*/       ScCompiler::Init();
/*N*/
/*N*/   //  ungueltige Zeichen vorne weglassen
/*N*/   xub_StrLen nPos = 0;
/*N*/   xub_StrLen nLen = rName.Len();
/*N*/   while ( nPos < nLen && !ScCompiler::IsWordChar( rName.GetChar(nPos) ) )
/*N*/       ++nPos;
/*N*/   if ( nPos>0 )
/*N*/       rName.Erase(0,nPos);
/*N*/
/*N*/   //  wenn vorne ein ungueltiges Anfangszeichen steht, '_' davor
/*N*/   if ( rName.Len() && !ScCompiler::IsCharWordChar( rName.GetChar(0) ) )
/*N*/       rName.Insert('_',0);
/*N*/
/*N*/   //  ungueltige durch '_' ersetzen
/*N*/   nLen = rName.Len();
/*N*/   for (nPos=0; nPos<nLen; nPos++)
/*N*/   {
/*N*/       if ( !ScCompiler::IsWordChar( rName.GetChar(nPos) ) )
/*N*/           rName.SetChar( nPos, '_' );
/*N*/   }
/*N*/
/*N*/   // Name darf keine Referenz beinhalten, wie in IsNameValid
/*N*/   BOOL bOk;
/*N*/   do
/*N*/   {
/*N*/       bOk = TRUE;
/*N*/       ScRange aRange;
/*N*/       if( aRange.Parse( rName, NULL ) )
/*N*/           bOk = FALSE;
/*N*/       else
/*N*/       {
/*N*/           ScAddress aAddr;
/*N*/           if ( aAddr.Parse( rName, NULL ) )
/*N*/               bOk = FALSE;
/*N*/       }
/*N*/       if ( !bOk )
/*N*/       {   //! Range Parse auch bei Bereich mit ungueltigem Tabellennamen gueltig
/*N*/           //! Address Parse dito, Name erzeugt deswegen bei Compile ein #REF!
/*N*/           if ( rName.SearchAndReplace( ':', '_' ) == STRING_NOTFOUND
/*N*/             && rName.SearchAndReplace( '.', '_' ) == STRING_NOTFOUND )
/*N*/               rName.Insert('_',0);
/*N*/       }
/*N*/   } while ( !bOk );
/*N*/ }



/*N*/ USHORT ScRangeData::GetErrCode()
/*N*/ {
/*N*/   return pCode ? pCode->GetError() : 0;
/*N*/ }

/*N*/ BOOL ScRangeData::HasReferences() const
/*N*/ {
/*N*/   pCode->Reset();
/*N*/   return BOOL( pCode->GetNextReference() != NULL );
/*N*/ }



/*N*/ void ScRangeData::ValidateTabRefs()
/*N*/ {
/*N*/   //  try to make sure all relative references and the reference position
/*N*/   //  are within existing tables, so they can be represented as text
/*N*/   //  (if the range of used tables is more than the existing tables,
/*N*/   //  the result may still contain invalid tables, because the relative
/*N*/   //  references aren't changed so formulas stay the same)
/*N*/
/*N*/   //  find range of used tables
/*N*/
/*N*/   USHORT nMinTab = aPos.Tab();
/*N*/   USHORT nMaxTab = nMinTab;
/*N*/   ScToken* t;
/*N*/   pCode->Reset();
/*N*/   while (( t = pCode->GetNextReference() ))
/*N*/   {
/*N*/       SingleRefData& rRef1 = t->GetSingleRef();
/*N*/       if ( rRef1.IsTabRel() && !rRef1.IsTabDeleted() )
/*N*/       {
/*N*/           if ( rRef1.nTab < nMinTab )
/*N*/               nMinTab = rRef1.nTab;
/*N*/           if ( rRef1.nTab > nMaxTab )
/*N*/               nMaxTab = rRef1.nTab;
/*N*/       }
/*N*/       if ( t->GetType() == svDoubleRef )
/*N*/       {
/*N*/           SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
/*N*/           if ( rRef2.IsTabRel() && !rRef2.IsTabDeleted() )
/*N*/           {
/*?*/               if ( rRef2.nTab < nMinTab )
/*?*/                   nMinTab = rRef2.nTab;
/*?*/               if ( rRef2.nTab > nMaxTab )
/*?*/                   nMaxTab = rRef2.nTab;
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/
/*N*/   USHORT nTabCount = pDoc->GetTableCount();
/*N*/   if ( nMaxTab >= nTabCount && nMinTab > 0 )
/*N*/   {
/*?*/       //  move position and relative tab refs
/*?*/       //  The formulas that use the name are not changed by this
/*?*/
/*?*/       USHORT nMove = nMinTab;
/*?*/       aPos.SetTab( aPos.Tab() - nMove );
/*?*/
/*?*/       pCode->Reset();
/*?*/       while (( t = pCode->GetNextReference() ))
/*?*/       {
/*?*/           SingleRefData& rRef1 = t->GetSingleRef();
/*?*/           if ( rRef1.IsTabRel() && !rRef1.IsTabDeleted() )
/*?*/               rRef1.nTab -= nMove;
/*?*/           if ( t->GetType() == svDoubleRef )
/*?*/           {
/*?*/               SingleRefData& rRef2 = t->GetDoubleRef().Ref2;
/*?*/               if ( rRef2.IsTabRel() && !rRef2.IsTabDeleted() )
/*?*/                   rRef2.nTab -= nMove;
/*?*/           }
/*?*/       }
/*N*/   }
/*N*/ }


/*N*/ int
/*N*/ #ifdef WNT
/*N*/ __cdecl
/*N*/ #endif
/*N*/ ScRangeData::QsortNameCompare( const void* p1, const void* p2 )
/*N*/ {
/*N*/   return (int) ScGlobal::pCollator->compareString(
/*N*/           (*(const ScRangeData**)p1)->aName,
/*N*/           (*(const ScRangeData**)p2)->aName );
/*N*/ }


//========================================================================
// ScRangeName
//========================================================================

/*N*/ ScRangeName::ScRangeName(const ScRangeName& rScRangeName, ScDocument* pDocument) :
/*N*/               SortedCollection ( rScRangeName ),
/*N*/               pDoc ( pDocument ),
/*N*/               nSharedMaxIndex (rScRangeName.nSharedMaxIndex)
/*N*/ {
/*N*/   for (USHORT i = 0; i < nCount; i++)
/*N*/   {
/*N*/       ((ScRangeData*)At(i))->SetDocument(pDocument);
/*N*/       ((ScRangeData*)At(i))->SetIndex(((ScRangeData*)rScRangeName.At(i))->GetIndex());
/*N*/   }
/*N*/ }

/*N*/ short ScRangeName::Compare(DataObject* pKey1, DataObject* pKey2) const
/*N*/ {
/*N*/   USHORT i1 = ((ScRangeData*)pKey1)->GetIndex();
/*N*/   USHORT i2 = ((ScRangeData*)pKey2)->GetIndex();
/*N*/   return (short) i1 - (short) i2;
/*N*/ }

/*N*/ BOOL ScRangeName::SearchName( const String& rName, USHORT& rIndex ) const
/*N*/ {
/*N*/   USHORT i = 0;
/*N*/   while (i < nCount)
/*N*/   {
/*N*/       String aName;
/*N*/       ((*this)[i])->GetName( aName );
/*N*/         if ( ScGlobal::pTransliteration->isEqual( aName, rName ) )
/*N*/       {
/*N*/           rIndex = i;
/*N*/           return TRUE;
/*N*/       }
/*N*/       i++;
/*N*/   }
/*N*/   return FALSE;
/*N*/ }

/*N*/ BOOL ScRangeName::Load( SvStream& rStream, USHORT nVer )
/*N*/ {
/*N*/   BOOL bSuccess = TRUE;
/*N*/   USHORT nNewCount;
/*N*/
/*N*/   while( nCount > 0 )
/*?*/       AtFree(0);                  // alles loeschen
/*N*/
/*N*/   ScMultipleReadHeader aHdr( rStream );
/*N*/
/*N*/   USHORT nDummy;
/*N*/   if( nVer >= SC_NEW_TOKEN_ARRAYS )
/*N*/       rStream >> nSharedMaxIndex >> nNewCount;
/*N*/   else
/*N*/       rStream >> nSharedMaxIndex >> nDummy >> nNewCount;
/*N*/   for (USHORT i=0; i<nNewCount && bSuccess; i++)
/*N*/   {
/*N*/       ScRangeData* pData = new ScRangeData( rStream, aHdr, nVer );
/*N*/       pData->SetDocument(pDoc);
/*N*/       Insert( pData );
/*N*/       if( rStream.GetError() != SVSTREAM_OK )
/*N*/           bSuccess = FALSE;
/*N*/   }
/*N*/   return bSuccess;
/*N*/ }

/*N*/ void ScRangeName::UpdateReference(    UpdateRefMode eUpdateRefMode,
/*N*/                                   const ScRange& rRange,
/*N*/                                   short nDx, short nDy, short nDz )
/*N*/ {
/*N*/   for (USHORT i=0; i<nCount; i++)
/*N*/       ((ScRangeData*)pItems[i])->UpdateReference(eUpdateRefMode, rRange,
/*N*/                                                  nDx, nDy, nDz);
/*N*/ }




/*N*/ BOOL ScRangeName::Insert(DataObject* pDataObject)
/*N*/ {
/*N*/   if (!((ScRangeData*)pDataObject)->GetIndex())       // schon gesetzt?
/*N*/   {
/*N*/       ((ScRangeData*)pDataObject)->SetIndex( GetEntryIndex() );
/*N*/   }
/*N*/
/*N*/   return SortedCollection::Insert(pDataObject);
/*N*/ }

// Suche nach einem freien Index

/*N*/ USHORT ScRangeName::GetEntryIndex()
/*N*/ {
/*N*/   USHORT nLast = 0;
/*N*/   for ( USHORT i = 0; i < nCount; i++ )
/*N*/   {
/*N*/       USHORT nIdx = ((ScRangeData*)pItems[i])->GetIndex();
/*N*/       if( nIdx > nLast )
/*N*/       {
/*N*/           nLast = nIdx;
/*N*/       }
/*N*/   }
/*N*/   return nLast + 1;
/*N*/ }

/*N*/ ScRangeData* ScRangeName::FindIndex( USHORT nIndex )
/*N*/ {
/*N*/   ScRangeData aDataObj( nIndex );
/*N*/   USHORT n;
/*N*/   if( Search( &aDataObj, n ) )
/*N*/       return (*this)[ n ];
/*N*/   else
/*N*/       return NULL;
/*N*/ }



}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
