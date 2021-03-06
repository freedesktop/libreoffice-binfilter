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

#ifndef _FORBIDDENCHARACTERSTABLE_HXX
#define _FORBIDDENCHARACTERSTABLE_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_tools/table.hxx>

#include <salhelper/simplereferenceobject.hxx>

#include <com/sun/star/uno/Reference.hxx>

#include <com/sun/star/i18n/ForbiddenCharacters.hpp>
namespace com {
namespace sun {
namespace star {
namespace lang {
    class XMultiServiceFactory;
}}}}
namespace binfilter {


struct ForbiddenCharactersInfo
{
    ::com::sun::star::i18n::ForbiddenCharacters aForbiddenChars;
    BOOL bTemporary;
};

DECLARE_TABLE( SvxForbiddenCharactersTableImpl, ForbiddenCharactersInfo* )

class SvxForbiddenCharactersTable : public SvxForbiddenCharactersTableImpl, public salhelper::SimpleReferenceObject
{
private:
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxMSF;

public:
            SvxForbiddenCharactersTable( ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xMSF, USHORT nISize = 4, USHORT nGrow = 4 );
            ~SvxForbiddenCharactersTable();

    const ::com::sun::star::i18n::ForbiddenCharacters* GetForbiddenCharacters( USHORT nLanuage, BOOL bGetDefault ) const;
    void    SetForbiddenCharacters(  USHORT nLanuage , const ::com::sun::star::i18n::ForbiddenCharacters& );
};

}//end of namespace binfilter
#endif // _FORBIDDENCHARACTERSTABLE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
