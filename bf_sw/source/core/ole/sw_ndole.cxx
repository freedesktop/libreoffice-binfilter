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


#include <tools/urlobj.hxx>
#include <bf_sfx2/docfile.hxx>
#include <unotools/configitem.hxx>
#include <vcl/outdev.hxx>

#include <fmtanchr.hxx>

#include <horiornt.hxx>
#include <osl/diagnose.h>

#include <doc.hxx>
#include <pam.hxx>
#include <section.hxx>
#include <cntfrm.hxx>
#include <docsh.hxx>
#include <ndole.hxx>
#include <sw3io.hxx>
#include "bf_so3/staticbaseurl.hxx"
namespace binfilter {

using namespace ::utl;
using namespace ::rtl;
using namespace ::com::sun::star::uno;


class SwOLELRUCache : private SvPtrarr, private ::utl::ConfigItem
{
    sal_uInt16 nLRU_InitSize;
    sal_Bool bInUnload;
    ::com::sun::star::uno::Sequence< ::rtl::OUString > GetPropertyNames();

public:
    SwOLELRUCache();

    void Load();

    using SvPtrarr::Count;

    virtual void Commit();
    virtual void Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );

    void Insert( SwOLEObj& rObj );

    void RemovePtr( SwOLEObj* pObj )
    {
        USHORT nPos = SvPtrarr::GetPos( pObj );
        if( USHRT_MAX != nPos )
            SvPtrarr::Remove( nPos );
    }
};

void SwOLELRUCache::Commit() {}
void SwOLELRUCache::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& /*aPropertyNames*/ ) {}

SwOLELRUCache* SwOLEObj::pOLELRU_Cache = 0;

// --------------------
// SwOLENode
// --------------------

/*N*/ SwOLENode::SwOLENode( const SwNodeIndex &rWhere,
/*N*/                   SvInPlaceObject *pObj,
/*N*/                   SwGrfFmtColl *pGrfColl,
/*N*/                   SwAttrSet* pAutoAttr ) :
/*N*/   SwNoTxtNode( rWhere, ND_OLENODE, pGrfColl, pAutoAttr ),
/*N*/   aOLEObj( pObj ),
/*N*/   bOLESizeInvalid( FALSE )
/*N*/ {
/*N*/   aOLEObj.SetNode( this );
/*N*/ }

/*N*/ SwOLENode::SwOLENode( const SwNodeIndex &rWhere,
/*N*/                   const String &rString,
/*N*/                   SwGrfFmtColl *pGrfColl,
/*N*/                   SwAttrSet* pAutoAttr ) :
/*N*/   SwNoTxtNode( rWhere, ND_OLENODE, pGrfColl, pAutoAttr ),
/*N*/   aOLEObj( rString ),
/*N*/   bOLESizeInvalid( FALSE )
/*N*/ {
/*N*/   aOLEObj.SetNode( this );
/*N*/ }

/*N*/ SwCntntNode *SwOLENode::SplitNode( const SwPosition & )
/*N*/ {
/*N*/   // OLE-Objecte vervielfaeltigen ??
/*N*/   OSL_ENSURE( FALSE, "OleNode: can't split." );
/*N*/   return this;
/*N*/ }

// Laden eines in den Undo-Bereich verschobenen OLE-Objekts


// Sichern eines in den Undo-Bereich zu verschiebenden OLE-Objekts



/*N*/ SwOLENode * SwNodes::MakeOLENode( const SwNodeIndex & rWhere,
/*N*/                                   SvInPlaceObject *pObj,
/*N*/                                   SwGrfFmtColl* pGrfColl,
/*N*/                                   SwAttrSet* pAutoAttr )
/*N*/ {
/*N*/   OSL_ENSURE( pGrfColl,"SwNodes::MakeOLENode: Formatpointer ist 0." );
/*N*/
/*N*/   SwOLENode *pNode =
/*N*/       new SwOLENode( rWhere, pObj, pGrfColl, pAutoAttr );
/*N*/
/*N*/   return pNode;
/*N*/ }


/*N*/ SwOLENode * SwNodes::MakeOLENode( const SwNodeIndex & rWhere,
/*N*/                                   String &rName,
/*N*/                                   SwGrfFmtColl* pGrfColl,
/*N*/                                   SwAttrSet* pAutoAttr )
/*N*/ {
/*N*/   OSL_ENSURE( pGrfColl,"SwNodes::MakeOLENode: Formatpointer ist 0." );
/*N*/
/*N*/   SwOLENode *pNode =
/*N*/       new SwOLENode( rWhere, rName, pGrfColl, pAutoAttr );
/*N*/
/*N*/   return pNode;
/*N*/ }


/*N*/ Size SwOLENode::GetTwipSize() const
/*N*/ {
/*N*/   SvInPlaceObjectRef xRef( ((SwOLENode*)this)->aOLEObj.GetOleRef() );
/*N*/   Size aSz( xRef->GetVisArea().GetSize() );
/*N*/   const MapMode aDest( MAP_TWIP );
/*N*/   const MapMode aSrc ( xRef->GetMapUnit() );
/*N*/   return OutputDevice::LogicToLogic( aSz, aSrc, aDest );
/*N*/ }


/*N*/ SwCntntNode* SwOLENode::MakeCopy( SwDoc* pDoc, const SwNodeIndex& rIdx ) const
/*N*/ {
/*N*/   // Falls bereits eine SvPersist-Instanz existiert, nehmen wir diese
/*N*/   SvPersist* p = pDoc->GetPersist();
/*N*/   if( !p )
/*N*/   {
/*?*/       OSL_ENSURE( pDoc->GetRefForDocShell(),
/*?*/                       "wo ist die Ref-Klasse fuer die DocShell?");
/*?*/       p = new SwDocShell( pDoc, SFX_CREATE_MODE_INTERNAL );
/*?*/       *pDoc->GetRefForDocShell() = p;
/*?*/       p->DoInitNew( NULL );
/*N*/   }
/*N*/
/*N*/   // Wir hauen das Ding auf SvPersist-Ebene rein
/*N*/   String aNewName( Sw3Io::UniqueName( p->GetStorage(), "Obj" ) );
/*N*/   SvPersist* pSrc = GetDoc()->GetPersist();
/*N*/
/*N*/   p->CopyObject( aOLEObj.aName, aNewName, pSrc );
/*N*/   SwOLENode* pOLENd = pDoc->GetNodes().MakeOLENode( rIdx, aNewName,
/*N*/                                   (SwGrfFmtColl*)pDoc->GetDfltGrfFmtColl(),
/*N*/                                   (SwAttrSet*)GetpSwAttrSet() );
/*N*/
/*N*/   pOLENd->SetChartTblName( GetChartTblName() );
/*N*/   pOLENd->SetAlternateText( GetAlternateText() );
/*N*/     pOLENd->SetContour( HasContour(), HasAutomaticContour() );
/*N*/
/*N*/   pOLENd->SetOLESizeInvalid( TRUE );
/*N*/   pDoc->SetOLEPrtNotifyPending();
/*N*/
/*N*/   return pOLENd;
/*N*/ }

/*N*/ SwOLEObj::SwOLEObj( SvInPlaceObject *pObj )
/*N*/   : pOLENd( 0 )
/*N*/   , pOLERef( new SvInPlaceObjectRef( pObj ) )
/*N*/ {
/*N*/ }


/*N*/ SwOLEObj::SwOLEObj( const String &rString )
/*N*/   : pOLENd( 0 )
/*N*/   , pOLERef( 0 )
/*N*/   , aName( rString )
/*N*/ {
/*N*/ }


/*N*/ SwOLEObj::~SwOLEObj()
/*N*/ {
/*N*/   if( pOLERef && pOLERef->Is() )
/*N*/       //#41499# Kein DoClose(). Beim Beenden ruft der Sfx ein DoClose auf
/*N*/       //die offenen Objekte. Dadurch wird ggf. eine temp. OLE-Grafik wieder
/*N*/       //in eine Grafik gewandelt. Der OLE-Node wird zerstoert. Das DoClose
/*N*/       //wueder in das leere laufen, weil das Objekt bereits im DoClose steht.
/*N*/       //Durch das remove unten waere das DoClose aber nicht vollstaendig.
/*N*/       (*pOLERef)->GetProtocol().Reset();
/*N*/   delete pOLERef;
/*N*/   // Object aus dem Storage removen!!
/*N*/   if( pOLENd && !pOLENd->GetDoc()->IsInDtor() )   //NIcht notwendig im DTor (MM)
/*N*/   {
/*N*/       SvPersist* p = pOLENd->GetDoc()->GetPersist();
/*N*/       if( p )     // muss er existieren ?
/*N*/           p->Remove( aName );
/*N*/   }
/*N*/
/*N*/   if( pOLELRU_Cache )
/*N*/   {
/*N*/       pOLELRU_Cache->RemovePtr( this );
/*N*/       if( !pOLELRU_Cache->Count() )
/*N*/           // der letzte macht die Tuer zu
/*N*/           delete pOLELRU_Cache, pOLELRU_Cache = 0;
/*N*/   }
/*N*/ }


/*N*/ void SwOLEObj::SetNode( SwOLENode* pNode )
/*N*/ {
/*N*/   pOLENd = pNode;
/*N*/   if ( pOLERef && !aName.Len() )
/*N*/   {
/*N*/       SwDoc* pDoc = pNode->GetDoc();
/*N*/
/*N*/       // Falls bereits eine SvPersist-Instanz existiert, nehmen wir diese
/*N*/       SvPersist* p = pDoc->GetPersist();
/*N*/       if( !p )
/*N*/       {
/*?*/           OSL_ENSURE( !this, "warum wird hier eine DocShell angelegt?" );
/*?*/           p = new SwDocShell( pDoc, SFX_CREATE_MODE_INTERNAL );
/*?*/           p->DoInitNew( NULL );
/*N*/       }
/*N*/       // Wir hauen das Ding auf SvPersist-Ebene rein
/*N*/       aName = Sw3Io::UniqueName( p->GetStorage(), "Obj" );
/*N*/       SvInfoObjectRef refObj = new SvEmbeddedInfoObject( *pOLERef, aName );
/*N*/
/*N*/       ULONG nLstLen = p->GetObjectList() ? p->GetObjectList()->Count() : 0;
/*N*/       if ( !p->Move( refObj, aName ) ) // Eigentuemer Uebergang!
/*?*/           refObj.Clear();
/*N*/       else if( nLstLen == p->GetObjectList()->Count() )
/*N*/       {
/*N*/           // Task 91051: Info-Object not insertet, so it exist another
/*N*/           //              InfoObject to the same Object and the Objects
/*N*/           //              is stored in the persist. This InfoObject we must
/*N*/           //              found.
/*?*/           p->Insert( refObj );
/*N*/       }
/*N*/       OSL_ENSURE( refObj.Is(), "InsertObject failed" );
/*N*/   }
/*N*/ }

/*N*/ BOOL SwOLEObj::IsOleRef() const
/*N*/ {
/*N*/   return pOLERef && pOLERef->Is();
/*N*/ }

/*N*/ SvInPlaceObjectRef SwOLEObj::GetOleRef()
/*N*/ {
/*N*/   if( !pOLERef || !pOLERef->Is() )
/*N*/   {
/*N*/       SvPersist* p = pOLENd->GetDoc()->GetPersist();
/*N*/       OSL_ENSURE( p, "kein SvPersist vorhanden" );
/*N*/
/*N*/       // MIB 18.5.97: DIe Base-URL wird jetzt gesetzt, damit Plugins
/*N*/       // nach dem Laden und vor dem Aktivieren des Frames korrekt
/*N*/       // geladen werden koennen
/*N*/       String sBaseURL( ::binfilter::StaticBaseUrl::GetBaseURL() );
/*N*/       const SwDocShell *pDocSh = pOLENd->GetDoc()->GetDocShell();
/*N*/       const SfxMedium *pMedium;
/*N*/       if( pDocSh && 0 != (pMedium = pDocSh->GetMedium()) &&
/*N*/           pMedium->GetName() != sBaseURL )
/*N*/               ::binfilter::StaticBaseUrl::SetBaseURL( pMedium->GetName() );
/*N*/
/*N*/       SvPersistRef xObj = p->GetObject( aName );
/*N*/       OSL_ENSURE( !pOLERef || !pOLERef->Is(),
/*N*/               "rekursiver Aufruf von GetOleRef() ist nicht erlaubt" );
/*N*/
/*N*/       ::binfilter::StaticBaseUrl::SetBaseURL( sBaseURL );
/*N*/
/*N*/       if ( !xObj.Is() )
/*N*/       {
/*?*/           //Das Teil konnte nicht geladen werden (wahrsch. Kaputt).
/*?*/           Rectangle aArea;
/*?*/           SwFrm *pFrm = pOLENd->GetFrm();
/*?*/           if ( pFrm )
/*?*/           {
/*?*/               Size aSz( pFrm->Frm().SSize() );
/*?*/               const MapMode aSrc ( MAP_TWIP );
/*?*/               const MapMode aDest( MAP_100TH_MM );
/*?*/               aSz = OutputDevice::LogicToLogic( aSz, aSrc, aDest );
/*?*/               aArea.SetSize( aSz );
/*?*/           }
/*?*/           else
/*?*/               aArea.SetSize( Size( 5000,  5000 ) );
/*?*/           xObj = new SvDeathObject( aArea );
/*N*/       }
/*N*/
/*N*/       if( pOLERef )
/*?*/           *pOLERef = &xObj;
/*N*/       else
/*N*/           pOLERef = new SvInPlaceObjectRef( xObj );
/*N*/   }
/*N*/
/*N*/   if( !pOLELRU_Cache )
/*N*/       pOLELRU_Cache = new SwOLELRUCache;
/*N*/
/*N*/   pOLELRU_Cache->Insert( *this );
/*N*/
/*N*/   return *pOLERef;
/*N*/ }


/*N*/ BOOL SwOLEObj::RemovedFromLRU()
/*N*/ {
/*N*/   BOOL bRet = TRUE;
/*N*/   return bRet;
/*N*/ }

/*N*/ SwOLELRUCache::SwOLELRUCache()
/*N*/   : SvPtrarr( 64, 16 )
/*N*/   , ::utl::ConfigItem( OUString( RTL_CONSTASCII_USTRINGPARAM( "Office.Common/Cache" )))
/*N*/   , nLRU_InitSize( 20 )
/*N*/   , bInUnload( sal_False )
/*N*/ {
/*N*/   EnableNotification( GetPropertyNames() );
/*N*/   Load();
/*N*/ }

/*N*/ ::com::sun::star::uno::Sequence< ::rtl::OUString > SwOLELRUCache::GetPropertyNames()
/*N*/ {
/*N*/   Sequence< OUString > aNames( 1 );
/*N*/   OUString* pNames = aNames.getArray();
/*N*/   pNames[0] = OUString( RTL_CONSTASCII_USTRINGPARAM( "Writer/OLE_Objects" ));
/*N*/   return aNames;
/*N*/ }



/*N*/ void SwOLELRUCache::Load()
/*N*/ {
/*N*/   Sequence< OUString > aNames( GetPropertyNames() );
/*N*/   Sequence< Any > aValues = GetProperties( aNames );
/*N*/   const Any* pValues = aValues.getConstArray();
/*N*/   DBG_ASSERT( aValues.getLength() == aNames.getLength(), "GetProperties failed" );
/*N*/   if( aValues.getLength() == aNames.getLength() &&
/*N*/       pValues->hasValue() )
/*N*/   {
/*N*/       sal_Int32 nVal(0);
/*N*/       *pValues >>= nVal;
/*N*/       if( 20 > nVal )
/*N*/           nVal = 20;
/*N*/
/*N*/       if( !bInUnload )
/*N*/       {
/*N*/           USHORT nPos = SvPtrarr::Count();
/*N*/           if( nVal < nLRU_InitSize && nPos > nVal )
/*N*/           {
/*N*/               // remove the last entries
/*N*/               while( nPos > nVal )
/*N*/               {
/*?*/                   SwOLEObj* pObj = (SwOLEObj*) SvPtrarr::GetObject( --nPos );
/*?*/                   if( pObj->RemovedFromLRU() )
/*?*/                       SvPtrarr::Remove( nPos );
/*?*/               }
/*N*/           }
/*N*/       }
/*N*/       nLRU_InitSize = (USHORT)nVal;
/*N*/   }
/*N*/ }

/*N*/ void SwOLELRUCache::Insert( SwOLEObj& rObj )
/*N*/ {
/*N*/   if( !bInUnload )
/*N*/   {
/*N*/       SwOLEObj* pObj = &rObj;
/*N*/       USHORT nPos = SvPtrarr::GetPos( pObj );
/*N*/       if( nPos )  // der auf der 0. Pos muss nicht verschoben werden!
/*N*/       {
/*N*/           if( USHRT_MAX != nPos )
/*?*/               SvPtrarr::Remove( nPos );
/*N*/
/*N*/           SvPtrarr::Insert( pObj, 0 );
/*N*/
/*N*/           nPos = SvPtrarr::Count();
/*N*/           while( nPos > nLRU_InitSize )
/*N*/           {
/*?*/               pObj = (SwOLEObj*) SvPtrarr::GetObject( --nPos );
/*?*/               if( pObj->RemovedFromLRU() )
/*?*/                   SvPtrarr::Remove( nPos );
/*N*/           }
/*N*/       }
/*N*/   }
/*N*/ #ifdef DBG_UTIL
/*N*/   else
/*N*/   {
/*?*/       SwOLEObj* pObj = &rObj;
/*?*/       USHORT nPos = SvPtrarr::GetPos( pObj );
/*?*/       OSL_ENSURE( USHRT_MAX != nPos, "Insert a new OLE object into a looked cache" );
/*?*/   }
/*N*/ #endif
/*N*/ }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
