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


#include "xmlfilti.hxx"
#include "xmlimprt.hxx"

#include "convuno.hxx"
#include "XMLConverter.hxx"

#include <bf_xmloff/nmspmap.hxx>
namespace binfilter {

using namespace ::com::sun::star;
using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLFilterContext::ScXMLFilterContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDatabaseRangeContext* pTempDatabaseRangeContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName ),
    aFilterFields(),
    bSkipDuplicates(sal_False),
    bCopyOutputData(sal_False),
    bUseRegularExpressions(sal_False),
    bConnectionOr(sal_True),
    bNextConnectionOr(sal_True),
    bConditionSourceRange(sal_False)
{
    ScDocument* pDoc = GetScImport().GetDocument();
    pDatabaseRangeContext = pTempDatabaseRangeContext;

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetFilterAttrTokenMap();
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        ::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
        ::rtl::OUString aLclLocalName;
        USHORT nLclPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                            sAttrName, &aLclLocalName );
        ::rtl::OUString sValue = xAttrList->getValueByIndex( i );

        switch( rAttrTokenMap.Get( nLclPrefix, aLclLocalName ) )
        {
            case XML_TOK_FILTER_ATTR_TARGET_RANGE_ADDRESS :
            {
                ScRange aScRange;
                sal_Int32 nOffset(0);
                if (ScXMLConverter::GetRangeFromString( aScRange, sValue, pDoc, nOffset ))
                {
                    ScUnoConversion::FillApiAddress( aOutputPosition, aScRange.aStart );
                    bCopyOutputData = sal_True;
                }
            }
            break;
            case XML_TOK_FILTER_ATTR_CONDITION_SOURCE_RANGE_ADDRESS :
            {
                sal_Int32 nOffset(0);
                if (ScXMLConverter::GetRangeFromString( aConditionSourceRangeAddress, sValue, pDoc, nOffset ))
                    bConditionSourceRange = sal_True;
            }
            break;
            case XML_TOK_FILTER_ATTR_CONDITION_SOURCE :
            {
                // not supported by StarOffice
            }
            break;
            case XML_TOK_FILTER_ATTR_DISPLAY_DUPLICATES :
            {
                bSkipDuplicates = !IsXMLToken(sValue, XML_TRUE);
            }
            break;
        }
    }
}

ScXMLFilterContext::~ScXMLFilterContext()
{
}

SvXMLImportContext *ScXMLFilterContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetFilterElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
        case XML_TOK_FILTER_AND:
        {
            pContext = new ScXMLAndContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, this);
        }
        break;
        case XML_TOK_FILTER_OR:
        {
            pContext = new ScXMLOrContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, this);
        }
        break;
        case XML_TOK_FILTER_CONDITION:
        {
            pContext = new ScXMLConditionContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, this);
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLFilterContext::EndElement()
{
    pDatabaseRangeContext->SetFilterUseRegularExpressions(bUseRegularExpressions);
    if (bCopyOutputData)
    {
        pDatabaseRangeContext->SetFilterOutputPosition(aOutputPosition);
        pDatabaseRangeContext->SetFilterCopyOutputData(bCopyOutputData);
    }
    else
        pDatabaseRangeContext->SetFilterCopyOutputData(sal_False);
    pDatabaseRangeContext->SetFilterIsCaseSensitive(bIsCaseSensitive);
    pDatabaseRangeContext->SetFilterSkipDuplicates(bSkipDuplicates);
    pDatabaseRangeContext->SetFilterFields(aFilterFields);
    if (bConditionSourceRange)
        pDatabaseRangeContext->SetFilterConditionSourceRangeAddress(aConditionSourceRangeAddress);
}

ScXMLAndContext::ScXMLAndContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& /*xAttrList*/,
                                        ScXMLFilterContext* pTempFilterContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName )
{
    pFilterContext = pTempFilterContext;
    pFilterContext->OpenConnection(sal_False);
}

ScXMLAndContext::~ScXMLAndContext()
{
}

SvXMLImportContext *ScXMLAndContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetFilterElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
        case XML_TOK_FILTER_OR:
        {
            // not supported in StarOffice
        }
        break;
        case XML_TOK_FILTER_CONDITION:
        {
            pContext = new ScXMLConditionContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, pFilterContext);
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLAndContext::EndElement()
{
    pFilterContext->CloseConnection();
}

ScXMLOrContext::ScXMLOrContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& /*xAttrList*/,
                                        ScXMLFilterContext* pTempFilterContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName )
{
    pFilterContext = pTempFilterContext;
    pFilterContext->OpenConnection(sal_True);
}

ScXMLOrContext::~ScXMLOrContext()
{
}

SvXMLImportContext *ScXMLOrContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetFilterElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
        case XML_TOK_FILTER_AND:
        {
            pContext = new ScXMLAndContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, pFilterContext);
        }
        break;
        case XML_TOK_FILTER_CONDITION:
        {
            pContext = new ScXMLConditionContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, pFilterContext);
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLOrContext::EndElement()
{
    pFilterContext->CloseConnection();
}

ScXMLConditionContext::ScXMLConditionContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLFilterContext* pTempFilterContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName ),
    bIsCaseSensitive(sal_False)
{
    pFilterContext = pTempFilterContext;
    sDataType = GetXMLToken(XML_TEXT);

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetFilterConditionAttrTokenMap();
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        ::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
        ::rtl::OUString aLclLocalName;
        USHORT nLclPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                            sAttrName, &aLclLocalName );
        ::rtl::OUString sValue = xAttrList->getValueByIndex( i );

        switch( rAttrTokenMap.Get( nLclPrefix, aLclLocalName ) )
        {
            case XML_TOK_CONDITION_ATTR_FIELD_NUMBER :
            {
                nField = sValue.toInt32();
            }
            break;
            case XML_TOK_CONDITION_ATTR_CASE_SENSITIVE :
            {
                bIsCaseSensitive = IsXMLToken(sValue, XML_TRUE);
            }
            break;
            case XML_TOK_CONDITION_ATTR_DATA_TYPE :
            {
                sDataType = sValue;
            }
            break;
            case XML_TOK_CONDITION_ATTR_VALUE :
            {
                sConditionValue = sValue;
            }
            break;
            case XML_TOK_CONDITION_ATTR_OPERATOR :
            {
                sOperator = sValue;
            }
            break;
        }
    }
}

ScXMLConditionContext::~ScXMLConditionContext()
{
}

SvXMLImportContext *ScXMLConditionContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& /*xAttrList*/ )
{
    SvXMLImportContext *pContext = 0;

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLConditionContext::getOperatorXML(const ::rtl::OUString sTempOperator, sheet::FilterOperator& aFilterOperator, sal_Bool& bUseRegularExpressions) const
{
    bUseRegularExpressions = sal_False;
    if (IsXMLToken(sTempOperator, XML_MATCH))
    {
        bUseRegularExpressions = sal_True;
        aFilterOperator = sheet::FilterOperator_EQUAL;
    }
    else if (IsXMLToken(sTempOperator, XML_NOMATCH))
    {
        bUseRegularExpressions = sal_True;
        aFilterOperator = sheet::FilterOperator_NOT_EQUAL;
    }
    else if (sTempOperator.compareToAscii("=") == 0)
        aFilterOperator = sheet::FilterOperator_EQUAL;
    else if (sTempOperator.compareToAscii("!=") == 0)
        aFilterOperator = sheet::FilterOperator_NOT_EQUAL;
    else if (IsXMLToken(sTempOperator, XML_BOTTOM_PERCENT))
        aFilterOperator = sheet::FilterOperator_BOTTOM_PERCENT;
    else if (IsXMLToken(sTempOperator, XML_BOTTOM_VALUES))
        aFilterOperator = sheet::FilterOperator_BOTTOM_VALUES;
    else if (IsXMLToken(sTempOperator, XML_EMPTY))
        aFilterOperator = sheet::FilterOperator_EMPTY;
    else if (sTempOperator.compareToAscii(">") == 0)
        aFilterOperator = sheet::FilterOperator_GREATER;
    else if (sTempOperator.compareToAscii(">=") == 0)
        aFilterOperator = sheet::FilterOperator_GREATER_EQUAL;
    else if (sTempOperator.compareToAscii("<") == 0)
        aFilterOperator = sheet::FilterOperator_LESS;
    else if (sTempOperator.compareToAscii("<=") == 0)
        aFilterOperator = sheet::FilterOperator_LESS_EQUAL;
    else if (IsXMLToken(sTempOperator, XML_NOEMPTY))
        aFilterOperator = sheet::FilterOperator_NOT_EMPTY;
    else if (IsXMLToken(sTempOperator, XML_TOP_PERCENT))
        aFilterOperator = sheet::FilterOperator_TOP_PERCENT;
    else if (IsXMLToken(sTempOperator, XML_TOP_VALUES))
        aFilterOperator = sheet::FilterOperator_TOP_VALUES;
}

void ScXMLConditionContext::EndElement()
{
    sheet::TableFilterField aFilterField;
    if (pFilterContext->GetConnection())
        aFilterField.Connection = sheet::FilterConnection_OR;
    else
        aFilterField.Connection = sheet::FilterConnection_AND;
    pFilterContext->SetIsCaseSensitive(bIsCaseSensitive);
    sal_Bool bUseRegularExpressions;
    getOperatorXML(sOperator, aFilterField.Operator, bUseRegularExpressions);
    pFilterContext->SetUseRegularExpressions(bUseRegularExpressions);
    aFilterField.Field = nField;
    if (IsXMLToken(sDataType, XML_NUMBER))
    {
        aFilterField.NumericValue = sConditionValue.toDouble();
        aFilterField.IsNumeric = sal_True;
    }
    else
    {
        aFilterField.StringValue = sConditionValue;
        aFilterField.IsNumeric = sal_False;
    }
    pFilterContext->AddFilterField(aFilterField);
}

//==========================================================================

ScXMLDPFilterContext::ScXMLDPFilterContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDataPilotTableContext* pTempDataPilotTableContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName ),
    aFilterFields(),
    nFilterFieldCount(0),
    bSkipDuplicates(sal_False),
    bCopyOutputData(sal_False),
    bUseRegularExpressions(sal_False),
    bConnectionOr(sal_True),
    bNextConnectionOr(sal_True),
    bConditionSourceRange(sal_False)
{
    ScDocument* pDoc = GetScImport().GetDocument();
    pDataPilotTable = pTempDataPilotTableContext;

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetFilterAttrTokenMap();
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        ::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
        ::rtl::OUString aLclLocalName;
        USHORT nLclPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                            sAttrName, &aLclLocalName );
        ::rtl::OUString sValue = xAttrList->getValueByIndex( i );

        switch( rAttrTokenMap.Get( nLclPrefix, aLclLocalName ) )
        {
            case XML_TOK_FILTER_ATTR_TARGET_RANGE_ADDRESS :
            {
                ScRange aScRange;
                sal_Int32 nOffset(0);
                if (ScXMLConverter::GetRangeFromString( aScRange, sValue, pDoc, nOffset ))
                {
                    aOutputPosition = aScRange.aStart;
                    bCopyOutputData = sal_True;
                }
            }
            break;
            case XML_TOK_FILTER_ATTR_CONDITION_SOURCE_RANGE_ADDRESS :
            {
                sal_Int32 nOffset(0);
                if(ScXMLConverter::GetRangeFromString( aConditionSourceRangeAddress, sValue, pDoc, nOffset ))
                    bConditionSourceRange = sal_True;
            }
            break;
            case XML_TOK_FILTER_ATTR_CONDITION_SOURCE :
            {
                // not supported by StarOffice
            }
            break;
            case XML_TOK_FILTER_ATTR_DISPLAY_DUPLICATES :
            {
                bSkipDuplicates = !IsXMLToken(sValue, XML_TRUE);
            }
            break;
        }
    }
}

ScXMLDPFilterContext::~ScXMLDPFilterContext()
{
}

SvXMLImportContext *ScXMLDPFilterContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetFilterElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
        case XML_TOK_FILTER_AND:
        {
            pContext = new ScXMLDPAndContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, this);
        }
        break;
        case XML_TOK_FILTER_OR:
        {
            pContext = new ScXMLDPOrContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, this);
        }
        break;
        case XML_TOK_FILTER_CONDITION:
        {
            pContext = new ScXMLDPConditionContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, this);
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLDPFilterContext::EndElement()
{
    aFilterFields.bRegExp = bUseRegularExpressions;
    aFilterFields.bCaseSens = bIsCaseSensitive;
    aFilterFields.bDuplicate = !bSkipDuplicates;
//  pDataPilotTable->SetFilterUseRegularExpressions(bUseRegularExpressions);
    if (bCopyOutputData)
    {
        pDataPilotTable->SetFilterOutputPosition(aOutputPosition);
        pDataPilotTable->SetFilterCopyOutputData(bCopyOutputData);
    }
    else
        pDataPilotTable->SetFilterCopyOutputData(sal_False);
//  pDataPilotTable->SetFilterIsCaseSensitive(bIsCaseSensitive);
//  pDataPilotTable->SetFilterSkipDuplicates(bSkipDuplicates);
    pDataPilotTable->SetSourceQueryParam(aFilterFields);
    if (bConditionSourceRange)
        pDataPilotTable->SetFilterSourceRange(aConditionSourceRangeAddress);
}

void ScXMLDPFilterContext::AddFilterField (const ScQueryEntry& aFilterField)
{
    aFilterFields.Resize(nFilterFieldCount + 1);
    ScQueryEntry& rEntry = aFilterFields.GetEntry(nFilterFieldCount);
    rEntry = aFilterField;
    rEntry.bDoQuery = sal_True;
    nFilterFieldCount++;
}

ScXMLDPAndContext::ScXMLDPAndContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& /*xAttrList*/,
                                        ScXMLDPFilterContext* pTempFilterContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName )
{
    pFilterContext = pTempFilterContext;
    pFilterContext->OpenConnection(sal_False);
}

ScXMLDPAndContext::~ScXMLDPAndContext()
{
}

SvXMLImportContext *ScXMLDPAndContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetFilterElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
        case XML_TOK_FILTER_OR:
        {
            // not supported in StarOffice
        }
        break;
        case XML_TOK_FILTER_CONDITION:
        {
            pContext = new ScXMLDPConditionContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, pFilterContext);
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLDPAndContext::EndElement()
{
    pFilterContext->CloseConnection();
}

ScXMLDPOrContext::ScXMLDPOrContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& /*xAttrList*/,
                                        ScXMLDPFilterContext* pTempFilterContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName )
{
    pFilterContext = pTempFilterContext;
    pFilterContext->OpenConnection(sal_True);
}

ScXMLDPOrContext::~ScXMLDPOrContext()
{
}

SvXMLImportContext *ScXMLDPOrContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = GetScImport().GetFilterElemTokenMap();
    switch( rTokenMap.Get( nInPrefix, rLName ) )
    {
        case XML_TOK_FILTER_AND:
        {
            pContext = new ScXMLDPAndContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, pFilterContext);
        }
        break;
        case XML_TOK_FILTER_CONDITION:
        {
            pContext = new ScXMLDPConditionContext( GetScImport(), nInPrefix,
                                                          rLName, xAttrList, pFilterContext);
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLDPOrContext::EndElement()
{
    pFilterContext->CloseConnection();
}

ScXMLDPConditionContext::ScXMLDPConditionContext( ScXMLImport& rInImport,
                                      USHORT nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDPFilterContext* pTempFilterContext) :
    SvXMLImportContext( rInImport, nPrfx, rLName ),
    sDataType(GetXMLToken(XML_TEXT)),
    bIsCaseSensitive(sal_False)
{
    pFilterContext = pTempFilterContext;

    sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    const SvXMLTokenMap& rAttrTokenMap = GetScImport().GetFilterConditionAttrTokenMap();
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        ::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
        ::rtl::OUString aLclLocalName;
        USHORT nLclPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                            sAttrName, &aLclLocalName );
        ::rtl::OUString sValue = xAttrList->getValueByIndex( i );

        switch( rAttrTokenMap.Get( nLclPrefix, aLclLocalName ) )
        {
            case XML_TOK_CONDITION_ATTR_FIELD_NUMBER :
            {
                nField = sValue.toInt32();
            }
            break;
            case XML_TOK_CONDITION_ATTR_CASE_SENSITIVE :
            {
                bIsCaseSensitive = IsXMLToken(sValue, XML_TRUE);
            }
            break;
            case XML_TOK_CONDITION_ATTR_DATA_TYPE :
            {
                sDataType = sValue;
            }
            break;
            case XML_TOK_CONDITION_ATTR_VALUE :
            {
                sConditionValue = sValue;
            }
            break;
            case XML_TOK_CONDITION_ATTR_OPERATOR :
            {
                sOperator = sValue;
            }
            break;
        }
    }
}

ScXMLDPConditionContext::~ScXMLDPConditionContext()
{
}

SvXMLImportContext *ScXMLDPConditionContext::CreateChildContext( USHORT nInPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& /*xAttrList*/ )
{
    SvXMLImportContext *pContext = 0;

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nInPrefix, rLName );

    return pContext;
}

void ScXMLDPConditionContext::getOperatorXML(const ::rtl::OUString sTempOperator, ScQueryOp& aFilterOperator, sal_Bool& bUseRegularExpressions,
                                            double& dVal) const
{
    bUseRegularExpressions = sal_False;
    if (IsXMLToken(sTempOperator, XML_MATCH))
    {
        bUseRegularExpressions = sal_True;
        aFilterOperator = SC_EQUAL;
    }
    else if (IsXMLToken(sTempOperator, XML_NOMATCH))
    {
        bUseRegularExpressions = sal_True;
        aFilterOperator = SC_NOT_EQUAL;
    }
    else if (sTempOperator.compareToAscii("=") == 0)
        aFilterOperator = SC_EQUAL;
    else if (sTempOperator.compareToAscii("!=") == 0)
        aFilterOperator = SC_NOT_EQUAL;
    else if (IsXMLToken(sTempOperator, XML_BOTTOM_PERCENT))
        aFilterOperator = SC_BOTPERC;
    else if (IsXMLToken(sTempOperator, XML_BOTTOM_VALUES))
        aFilterOperator = SC_BOTVAL;
    else if (IsXMLToken(sTempOperator, XML_EMPTY))
        dVal = SC_EMPTYFIELDS;
    else if (sTempOperator.compareToAscii(">") == 0)
        aFilterOperator = SC_GREATER;
    else if (sTempOperator.compareToAscii(">=") == 0)
        aFilterOperator = SC_GREATER_EQUAL;
    else if (sTempOperator.compareToAscii("<") == 0)
        aFilterOperator = SC_LESS;
    else if (sTempOperator.compareToAscii("<=") == 0)
        aFilterOperator = SC_LESS_EQUAL;
    else if (IsXMLToken(sTempOperator, XML_NOEMPTY))
        dVal = SC_NONEMPTYFIELDS;
    else if (IsXMLToken(sTempOperator, XML_TOP_PERCENT))
        aFilterOperator = SC_TOPPERC;
    else if (IsXMLToken(sTempOperator, XML_TOP_VALUES))
        aFilterOperator = SC_TOPVAL;
}

void ScXMLDPConditionContext::EndElement()
{
    ScQueryEntry aFilterField;
    if (pFilterContext->GetConnection())
        aFilterField.eConnect = SC_OR;
    else
        aFilterField.eConnect = SC_AND;
    pFilterContext->SetIsCaseSensitive(bIsCaseSensitive);
    sal_Bool bUseRegularExpressions;
    double dVal(0.0);
    getOperatorXML(sOperator, aFilterField.eOp, bUseRegularExpressions, dVal);
    pFilterContext->SetUseRegularExpressions(bUseRegularExpressions);
    aFilterField.nField = static_cast<USHORT>(nField);
    if (IsXMLToken(sDataType, XML_NUMBER))
    {
        aFilterField.nVal = sConditionValue.toDouble();
        *aFilterField.pStr = sConditionValue;
        aFilterField.bQueryByString = sal_False;
        if (dVal != 0.0)
        {
            aFilterField.nVal = dVal;
            *aFilterField.pStr = EMPTY_STRING;
        }
    }
    else
    {
        aFilterField.pStr = new String(sConditionValue);
        aFilterField.bQueryByString = sal_True;
        aFilterField.nVal = 0;
    }
    pFilterContext->AddFilterField(aFilterField);
}



}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
