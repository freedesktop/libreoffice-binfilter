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

#include <bf_svtools/zforlist.hxx>
#include <bf_so3/svstor.hxx>


#include <node.hxx>
#include <doc.hxx>
#include <pam.hxx>
#include <swerror.h>
#include <fltini.hxx>
#include <exlpar.hxx>
#include <fltglbls.hxx>
#include <excxfbuf.hxx>
#include <excvfbff.hxx>
#include <excfntbf.hxx>
#include <swfltopt.hxx>

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <comphelper/processfactory.hxx>
#include <legacysmgr/legacy_binfilters_smgr.hxx>
namespace binfilter {
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;

ExcGlob *pExcGlob = NULL;
//const double SwExcelParser::fExcToTwips = ( double ) TWIPS_PER_CHAR / 256;
const double SwExcelParser::fExcToTwips =
    ( double ) ((20.0 * 72.27) / 13.6) / 256;




//Diese Methode hier weil sie natuerlich nicht inline sein darf.
void SwFilterBase::Read( String &rS )
    {// liest 0-terminierten C-String!
    DBG_ASSERT( nReadBuffSize > 242,
        "-SwFilterBase::Read( String& ): Read-Buffer zu klein!!!!!!!!!" );

    sal_Char *pC = pReadBuff;
    register USHORT nCount = 0; // Einer wird mindestens gelesen

    // String wird komplett mit 0-Terminator eingelesen
    do  {
        *pIn >> *pC;
        nCount++;
        }
    while( *(pC++) != 0 );

    nBytesLeft -= nCount;
    rS = String(pReadBuff, eQuellChar);
    }



ExcGlob::ExcGlob( SwDoc& rDoc, const SwPaM& rPam )
    : FilterGlobals( rDoc, rPam )
{
    pExcGlob = this;            // die Buffer greifen schon auf den Pointer zu
    pXF_Buff = new XF_Buffer;
    pFontBuff = new FontBuffer();
    pColorBuff = new ColorBuffer();

    Reference< XMultiServiceFactory > xMSF = ::legacy_binfilters::getLegacyProcessServiceFactory();
    pNumFormatter = new SvNumberFormatter( xMSF, LANGUAGE_SYSTEM );
}

ExcGlob::~ExcGlob()
{
    delete pXF_Buff;
    delete pFontBuff;
    delete pColorBuff;
    delete pNumFormatter;
}


SwExcelParser::SwExcelParser( SwDoc &rDoc, const SwPaM & rCrsr,
                                SvStream& rInInit, int bReadNewDoc,
                                CharSet eQ )
    : bNewDoc( bReadNewDoc )
{
    pIn = &rInInit;
    eQuellChar = eQ;
    pExcGlob = new ExcGlob( rDoc, rCrsr );

    pValueFormBuffer = new ValueFormBuffer;

    nReadBuffSize = 2048;
    pReadBuff = new sal_Char[ nReadBuffSize ];

    eDateiTyp = BiffX;
    nLastCol = nLastRow = 0;
    aColRowBuff.SetDefWidth( ( USHORT ) ( fExcToTwips * 1024 ) );   // 4 Zeichen Standardbreite

    bResultString = FALSE;
}

SwExcelParser::~SwExcelParser()
{
    delete pExcGlob;
    delete pValueFormBuffer;
    delete[] pReadBuff;
    pExcGlob = NULL;
}


ULONG SwExcelParser::CallParser()
{
    static const sal_Char* aNames[4] = {
        "Excel_Lotus/MinRow", "Excel_Lotus/MaxRow",
        "Excel_Lotus/MinCol", "Excel_Lotus/MaxCol"
    };
    sal_uInt32 aVal[4];
    SwFilterOptions aOpt( 4, aNames, aVal );

    USHORT nMinRow = ( USHORT ) aVal[ 0 ];
    USHORT nMaxRow = ( USHORT ) aVal[ 1 ];
    USHORT nMinCol = ( USHORT ) aVal[ 2 ];
    USHORT nMaxCol = ( USHORT ) aVal[ 3 ];

    USHORT nAnzNodes = 65000U - pExcGlob->pD->GetNodes().Count();

    if( nMaxRow < nMinRow )
    {
        USHORT nTemp = nMinRow;
        nMinRow = nMaxRow;
        nMaxRow = nTemp;
    }

    if( nMaxCol < nMinCol )
    {
        USHORT nTemp = nMinCol;
        nMinCol = nMaxCol;
        nMaxCol = nTemp;
    }

    if( nMaxRow - nMinRow == 0 )
        nMaxRow = nMinRow + 30;     // Default bei 0 Rows

    if( nMaxCol - nMinCol == 0 )
        nMaxCol = nMinCol + 15;     // Default bei 0 Cols

    if( nAnzNodes < ( nMaxRow - nMinRow ) * ( nMaxCol - nMinCol ) * 3 )
        return ERR_EXCLOT_WRONG_RANGE;

    pExcGlob->SetRange( nMinCol, nMaxCol, nMinRow, nMaxRow ); // Default-Einstellung
    Parse();
    return 0;
}



ULONG ExcelReader::Read( SwDoc &rDoc, SwPaM &rPam,
                    const String & /* FileName, falls benoetigt wird */ )
{
    if( rPam.GetNode()->FindTableNode() )
        return ERR_SWG_READ_ERROR;

    USHORT nOldBuffSize = 32768;

    ULONG nRet = 0;
    SvStorageStreamRef refStrm;         // damit uns keiner den Stream klaut
    SvStream* pIn = pStrm;
    if( pStg )
    {
        nRet = OpenMainStream( refStrm, nOldBuffSize );
        pIn = &refStrm;
    }
    else if( !pStrm )
    {
        OSL_ENSURE( FALSE, "ExcelReader-Read ohne Stream/Storage" );
        nRet = ERR_SWG_READ_ERROR;
    }

    if( !nRet )
    {
        SwExcelParser* pParser = new SwExcelParser( rDoc, rPam, *pIn, !bInsertMode, osl_getThreadTextEncoding() );
        nRet = pParser->CallParser();   // 0 == kein Fehler aufgetreten

        delete pParser;

        if( refStrm.Is() )
            refStrm->SetBufferSize( nOldBuffSize );
    }

    return nRet;
}

int ExcelReader::GetReaderType()
{
    return SW_STORAGE_READER | SW_STREAM_READER;
}



}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
