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

#ifndef _SC_XMLEXPORTDDELINKS_HXX
#define _SC_XMLEXPORTDDELINKS_HXX

#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>

namespace binfilter {

class String;
class ScXMLExport;

class ScXMLExportDDELinks
{
    ScXMLExport&        rExport;

    sal_Bool            CellsEqual(const sal_Bool bPrevEmpty, const sal_Bool bPrevString, const String& sPrevValue, const double& fPrevValue,
                                    const sal_Bool bEmpty, const sal_Bool bString, const String& sValue, const double& fValue);
    void                WriteCell(const sal_Bool bEmpty, const sal_Bool bString, const String& sValue, const double& fValue, const sal_Int32 nRepeat);
    void                WriteTable(const sal_Int32 nPos);
public:
    ScXMLExportDDELinks(ScXMLExport& rExport);
    ~ScXMLExportDDELinks();
    void WriteDDELinks(::com::sun::star::uno::Reference < ::com::sun::star::sheet::XSpreadsheetDocument >& xSpreadDoc);
};

} //namespace binfilter
#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
