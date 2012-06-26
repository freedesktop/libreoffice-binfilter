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

#ifndef _XMLOFF_XMLRECTANGLEMEMBERSHANDLER_HXX
#define _XMLOFF_XMLRECTANGLEMEMBERSHANDLER_HXX

#include "xmlprhdl.hxx"
namespace binfilter {

struct SvXMLEnumMapEntry;

///////////////////////////////////////////////////////////////////////////
//
/** Abstract base-class for different XML-types. Derivations of this class
    knows how to compare, im/export a special XML-type
*/
class XMLRectangleMembersHdl: public XMLPropertyHandler
{
private:
    sal_Int32 mnType;
public:

    XMLRectangleMembersHdl( sal_Int32 nType );

    // Just needed for virtual destruction
    virtual ~XMLRectangleMembersHdl();

    /// Imports the given value in cas of the given XML-data-type
    virtual sal_Bool importXML(
            const ::rtl::OUString& rStrImpValue,
            ::com::sun::star::uno::Any& rValue,
            const SvXMLUnitConverter& rUnitConverter ) const;

    /// Exports the given value in cas of the given XML-data-type
    virtual sal_Bool exportXML(
            ::rtl::OUString& rStrExpValue,
            const ::com::sun::star::uno::Any& rValue,
            const SvXMLUnitConverter& rUnitConverter ) const;
};

}//end of namespace binfilter
#endif      // _XMLOFF_XMLRECTANGLEMEMBERSHANDLER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
