/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: dbg_lay.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 09:24:48 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _DBG_LAY_HXX
#define _DBG_LAY_HXX


#define PROT_FILE_INIT  0x00000000
#define PROT_INIT       0x00000001
#define PROT_MAKEALL    0x00000002
#define PROT_MOVE_FWD   0x00000004
#define PROT_MOVE_BWD   0x00000008
#define PROT_GROW       0x00000010
#define PROT_SHRINK     0x00000020
#define PROT_GROW_TST   0x00000040
#define PROT_SHRINK_TST 0x00000080
#define PROT_SIZE       0x00000100
#define PROT_PRTAREA    0x00000200
#define PROT_POS        0x00000400
#define PROT_ADJUSTN    0x00000800
#define PROT_SECTION    0x00001000
#define PROT_CUT        0x00002000
#define PROT_PASTE      0x00004000
#define PROT_LEAF       0x00008000
#define PROT_TESTFORMAT 0x00010000
#define PROT_FRMCHANGES 0x00020000
#define PROT_SNAPSHOT   0x00040000

#define ACT_START           1
#define ACT_END             2
#define ACT_CREATE_MASTER   3
#define ACT_CREATE_FOLLOW   4
#define ACT_DEL_MASTER      5
#define ACT_DEL_FOLLOW      6
#define ACT_MERGE           7
#define ACT_NEXT_SECT       8
#define ACT_PREV_SECT       9

#define SNAP_LOWER       0x00000001
#define SNAP_FLYFRAMES   0x00000002
#define SNAP_TABLECONT   0x00000004

#ifndef PRODUCT

// auto strip #include "swtypes.hxx"
#include <tools/debug.hxx>
namespace binfilter {
class SwImplProtocol;
class SwFrm;
class SwImplEnterLeave;

class SwProtocol
{
    static ULONG nRecord;
    static SwImplProtocol* pImpl;
    static BOOL Start() { return 0 != ( PROT_INIT & nRecord ); }
public:
    static ULONG Record() { return nRecord; }
    static void SetRecord( ULONG nNew ) { nRecord = nNew; }
    static BOOL Record( ULONG nFunc ) { return 0 != (( nFunc | PROT_INIT ) & nRecord); }
    static void Record( const SwFrm* pFrm, ULONG nFunction, ULONG nAction, void* pParam ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 static void Record( const SwFrm* pFrm, ULONG nFunction, ULONG nAction, void* pParam );
    static void Init();
    static void Stop();
//STRIP001     static void SnapShot( const SwFrm* pFrm, ULONG nFlags );
//STRIP001     static void GetVar( const USHORT nNo, long& rVar );
};

class SwEnterLeave
{
    SwImplEnterLeave* pImpl;
    void Ctor( const SwFrm* pFrm, ULONG nFunc, ULONG nAct, void* pPar ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void Ctor( const SwFrm* pFrm, ULONG nFunc, ULONG nAct, void* pPar );
    void Dtor(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void Dtor();
public:
     SwEnterLeave( const SwFrm* pFrm, ULONG nFunc, ULONG nAct, void* pPar )
    { if( SwProtocol::Record( nFunc ) ) Ctor( pFrm, nFunc, nAct, pPar ); else pImpl = NULL; }
    ~SwEnterLeave() { if( pImpl ) Dtor(); }
};

#define PROTOCOL( pFrm, nFunc, nAct, pPar ) {   if( SwProtocol::Record( nFunc ) )\
                                                    SwProtocol::Record( pFrm, nFunc, nAct, pPar ); }
#define PROTOCOL_INIT SwProtocol::Init();
#define PROTOCOL_STOP SwProtocol::Stop();
#define PROTOCOL_ENTER( pFrm, nFunc, nAct, pPar ) SwEnterLeave aEnter( pFrm, nFunc, nAct, pPar );
#define PROTOCOL_SNAPSHOT( pFrm, nFlags ) SwProtocol::SnapShot( pFrm, nFlags );
#define GET_VARIABLE( nNo, nVar ) SwProtocol::GetVar( nNo, nVar );
} //STRIP008 end of namespace binfilter
#else
namespace binfilter {
#define PROTOCOL( pFrm, nFunc, nAct, pPar )
#define PROTOCOL_INIT
#define PROTOCOL_STOP
#define PROTOCOL_ENTER( pFrm, nFunc, nAct, pPar )
#define PROTOCOL_SNAPSHOT( pFrm, nFlags )
#define GET_VARIABLE( nNo, nVar )
} //namespace binfilter
#endif

#endif
