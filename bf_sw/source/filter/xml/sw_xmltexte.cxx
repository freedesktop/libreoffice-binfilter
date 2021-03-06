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

#include <sot/clsids.hxx>

#include <bf_xmloff/xmlnmspe.hxx>
#include <bf_xmloff/txtprmap.hxx>

#include <bf_sfx2/frmdescr.hxx>


#include <horiornt.hxx>

#include <doc.hxx>

#include <osl/diagnose.h>

#include <ndole.hxx>

#include <cppuhelper/implbase4.hxx>

#include <unostyle.hxx>
#include <unoframe.hxx>
#include <ndgrf.hxx>

#include "xmlexp.hxx"
#include "xmltexte.hxx"




#include <SwAppletImpl.hxx>

#define _SVSTDARR_ULONGS
#include <bf_svtools/svstdarr.hxx>

#include <SwStyleNameMapper.hxx>
#include "bf_so3/staticbaseurl.hxx"
namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::style;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::document;
using namespace ::binfilter::xmloff::token;

using rtl::OUString;
using rtl::OUStringBuffer;

enum SvEmbeddedObjectTypes
{
    SV_EMBEDDED_OWN,
    SV_EMBEDDED_OUTPLACE,
    SV_EMBEDDED_APPLET,
    SV_EMBEDDED_PLUGIN,
    SV_EMBEDDED_FRAME
};

// ---------------------------------------------------------------------

SwNoTxtNode *SwXMLTextParagraphExport::GetNoTxtNode(
    const Reference < XPropertySet >& rPropSet ) const
{
    Reference<XUnoTunnel> xCrsrTunnel( rPropSet, UNO_QUERY );
    OSL_ENSURE( xCrsrTunnel.is(), "missing XUnoTunnel for embedded" );
    SwXFrame *pFrame =
                (SwXFrame *)xCrsrTunnel->getSomething(
                                    SwXFrame::getUnoTunnelId() );
    OSL_ENSURE( pFrame, "SwXFrame missing" );
    SwFrmFmt *pFrmFmt = pFrame->GetFrmFmt();
    const SwFmtCntnt& rCntnt = pFrmFmt->GetCntnt();
    const SwNodeIndex *pNdIdx = rCntnt.GetCntntIdx();
    return  pNdIdx->GetNodes()[pNdIdx->GetIndex() + 1]->GetNoTxtNode();
}

void SwXMLTextParagraphExport::exportStyleContent(
        const Reference< XStyle > & rStyle )
{

    const SwXStyle* pStyle = 0;
    Reference<XUnoTunnel> xStyleTunnel( rStyle, UNO_QUERY);
    if( xStyleTunnel.is() )
    {
        pStyle = (SwXStyle*)xStyleTunnel->getSomething(
                                            SwXStyle::getUnoTunnelId() );
    }
    if( pStyle && SFX_STYLE_FAMILY_PARA == pStyle->GetFamily() )
    {
        const SwDoc *pDoc = pStyle->GetDoc();
        const SwTxtFmtColl *pColl =
            pDoc->FindTxtFmtCollByName( pStyle->GetStyleName() );
        OSL_ENSURE( pColl, "There is the text collection?" );
        if( pColl && RES_CONDTXTFMTCOLL == pColl->Which() )
        {
            const SwFmtCollConditions& rConditions =
                ((const SwConditionTxtFmtColl *)pColl)->GetCondColls();
            for( sal_uInt16 i=0; i < rConditions.Count(); i++ )
            {
                const SwCollCondition& rCond = *rConditions[i];

                enum XMLTokenEnum eFunc = XML_TOKEN_INVALID;
                OUStringBuffer sBuffer( 20 );
                switch( rCond.GetCondition() )
                {
                case PARA_IN_LIST:
                    eFunc = XML_LIST_LEVEL;
                    sBuffer.append( (sal_Int32)(rCond.GetSubCondition()+1) );
                    break;
                case PARA_IN_OUTLINE:
                    eFunc = XML_OUTLINE_LEVEL;
                    sBuffer.append( (sal_Int32)(rCond.GetSubCondition()+1) );
                    break;
                case PARA_IN_FRAME:
                    eFunc = XML_TEXT_BOX;
                    break;
                case PARA_IN_TABLEHEAD:
                    eFunc = XML_TABLE_HEADER;
                    break;
                case PARA_IN_TABLEBODY:
                    eFunc = XML_TABLE;
                    break;
                case PARA_IN_SECTION:
                    eFunc = XML_SECTION;
                    break;
                case PARA_IN_FOOTENOTE:
                    eFunc = XML_FOOTNOTE;
                    break;
                case PARA_IN_FOOTER:
                    eFunc = XML_FOOTER;
                    break;
                case PARA_IN_HEADER:
                    eFunc = XML_HEADER;
                    break;
                case PARA_IN_ENDNOTE:
                    eFunc = XML_ENDNOTE;
                    break;
                }
                OUString sVal( sBuffer.makeStringAndClear() );

                DBG_ASSERT( eFunc != XML_TOKEN_INVALID,
                            "SwXMLExport::ExportFmt: unknown condition" );
                if( eFunc != XML_TOKEN_INVALID )
                {
                    sBuffer.append( GetXMLToken(eFunc) );
                    sBuffer.append( (sal_Unicode)'(' );
                    sBuffer.append( (sal_Unicode)')' );
                    if( sVal.getLength() )
                    {
                        sBuffer.append( (sal_Unicode)'=' );
                        sBuffer.append( sVal );
                    }

                    GetExport().AddAttribute( XML_NAMESPACE_STYLE,
                                XML_CONDITION,
                                sBuffer.makeStringAndClear() );
                    String aString;
                    SwStyleNameMapper::FillProgName(
                                    rCond.GetTxtFmtColl()->GetName(),
                                    aString,
                                    GET_POOLID_TXTCOLL,
                                    sal_True);
                    GetExport().AddAttribute( XML_NAMESPACE_STYLE,
                                XML_APPLY_STYLE_NAME, aString );
                    SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_STYLE,
                                              XML_MAP, sal_True, sal_True );
                }
            }
        }
    }
}

SwXMLTextParagraphExport::SwXMLTextParagraphExport(
        SwXMLExport& rExp,
         SvXMLAutoStylePoolP& rInAutoStylePool ) :
    XMLTextParagraphExport( rExp, rInAutoStylePool ),
    sTextTable( RTL_CONSTASCII_USTRINGPARAM( "TextTable" ) ),
    sEmbeddedObjectProtocol( RTL_CONSTASCII_USTRINGPARAM( "vnd.sun.star.EmbeddedObject:" ) ),
    aAppletClassId( SO3_APPLET_CLASSID ),
    aPluginClassId( SO3_PLUGIN_CLASSID ),
    aIFrameClassId( BF_SO3_IFRAME_CLASSID ),
    aOutplaceClassId( SO3_OUT_CLASSID )
{
}

SwXMLTextParagraphExport::~SwXMLTextParagraphExport()
{
}

void SwXMLTextParagraphExport::setTextEmbeddedGraphicURL(
    const Reference < XPropertySet >& rPropSet,
    OUString& rURL) const
{
    if( !rURL.getLength() )
        return;

    SwGrfNode *pGrfNd = GetNoTxtNode( rPropSet )->GetGrfNode();
    if( !pGrfNd->IsGrfLink() )
    {
        String aNewURL( RTL_CONSTASCII_USTRINGPARAM("vnd.sun.star.Package:") );
        aNewURL += String(rURL.copy( 1 ) );
        pGrfNd->SetNewStreamName( aNewURL );
    }
}

static void lcl_addParam ( SvXMLExport &rInExport, const SvCommand &rCommand )
{
    rInExport.AddAttribute( XML_NAMESPACE_DRAW, XML_NAME, rCommand.GetCommand() );
    rInExport.AddAttribute( XML_NAMESPACE_DRAW, XML_VALUE, rCommand.GetArgument() );
    SvXMLElementExport aElem( rInExport, XML_NAMESPACE_DRAW, XML_PARAM, sal_False, sal_True );
}
static void lcl_addURL ( SvXMLExport &rInExport, const String &rURL,
                         sal_Bool bToRel = sal_True )
{
    String sRelURL;

    if( bToRel && (rURL.Len() > 0) )
        sRelURL = ::binfilter::StaticBaseUrl::AbsToRel(rURL,
                      INetURLObject::WAS_ENCODED,
                      INetURLObject::DECODE_UNAMBIGUOUS);
    else
        sRelURL = rURL;

    if (sRelURL.Len())
    {
        rInExport.AddAttribute ( XML_NAMESPACE_XLINK, XML_HREF, sRelURL );
        rInExport.AddAttribute ( XML_NAMESPACE_XLINK, XML_TYPE, XML_SIMPLE );
        rInExport.AddAttribute ( XML_NAMESPACE_XLINK, XML_SHOW, XML_EMBED );
        rInExport.AddAttribute ( XML_NAMESPACE_XLINK, XML_ACTUATE, XML_ONLOAD );
    }
}

void lcl_addOutplaceProperties(
        const SvInfoObject *pInfo,
        const XMLPropertyState **pStates,
        const UniReference < XMLPropertySetMapper >& rMapper )
{
    SvEmbeddedInfoObject * pEmbed = PTR_CAST(SvEmbeddedInfoObject, pInfo );
    if( pEmbed )
    {
        const Rectangle& rVisArea = pEmbed->GetVisArea();
        if( !rVisArea.IsEmpty() )
        {
            Any aAny;

            aAny <<= (sal_Int32)rVisArea.Left();
            *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_OLE_VIS_AREA_LEFT ), aAny );
            pStates++;

            aAny <<= (sal_Int32)rVisArea.Top();
            *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_OLE_VIS_AREA_TOP ), aAny );
            pStates++;

            aAny <<= (sal_Int32)rVisArea.GetWidth();
            *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_OLE_VIS_AREA_WIDTH ), aAny );
            pStates++;

            aAny <<= (sal_Int32)rVisArea.GetHeight();
            *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_OLE_VIS_AREA_HEIGHT ), aAny );
            pStates++;

            aAny <<= (sal_Int32)pEmbed->GetViewAspect();
            *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_OLE_DRAW_ASPECT ), aAny );
            pStates++;

            // TODO: aspect
        }
    }
}

void lcl_addFrameProperties(
        const SfxFrameDescriptor *pDescriptor,
        const XMLPropertyState **pStates,
        const UniReference < XMLPropertySetMapper >& rMapper )
{
    if( ScrollingAuto != pDescriptor->GetScrollingMode() )
    {
        sal_Bool bValue = ScrollingYes == pDescriptor->GetScrollingMode();
        Any aAny( &bValue, ::getBooleanCppuType() );
        *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_FRAME_DISPLAY_SCROLLBAR ), aAny );
        pStates++;
    }
    if( pDescriptor->IsFrameBorderSet() )
    {
        sal_Bool bValue = pDescriptor->IsFrameBorderOn();
        Any aAny( &bValue, ::getBooleanCppuType() );
        *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_FRAME_DISPLAY_BORDER ), aAny );
        pStates++;
    }
    const Size& rMargins = pDescriptor->GetMargin();
    if( SIZE_NOT_SET != rMargins.Width() )
    {
        Any aAny;
        aAny <<= (sal_Int32)rMargins.Width();
        *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_FRAME_MARGIN_HORI ), aAny );
        pStates++;
    }
    if( SIZE_NOT_SET != rMargins.Height() )
    {
        Any aAny;
        aAny <<= (sal_Int32)rMargins.Height();
        *pStates = new XMLPropertyState( rMapper->FindEntryIndex( CTF_FRAME_MARGIN_VERT ), aAny );
        pStates++;
    }
}

void SwXMLTextParagraphExport::_collectTextEmbeddedAutoStyles(
        const Reference < XPropertySet > & rPropSet )
{
    SwOLENode *pOLENd = GetNoTxtNode( rPropSet )->GetOLENode();
    SwOLEObj& rOLEObj = pOLENd->GetOLEObj();
    SvPersist *pPersist = pOLENd->GetDoc()->GetPersist();
    OSL_ENSURE( pPersist, "no persist" );
    const SvInfoObject *pInfo = pPersist->Find( rOLEObj.GetName() );
    DBG_ASSERT( pInfo, "no info object for OLE object found" );

    if( !pInfo )
        return;

    const XMLPropertyState *aStates[7] = { 0, 0, 0, 0, 0, 0, 0 };
    SvGlobalName aClassId( pInfo->GetClassName() );

    if( aOutplaceClassId == aClassId )
    {
        lcl_addOutplaceProperties( pInfo, aStates,
               GetAutoFramePropMapper()->getPropertySetMapper() );
    }
    else if( aIFrameClassId == aClassId )
    {
        SfxFrameObjectRef xFrame( rOLEObj.GetOleRef() );
        OSL_ENSURE( xFrame.Is(), "wrong class id for frame" );

        lcl_addFrameProperties( NULL, aStates, GetAutoFramePropMapper()->getPropertySetMapper() );
    }

    Add( XML_STYLE_FAMILY_TEXT_FRAME, rPropSet, aStates );

    const XMLPropertyState **pStates = aStates;
    while( *pStates )
    {
        delete *pStates;
        pStates++;
    }
}

void SwXMLTextParagraphExport::_exportTextEmbedded(
        const Reference < XPropertySet > & rPropSet,
        const Reference < XPropertySetInfo > & rPropSetInfo )
{
    SwOLENode *pOLENd = GetNoTxtNode( rPropSet )->GetOLENode();
    SwOLEObj& rOLEObj = pOLENd->GetOLEObj();
    SvPersist *pPersist = pOLENd->GetDoc()->GetPersist();
    const SvInfoObject *pInfo = pPersist->Find( rOLEObj.GetName() );
    DBG_ASSERT( pInfo, "no info object for OLE object found" );

    if( !pInfo )
        return;

    SvGlobalName aClassId( pInfo->GetClassName() );

    SvEmbeddedObjectTypes nType = SV_EMBEDDED_OWN;
    SvPlugInObjectRef xPlugin;
    SvAppletObjectRef xApplet;
    SfxFrameObjectRef xFrame;
    if( aPluginClassId == aClassId )
    {
        xPlugin = SvPlugInObjectRef( rOLEObj.GetOleRef() );
        OSL_ENSURE( xPlugin.Is(), "wrong class id for plugin" );
        nType = SV_EMBEDDED_PLUGIN;
    }
    else if( aAppletClassId == aClassId )
    {
        xApplet = SvAppletObjectRef( rOLEObj.GetOleRef() );
        OSL_ENSURE( xApplet.Is(), "wrong class id for applet" );
        nType = SV_EMBEDDED_APPLET;
    }
    else if( aIFrameClassId == aClassId )
    {
        xFrame = SfxFrameObjectRef( rOLEObj.GetOleRef() );
        OSL_ENSURE( xFrame.Is(), "wrong class id for frame" );
        nType = SV_EMBEDDED_FRAME;
    }
    else if( aOutplaceClassId == aClassId )
    {
        nType = SV_EMBEDDED_OUTPLACE;
    }

    SvULongs aParams;
    enum XMLTokenEnum eElementName = XML_TOKEN_INVALID;
    SvXMLExport &rLclExport = GetExport();

    // First the stuff common to each of Applet/Plugin/Floating Frame
    OUString sStyle;
    Any aAny;
    if( rPropSetInfo->hasPropertyByName( sFrameStyleName ) )
    {
        aAny = rPropSet->getPropertyValue( sFrameStyleName );
        aAny >>= sStyle;
    }

    const XMLPropertyState *aStates[7] = { 0, 0, 0, 0, 0, 0, 0 };
    switch( nType )
    {
    case SV_EMBEDDED_FRAME:
        lcl_addFrameProperties( NULL, aStates, GetAutoFramePropMapper()->getPropertySetMapper() );
        break;
    case SV_EMBEDDED_OUTPLACE:
        lcl_addOutplaceProperties( pInfo, aStates,
            GetAutoFramePropMapper()->getPropertySetMapper() );
        break;
    default:
        break;
    }

    OUString sAutoStyle( sStyle );
    sAutoStyle = Find( XML_STYLE_FAMILY_TEXT_FRAME, rPropSet, sStyle,
                       aStates );
    const XMLPropertyState **pStates = aStates;
    while( *pStates )
    {
        delete *pStates;
        pStates++;
    }

    if( sAutoStyle.getLength() )
        rLclExport.AddAttribute( XML_NAMESPACE_DRAW, XML_STYLE_NAME, sAutoStyle );
    addTextFrameAttributes( rPropSet, sal_False );

    switch (nType)
    {
    case SV_EMBEDDED_OUTPLACE:
    case SV_EMBEDDED_OWN:
        if( (rLclExport.getExportFlags() & EXPORT_EMBEDDED) == 0 )
        {
            OUString sURL( sEmbeddedObjectProtocol );
            sURL += pInfo->GetStorageName();
            sURL = GetExport().AddEmbeddedObject( sURL );
            lcl_addURL( rLclExport, sURL, sal_False );
        }
        if( SV_EMBEDDED_OWN == nType && pOLENd->GetChartTblName().Len() )
        {
            OUString sRange( pOLENd->GetChartTblName() );
            OUStringBuffer aBuffer( sRange.getLength() + 2 );
            for( sal_Int32 i=0; i < sRange.getLength(); i++ )
            {
                sal_Unicode c = sRange[i];
                switch( c  )
                {
                    case ' ':
                    case '.':
                    case '\'':
                    case '\\':
                        if( !aBuffer.getLength() )
                        {
                            aBuffer.append( (sal_Unicode)'\'' );
                            aBuffer.append( sRange.copy( 0, i ) );
                        }
                        if( '\'' == c || '\\' == c )
                            aBuffer.append( (sal_Unicode)'\\' );
                        // no break!
                    default:
                        if( aBuffer.getLength() )
                            aBuffer.append( c );
                }
            }
            if( aBuffer.getLength() )
            {
                aBuffer.append( (sal_Unicode)'\'' );
                sRange = aBuffer.makeStringAndClear();
            }

            rLclExport.AddAttribute( XML_NAMESPACE_DRAW, XML_NOTIFY_ON_UPDATE_OF_RANGES,
            sRange );
        }
        eElementName = SV_EMBEDDED_OUTPLACE==nType ? XML_OBJECT_OLE
                                                   : XML_OBJECT;
        break;
    case SV_EMBEDDED_APPLET:
        {
            // It's an applet!
            const XubString & rURL = xApplet->GetCodeBase();
            if (rURL.Len() )
                lcl_addURL(rLclExport, rURL);

            const String &rName = xApplet->GetName();
            if (rName.Len())
                rLclExport.AddAttribute( XML_NAMESPACE_DRAW, XML_APPLET_NAME,
                                      rName );

            rLclExport.AddAttribute( XML_NAMESPACE_DRAW, XML_CODE,
                                  xApplet->GetClass() );

            const SvCommandList& rCommands = xApplet->GetCommandList();

            ULONG i = rCommands.Count();
            while ( i > 0 )
            {
                const SvCommand& rCommand = rCommands [ --i ];
                const String &rLclName = rCommand.GetCommand();
                USHORT nLclType = 0;
                if ( nLclType == SWHTML_OPTTYPE_TAG)
                    rLclExport.AddAttribute( XML_NAMESPACE_DRAW, rLclName, rCommand.GetArgument());
                else if (SWHTML_OPTTYPE_PARAM == nLclType ||
                            SWHTML_OPTTYPE_SIZE == nLclType )
                    aParams.Insert( i, aParams.Count() );
            }

            rLclExport.AddAttribute( XML_NAMESPACE_DRAW, XML_MAY_SCRIPT,
                        xApplet->IsMayScript() ? XML_TRUE : XML_FALSE );
            eElementName = XML_APPLET;
        }
        break;
    case SV_EMBEDDED_PLUGIN:
        {
            // It's a plugin!
            lcl_addURL( rLclExport, xPlugin->GetURL()->GetMainURL( INetURLObject::NO_DECODE ) );
            const String &rType = xPlugin->GetMimeType();
            if (rType.Len())
                rLclExport.AddAttribute( XML_NAMESPACE_DRAW, XML_MIME_TYPE, rType );
            eElementName = XML_PLUGIN;
        }
        break;
    case SV_EMBEDDED_FRAME:
        break;
    default:
        OSL_ENSURE( !this, "unknown object type! Base class should have been called!" );
    }

    SvXMLElementExport aElem( rLclExport, XML_NAMESPACE_DRAW, eElementName,
                                  sal_False, sal_True );
    switch( nType )
    {
    case SV_EMBEDDED_OWN:
        if( (rLclExport.getExportFlags() & EXPORT_EMBEDDED) != 0 )
        {
            Reference < XEmbeddedObjectSupplier > xEOS( rPropSet, UNO_QUERY );
            OSL_ENSURE( xEOS.is(), "no embedded object supplier for own object" );
            Reference < XComponent > xComp = xEOS->getEmbeddedObject();
            rLclExport.ExportEmbeddedOwnObject( xComp );
        }
        break;
    case SV_EMBEDDED_OUTPLACE:
        if( (rLclExport.getExportFlags() & EXPORT_EMBEDDED) != 0 )
        {
            OUString sURL( sEmbeddedObjectProtocol );
            sURL += rOLEObj.GetName();
            GetExport().AddEmbeddedObjectAsBase64( sURL );
        }
        break;
    case SV_EMBEDDED_APPLET:
        {
            const SvCommandList& rCommands = xApplet->GetCommandList();
            USHORT ii = aParams.Count();
            while ( ii > 0 )
            {
                const SvCommand& rCommand = rCommands [ aParams [ --ii] ];
                lcl_addParam (rLclExport, rCommand );
            }
        }
        break;
    case SV_EMBEDDED_PLUGIN:
        {
            const SvCommandList& rCommands = xPlugin->GetCommandList();
            ULONG nCommands = rCommands.Count();
            for ( ULONG i = 0; i < nCommands; i++)
            {
                const SvCommand& rCommand = rCommands [ i ];
                if (0 == SWHTML_OPTTYPE_TAG )
                    lcl_addParam (rLclExport, rCommand );
            }
        }
    default:
        break;
    }

    // Lastly the stuff common to each of Applet/Plugin/Floating Frame
    exportEvents( rPropSet );
    exportAlternativeText( rPropSet, rPropSetInfo );
    exportContour( rPropSet, rPropSetInfo );
}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
