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

#ifndef _BREAKIT_HXX
#define _BREAKIT_HXX

#include <bf_svtools/bf_solar.h>
#include <i18npool/lang.h>

/*************************************************************************
 *                      class SwBreakIt
 *************************************************************************/

#include <com/sun/star/uno/Reference.h>

#include <com/sun/star/i18n/XBreakIterator.hpp>

#include <com/sun/star/i18n/ForbiddenCharacters.hdl>
class String;
namespace binfilter {



class SwBreakIt
{
public:
    ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XBreakIterator > xBreak;
private:
    ::com::sun::star::lang::Locale* pLocale;
    ::com::sun::star::i18n::ForbiddenCharacters* pForbidden;
    LanguageType aLast;          // language of the current locale
    LanguageType aForbiddenLang; // language of the current forbiddenChar struct

    void _GetLocale( const LanguageType aLang );
    void _GetForbidden( const LanguageType  aLang );
public:
    SwBreakIt();
    ~SwBreakIt() { delete pLocale; delete pForbidden; }
    ::com::sun::star::lang::Locale& GetLocale( const LanguageType aLang )
    {
        if( aLast != aLang )
            _GetLocale( aLang );
        return *pLocale;
    }
    ::com::sun::star::i18n::ForbiddenCharacters& GetForbidden( const LanguageType aLang )
    {
        if( !pForbidden || aForbiddenLang != aLang )
            _GetForbidden( aLang );
        return *pForbidden;
    }

    USHORT GetRealScriptOfText( const String& rTxt, xub_StrLen nPos ) const;
};

extern SwBreakIt* pBreakIt;


} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
