/*************************************************************************
 *
 *  $RCSfile: attriblistmerge.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-08-03 19:56:07 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _XMLOFF_FORMS_ATTRIBLISTMERGE_HXX_
#define _XMLOFF_FORMS_ATTRIBLISTMERGE_HXX_

#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
// auto strip #ifndef _OSL_MUTEX_HXX_
// auto strip #include <osl/mutex.hxx>
// auto strip #endif
#ifndef _COM_SUN_STAR_XML_SAX_XATTRIBUTELIST_HPP_
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#endif
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

    //=====================================================================
    //= OAttribListMerger
    //=====================================================================
    typedef ::cppu::WeakImplHelper1 <   ::com::sun::star::xml::sax::XAttributeList
                                    >   OAttribListMerger_Base;
    /** implements the XAttributeList list by merging different source attribute lists

        <p>Currently, the time behavious is O(n), though it would be possible to change it to O(log n).</p>
    */
    class OAttribListMerger : public OAttribListMerger_Base
    {
    protected:
        ::osl::Mutex        m_aMutex;
        DECLARE_STL_VECTOR( ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >, AttributeListArray );
        AttributeListArray  m_aLists;

        ~OAttribListMerger() { }

    public:
        OAttribListMerger() { }

        // attribute list handling
        // (very thinn at the moment ... only adding lists is allowed ... add more if you need it :)
        void addList(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& _rList);

        // XAttributeList
        virtual sal_Int16 SAL_CALL getLength(  ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getNameByIndex( sal_Int16 i ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getTypeByIndex( sal_Int16 i ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getTypeByName( const ::rtl::OUString& aName ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getValueByIndex( sal_Int16 i ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getValueByName( const ::rtl::OUString& aName ) throw(::com::sun::star::uno::RuntimeException);

    protected:
        sal_Bool seekToIndex(sal_Int16 _nGlobalIndex, ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& _rSubList, sal_Int16& _rLocalIndex);
        sal_Bool seekToName(const ::rtl::OUString& _rName, ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& _rSubList, sal_Int16& _rLocalIndex);
    };


//.........................................................................
}   // namespace xmloff
//.........................................................................

}//end of namespace binfilter
#endif // _XMLOFF_FORMS_ATTRIBLISTMERGE_HXX_

/*************************************************************************
 * history:
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1.20.1  2004/07/08 16:32:17  aw
 *  #117295#
 *
 *  Revision 1.1  2003/12/05 09:34:00  aw
 *  Added bf_xmloff files
 *
 *  Revision 1.1  2000/12/12 12:02:13  fs
 *  initial checkin - helper class for mergin XAttributeList instances
 *
 *
 *  Revision 1.0 12.12.00 10:25:25  fs
 ************************************************************************/

