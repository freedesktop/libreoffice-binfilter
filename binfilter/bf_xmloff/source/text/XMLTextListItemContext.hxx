/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: XMLTextListItemContext.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 10:11:46 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _XMLTEXTLISTITEMCONTEXT_HXX
#define _XMLTEXTLISTITEMCONTEXT_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif
namespace binfilter {

class XMLTextImportHelper;

class XMLTextListItemContext : public SvXMLImportContext
{
    XMLTextImportHelper& rTxtImport;

    sal_Int16                   nStartValue;

//  SwXMLImport& GetSwImport() { return (SwXMLImport&)GetImport(); }

public:

    TYPEINFO();

    XMLTextListItemContext(
            SvXMLImport& rImport,
            XMLTextImportHelper& rTxtImp, sal_uInt16 nPrfx,
            const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
            sal_Bool bIsHeader = sal_False );
    virtual ~XMLTextListItemContext();

    virtual void EndElement();

    SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                 const ::rtl::OUString& rLocalName,
                 const ::com::sun::star::uno::Reference<
                     ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

    sal_Bool HasStartValue() const { return -1 != nStartValue; }
    sal_Int16 GetStartValue() const { return nStartValue; }
};


}//end of namespace binfilter
#endif
