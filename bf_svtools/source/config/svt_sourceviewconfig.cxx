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

#include <bf_svtools/sourceviewconfig.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <rtl/instance.hxx>
#include <bf_svtools/smplhint.hxx>

#include <itemholder1.hxx>

using namespace utl;
using namespace com::sun::star::uno;

using ::rtl::OUString;

namespace binfilter
{

class SourceViewConfig_Impl : public utl::ConfigItem, public SfxBroadcaster
{
private:
    OUString        m_sFontName;
    sal_Int16       m_nFontHeight;
    sal_Bool        m_bProportionalFontOnly;

    void        Load();

    static Sequence< OUString > GetPropertyNames();

public:
    SourceViewConfig_Impl();
    ~SourceViewConfig_Impl();

    virtual void    Notify( const Sequence< rtl::OUString >& aPropertyNames );
    virtual void    Commit();

    const rtl::OUString&    GetFontName() const
                                {return m_sFontName;}
    void                    SetFontName(const rtl::OUString& rName)
                                {
                                    if(rName != m_sFontName)
                                    {
                                        m_sFontName = rName;
                                        SetModified();
                                    }
                                }

    sal_Int16               GetFontHeight() const
                                {return m_nFontHeight;}
    void                    SetFontHeight(sal_Int16 nHeight)
                                {
                                    if(m_nFontHeight != nHeight)
                                    {
                                        m_nFontHeight = nHeight;
                                        SetModified();
                                    }
                                }

    sal_Bool                IsShowProportionalFontsOnly() const
                                {return m_bProportionalFontOnly;}
    void                    SetShowProportionalFontsOnly(sal_Bool bSet)
                                {
                                    if(m_bProportionalFontOnly != bSet)
                                    {
                                        m_bProportionalFontOnly = bSet;
                                        SetModified();
                                    }
                                }
};
// initialization of static members --------------------------------------
SourceViewConfig_Impl* SourceViewConfig::m_pImplConfig = 0;
sal_Int32              SourceViewConfig::m_nRefCount = 0;
namespace { struct lclMutex : public rtl::Static< ::osl::Mutex, lclMutex > {}; }

SourceViewConfig_Impl::SourceViewConfig_Impl() :
    ConfigItem(OUString( RTL_CONSTASCII_USTRINGPARAM( "Office.Common/Font/SourceViewFont" ))),
    m_nFontHeight(12),
    m_bProportionalFontOnly(sal_False)
{
    Load();
}

SourceViewConfig_Impl::~SourceViewConfig_Impl()
{
}

Sequence< OUString > SourceViewConfig_Impl::GetPropertyNames()
{
    //this list needs exactly to mach the enum PropertyNameIndex
    static const char* aPropNames[] =
    {
        "FontName"                  // 0
        ,"FontHeight"               // 1
        ,"NonProportionalFontsOnly" // 2
    };
    const int nCount = sizeof( aPropNames ) / sizeof( const char* );
    Sequence< OUString > aNames( nCount );
    OUString* pNames = aNames.getArray();
    for ( int i = 0; i < nCount; i++ )
        pNames[i] = OUString::createFromAscii( aPropNames[i] );

    return aNames;
}


void SourceViewConfig_Impl::Load()
{
    Sequence< OUString > aNames = GetPropertyNames();
    Sequence< Any > aValues = GetProperties( aNames );
    EnableNotification( aNames );
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT( aValues.getLength() == aNames.getLength(), "GetProperties failed" );
    if ( aValues.getLength() == aNames.getLength() )
    {
        for ( int nProp = 0; nProp < aNames.getLength(); nProp++ )
        {
            if ( pValues[nProp].hasValue() )
            {
                switch( nProp )
                {
                    case 0:  pValues[nProp] >>= m_sFontName;         break;
                    case 1:  pValues[nProp] >>= m_nFontHeight;      break;
                    case 2:  pValues[nProp] >>= m_bProportionalFontOnly;     break;
                }
            }
        }
    }
}

void SourceViewConfig_Impl::Notify( const Sequence< OUString >& )
{
    Load();
}

void SourceViewConfig_Impl::Commit()
{
    ClearModified();
    Sequence< OUString > aNames = GetPropertyNames();
    Sequence< Any > aValues( aNames.getLength() );
    Any* pValues = aValues.getArray();
    for ( int nProp = 0; nProp < aNames.getLength(); nProp++ )
    {
        switch( nProp )
        {
            case 0:  pValues[nProp] <<= m_sFontName;         break;
            case 1:  pValues[nProp] <<= m_nFontHeight;      break;
            case 2:  pValues[nProp] <<= m_bProportionalFontOnly;     break;
            default:
                DBG_ERRORFILE( "invalid index to save a user token" );
        }
    }
    PutProperties( aNames, aValues );

    //notify SfxListener
    {
        SfxSimpleHint aHint = SfxSimpleHint( SFX_HINT_DATACHANGED );
        Broadcast(aHint);
    }
}

SourceViewConfig::SourceViewConfig()
{
    {
        ::osl::MutexGuard aGuard( lclMutex::get() );
        if(!m_pImplConfig)
        {
            m_pImplConfig = new SourceViewConfig_Impl;
            ItemHolder1::holdConfigItem(E_SOURCEVIEWCONFIG);
        }

         ++m_nRefCount;
    }
    StartListening( *m_pImplConfig, TRUE );
}

SourceViewConfig::~SourceViewConfig()
{
    EndListening( *m_pImplConfig, TRUE );
    ::osl::MutexGuard aGuard( lclMutex::get() );
    if( !--m_nRefCount )
    {
        if( m_pImplConfig->IsModified() )
            m_pImplConfig->Commit();
        DELETEZ( m_pImplConfig );
    }
}

void SourceViewConfig::Notify( SfxBroadcaster&, const SfxHint& rHint )
{
    Broadcast( rHint );
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
