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
#ifndef _SVTOOLS_CTYPEITM_HXX
#define _SVTOOLS_CTYPEITM_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_svtools/inettype.hxx>
#include <bf_svtools/custritm.hxx>

namespace binfilter {

//=========================================================================

class CntContentTypeItem : public CntUnencodedStringItem
{
private:
    INetContentType _eType;
    XubString       _aPresentation;

public:
    TYPEINFO();

    CntContentTypeItem();
    CntContentTypeItem( USHORT nWhich, const XubString& rType );
    CntContentTypeItem( const CntContentTypeItem& rOrig );

    virtual SfxPoolItem* Create( SvStream& rStream,
                                 USHORT nItemVersion ) const;
    virtual SvStream & Store(SvStream & rStream, USHORT) const { return rStream; }

    virtual int          operator==( const SfxPoolItem& rOrig ) const;

    virtual USHORT GetVersion(USHORT) const;

    virtual SfxPoolItem* Clone( SfxItemPool *pPool = NULL ) const;

    void SetValue( const XubString& rNewVal );

    using SfxPoolItem::Compare;
    virtual int Compare( const SfxPoolItem &rWith, const ::IntlWrapper& rIntlWrapper ) const;

    virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
                                                 SfxMapUnit eCoreMetric,
                                                 SfxMapUnit ePresMetric,
                                                 XubString &rText,
                                                 const ::IntlWrapper* pIntlWrapper = 0 ) const;

    virtual bool QueryValue( ::com::sun::star::uno::Any& rVal,
                             BYTE nMemberId = 0 ) const;
    virtual bool PutValue  ( const ::com::sun::star::uno::Any& rVal,
                             BYTE nMemberId = 0);

    INetContentType GetEnumValue() const;

    void                  SetValue( const INetContentType eType );
};

}

#endif /* !_SVTOOLS_CTYPEITM_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
