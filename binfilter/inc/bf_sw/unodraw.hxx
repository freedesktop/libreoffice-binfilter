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
#ifndef _UNODRAW_HXX
#define _UNODRAW_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_svx/fmdpage.hxx>
#include <calbck.hxx>
#include <frmfmt.hxx>
#include <com/sun/star/text/XTextContent.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <cppuhelper/implbase3.hxx> // helper for implementations
#include <cppuhelper/implbase5.hxx> // helper for implementations
#include <bf_svtools/itemprop.hxx>
namespace binfilter {

class SdrMarkList;
class SdrView;
class SwDoc;

class SwFmDrawPage : public SvxFmDrawPage
{
    SdrPageView*        pPageView;
protected:

    // Erzeugen eines SdrObjects anhand einer Description. Kann von
    // abgeleiteten Klassen dazu benutzt werden, eigene ::com::sun::star::drawing::Shapes zu
    // unterstuetzen (z.B. Controls)
    virtual SdrObject *_CreateSdrObject( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > & xShape ) throw ();

public:
    SwFmDrawPage( SdrPage* pPage );
    virtual ~SwFmDrawPage() throw ();

    const SdrMarkList&  PreGroup(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > & xShapes);
    void                PreUnGroup(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup >   xShapeGroup);
//  void                PostGroup(); ?? wird es noch gebraucht ??

    SdrView*            GetDrawView() {return pView;}
    SdrPageView*        GetPageView();
    void                RemovePageView();

    // Die folgende Methode wird gerufen, wenn ein SvxShape-Objekt angelegt
    // werden soll. abgeleitete Klassen koennen hier eine Ableitung oder
    // ein ein SvxShape aggregierendes Objekt anlegen.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >  _CreateShape( SdrObject *pObj ) const throw ();
};



typedef cppu::WeakAggImplHelper3
<
    ::com::sun::star::drawing::XDrawPage,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::drawing::XShapeGrouper
>
SwXDrawPageBaseClass;
class SwXDrawPage : public SwXDrawPageBaseClass
{
    SwDoc*          pDoc;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation >     xPageAgg;
    SwFmDrawPage*   pDrawPage;
public:
    SwXDrawPage(SwDoc* pDoc);
    ~SwXDrawPage();

    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);

    //XIndexAccess
    virtual sal_Int32 SAL_CALL getCount(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 nIndex) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException );

    //XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  ) throw(::com::sun::star::uno::RuntimeException);

    //XShapes
    virtual void SAL_CALL add(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > & xShape) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL remove(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > & xShape) throw( ::com::sun::star::uno::RuntimeException );

    //XShapeGrouper
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup >  SAL_CALL group(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > & xShapes) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL ungroup(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapeGroup > & aGroup) throw( ::com::sun::star::uno::RuntimeException );

    //XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

    SwFmDrawPage*   GetSvxPage();
    void    Invalidate() {pDoc = 0;}
};


class SwShapeDescriptor_Impl;
class SwXGroupShape;
typedef
cppu::WeakAggImplHelper5
<
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::beans::XPropertyState,
    ::com::sun::star::text::XTextContent,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::lang::XUnoTunnel
>
SwXShapeBaseClass;
class SwXShape : public SwXShapeBaseClass,
    public SwClient
{
    friend class SwHTMLImageWatcher;
    friend class SwHTMLParser;
    friend class SwXGroupShape;
    friend class SwXDrawPage;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation >                 xShapeAgg;
    SfxItemPropertySet          aPropSet;
    const SfxItemPropertyMap*   _pMap;
    ::com::sun::star::uno::Sequence< sal_Int8 >*                                              pImplementationId;

    SwShapeDescriptor_Impl*     pImpl;

    sal_Bool                        m_bDescriptor;

    SwFrmFmt*               GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }

    SvxShape*               GetSvxShape();
protected:
    virtual ~SwXShape();
public:
    SwXShape(::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & xShape);


    TYPEINFO();
    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw(::com::sun::star::uno::RuntimeException);

    //XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);


    //XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    //XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    //XTextContent
    virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >  SAL_CALL getAnchor(void) throw( ::com::sun::star::uno::RuntimeException );

    //XComponent
    virtual void SAL_CALL dispose(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );

    //XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

    //SwClient
    virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew);

    SwShapeDescriptor_Impl*     GetDescImpl() {return pImpl;}
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation >                 GetAggregationInterface() {return xShapeAgg;}
};


class SwXGroupShape :
    public SwXShape,
    public ::com::sun::star::drawing::XShapes
{
protected:
    virtual ~SwXGroupShape();
public:
    SwXGroupShape(::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & xShape);


    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

    //XShapes
    virtual void SAL_CALL add( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL remove( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape ) throw (::com::sun::star::uno::RuntimeException);

    //XIndexAccess
    virtual sal_Int32 SAL_CALL getCount(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 nIndex) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException );

    //XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  ) throw(::com::sun::star::uno::RuntimeException);
};
} //namespace binfilter
#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
