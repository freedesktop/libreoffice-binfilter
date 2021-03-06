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

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "XMLExportDDELinks.hxx"

#include <bf_xmloff/xmlnmspe.hxx>
#include <bf_xmloff/xmluconv.hxx>

#include "xmlexprt.hxx"
#include "unonames.hxx"
#include "document.hxx"

#include <com/sun/star/sheet/XDDELink.hpp>
namespace binfilter {

class ScMatrix;

using namespace ::com::sun::star;
using namespace xmloff::token;

ScXMLExportDDELinks::ScXMLExportDDELinks(ScXMLExport& rTempExport)
    : rExport(rTempExport)
{
}

ScXMLExportDDELinks::~ScXMLExportDDELinks()
{
}

void ScXMLExportDDELinks::WriteCell(const sal_Int32 nRepeat)
{
    if (nRepeat > 1)
    {
        ::rtl::OUStringBuffer sBuffer;
        rExport.GetMM100UnitConverter().convertNumber(sBuffer, nRepeat);
        rExport.AddAttribute(XML_NAMESPACE_TABLE, XML_NUMBER_COLUMNS_REPEATED, sBuffer.makeStringAndClear());
    }
    SvXMLElementExport(rExport, XML_NAMESPACE_TABLE, XML_TABLE_CELL, sal_True, sal_True);
}

void ScXMLExportDDELinks::WriteTable(const sal_Int32 nPos)
{
    sal_Int32 nRowCount;
    sal_Int32 nColCount;
    ScMatrix* pMatrix = NULL;
    sal_uInt16 nuRow, nuCol;
    if (rExport.GetDocument() &&
        rExport.GetDocument()->GetDdeLinkResultDimension(static_cast<USHORT>(nPos), nuCol, nuRow, pMatrix))
    {
        nRowCount = nuRow;
        nColCount = nuCol;
        SvXMLElementExport aTableElem(rExport, XML_NAMESPACE_TABLE, XML_TABLE, sal_True, sal_True);
        ::rtl::OUStringBuffer sBuffer;
        if (nColCount > 1)
        {
            rExport.GetMM100UnitConverter().convertNumber(sBuffer, nColCount);
            rExport.AddAttribute(XML_NAMESPACE_TABLE, XML_NUMBER_COLUMNS_REPEATED, sBuffer.makeStringAndClear());
        }
        {
            SvXMLElementExport aElemCol(rExport, XML_NAMESPACE_TABLE, XML_TABLE_COLUMN, sal_True, sal_True);
        }
        sal_Int32 nRepeatColsCount(1);
        for(sal_Int32 nRow = 0; nRow < nRowCount; nRow++)
        {
            SvXMLElementExport aElemRow(rExport, XML_NAMESPACE_TABLE, XML_TABLE_ROW, sal_True, sal_True);
            for(sal_Int32 nColumn = 0; nColumn < nColCount; nColumn++)
            {
                if (nColumn != 0)
                {
                    nRepeatColsCount++;
                }
            }
            WriteCell(nRepeatColsCount);
            nRepeatColsCount = 1;
        }
    }
}

void ScXMLExportDDELinks::WriteDDELinks(uno::Reference<sheet::XSpreadsheetDocument>& xSpreadDoc)
{
    uno::Reference <beans::XPropertySet> xPropertySet (xSpreadDoc, uno::UNO_QUERY);
    if (xPropertySet.is())
    {
        uno::Any aDDELinks = xPropertySet->getPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNO_DDELINKS)));
        uno::Reference<container::XIndexAccess> xIndex;
        if (aDDELinks >>= xIndex)
        {
            sal_Int32 nCount = xIndex->getCount();
            if (nCount)
            {
                SvXMLElementExport aElemDDEs(rExport, XML_NAMESPACE_TABLE, XML_DDE_LINKS, sal_True, sal_True);
                for (sal_uInt16 nDDELink = 0; nDDELink < nCount; nDDELink++)
                {
                    uno::Any aDDELink = xIndex->getByIndex(nDDELink);
                    uno::Reference<sheet::XDDELink> xDDELink;
                    if (aDDELink >>= xDDELink)
                    {
                        SvXMLElementExport aElemDDE(rExport, XML_NAMESPACE_TABLE, XML_DDE_LINK, sal_True, sal_True);
                        {
                            rExport.AddAttribute(XML_NAMESPACE_OFFICE, XML_DDE_APPLICATION, xDDELink->getApplication());
                            rExport.AddAttribute(XML_NAMESPACE_OFFICE, XML_DDE_TOPIC, xDDELink->getTopic());
                            rExport.AddAttribute(XML_NAMESPACE_OFFICE, XML_DDE_ITEM, xDDELink->getItem());
                            rExport.AddAttribute(XML_NAMESPACE_OFFICE, XML_AUTOMATIC_UPDATE, XML_TRUE);
                            sal_uInt16 nMode;
                            if (rExport.GetDocument() &&
                                rExport.GetDocument()->GetDdeLinkMode(nDDELink, nMode))
                            {
                                switch (nMode)
                                {
                                    case SC_DDE_ENGLISH :
                                        rExport.AddAttribute(XML_NAMESPACE_TABLE, XML_CONVERSION_MODE, XML_INTO_ENGLISH_NUMBER);
                                    case SC_DDE_TEXT :
                                        rExport.AddAttribute(XML_NAMESPACE_TABLE, XML_CONVERSION_MODE, XML_LET_TEXT);
                                }
                            }
                            SvXMLElementExport(rExport, XML_NAMESPACE_OFFICE, XML_DDE_SOURCE, sal_True, sal_True);
                        }
                        WriteTable(nDDELink);
                    }
                }
            }
        }
    }
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
