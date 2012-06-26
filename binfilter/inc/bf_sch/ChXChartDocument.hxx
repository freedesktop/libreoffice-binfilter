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

#ifndef _SCH_CHXCHARTDOCUMENT_HXX
#define _SCH_CHXCHARTDOCUMENT_HXX

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/chart/XChartDocument.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/chart/XChartData.hpp>
#include <com/sun/star/chart/XChartDataChangeEventListener.hpp>
#include <com/sun/star/chart/XDiagram.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/lang/XEventListener.hpp>

// header for SfxBaseModel
#include <bf_sfx2/sfxbasemodel.hxx>
// header for SvxUnoDrawMSFactory
#include <bf_svx/unomod.hxx>
#include <osl/mutex.hxx>

// header for SvxItemPropertySet
#include <bf_svx/unoprov.hxx>

#include <cppuhelper/weak.hxx>

#include "SchAddInCollection.hxx"
namespace binfilter {

class ChartModel;
class ChXDiagram;
class SchChartDocShell;
class SchMemChart;

class ChXChartDocument :
    public ::com::sun::star::beans::XPropertySet,
    public ::com::sun::star::chart::XChartDocument,
    public ::com::sun::star::lang::XServiceInfo,
    public ::com::sun::star::util::XNumberFormatsSupplier,
    public ::com::sun::star::drawing::XDrawPageSupplier,
    public ::com::sun::star::lang::XUnoTunnel,
    public SfxBaseModel,        // : XModel
    public SvxUnoDrawMSFactory  // : XMultiServiceFactory
{
private:
    ChartModel*         m_pModel;
    SvxItemPropertySet  m_aPropSet;
    SchChartDocShell*   m_pDocShell;

    ::osl::Mutex maMutex;

    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > maTypeSequence;

    ::com::sun::star::uno::Reference< ::com::sun::star::chart::XDiagram > m_rXDiagram;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartData > m_xChartData;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener > m_xEventListener;
    ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier > mrNumberFormatter;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage > mrDrawPage;

    // some shape references (ChXChartObjects)
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxMainTitle;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxSubTitle;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxLegend;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > mxArea;

    // used by AddIns to access core functionality
    ::rtl::OUString maBaseDiagramType;

    // tables for drawing styles
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xDashTable;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xGradientTable;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xHatchTable;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xBitmapTable;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xTransparencyGradientTable;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xMarkerTable;

    static sal_Int32             mnInstanceCounter;
    static SchAddInCollection *  mpAddInCollection;
    static SchAddInCollection &  GetAddInCollection();

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > GetAdditionalShapes();

public:
    ChXChartDocument( SchChartDocShell* pShell );
    virtual ~ChXChartDocument();

    ::osl::Mutex& GetMutex()    { return maMutex; }

    void setDiagramType( const ::rtl::OUString&, sal_Bool bKeepAddin = sal_False ) throw();
    sal_Bool setBaseDiagramType( sal_Bool bSet ) throw();

    void SetChartModel( ChartModel* ) throw();
    void RefreshData( const ::com::sun::star::chart::ChartDataChangeEvent& ) throw();
    void InitNumberFormatter() throw( ::com::sun::star::uno::RuntimeException );
    void ClearNumberFormatter()        { mrNumberFormatter = NULL; }
    ::com::sun::star::uno::Sequence< sal_Int32 > GetTransSequence( SchMemChart* pData, bool bColumns );
    bool SetTransSequence( SchMemChart* pData, bool bColumns, const ::com::sun::star::uno::Sequence< sal_Int32 >& rSeq );

    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId() throw();
    static ChXChartDocument* getImplementation( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > ) throw();

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType )
        throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL acquire() throw();
    virtual void SAL_CALL release() throw();

    // XMultiServiceFactory ( ::SvxUnoDrawMSFactory )
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstance(
        const ::rtl::OUString& aServiceSpecifier )
        throw( ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstanceWithArguments(
        const ::rtl::OUString& ServiceSpecifier,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Arguments )
        throw( ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAvailableServiceNames()
        throw( ::com::sun::star::uno::RuntimeException );

    // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName,
                                            const ::com::sun::star::uno::Any& aValue )
        throw( ::com::sun::star::beans::UnknownPropertyException,
               ::com::sun::star::beans::PropertyVetoException,
               ::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName )
        throw( ::com::sun::star::beans::UnknownPropertyException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
                const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener )
        throw( ::com::sun::star::beans::UnknownPropertyException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
                const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener )
        throw( ::com::sun::star::beans::UnknownPropertyException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName,
                const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
        throw( ::com::sun::star::beans::UnknownPropertyException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
                const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
        throw( ::com::sun::star::beans::UnknownPropertyException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException );

    // XChartDocument
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > SAL_CALL getTitle()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > SAL_CALL getSubTitle()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > SAL_CALL getLegend()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getArea()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart::XDiagram > SAL_CALL getDiagram()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL setDiagram( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XDiagram >& )
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartData > SAL_CALL getData()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL attachData( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartData >& )
        throw( ::com::sun::star::uno::RuntimeException );

    // XModel ( ::SfxBaseModel )
    virtual sal_Bool SAL_CALL attachResource(
        const ::rtl::OUString& aURL,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs )
        throw( ::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getURL() throw( ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getArgs()
        throw( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL connectController(
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >& xController )
        throw( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL disconnectController(
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >& xController )
        throw( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL lockControllers() throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL unlockControllers() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL hasControllersLocked() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController > SAL_CALL getCurrentController()
        throw( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setCurrentController(
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >& xController )
        throw( ::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getCurrentSelection()
        throw( ::com::sun::star::uno::RuntimeException);

    // XTypeProvider ( ::SfxBaseModel )
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId()
        throw( ::com::sun::star::uno::RuntimeException );

    // XComponent ( ::XModel ::SfxBaseModel )
    virtual void SAL_CALL dispose() throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL addEventListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener )
        throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removeEventListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener )
        throw( ::com::sun::star::uno::RuntimeException );

    // XNumberFormatsSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getNumberFormatSettings()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormats > SAL_CALL getNumberFormats()
        throw( ::com::sun::star::uno::RuntimeException );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw( ::com::sun::star::uno::RuntimeException );

    // XDrawPageSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage > SAL_CALL getDrawPage()
        throw( ::com::sun::star::uno::RuntimeException );

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier )
        throw( ::com::sun::star::uno::RuntimeException );

    //  XEventListener
    virtual void SAL_CALL
        disposing
            (const ::com::sun::star::lang::EventObject & Source)
                throw (::com::sun::star::uno::RuntimeException);

    using SfxBaseModel::addEventListener;
    using SfxBaseModel::removeEventListener;
};

} //namespace binfilter
#endif // _SCH_CHXCHARTDOCUMENT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
