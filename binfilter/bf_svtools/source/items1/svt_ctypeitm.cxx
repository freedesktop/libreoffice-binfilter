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

#include <com/sun/star/uno/Any.hxx>

#include <unotools/intlwrapper.hxx>

#include <tools/stream.hxx>
#include <bf_svtools/ctypeitm.hxx>

namespace binfilter
{

//============================================================================
// The following defines are copied from chaos/source/items/cstritem.cxx:
#define CNTSTRINGITEM_STREAM_MAGIC   ( (UINT32)0xfefefefe )
#define CNTSTRINGITEM_STREAM_SEEKREL (-( (long)( sizeof( UINT32 ) ) ) )

//============================================================================
//
// class CntContentTypeItem Implementation.
//
//============================================================================

TYPEINIT1_AUTOFACTORY( CntContentTypeItem, CntUnencodedStringItem );

#define CONTENT_TYPE_NOT_INIT ( (INetContentType)-1 )

//----------------------------------------------------------------------------
CntContentTypeItem::CntContentTypeItem()
: CntUnencodedStringItem(),
  _eType( CONTENT_TYPE_NOT_INIT )
{
}

//----------------------------------------------------------------------------
CntContentTypeItem::CntContentTypeItem( USHORT which, const XubString& rType )
: CntUnencodedStringItem( which, rType ),
  _eType( CONTENT_TYPE_NOT_INIT )
{
}

//----------------------------------------------------------------------------
CntContentTypeItem::CntContentTypeItem( const CntContentTypeItem& rOrig )
: CntUnencodedStringItem( rOrig ),
  _eType( rOrig._eType ),
  _aPresentation( rOrig._aPresentation )
{
}

//============================================================================
// virtual
USHORT CntContentTypeItem::GetVersion(USHORT) const
{
    return 1; // because it uses SfxPoolItem::read/writeUnicodeString()
}

//----------------------------------------------------------------------------
// virtual
SfxPoolItem* CntContentTypeItem::Create( SvStream& rStream,
                                         USHORT nItemVersion ) const
{
    // CntContentTypeItem used to be derived from CntStringItem, so take that
    // into account:
    UniString aValue;
    readUnicodeString(rStream, aValue, nItemVersion >= 1);
    UINT32 nMagic = 0;
    rStream >> nMagic;
    if (nMagic == CNTSTRINGITEM_STREAM_MAGIC)
    {
        BOOL bEncrypted = FALSE;
        rStream >> bEncrypted;
        DBG_ASSERT(!bEncrypted,
                   "CntContentTypeItem::Create() reads encrypted data");
    }
    else
        rStream.SeekRel(CNTSTRINGITEM_STREAM_SEEKREL);

    return new CntContentTypeItem(Which(), aValue);
}

//----------------------------------------------------------------------------
// virtual
int CntContentTypeItem::operator==( const SfxPoolItem& rOrig ) const
{
    const CntContentTypeItem& rOther = (const CntContentTypeItem&)rOrig;

    if ( ( _eType != CONTENT_TYPE_NOT_INIT ) &&
         ( rOther._eType != CONTENT_TYPE_NOT_INIT ) )
        return _eType == rOther._eType;
    else
        return CntUnencodedStringItem::operator==( rOther );
}

//----------------------------------------------------------------------------
// virtual
SfxPoolItem* CntContentTypeItem::Clone( SfxItemPool* /* pPool */ ) const
{
    return new CntContentTypeItem( *this );
}

//----------------------------------------------------------------------------
void CntContentTypeItem::SetValue( const XubString& rNewVal )
{
    // De-initialize enum type and presentation.
    _eType = CONTENT_TYPE_NOT_INIT;
    _aPresentation.Erase();

    CntUnencodedStringItem::SetValue( rNewVal );
}

//----------------------------------------------------------------------------
int CntContentTypeItem::Compare( const SfxPoolItem &rWith, const ::IntlWrapper& rIntlWrapper ) const
{
    String aOwnText, aWithText;
    GetPresentation( SFX_ITEM_PRESENTATION_NAMELESS,
                     SFX_MAPUNIT_APPFONT, SFX_MAPUNIT_APPFONT, aOwnText, &rIntlWrapper );
    rWith.GetPresentation( SFX_ITEM_PRESENTATION_NAMELESS,
                           SFX_MAPUNIT_APPFONT, SFX_MAPUNIT_APPFONT, aWithText, &rIntlWrapper );
    return rIntlWrapper.getCollator()->compareString( aOwnText, aWithText );
}

//----------------------------------------------------------------------------
SfxItemPresentation CntContentTypeItem::GetPresentation(
    SfxItemPresentation ePres,
    SfxMapUnit          eCoreMetric,
    SfxMapUnit          ePresMetric,
    XubString         & rText,
    const ::IntlWrapper * pIntlWrapper) const
{
    if (_aPresentation.Len() == 0)
    {
        DBG_ASSERT(pIntlWrapper,
                   "CntContentTypeItem::GetPresentation(): No IntlWrapper");
        if (pIntlWrapper)
            (const_cast< CntContentTypeItem * >(this))->_aPresentation
             = INetContentTypes::GetPresentation(GetEnumValue(),
                                                 pIntlWrapper->
                                                  getLocale());
    }
    if (_aPresentation.Len() > 0)
    {
        rText = _aPresentation;
        return SFX_ITEM_PRESENTATION_COMPLETE;
    }
    else
        return CntUnencodedStringItem::GetPresentation(ePres, eCoreMetric,
                                                       ePresMetric, rText,
                                                       pIntlWrapper);
}

//----------------------------------------------------------------------------
INetContentType CntContentTypeItem::GetEnumValue() const
{
    if ( _eType == CONTENT_TYPE_NOT_INIT )
    {
        // Not yet initialized... Get enum value for string content type.

        CntContentTypeItem* pVarThis = (const_cast< CntContentTypeItem* >(this));

        pVarThis->_eType = INetContentTypes::GetContentType( GetValue() );
    }

    return _eType;
}

//----------------------------------------------------------------------------
void CntContentTypeItem::SetValue( const INetContentType eType )
{
    SetValue( INetContentTypes::GetContentType( eType ) );

    // Note: SetValue( const String& ....) resets _eType. Set new enum value
    //       after(!) calling it.
    _eType = eType;
}

//----------------------------------------------------------------------------
// virtual
bool CntContentTypeItem::QueryValue( com::sun::star::uno::Any& rVal,BYTE ) const
{
    rVal <<= rtl::OUString(GetValue());
    return true;
}

//----------------------------------------------------------------------------
// virtual
bool CntContentTypeItem::PutValue( const com::sun::star::uno::Any& rVal,BYTE )
{
    rtl::OUString aValue;
    if ( rVal >>= aValue )
    {
        // SetValue with an empty string resets the item; so call that
        // function when PutValue is called with an empty string
        if (aValue.getLength() == 0)
            SetValue(aValue);
        else
            SetValue(
                INetContentTypes::RegisterContentType(aValue, UniString()));
        return true;
    }

    OSL_FAIL( "CntContentTypeItem::PutValue - Wrong type!" );
    return false;
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
