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

#ifndef _XMLOFF_XMLCNITM_HXX
#define _XMLOFF_XMLCNITM_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_svtools/poolitem.hxx>
namespace rtl { class OUString; }
namespace binfilter {

class SvXMLNamespaceMap;

//============================================================================

class SvXMLAttrContainerItem_Impl;

class SvXMLAttrContainerItem: public SfxPoolItem
{
    SvXMLAttrContainerItem_Impl *pImpl;

public:
    TYPEINFO();

    SvXMLAttrContainerItem( sal_uInt16 nWhich = 0 );
    SvXMLAttrContainerItem( const SvXMLAttrContainerItem& );
    virtual ~SvXMLAttrContainerItem();

    virtual int operator==( const SfxPoolItem& ) const;
    using SfxPoolItem::Compare;
    virtual int Compare( const SfxPoolItem &rWith ) const;

    virtual SfxItemPresentation GetPresentation(
                                SfxItemPresentation ePresentation,
                                SfxMapUnit eCoreMetric,
                                SfxMapUnit ePresentationMetric,
                                XubString &rText,
                                const ::IntlWrapper *pIntlWrapper = 0 ) const;

    virtual sal_uInt16 GetVersion( sal_uInt16 nFileFormatVersion ) const;

    virtual bool QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual bool PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

    virtual SfxPoolItem *Clone( SfxItemPool * = 0) const
    { return new SvXMLAttrContainerItem( *this ); }

    sal_Bool AddAttr( const ::rtl::OUString& rLName,
                          const ::rtl::OUString& rValue );
    sal_Bool AddAttr( const ::rtl::OUString& rPrefix,
                          const ::rtl::OUString& rNamespace,
                          const ::rtl::OUString& rLName,
                          const ::rtl::OUString& rValue );

    sal_uInt16 GetAttrCount() const;
    ::rtl::OUString GetAttrNamespace( sal_uInt16 i ) const;
    ::rtl::OUString GetAttrPrefix( sal_uInt16 i ) const;
    const ::rtl::OUString& GetAttrLName( sal_uInt16 i ) const;
    const ::rtl::OUString& GetAttrValue( sal_uInt16 i ) const;

    sal_uInt16 GetFirstNamespaceIndex() const; // { return 0; } DBG_BF_ASSERT
    sal_uInt16 GetNextNamespaceIndex( sal_uInt16 nIdx ) const;  // { return 0; } DBG_BF_ASSERT
    const ::rtl::OUString& GetNamespace( sal_uInt16 i ) const;  // DBG_BF_ASSERT
    const ::rtl::OUString& GetPrefix( sal_uInt16 i ) const;     // DBG_BF_ASSERT
};

}//end of namespace binfilter
#endif //  _XMLOFF_XMLCNITM_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
