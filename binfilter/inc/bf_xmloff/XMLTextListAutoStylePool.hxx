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

#ifndef _XMLOFF_XMLTEXTLISTAUTOSTYLEPOOL_HXX
#define _XMLOFF_XMLTEXTLISTAUTOSTYLEPOOL_HXX


#include <com/sun/star/ucb/XAnyCompare.hpp>

namespace com { namespace sun { namespace star { namespace container {
    class XIndexReplace; } } } }
namespace rtl { class OUString; }
namespace binfilter {

class XMLTextListAutoStylePool_Impl;
class XMLTextListAutoStylePoolNames_Impl;
class XMLTextListAutoStylePoolEntry_Impl;
class SvXMLExport;

class XMLTextListAutoStylePool
{
    SvXMLExport& rExport;

    const ::rtl::OUString sPrefix;

    XMLTextListAutoStylePool_Impl *pPool;
    XMLTextListAutoStylePoolNames_Impl *pNames;
    sal_uInt32 nName;

    /** this is an optional NumRule compare component for applications where
        the NumRules don't have names */
    ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XAnyCompare > mxNumRuleCompare;

    sal_uInt32 Find( XMLTextListAutoStylePoolEntry_Impl* pEntry ) const;
public:

    XMLTextListAutoStylePool( SvXMLExport& rExport );
    ~XMLTextListAutoStylePool();

    void RegisterName( const ::rtl::OUString& rName );

    ::rtl::OUString Add(
            const ::com::sun::star::uno::Reference <
                ::com::sun::star::container::XIndexReplace > & rNumRules );

    ::rtl::OUString Find(
            const ::com::sun::star::uno::Reference <
                ::com::sun::star::container::XIndexReplace > & rNumRules ) const;
    ::rtl::OUString Find( const ::rtl::OUString& rInternalName ) const;

    void exportXML() const;
};

}//end of namespace binfilter
#endif  //  _XMLOFF_XMLTEXTLISTAUTOSTYLEPOOL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
