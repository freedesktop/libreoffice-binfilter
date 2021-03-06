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


#include "xmlrowi.hxx"
#include "xmlcelli.hxx"
#include "xmlstyli.hxx"
#include "document.hxx"
#include "docuno.hxx"
#include "olinetab.hxx"

#include <bf_xmloff/nmspmap.hxx>
#include <bf_xmloff/xmlnmspe.hxx>

#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/sheet/XPrintAreas.hpp>

namespace binfilter {

#define SC_ISVISIBLE "IsVisible"
#define SC_OPTIMALHEIGHT "OptimalHeight"
#define SC_ISFILTERED "IsFiltered"

using namespace ::com::sun::star;
using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLTableRowContext::ScXMLTableRowContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList ) :
    SvXMLImportContext( rInImport, nPrfx, rLName ),
    sVisibility(GetXMLToken(XML_VISIBLE)),
    nRepeatedRows(1),
    bHasCell(sal_False)
{
    ::rtl::OUString sCellStyleName;
    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetTableRowAttrTokenMap();
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        ::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
        ::rtl::OUString aLclLocalName;
        sal_uInt16 nLclPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                            sAttrName, &aLclLocalName );
        ::rtl::OUString sValue = xAttrList->getValueByIndex( i );

        switch( rAttrTokenMap.Get( nLclPrefix, aLclLocalName ) )
        {
            case XML_TOK_TABLE_ROW_ATTR_STYLE_NAME:
            {
                sStyleName = sValue;
            }
            break;
            case XML_TOK_TABLE_ROW_ATTR_VISIBILITY:
            {
                sVisibility = sValue;
            }
            break;
            case XML_TOK_TABLE_ROW_ATTR_REPEATED:
            {
                nRepeatedRows = sValue.toInt32();
            }
            break;
            case XML_TOK_TABLE_ROW_ATTR_DEFAULT_CELL_STYLE_NAME:
            {
                sCellStyleName = sValue;
            }
            break;
            /*case XML_TOK_TABLE_ROW_ATTR_USE_OPTIMAL_HEIGHT:
            {
                sOptimalHeight = sValue;
            }
            break;*/
        }
    }
    GetScImport().GetTables().AddRow();
    GetScImport().GetTables().SetRowStyle(sCellStyleName);
}

ScXMLTableRowContext::~ScXMLTableRowContext()
{
}

SvXMLImportContext *ScXMLTableRowContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetTableRowElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
    case XML_TOK_TABLE_ROW_CELL:
        {
            bHasCell = sal_True;
            pContext = new ScXMLTableRowCellContext( GetScImport(), nInPrefix,
                                                      rLName, xAttrList, sal_False, nRepeatedRows
                                                      //this
                                                      );
        }
        break;
    case XML_TOK_TABLE_ROW_COVERED_CELL:
        {
            bHasCell = sal_True;
            pContext = new ScXMLTableRowCellContext( GetScImport(), nInPrefix,
                                                      rLName, xAttrList, sal_True, nRepeatedRows
                                                      //this
                                                      );
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLTableRowContext::EndElement()
{
    ScXMLImport& rXMLImport = GetScImport();
    if (!bHasCell && nRepeatedRows > 1)
    {
        for (sal_Int32 i = 0; i < nRepeatedRows - 1; ++i) //one row is always added
            GetScImport().GetTables().AddRow();
        DBG_ERRORFILE("it seems here is a nonvalid file; possible missing of table:table-cell element");
    }
    sal_Int32 nCurrentRow = rXMLImport.GetTables().GetCurrentRow();
    uno::Reference<sheet::XSpreadsheet> xSheet = rXMLImport.GetTables().GetCurrentXSheet();
    if(xSheet.is())
    {
        sal_Int32 nFirstRow(nCurrentRow - nRepeatedRows + 1);
        if (nFirstRow > MAXROW)
            nFirstRow = MAXROW;
        if (nCurrentRow > MAXROW)
            nCurrentRow = MAXROW;
        uno::Reference <table::XCellRange> xCellRange = xSheet->getCellRangeByPosition(0, nFirstRow, 0, nCurrentRow);
        if (xCellRange.is())
        {
            uno::Reference<table::XColumnRowRange> xColumnRowRange (xCellRange, uno::UNO_QUERY);
            if (xColumnRowRange.is())
            {
                uno::Reference<table::XTableRows> xTableRows = xColumnRowRange->getRows();
                if (xTableRows.is())
                {
                    uno::Reference <beans::XPropertySet> xRowProperties(xTableRows, uno::UNO_QUERY);
                    if (xRowProperties.is())
                    {
                        if (sStyleName.getLength())
                        {
                            XMLTableStylesContext *pStyles = (XMLTableStylesContext *)rXMLImport.GetAutoStyles();
                            XMLTableStyleContext* pStyle = (XMLTableStyleContext *)pStyles->FindStyleChildContext(
                                XML_STYLE_FAMILY_TABLE_ROW, sStyleName, sal_True);
                            if (pStyle)
                                pStyle->FillPropertySet(xRowProperties);
                        }
                        uno::Any aVisibleAny;
                        uno::Any aFilteredAny;
                        sal_Bool bVisible (sal_True);
                        sal_Bool bFiltered (sal_False);
                        if (IsXMLToken(sVisibility, XML_COLLAPSE))
                        {
                            bVisible = sal_False;
                            aVisibleAny <<= bVisible;
                            aFilteredAny <<= bFiltered;
                        }
                        else if (IsXMLToken(sVisibility, XML_FILTER))
                        {
                            bVisible = sal_False;
                            aVisibleAny <<= bVisible;
                            bFiltered = sal_True;
                            aFilteredAny <<= bFiltered;
                        }
                        if (!bVisible)
                            xRowProperties->setPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ISVISIBLE)), aVisibleAny);
                        if (bFiltered)
                            xRowProperties->setPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_ISFILTERED)), aFilteredAny);
                    }
                }
            }
        }
    }
}

ScXMLTableRowsContext::ScXMLTableRowsContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                      const sal_Bool bTempHeader, const sal_Bool bTempGroup ) :
    SvXMLImportContext( rInImport, nPrfx, rLName ),
    nHeaderStartRow(0),
    nHeaderEndRow(0),
    nGroupStartRow(0),
    nGroupEndRow(0),
    bHeader(bTempHeader),
    bGroup(bTempGroup),
    bGroupDisplay(sal_True)
{
    // don't have any attributes
    if (bHeader)
    {
        nHeaderStartRow = rInImport.GetTables().GetCurrentRow();
        nHeaderStartRow++;
    }
    else if (bGroup)
    {
        nGroupStartRow = rInImport.GetTables().GetCurrentRow();
        nGroupStartRow++;
        sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
        for( sal_Int16 i=0; i < nAttrCount; i++ )
        {
            ::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
            ::rtl::OUString aLclLocalName;
            /*sal_uInt16 nLclPrefix =*/ GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                                sAttrName, &aLclLocalName );
            ::rtl::OUString sValue = xAttrList->getValueByIndex( i );

            if ((nPrfx == XML_NAMESPACE_TABLE) && IsXMLToken(aLclLocalName, XML_DISPLAY))
                bGroupDisplay = IsXMLToken(sValue, XML_TRUE);
        }
    }
}

ScXMLTableRowsContext::~ScXMLTableRowsContext()
{
}

SvXMLImportContext *ScXMLTableRowsContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetTableRowsElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
    case XML_TOK_TABLE_ROWS_ROW_GROUP:
        pContext = new ScXMLTableRowsContext( GetScImport(), nInPrefix,
                                                   rLName, xAttrList,
                                                   sal_False, sal_True );
        break;
    case XML_TOK_TABLE_ROWS_HEADER_ROWS:
        pContext = new ScXMLTableRowsContext( GetScImport(), nInPrefix,
                                                   rLName, xAttrList,
                                                   sal_True, sal_False );
        break;
    case XML_TOK_TABLE_ROWS_ROWS:
        pContext = new ScXMLTableRowsContext( GetScImport(), nInPrefix,
                                                   rLName, xAttrList,
                                                   sal_False, sal_False );
        break;
    case XML_TOK_TABLE_ROWS_ROW:
            pContext = new ScXMLTableRowContext( GetScImport(), nInPrefix,
                                                      rLName, xAttrList//,
                                                      //this
                                                      );
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLTableRowsContext::EndElement()
{
    ScXMLImport& rXMLImport = GetScImport();
    if (bHeader)
    {
        nHeaderEndRow = rXMLImport.GetTables().GetCurrentRow();
        if (nHeaderStartRow <= nHeaderEndRow)
        {
            uno::Reference<sheet::XSpreadsheet> xSheet = rXMLImport.GetTables().GetCurrentXSheet();
            if(xSheet.is())
            {
                uno::Reference <sheet::XPrintAreas> xPrintAreas (xSheet, uno::UNO_QUERY);
                if (xPrintAreas.is())
                {
                    if (!xPrintAreas->getPrintTitleRows())
                    {
                        xPrintAreas->setPrintTitleRows(sal_True);
                        table::CellRangeAddress aRowHeaderRange;
                        aRowHeaderRange.StartRow = nHeaderStartRow;
                        aRowHeaderRange.EndRow = nHeaderEndRow;
                        xPrintAreas->setTitleRows(aRowHeaderRange);
                    }
                    else
                    {
                        table::CellRangeAddress aRowHeaderRange = xPrintAreas->getTitleRows();
                        aRowHeaderRange.EndRow = nHeaderEndRow;
                        xPrintAreas->setTitleRows(aRowHeaderRange);
                    }
                }
            }
        }
    }
    else if (bGroup)
    {
        nGroupEndRow = rXMLImport.GetTables().GetCurrentRow();
        sal_Int16 nSheet = rXMLImport.GetTables().GetCurrentSheet();
        if (nGroupStartRow <= nGroupEndRow)
        {
            ScDocument* pDoc = GetScImport().GetDocument();
            if (pDoc)
            {
                GetScImport().LockSolarMutex();
                ScOutlineTable* pOutlineTable = pDoc->GetOutlineTable(nSheet, sal_True);
                ScOutlineArray* pRowArray = pOutlineTable->GetRowArray();
                sal_Bool bResized;
                pRowArray->Insert(static_cast<USHORT>(nGroupStartRow), static_cast<USHORT>(nGroupEndRow), bResized, !bGroupDisplay, sal_True);
                GetScImport().UnlockSolarMutex();
            }
        }
    }
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
