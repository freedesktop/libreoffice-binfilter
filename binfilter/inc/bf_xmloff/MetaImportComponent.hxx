/*************************************************************************
 *
 *  $RCSfile: MetaImportComponent.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: aw $ $Date: 2003-12-05 09:17:47 $
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

#ifndef _XMLOFF_METAIMPORTCOMPONENT_HXX
#define _XMLOFF_METAIMPORTCOMPONENT_HXX

#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFO_HPP_
#include <com/sun/star/document/XDocumentInfo.hpp>
#endif
namespace binfilter {

class XMLMetaImportComponent : public SvXMLImport
{
private:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::document::XDocumentInfo>  xDocInfo;

public:
    // #110680#
    // XMLMetaImportComponent() throw();
    XMLMetaImportComponent(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory
        ) throw();

    ~XMLMetaImportComponent() throw();


protected:

    virtual SvXMLImportContext* CreateContext(
        sal_uInt16 nPrefix,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

    // XImporter
    virtual void SAL_CALL setTargetDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc )
        throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
};


// global functions to support the component

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
    XMLMetaImportComponent_getSupportedServiceNames()
    throw();

::rtl::OUString SAL_CALL XMLMetaImportComponent_getImplementationName()
    throw();

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
    XMLMetaImportComponent_createInstance(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::lang::XMultiServiceFactory > & )
    throw( ::com::sun::star::uno::Exception );

}//end of namespace binfilter
#endif

