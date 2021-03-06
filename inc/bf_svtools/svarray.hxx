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

#ifndef _SVARRAY_HXX
#define _SVARRAY_HXX

#include <string.h>     // memmove()
#include <limits.h>     // USHRT_MAX

#include <rtl/alloc.h>

#include <bf_svtools/bf_solar.h>

class String;

#ifndef CONCAT
#define CONCAT(x,y) x##y
#endif

class DummyType;
inline void* operator new( size_t, DummyType* pPtr )
{
    return pPtr;
}
inline void operator delete( void*, DummyType* ) {}

namespace binfilter {

#define _SV_DECL_VARARR_GEN(nm, AE, IS, GS, AERef, vis )\
typedef BOOL (*FnForEach_##nm)( const AERef, void* );\
class vis nm\
{\
protected:\
    AE    *pData;\
    USHORT nFree;\
    USHORT nA;\
\
    void _resize(size_t n);\
\
public:\
    nm( USHORT= IS, BYTE= GS );\
    ~nm() { rtl_freeMemory( pData ); }\
\
    AE& operator[](USHORT nP) const { return *(pData+nP); }\
\
    void Insert( const nm * pI, USHORT nP,\
             USHORT nS = 0, USHORT nE = USHRT_MAX )\
    {\
        if( USHRT_MAX == nE ) \
            nE = pI->nA; \
        if( nS < nE ) \
            Insert( (const AE*)pI->pData+nS, (USHORT)nE-nS, nP );\
    }\
    AERef GetObject(USHORT nP) const { return (*this)[nP]; } \
\
    void Insert( const AERef aE, USHORT nP )\
    {\
        if (nFree < 1)\
            _resize (nA + ((nA > 1) ? nA : 1));\
        if( pData && nP < nA )\
            memmove( pData+nP+1, pData+nP, (nA-nP) * sizeof( AE ));\
        *(pData+nP) = (AE&)aE;\
        ++nA; --nFree;\
    }\
    void Insert( const AE *pE, USHORT nL, USHORT nP )\
    {\
        if (nFree < nL)\
            _resize (nA + ((nA > nL) ? nA : nL));\
        if( pData && nP < nA )\
            memmove( pData+nP+nL, pData+nP, (nA-nP) * sizeof( AE ));\
        if( pE )\
            memcpy( pData+nP, pE, nL * sizeof( AE ));\
        nA = nA + nL; nFree = nFree - nL;\
    }\
\
    void Remove( USHORT nP, USHORT nL = 1 )\
    {\
        if( !nL )\
            return;\
        if( pData && nP+1 < nA )\
            memmove( pData+nP, pData+nP+nL, (nA-nP-nL) * sizeof( AE ));\
        nA = nA - nL; nFree = nFree + nL;\
        if (nFree > nA)\
            _resize (nA);\
    }\
\
    void Replace( const AERef aE, USHORT nP )\
    {\
        if( nP < nA )\
            *(pData+nP) = (AE&)aE;\
    }\
\
    void Replace( const AE *pE, USHORT nL, USHORT nP )\
    {\
        if( pE && nP < nA )\
        {\
            if( nP + nL < nA )\
                memcpy( pData + nP, pE, nL * sizeof( AE ));\
            else if( nP + nL < nA + nFree )\
            {\
                memcpy( pData + nP, pE, nL * sizeof( AE ));\
                nP = nP + (nL - nA); \
                nFree = nP;\
            }\
            else \
            {\
                USHORT nTmpLen = nA + nFree - nP; \
                memcpy( pData + nP, pE, nTmpLen * sizeof( AE ));\
                nA = nA + nFree; \
                nFree = 0; \
                Insert( pE + nTmpLen, nL - nTmpLen, nA );\
            }\
        }\
    }\
\
    USHORT Count() const { return nA; }\
    const AE* GetData() const { return (const AE*)pData; }\
\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, fnForEach, pArgs );\
    }\
\
    void ForEach( USHORT nS, USHORT nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, fnForEach, pArgs );\
    }\
\
    void _ForEach( USHORT nStt, USHORT nE, \
            CONCAT( FnForEach_, nm ) fnCall, void* pArgs = 0 )\
    {\
        if( nStt >= nE || nE > nA )\
            return;\
        for( ; nStt < nE && (*fnCall)( *(const AE*)(pData+nStt), pArgs ); nStt++)\
            ;\
    }\
\

#define SV_DECL_VARARR_GEN(nm, AE, IS, GS, AERef, vis )\
_SV_DECL_VARARR_GEN(nm, AE, IS, GS, AERef, vis )\
private:\
nm( const nm& );\
nm& operator=( const nm& );\
};

#define SV_DECL_VARARR(nm, AE, IS, GS ) \
SV_DECL_VARARR_GEN(nm, AE, IS, GS, AE &, )

#define SV_DECL_VARARR_VISIBILITY(nm, AE, IS, GS, vis ) \
SV_DECL_VARARR_GEN(nm, AE, IS, GS, AE &, vis )

#define SV_IMPL_VARARR( nm, AE ) \
nm::nm( USHORT nInit, BYTE )\
    : pData (0),\
      nFree (nInit),\
      nA    (0)\
{\
    if( nInit )\
    {\
        pData = (AE*)(rtl_allocateMemory(sizeof(AE) * nInit));\
        DBG_ASSERT( pData, "CTOR, allocate");\
    }\
}\
\
void nm::_resize (size_t n)\
{\
    USHORT nL = ((n < USHRT_MAX) ? USHORT(n) : USHRT_MAX);\
    AE* pE = (AE*)(rtl_reallocateMemory (pData, sizeof(AE) * nL));\
    if ((pE != 0) || (nL == 0))\
    {\
        pData = pE;\
        nFree = nL - nA;\
    }\
}\
\

#define SV_DECL_PTRARR_GEN(nm, AE, IS, GS, AERef, VPRef, vis )\
typedef BOOL (*FnForEach_##nm)( const AERef, void* );\
class vis nm: public SvPtrarr \
{\
public:\
    nm( USHORT nIni=IS, BYTE nG=GS )\
        : SvPtrarr(nIni,nG) {}\
    void Insert( const nm *pI, USHORT nP, \
            USHORT nS = 0, USHORT nE = USHRT_MAX ) {\
        SvPtrarr::Insert((const SvPtrarr*)pI, nP, nS, nE);\
    }\
    void Insert( const AERef aE, USHORT nP ) {\
        SvPtrarr::Insert( (const VPRef )aE, nP );\
    }\
    void Insert( const AE *pE, USHORT nL, USHORT nP ) {\
        SvPtrarr::Insert( (const VoidPtr*)pE, nL, nP );\
    }\
    void Replace( const AERef aE, USHORT nP ) {\
        SvPtrarr::Replace( (const VPRef)aE, nP );\
    }\
    void Replace( const AE *pE, USHORT nL, USHORT nP ) {\
        SvPtrarr::Replace( (const VoidPtr*)pE, nL, nP );\
    }\
    void Remove( USHORT nP, USHORT nL = 1) {\
        SvPtrarr::Remove(nP,nL);\
    }\
    const AE* GetData() const {\
        return (const AE*)SvPtrarr::GetData();\
    }\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    void ForEach( USHORT nS, USHORT nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    AE operator[]( USHORT nP )const  { \
        return (AE)SvPtrarr::operator[](nP); }\
    AE GetObject(USHORT nP) const { \
        return (AE)SvPtrarr::GetObject(nP); }\
    \
    USHORT GetPos( const AERef aE ) const { \
        return SvPtrarr::GetPos((const VPRef)aE);\
    }\
    void DeleteAndDestroy( USHORT nP, USHORT nL=1 );\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR(nm, AE, IS, GS )\
SV_DECL_PTRARR_GEN(nm, AE, IS, GS, AE &, VoidPtr &, )

#define SV_DECL_PTRARR_VISIBILITY(nm, AE, IS, GS, vis )\
SV_DECL_PTRARR_GEN(nm, AE, IS, GS, AE &, VoidPtr &, vis )

#define SV_DECL_PTRARR_DEL_GEN(nm, AE, IS, GS, AERef, VPRef, vis )\
typedef BOOL (*FnForEach_##nm)( const AERef, void* );\
class vis nm: public SvPtrarr \
{\
public:\
    nm( USHORT nIni=IS, BYTE nG=GS )\
        : SvPtrarr(nIni,nG) {}\
    ~nm() { DeleteAndDestroy( 0, Count() ); }\
    void Insert( const nm *pI, USHORT nP, \
            USHORT nS = 0, USHORT nE = USHRT_MAX ) {\
        SvPtrarr::Insert((const SvPtrarr*)pI, nP, nS, nE);\
    }\
    void Insert( const AERef aE, USHORT nP ) {\
        SvPtrarr::Insert((const VPRef)aE, nP );\
    }\
    void Insert( const AE *pE, USHORT nL, USHORT nP ) {\
        SvPtrarr::Insert( (const VoidPtr *)pE, nL, nP );\
    }\
    void Replace( const AERef aE, USHORT nP ) {\
        SvPtrarr::Replace( (const VPRef)aE, nP );\
    }\
    void Replace( const AE *pE, USHORT nL, USHORT nP ) {\
        SvPtrarr::Replace( (const VoidPtr*)pE, nL, nP );\
    }\
    void Remove( USHORT nP, USHORT nL = 1) {\
        SvPtrarr::Remove(nP,nL);\
    }\
    const AE* GetData() const {\
        return (const AE*)SvPtrarr::GetData();\
    }\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    void ForEach( USHORT nS, USHORT nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    AE operator[]( USHORT nP )const  { \
        return (AE)SvPtrarr::operator[](nP); }\
    AE GetObject( USHORT nP )const  { \
        return (AE)SvPtrarr::GetObject(nP); }\
    \
    USHORT GetPos( const AERef aE ) const { \
        return SvPtrarr::GetPos((const VPRef)aE);\
    } \
    void DeleteAndDestroy( USHORT nP, USHORT nL=1 );\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_DEL(nm, AE, IS, GS )\
SV_DECL_PTRARR_DEL_GEN(nm, AE, IS, GS, AE &, VoidPtr &, )

#define SV_DECL_PTRARR_DEL_VISIBILITY(nm, AE, IS, GS, vis )\
SV_DECL_PTRARR_DEL_GEN(nm, AE, IS, GS, AE &, VoidPtr &, vis)

#define SV_IMPL_PTRARR(nm, AE )\
void nm::DeleteAndDestroy( USHORT nP, USHORT nL )\
{ \
    if( nL ) {\
        DBG_ASSERT( nP < nA && nP + nL <= nA,"Del");\
        for( USHORT n=nP; n < nP + nL; n++ ) \
            delete *((AE*)pData+n); \
        SvPtrarr::Remove( nP, nL ); \
    } \
}

typedef void* VoidPtr;
_SV_DECL_VARARR_GEN( SvPtrarr, VoidPtr, 0, 1, VoidPtr &, )
USHORT GetPos( const VoidPtr & aE ) const;
};


// SORTARR - Begin

#define _SORT_CLASS_DEF(nm, AE, IS, GS, vis)\
typedef BOOL (*FnForEach_##nm)( const AE&, void* );\
class vis nm : private nm##_SAR \
{\
public:\
    nm(USHORT nSize = IS, BYTE nG = GS)\
        : nm##_SAR(nSize,nG) {}\
    void Insert( const nm *pI, USHORT nS=0, USHORT nE=USHRT_MAX )\
    {\
        if( USHRT_MAX == nE )\
            nE = pI->Count();\
        USHORT nP;\
        const AE * pIArr = pI->GetData();\
        for( ; nS < nE; ++nS )\
        {\
            if( ! Seek_Entry( *(pIArr+nS), &nP) )\
                    nm##_SAR::Insert( *(pIArr+nS), nP );\
            if( ++nP >= Count() )\
            {\
                nm##_SAR::Insert( pI, nP, nS+1, nE );\
                nS = nE;\
            }\
        }\
    }\
\
    BOOL Insert( const AE& aE )\
    {\
        USHORT nP;\
        BOOL bExist;\
        bExist = Seek_Entry( aE, &nP );\
        if( ! bExist )\
            nm##_SAR::Insert( aE, nP );\
        return !bExist;\
    }\
\
    BOOL Insert( const AE& aE, USHORT& rP )\
    {\
        BOOL bExist;\
        bExist = Seek_Entry( aE, &rP );\
        if( ! bExist )\
            nm##_SAR::Insert( aE, rP );\
        return !bExist;\
    }\
\
    void Insert( const AE *pE, USHORT nL )\
    {\
        USHORT nP;\
        for( USHORT n = 0; n < nL; ++n )\
            if( ! Seek_Entry( *(pE+n), &nP ))\
                nm##_SAR::Insert( *(pE+n), nP );\
    }\
\
    void Remove( USHORT nP, USHORT nL = 1 )\
    {\
        if( nL )\
            nm##_SAR::Remove( nP, nL);\
    }\
    void Remove( const AE& aE, USHORT nL = 1 )\
    {\
        USHORT nP;\
        if( nL && Seek_Entry( aE, &nP ) )   \
            nm##_SAR::Remove( nP, nL);\
    }\
    USHORT Count() const  {   return nm##_SAR::Count(); }\
    const AE* GetData() const { return (const AE*)pData; }\
\
/* Das Ende stehe im DECL-Makro !!! */

#define _SV_IMPL_SORTAR_ALG(nm, AE)

#define _SORTARR_BLC_CASTS(nm, AE )\
    USHORT GetPos( const AE& aE ) const { \
        return SvPtrarr::GetPos((const VoidPtr&)aE);\
    }

#define _SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, GS, vis)\
SV_DECL_PTRARR_VISIBILITY(nm##_SAR, AE, IS, GS, vis)\
_SORT_CLASS_DEF(nm, AE, IS, GS, vis)\
    AE operator[](USHORT nP) const {\
        return nm##_SAR::operator[]( nP );\
    }\
    AE GetObject(USHORT nP) const {\
        return nm##_SAR::GetObject( nP );\
    }\
    BOOL Seek_Entry( const AE aE, USHORT* pP = 0 ) const;\
    void ForEach( CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( 0, nA, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    void ForEach( USHORT nS, USHORT nE, \
                    CONCAT( FnForEach_, nm ) fnForEach, void* pArgs = 0 )\
    {\
        _ForEach( nS, nE, (FnForEach_SvPtrarr)fnForEach, pArgs );\
    }\
    void DeleteAndDestroy( USHORT nP, USHORT nL=1 ); \
    _SORTARR_BLC_CASTS(nm, AE )\
\
/* Das Ende stehe im DECL-Makro !!! */

#define _SV_DECL_PTRARR_SORT(nm, AE, IS, GS, vis)\
_SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, GS, vis)\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_SORT(nm, AE, IS, GS)\
_SV_DECL_PTRARR_SORT(nm, AE, IS, GS, )

#define SV_DECL_PTRARR_SORT_VISIBILITY(nm, AE, IS, GS, vis)\
_SV_DECL_PTRARR_SORT(nm, AE, IS, GS, vis)


#define _SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS, vis)\
_SV_DECL_PTRARR_SORT_ALG(nm, AE, IS, GS, vis)\
    ~nm() { DeleteAndDestroy( 0, Count() ); }\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS)\
_SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS, )

#define SV_DECL_PTRARR_SORT_DEL_VISIBILITY(nm, AE, IS, GS, vis)\
_SV_DECL_PTRARR_SORT_DEL(nm, AE, IS, GS, vis)

#define _SV_DECL_VARARR_SORT(nm, AE, IS, GS, vis)\
SV_DECL_VARARR_VISIBILITY(nm##_SAR, AE, IS, GS, vis)\
_SORT_CLASS_DEF(nm, AE, IS, GS, vis) \
    const AE& operator[](USHORT nP) const {\
        return nm##_SAR::operator[]( nP );\
    }\
    const AE& GetObject(USHORT nP) const {\
        return nm##_SAR::GetObject( nP );\
    }\
    BOOL Seek_Entry( const AE & aE, USHORT* pP = 0 ) const;\
private:\
    nm( const nm& );\
    nm& operator=( const nm& );\
};

#define SV_DECL_VARARR_SORT(nm, AE, IS, GS)\
_SV_DECL_VARARR_SORT(nm, AE, IS, GS,)

#define SV_IMPL_PTRARR_SORT( nm,AE )\
_SV_IMPL_SORTAR_ALG( nm,AE )\
    void nm::DeleteAndDestroy( USHORT nP, USHORT nL ) { \
        if( nL ) {\
            DBG_ASSERT( nP < nA && nP + nL <= nA, "ERR_VAR_DEL" );\
            for( USHORT n=nP; n < nP + nL; n++ ) \
                delete *((AE*)pData+n); \
            SvPtrarr::Remove( nP, nL ); \
        } \
    } \
    BOOL nm::Seek_Entry( const AE aE, USHORT* pP ) const\
    {\
        register USHORT nO  = nm##_SAR::Count(),\
                nM, \
                nU = 0;\
        if( nO > 0 )\
        {\
            nO--;\
            register long rCmp = (long)aE;\
            while( nU <= nO )\
            {\
                nM = nU + ( nO - nU ) / 2;\
                if( (long)*(pData + nM) == rCmp )\
                {\
                    if( pP ) *pP = nM;\
                    return TRUE;\
                }\
                else if( (long)*(pData+ nM) < (long)aE  )\
                    nU = nM + 1;\
                else if( nM == 0 )\
                {\
                    if( pP ) *pP = nU;\
                    return FALSE;\
                }\
                else\
                    nO = nM - 1;\
            }\
        }\
        if( pP ) *pP = nU;\
        return FALSE;\
    }



#define SV_IMPL_OP_PTRARR_SORT( nm,AE )\
_SV_IMPL_SORTAR_ALG( nm,AE )\
    void nm::DeleteAndDestroy( USHORT nP, USHORT nL ) { \
        if( nL ) {\
            DBG_ASSERT( nP < nA && nP + nL <= nA, "ERR_VAR_DEL" );\
            for( USHORT n=nP; n < nP + nL; n++ ) \
                delete *((AE*)pData+n); \
            SvPtrarr::Remove( nP, nL ); \
        } \
    } \
    BOOL nm::Seek_Entry( const AE aE, USHORT* pP ) const\
    {\
        register USHORT nO  = nm##_SAR::Count(),\
                nM, \
                nU = 0;\
        if( nO > 0 )\
        {\
            nO--;\
            while( nU <= nO )\
            {\
                nM = nU + ( nO - nU ) / 2;\
                if( *(*((AE*)pData + nM)) == *(aE) )\
                {\
                    if( pP ) *pP = nM;\
                    return TRUE;\
                }\
                else if( *(*((AE*)pData + nM)) < *(aE) )\
                    nU = nM + 1;\
                else if( nM == 0 )\
                {\
                    if( pP ) *pP = nU;\
                    return FALSE;\
                }\
                else\
                    nO = nM - 1;\
            }\
        }\
        if( pP ) *pP = nU;\
        return FALSE;\
    }

#define SV_IMPL_VARARR_SORT( nm,AE )\
SV_IMPL_VARARR(nm##_SAR, AE)\
_SV_IMPL_SORTAR_ALG( nm,AE )\
BOOL nm::Seek_Entry( const AE & aE, USHORT* pP ) const\
{\
    register USHORT nO  = nm##_SAR::Count(),\
            nM, \
            nU = 0;\
    if( nO > 0 )\
    {\
        nO--;\
        while( nU <= nO )\
        {\
            nM = nU + ( nO - nU ) / 2;\
            if( *(pData + nM) == aE )\
            {\
                if( pP ) *pP = nM;\
                return TRUE;\
            }\
            else if( *(pData + nM) < aE )\
                nU = nM + 1;\
            else if( nM == 0 )\
            {\
                if( pP ) *pP = nU;\
                return FALSE;\
            }\
            else\
                nO = nM - 1;\
        }\
    }\
    if( pP ) *pP = nU;\
    return FALSE;\
}

#if defined (C40) || defined (C41) || defined (C42) || defined(C50) || defined(C52)
#define C40_INSERT( c, p, n) Insert( (c const *) p, n )
#define C40_PUSH( c, p) Push( (c const *) p )
#define C40_PTR_INSERT( c, p) Insert( (c const *) p )
#define C40_REMOVE( c, p ) Remove( (c const *) p )
#define C40_REPLACE( c, p, n) Replace( (c const *) p, n )
#define C40_PTR_REPLACE( c, p) Replace( (c const *) p )
#define C40_GETPOS( c, r) GetPos( (c const *)r )
#else
#if defined ICC || defined GCC || ( defined(WNT) && _MSC_VER >= 1400)
#define C40_INSERT( c, p, n ) Insert( (c const *&) p, n )
#define C40_PUSH( c, p) Push( (c const *&) p )
#define C40_PTR_INSERT( c, p ) Insert( (c const *&) p )
#define C40_REMOVE( c, p ) Remove( (c const *&) p )
#define C40_REPLACE( c, p, n ) Replace( (c const *&) p, n )
#define C40_PTR_REPLACE( c, p ) Replace( (c const *&) p )
#define C40_GETPOS( c, r) GetPos( (c const *&) r )
#else
#define C40_INSERT( c, p, n ) Insert( p, n )
#define C40_PUSH( c, p) Push( p )
#define C40_PTR_INSERT( c, p ) Insert( p )
#define C40_REMOVE( c, p) Remove( p )
#define C40_REPLACE( c, p, n ) Replace( p, n )
#define C40_PTR_REPLACE( c, p ) Replace( p )
#define C40_GETPOS( c, r) GetPos( r )
#endif
#endif

}

#endif  //_SVARRAY_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
