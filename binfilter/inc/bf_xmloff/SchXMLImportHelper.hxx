/*************************************************************************
 *
 *  $RCSfile: SchXMLImportHelper.hxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jmeng $ $Date: 2003-12-04 01:45:20 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef _XMLOFF_SCH_XMLIMPORTHELPER_HXX_
#define _XMLOFF_SCH_XMLIMPORTHELPER_HXX_

#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include <bf_xmloff/families.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_XSTRINGMAPPING_HPP_
#include <com/sun/star/util/XStringMapping.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_XCHARTDOCUMENT_HPP_
#include <com/sun/star/chart/XChartDocument.hpp>
#endif

namespace com { namespace sun { namespace star {
    namespace frame {
        class XModel;
    }
    namespace task {
        class XStatusIndicator;
    }
    namespace xml {
        namespace sax {
            class XAttributeList;
}}}}}
namespace binfilter {

class SvXMLUnitConverter;
class SvXMLStylesContext;
class XMLChartImportPropertyMapper;
class SvXMLTokenMap;
class SvXMLImportContext;
class SvXMLImport;

// ========================================

/** With this class you can import a <chart:chart> element containing
    its data as <table:table> element or without internal table. In
    the latter case you have to provide a table address mapper that
    converts table addresses in XML format to the appropriate application
    format.
 */
class SchXMLImportHelper : public UniRefBase
{
private:
    ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDocument > mxChartDoc;
    ::com::sun::star::uno::Reference< ::com::sun::star::util::XStringMapping > mxTableAddressMapper;
    SvXMLStylesContext* mpAutoStyles;

    SvXMLTokenMap* mpDocElemTokenMap;
    SvXMLTokenMap* mpTableElemTokenMap;
    SvXMLTokenMap* mpChartElemTokenMap;
    SvXMLTokenMap* mpPlotAreaElemTokenMap;
    SvXMLTokenMap* mpSeriesElemTokenMap;

    SvXMLTokenMap* mpChartAttrTokenMap;
    SvXMLTokenMap* mpPlotAreaAttrTokenMap;
    SvXMLTokenMap* mpAxisAttrTokenMap;
    SvXMLTokenMap* mpLegendAttrTokenMap;
    SvXMLTokenMap* mpAutoStyleAttrTokenMap;
    SvXMLTokenMap* mpCellAttrTokenMap;
    SvXMLTokenMap* mpSeriesAttrTokenMap;

public:

    SchXMLImportHelper();
    ~SchXMLImportHelper();

    /** get the context for reading the <chart:chart> element with subelements.
        The result is stored in the XModel given if it also implements
        XChartDocument
     */
    SvXMLImportContext* CreateChartContext(
        SvXMLImport& rImport,
        sal_uInt16 nPrefix, const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XModel > xChartModel,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

    /** set the auto-style context that will be used to retrieve auto-styles
        used inside the following <chart:chart> element to parse
     */
    void SetAutoStylesContext( SvXMLStylesContext* pAutoStyles ) { mpAutoStyles = pAutoStyles; }
    SvXMLStylesContext* GetAutoStylesContext() const { return mpAutoStyles; }

    /** set the string mapper that is used to convert cell addresses in
        XML format into the application format

        If the mapper is set it is automatically used for conversion
      */
    void SetTableAddressMapper( ::com::sun::star::uno::Reference<
                                    ::com::sun::star::util::XStringMapping >& xMapper )
        { mxTableAddressMapper = xMapper; }

    const ::com::sun::star::uno::Reference<
        ::com::sun::star::util::XStringMapping >& GetTableAddressMapper()
        { return mxTableAddressMapper; }

    const ::com::sun::star::uno::Reference<
        ::com::sun::star::chart::XChartDocument >& GetChartDocument()
        { return mxChartDoc; }

    const SvXMLTokenMap& GetDocElemTokenMap();
    const SvXMLTokenMap& GetTableElemTokenMap();
    const SvXMLTokenMap& GetChartElemTokenMap();
    const SvXMLTokenMap& GetPlotAreaElemTokenMap();
    const SvXMLTokenMap& GetSeriesElemTokenMap();

    const SvXMLTokenMap& GetChartAttrTokenMap();
    const SvXMLTokenMap& GetPlotAreaAttrTokenMap();
    const SvXMLTokenMap& GetAxisAttrTokenMap();
    const SvXMLTokenMap& GetLegendAttrTokenMap();
    const SvXMLTokenMap& GetAutoStyleAttrTokenMap();
    const SvXMLTokenMap& GetCellAttrTokenMap();
    const SvXMLTokenMap& GetSeriesAttrTokenMap();

    static sal_uInt16 GetChartFamilyID() { return XML_STYLE_FAMILY_SCH_CHART_ID; }

    sal_Int32   GetNumberOfSeries();
    sal_Int32   GetLengthOfSeries();
    void        ResizeChartData( sal_Int32 nSeries, sal_Int32 nDataPoints = -1 );
};

}//end of namespace binfilter
#endif  // _XMLOFF_SCH_XMLIMPORTHELPER_HXX_
