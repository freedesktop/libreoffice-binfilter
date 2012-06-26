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

#include <tools/debug.hxx>
#include <bf_tools/string.hxx>
#include <tools/stream.hxx>

#include <tools/vcompat.hxx>

#include <bf_svtools/cntwall.hxx>

#define CNTWALLPAPERITEM_STREAM_MAGIC   ( (UINT32)0xfefefefe )
#define CNTWALLPAPERITEM_STREAM_SEEKREL (-( (long)( sizeof( UINT32 ) ) ) )

namespace binfilter
{

TYPEINIT1( CntWallpaperItem, SfxPoolItem );

// -----------------------------------------------------------------------
CntWallpaperItem::CntWallpaperItem( USHORT which, SvStream& rStream, USHORT nVersion )
    : SfxPoolItem( which ), _nColor( COL_TRANSPARENT ), _nStyle( 0 )
{
    UINT32 nMagic = 0;
    rStream >> nMagic;
    if ( nMagic == CNTWALLPAPERITEM_STREAM_MAGIC )
    {
        // Okay, data were stored by CntWallpaperItem.

        readUnicodeString(rStream, _aURL, nVersion >= 1);
        // !!! Color stream operators do not work - they discard any
        // transparency info !!!
        _nColor.Read( rStream, TRUE );
        rStream >> _nStyle;
    }
    else
    {
        rStream.SeekRel( CNTWALLPAPERITEM_STREAM_SEEKREL );

        // Data were stored by SfxWallpaperItem ( SO < 6.0 ). The only
        // thing we can do here is to get the URL and to position the stream.

        {
            // "Read" Wallpaper member - The version compat object positions
            // the stream after the wallpaper data in its dtor. We must use
            // this trick here as no VCL must be used here ( No Wallpaper
            // object allowed ).
            VersionCompat aCompat( rStream, STREAM_READ );
        }

        // Read SfxWallpaperItem's string member _aURL.
        readUnicodeString(rStream, _aURL, false);

        // "Read" SfxWallpaperItem's string member _aFilter.
        read_lenPrefixed_uInt8s_ToOString<sal_uInt16>(rStream); //dummy
    }
}

// -----------------------------------------------------------------------
CntWallpaperItem::CntWallpaperItem( const CntWallpaperItem& rItem ) :
    SfxPoolItem( rItem ),
    _aURL( rItem._aURL ),
    _nColor( rItem._nColor ),
    _nStyle( rItem._nStyle )
{
}

// -----------------------------------------------------------------------
CntWallpaperItem::~CntWallpaperItem()
{
}

// -----------------------------------------------------------------------
int CntWallpaperItem::operator==( const SfxPoolItem& rItem ) const
{
    DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );

    const CntWallpaperItem& rWallItem = (const CntWallpaperItem&)rItem;

    if( ( rWallItem._nStyle == _nStyle ) &&
        ( rWallItem._nColor == _nColor ) &&
        ( rWallItem._aURL == _aURL ) )
        return TRUE;
    else
        return FALSE;
}

//============================================================================
// virtual
USHORT CntWallpaperItem::GetVersion(USHORT) const
{
    return 1; // because it uses SfxPoolItem::read/writeUnicodeString()
}

// -----------------------------------------------------------------------
SfxPoolItem* CntWallpaperItem::Create( SvStream& rStream, USHORT nVersion) const
{
    return new CntWallpaperItem( Which(), rStream, nVersion );
}

SfxPoolItem* CntWallpaperItem::Clone( SfxItemPool* ) const
{
    return new CntWallpaperItem( *this );
}

//----------------------------------------------------------------------------
// virtual
bool CntWallpaperItem::QueryValue( com::sun::star::uno::Any&,BYTE ) const
{
    OSL_FAIL("Not implemented!");
    return false;
}

//----------------------------------------------------------------------------
// virtual
bool CntWallpaperItem::PutValue( const com::sun::star::uno::Any&,BYTE )
{
    OSL_FAIL("Not implemented!");
    return false;
}


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
