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

#include <comphelper/proparrhlp.hxx>

#include <com/sun/star/util/XNumberFormatsSupplier.hpp>

#include <com/sun/star/awt/XKeyListener.hpp>

#include <tools/link.hxx>

#include "FormattedField.hxx"
#include "services.hxx"
#include "property.hrc"
#include "frm_resource.hrc"

#include <comphelper/numbers.hxx>
#include <connectivity/dbtools.hxx>
#include <connectivity/dbconversion.hxx>

#include <bf_svtools/zforlist.hxx>
#include <bf_svtools/numuno.hxx>

#include <vcl/svapp.hxx>
#include <i18npool/mslangid.hxx>
#include <rtl/textenc.h>

#include <com/sun/star/util/NumberFormat.hpp>
#include <com/sun/star/form/XSubmit.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XKeyListener.hpp>
#include <com/sun/star/form/FormComponentType.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/form/XForm.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <osl/mutex.hxx>
    // needed as long as we use the SolarMutex
#include <comphelper/streamsection.hxx>

#include <comphelper/property.hxx>

#include <com/sun/star/beans/PropertyAttribute.hpp>

namespace binfilter {

using namespace dbtools;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::util;

//.........................................................................
namespace frm
{

/*************************************************************************/

class StandardFormatsSupplier : protected SvNumberFormatsSupplierObj
{
protected:
    SvNumberFormatter*  m_pMyPrivateFormatter;

public:
    StandardFormatsSupplier(const Reference<XMultiServiceFactory>& _rxFactory,LanguageType _eSysLanguage);

    operator Reference<XNumberFormatsSupplier> ();
    using SvNumberFormatsSupplierObj::operator new;
    using SvNumberFormatsSupplierObj::operator delete;

protected:
    ~StandardFormatsSupplier() { if (m_pMyPrivateFormatter) delete m_pMyPrivateFormatter; }
};


//------------------------------------------------------------------
StandardFormatsSupplier::StandardFormatsSupplier(const Reference< XMultiServiceFactory > & _rxFactory,LanguageType _eSysLanguage)
    :SvNumberFormatsSupplierObj()
    ,m_pMyPrivateFormatter(new SvNumberFormatter(_rxFactory, _eSysLanguage))
{
    SetNumberFormatter(m_pMyPrivateFormatter);
}

//------------------------------------------------------------------
StandardFormatsSupplier::operator Reference<XNumberFormatsSupplier> ()
{
    return static_cast<XNumberFormatsSupplier*>(static_cast<SvNumberFormatsSupplierObj*>(this));
}

//------------------------------------------------------------------
Reference<XNumberFormatsSupplier>  OFormattedModel::s_xDefaultFormatter;
sal_Int32 OFormattedModel::nValueHandle = -1;

/*************************************************************************/
//------------------------------------------------------------------
InterfaceRef SAL_CALL OFormattedControl_CreateInstance(const Reference<XMultiServiceFactory>& _rxFactory)
{
    return *(new OFormattedControl(_rxFactory));
}

//------------------------------------------------------------------
Sequence<Type> OFormattedControl::_getTypes()
{
    return ::comphelper::concatSequences(
        OFormattedControl_BASE::getTypes(),
        OBoundControl::_getTypes()
    );
}

//------------------------------------------------------------------
Any SAL_CALL OFormattedControl::queryAggregation(const Type& _rType) throw (RuntimeException)
{
    Any aReturn = OBoundControl::queryAggregation(_rType);
    if (!aReturn.hasValue())
        aReturn = OFormattedControl_BASE::queryInterface(_rType);
    return aReturn;
}


DBG_NAME(OFormattedControl)
//------------------------------------------------------------------------------
OFormattedControl::OFormattedControl(const Reference<XMultiServiceFactory>& _rxFactory)
               :OBoundControl(_rxFactory, VCL_CONTROL_FORMATTEDFIELD)
               ,m_nKeyEvent(0)
{
    DBG_CTOR(OFormattedControl,NULL);

    increment(m_refCount);
    {   // als FocusListener anmelden
        Reference<XWindow>  xComp;
        if (query_aggregation(m_xAggregate, xComp))
        {
            xComp->addKeyListener(this);
        }
    }
    // Refcount wieder bei 1 fuer den Listener
    /*sal_Int32 n =*/ decrement(m_refCount);
}

//------------------------------------------------------------------------------
OFormattedControl::~OFormattedControl()
{
    if( m_nKeyEvent )
        Application::RemoveUserEvent( m_nKeyEvent );

    if (!OComponentHelper::rBHelper.bDisposed)
    {
        acquire();
        dispose();
    }

    DBG_DTOR(OFormattedControl,NULL);
}

// XKeyListener
//------------------------------------------------------------------------------
void OFormattedControl::disposing(const EventObject& _rSource) throw(RuntimeException)
{
    OBoundControl::disposing(_rSource);
}

//------------------------------------------------------------------------------
void OFormattedControl::keyPressed(const ::com::sun::star::awt::KeyEvent& e) throw ( ::com::sun::star::uno::RuntimeException)
{
    if( e.KeyCode != KEY_RETURN || e.Modifiers != 0 )
        return;

    // Steht das Control in einem Formular mit einer Submit-URL?
    Reference<com::sun::star::beans::XPropertySet>  xSet(getModel(), UNO_QUERY);
    if( !xSet.is() )
        return;

    Reference<XFormComponent>  xFComp(xSet, UNO_QUERY);
    InterfaceRef  xParent = xFComp->getParent();
    if( !xParent.is() )
        return;

    Reference<com::sun::star::beans::XPropertySet>  xFormSet(xParent, UNO_QUERY);
    if( !xFormSet.is() )
        return;

    Any aTmp(xFormSet->getPropertyValue( PROPERTY_TARGET_URL ));
    if (!isA(aTmp, static_cast< ::rtl::OUString* >(NULL)) ||
        !getString(aTmp).getLength() )
        return;

    Reference<XIndexAccess>  xElements(xParent, UNO_QUERY);
    sal_Int32 nCount = xElements->getCount();
    if( nCount > 1 )
    {

        Reference<com::sun::star::beans::XPropertySet>  xFCSet;
        for( sal_Int32 nIndex=0; nIndex < nCount; nIndex++ )
        {
            //  Any aElement(xElements->getByIndex(nIndex));
            xElements->getByIndex(nIndex) >>= xFCSet;

            if (hasProperty(PROPERTY_CLASSID, xFCSet) &&
                getINT16(xFCSet->getPropertyValue(PROPERTY_CLASSID)) == FormComponentType::TEXTFIELD)
            {
                // Noch ein weiteres Edit gefunden ==> dann nicht submitten
                if (xFCSet != xSet)
                    return;
            }
        }
    }

    // Da wir noch im Haender stehen, submit asynchron ausloesen
    if( m_nKeyEvent )
        Application::RemoveUserEvent( m_nKeyEvent );
    m_nKeyEvent = Application::PostUserEvent( LINK(this, OFormattedControl,
                                            OnKeyPressed) );
}

//------------------------------------------------------------------------------
void OFormattedControl::keyReleased(const ::com::sun::star::awt::KeyEvent& /*e*/) throw ( ::com::sun::star::uno::RuntimeException)
{
}

//------------------------------------------------------------------------------
IMPL_LINK(OFormattedControl, OnKeyPressed, void*, EMPTYARG)
{
    m_nKeyEvent = 0;

    Reference<XFormComponent>  xFComp(getModel(), UNO_QUERY);
    InterfaceRef  xParent = xFComp->getParent();
    Reference<XSubmit>  xSubmit(xParent, UNO_QUERY);
    if (xSubmit.is())
        xSubmit->submit( Reference<XControl> (), ::com::sun::star::awt::MouseEvent() );
    return 0L;
}

//------------------------------------------------------------------------------
StringSequence  OFormattedControl::getSupportedServiceNames() throw()
{
    StringSequence aSupported = OBoundControl::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 1);

    ::rtl::OUString*pArray = aSupported.getArray();
    pArray[aSupported.getLength()-1] = FRM_SUN_CONTROL_FORMATTEDFIELD;
    return aSupported;
}

//------------------------------------------------------------------------------
void OFormattedControl::setDesignMode(sal_Bool bOn) throw ( ::com::sun::star::uno::RuntimeException)
{
    OBoundControl::setDesignMode(bOn);
}

/*************************************************************************/
DBG_NAME(OFormattedModel)
//------------------------------------------------------------------
void OFormattedModel::implConstruct()
{
    // members
    m_bOriginalNumeric = sal_False;
    m_bNumeric = sal_False;
    m_xOriginalFormatter = NULL;
    m_nKeyType = NumberFormat::UNDEFINED;
    m_aNullDate = DBTypeConversion::getStandardDate();
    m_bAggregateListening = sal_False;
    m_pPropertyMultiplexer = NULL;
    m_nFieldType =  DataType::OTHER;

    // default our formats supplier
    increment(m_refCount);
    setPropertyToDefaultByHandle(PROPERTY_ID_FORMATSSUPPLIER);
    decrement(m_refCount);

    startAggregateListening();
    doSetDelegator();
}

//------------------------------------------------------------------
OFormattedModel::OFormattedModel(const Reference<XMultiServiceFactory>& _rxFactory)
    :OEditBaseModel(_rxFactory, VCL_CONTROLMODEL_FORMATTEDFIELD, FRM_CONTROL_FORMATTEDFIELD, sal_False )
                            // use the old control name for compytibility reasons
    ,OErrorBroadcaster( OComponentHelper::rBHelper )
    ,OPropertyChangeListener( m_aMutex )
{
    DBG_CTOR(OFormattedModel, NULL);

    implConstruct();

    m_sDataFieldConnectivityProperty = PROPERTY_EFFECTIVE_VALUE;
    m_nClassId = FormComponentType::TEXTFIELD;

    if (OFormattedModel::nValueHandle == -1)
        OFormattedModel::nValueHandle = getOriginalHandle(PROPERTY_ID_EFFECTIVE_VALUE);
}

//------------------------------------------------------------------
OFormattedModel::OFormattedModel( const OFormattedModel* _pOriginal, const Reference< XMultiServiceFactory >& _rxFactory )
    :OEditBaseModel( _pOriginal, _rxFactory )
    ,OErrorBroadcaster( OComponentHelper::rBHelper )
    ,OPropertyChangeListener( m_aMutex )
{
    DBG_CTOR(OFormattedModel, NULL);

    implConstruct();
}

//------------------------------------------------------------------------------
OFormattedModel::~OFormattedModel()
{
    doResetDelegator();
    releaseAggregateListener();

    DBG_DTOR(OFormattedModel, NULL);
}

// XCloneable
//------------------------------------------------------------------------------
IMPLEMENT_DEFAULT_CLONING( OFormattedModel )

//------------------------------------------------------------------------------
void OFormattedModel::startAggregateListening()
{
    DBG_ASSERT(!m_bAggregateListening, "OFormattedModel::startAggregateListening: already listening!");
    if (m_bAggregateListening)
        return;

    DBG_ASSERT(NULL == m_pPropertyMultiplexer, "OFormattedModel::startAggregateListening: previous listener not released!");
    if (m_pPropertyMultiplexer)
        releaseAggregateListener();

    m_pPropertyMultiplexer = new OPropertyChangeMultiplexer(this, m_xAggregateSet, sal_False);
    m_pPropertyMultiplexer->addProperty(PROPERTY_FORMATKEY);
    m_pPropertyMultiplexer->acquire();

    m_bAggregateListening = sal_True;
}

//------------------------------------------------------------------------------
void OFormattedModel::stopAggregateListening()
{
    DBG_ASSERT(m_bAggregateListening, "OFormattedModel::stopAggregateListening: not listening!");
    if (!m_bAggregateListening)
        return;

    if (m_pPropertyMultiplexer)
        m_pPropertyMultiplexer->dispose();

    m_bAggregateListening = sal_False;
}

//------------------------------------------------------------------------------
void OFormattedModel::releaseAggregateListener()
{
    DBG_ASSERT(!m_bAggregateListening, "OFormattedModel::releaseAggregateListener: still listening!");
    if (m_bAggregateListening)
        stopAggregateListening();

    if (m_pPropertyMultiplexer)
    {
        m_pPropertyMultiplexer->release();
        m_pPropertyMultiplexer = NULL;
    }
}

//------------------------------------------------------------------------------
void SAL_CALL OFormattedModel::disposing()
{
    stopAggregateListening();
    OErrorBroadcaster::disposing();
    OEditBaseModel::disposing();
}

// XServiceInfo
//------------------------------------------------------------------------------
StringSequence OFormattedModel::getSupportedServiceNames() throw()
{
    StringSequence aSupported = OEditBaseModel::getSupportedServiceNames();
    aSupported.realloc(aSupported.getLength() + 2);

    ::rtl::OUString*pArray = aSupported.getArray();
    pArray[aSupported.getLength()-2] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.form.component.DatabaseFormattedField" ));
    pArray[aSupported.getLength()-1] = FRM_SUN_COMPONENT_FORMATTEDFIELD;
    return aSupported;
}

// XAggregation
//------------------------------------------------------------------------------
Any SAL_CALL OFormattedModel::queryAggregation(const Type& _rType) throw(RuntimeException)
{
    Any aReturn = OEditBaseModel::queryAggregation( _rType );
    return aReturn.hasValue() ? aReturn : OErrorBroadcaster::queryInterface( _rType );
}

// XTypeProvider
//------------------------------------------------------------------------------
Sequence< Type > OFormattedModel::_getTypes()
{
    return ::comphelper::concatSequences(
        OEditBaseModel::_getTypes(),
        OErrorBroadcaster::getTypes()
    );
}

// XPersistObject
//------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OFormattedModel::getServiceName() throw ( ::com::sun::star::uno::RuntimeException)
{
    return ::rtl::OUString(FRM_COMPONENT_EDIT);
//  return ::rtl::OUString(FRM_COMPONENT_FORMATTEDFIELD);   // old (non-sun) name for compatibility !
}

// XPropertySet
//------------------------------------------------------------------------------
Reference<com::sun::star::beans::XPropertySetInfo> SAL_CALL OFormattedModel::getPropertySetInfo() throw(RuntimeException)
{
    Reference<com::sun::star::beans::XPropertySetInfo>  xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}

//------------------------------------------------------------------------------
::cppu::IPropertyArrayHelper& OFormattedModel::getInfoHelper()
{
    return *const_cast<OFormattedModel*>(this)->getArrayHelper();
}

//------------------------------------------------------------------------------
void OFormattedModel::fillProperties(
                Sequence< com::sun::star::beans::Property >& _rProps,
                Sequence< com::sun::star::beans::Property >& _rAggregateProps ) const
{
    FRM_BEGIN_PROP_HELPER(10)
        DECL_PROP1(NAME,                ::rtl::OUString,        BOUND);
        DECL_PROP2(CLASSID,             sal_Int16,              READONLY, TRANSIENT);
        DECL_BOOL_PROP1(EMPTY_IS_NULL,                          BOUND);
        DECL_PROP1(TAG,                 ::rtl::OUString,        BOUND);
        DECL_PROP1(TABINDEX,            sal_Int16,              BOUND);
        DECL_PROP1(CONTROLSOURCE,       ::rtl::OUString,        BOUND);
        DECL_IFACE_PROP3(BOUNDFIELD,    com::sun::star::beans::XPropertySet,BOUND,READONLY, TRANSIENT);
        DECL_BOOL_PROP2(FILTERPROPOSAL,                         BOUND, MAYBEDEFAULT);
        DECL_IFACE_PROP2(CONTROLLABEL,  com::sun::star::beans::XPropertySet,BOUND, MAYBEVOID);
        DECL_PROP2(CONTROLSOURCEPROPERTY,   rtl::OUString,  READONLY, TRANSIENT);

        // the supplier is readonly for us
//      ModifyPropertyAttributes(_rAggregateProps, PROPERTY_FORMATSSUPPLIER, PropertyAttribute::READONLY, 0);
            // remove this. The property does not need to be readonly anymore.
            //
            // The original idea why we made this readonly is that we change the supplier ourself when
            // the form which we belong to is loaded, and then we use the supplier from the data source
            // the for is bound to.
            // But this does not contradict (like I originally thought) the possibility to change the
            // formatter from outside as well. If it is changed when the form is not yet loaded,
            // then we will overwrite this upon loading, anyway. If it is changed while the form
            // is loaded, then this does no harm, too: The format is preserved (the aggregate cares for this),
            // and upon unloading, we restore the old formatter.

        // TreatAsNumeric nicht transient : wir wollen es an der UI anbinden (ist noetig, um dem EffectiveDefault
        // - der kann Text oder Zahl sein - einen Sinn zu geben)
        ModifyPropertyAttributes(_rAggregateProps, PROPERTY_TREATASNUMERIC, 0, PropertyAttribute::TRANSIENT);
        // same for FormatKey
        // (though the paragraph above for the TreatAsNumeric does not hold anymore - we do not have an UI for this.
        // But we have for the format key ...)
        ModifyPropertyAttributes(_rAggregateProps, PROPERTY_FORMATKEY, 0, PropertyAttribute::TRANSIENT);

        RemoveProperty(_rAggregateProps, PROPERTY_STRICTFORMAT);
            // no strict format property for formatted fields: it does not make sense, 'cause
            // there is no general way to decide which characters/sub strings are allowed during the input of an
            // arbitraryly formatted control
    FRM_END_PROP_HELPER();
}

//------------------------------------------------------------------------------
void OFormattedModel::getFastPropertyValue(Any& rValue, sal_Int32 nHandle) const
{
    OEditBaseModel::getFastPropertyValue(rValue, nHandle);
}

//------------------------------------------------------------------------------
void OFormattedModel::setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const Any& rValue) throw ( ::com::sun::star::uno::Exception)
{
    OEditBaseModel::setFastPropertyValue_NoBroadcast(nHandle, rValue);
}

//------------------------------------------------------------------------------
sal_Bool OFormattedModel::convertFastPropertyValue(Any& rConvertedValue, Any& rOldValue, sal_Int32 nHandle, const Any& rValue)
                                                        throw( IllegalArgumentException )
{
    return OEditBaseModel::convertFastPropertyValue(rConvertedValue, rOldValue, nHandle, rValue);
}

//------------------------------------------------------------------------------
void OFormattedModel::setPropertyToDefaultByHandle(sal_Int32 nHandle)
{
    if (nHandle == PROPERTY_ID_FORMATSSUPPLIER)
    {   // das aggregierte Model koennte auf die Idee kommen
        Reference<XNumberFormatsSupplier>  xSupplier = calcDefaultFormatsSupplier();
        DBG_ASSERT(m_xAggregateSet.is(), "OFormattedModel::setPropertyToDefaultByHandle(FORMATSSUPPLIER) : have no aggregate !");
        if (m_xAggregateSet.is())
            m_xAggregateSet->setPropertyValue(PROPERTY_FORMATSSUPPLIER, makeAny(xSupplier));
    }
    else
        OEditBaseModel::setPropertyToDefaultByHandle(nHandle);
}

//------------------------------------------------------------------------------
void OFormattedModel::setPropertyToDefault(const ::rtl::OUString& aPropertyName) throw( com::sun::star::beans::UnknownPropertyException, RuntimeException )
{
    OPropertyArrayAggregationHelper& rPH = (OPropertyArrayAggregationHelper&)getInfoHelper();
    sal_Int32 nHandle = rPH.getHandleByName( aPropertyName );

    if (nHandle == PROPERTY_ID_FORMATSSUPPLIER)
        setPropertyToDefaultByHandle(PROPERTY_ID_FORMATSSUPPLIER);
    else
        OEditBaseModel::setPropertyToDefault(aPropertyName);
}

//------------------------------------------------------------------------------
Any OFormattedModel::getPropertyDefaultByHandle( sal_Int32 nHandle ) const
{
    if (nHandle == PROPERTY_ID_FORMATSSUPPLIER)
    {
        Reference<XNumberFormatsSupplier>  xSupplier = calcDefaultFormatsSupplier();
        return makeAny(xSupplier);
    }
    else
        return OEditBaseModel::getPropertyDefaultByHandle(nHandle);
}

//------------------------------------------------------------------------------
Any SAL_CALL OFormattedModel::getPropertyDefault( const ::rtl::OUString& aPropertyName ) throw( com::sun::star::beans::UnknownPropertyException, RuntimeException )
{
    OPropertyArrayAggregationHelper& rPH = (OPropertyArrayAggregationHelper&)getInfoHelper();
    sal_Int32 nHandle = rPH.getHandleByName( aPropertyName );

    if (nHandle == PROPERTY_ID_FORMATSSUPPLIER)
        return getPropertyDefaultByHandle(PROPERTY_ID_FORMATSSUPPLIER);
    else
        return OEditBaseModel::getPropertyDefault(aPropertyName);
}

//------------------------------------------------------------------------------
void OFormattedModel::_propertyChanged( const com::sun::star::beans::PropertyChangeEvent& evt ) throw(RuntimeException)
{
    Reference< XPropertySet > xSourceSet(evt.Source, UNO_QUERY);
    if (xSourceSet.get() == m_xAggregateSet.get())
    {
        if (evt.PropertyName.equals(PROPERTY_FORMATKEY))
        {
            if (evt.NewValue.getValueType().getTypeClass() == TypeClass_LONG)
            {
                try
                {
                    Reference<XNumberFormatsSupplier> xSupplier(calcFormatsSupplier());
                    m_nKeyType  = getNumberFormatType(xSupplier->getNumberFormats(), getINT32(evt.NewValue));
                    // as m_aSaveValue (which is used by _commit) is format dependent we have
                    // to recalc it, which is done by _onValueChanged
                    if (m_xColumn.is() && m_xAggregateFastSet.is())
                    {
                        ::osl::MutexGuard aGuard(m_aMutex);     // _onValueChanged expects that ...
                        //_onValueChanged();
                    }
                }
                catch(Exception&)
                {
                }
            }
        }
        // our base class does not listen at the aggregate at the moment ... and it does not expect
        // to get events from objects other than the field we're bound to. So do not call the
        // base class method here.
        return;
    }
}

//------------------------------------------------------------------------------
Reference<XNumberFormatsSupplier>  OFormattedModel::calcFormatsSupplier() const
{
    Reference<XNumberFormatsSupplier>  xSupplier;

    DBG_ASSERT(m_xAggregateSet.is(), "OFormattedModel::calcFormatsSupplier : have no aggregate !");
    // hat mein aggregiertes Model einen FormatSupplier ?
    if( m_xAggregateSet.is() )

        m_xAggregateSet->getPropertyValue(PROPERTY_FORMATSSUPPLIER) >>= xSupplier;
    if (!xSupplier.is())
        // check if my parent form has a supplier
        xSupplier = calcFormFormatsSupplier();

    if (!xSupplier.is())
        xSupplier = calcDefaultFormatsSupplier();

    DBG_ASSERT(xSupplier.is(), "OFormattedModel::calcFormatsSupplier : no supplier !");
        // jetzt sollte aber einer da sein
    return xSupplier;
}

//------------------------------------------------------------------------------
Reference<XNumberFormatsSupplier>  OFormattedModel::calcFormFormatsSupplier() const
{
    Reference<XChild>  xMe;
    query_interface(static_cast<XWeak*>(const_cast<OFormattedModel*>(this)), xMe);
    // damit stellen wir sicher, dass wir auch fuer den Fall der Aggregation das richtige
    // Objekt bekommen
    DBG_ASSERT(xMe.is(), "OFormattedModel::calcFormFormatsSupplier : I should have a content interface !");

    // jetzt durchhangeln nach oben, bis wir auf eine starform treffen (angefangen mit meinem eigenen Parent)
    Reference<XChild>  xParent(xMe->getParent(), UNO_QUERY);
    Reference<XForm>  xNextParentForm(xParent, UNO_QUERY);
    while (!xNextParentForm.is() && xParent.is())
    {
        xParent         = xParent.query( xParent->getParent() );
        xNextParentForm = xNextParentForm.query( xParent );
    }

    if (!xNextParentForm.is())
    {
        OSL_FAIL("OFormattedModel::calcFormFormatsSupplier : have no ancestor which is a form !");
        return NULL;
    }

    // den FormatSupplier von meinem Vorfahren (falls der einen hat)
    Reference< XRowSet > xRowSet( xNextParentForm, UNO_QUERY );
    Reference< XNumberFormatsSupplier > xSupplier;
    if (xRowSet.is())
        xSupplier = getNumberFormats(getConnection(xRowSet), sal_True, m_xServiceFactory);
    return xSupplier;
}

//------------------------------------------------------------------------------
Reference<XNumberFormatsSupplier>  OFormattedModel::calcDefaultFormatsSupplier() const
{
    if ( !s_xDefaultFormatter.is() )
    {
        // get the Office's UI locale
        const Locale& rSysLocale = SvtSysLocale().GetLocaleData().getLocale();
        // translate
        LanguageType eSysLanguage = MsLangId::convertLocaleToLanguage( rSysLocale );
        s_xDefaultFormatter = *new StandardFormatsSupplier(m_xServiceFactory,eSysLanguage);
    }
    return s_xDefaultFormatter;
}

// XBoundComponent
//------------------------------------------------------------------------------
void OFormattedModel::loaded(const EventObject& rEvent) throw ( ::com::sun::star::uno::RuntimeException)
{
    // HACK : our _loaded accesses our NumberFormatter which locks the solar mutex (as it doesn't have
    // an own one). To prevent deadlocks with other threads which may request a property from us in an
    // UI-triggered action (e.g. an tooltip) we lock the solar mutex _here_ before our base class locks
    // it's own muext (which is used for property requests)
    // alternative a): we use two mutexes, one which is passed to the OPropertysetHelper and used for
    // property requests and one for our own code. This would need a lot of code rewriting
    // alternative b): The NumberFormatter has to be really threadsafe (with an own mutex), which is
    // the only "clean" solution for me.

    SolarMutexGuard aGuard;
    OEditBaseModel::loaded(rEvent);
}

//------------------------------------------------------------------------------
void OFormattedModel::write(const Reference<XObjectOutputStream>& _rxOutStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    OEditBaseModel::write(_rxOutStream);
    _rxOutStream->writeShort(0x0003);

    DBG_ASSERT(m_xAggregateSet.is(), "OFormattedModel::write : have no aggregate !");

    // mein Format (evtl. void) in ein persistentes Format bringen (der Supplier zusammen mit dem Key ist es zwar auch,
    // aber deswegen muessen wir ja nicht gleich den ganzen Supplier speichern, das waere ein klein wenig Overhead ;)

        Reference<XNumberFormatsSupplier>  xSupplier;
        Any aFmtKey;
    sal_Bool bVoidKey = sal_True;
    if (m_xAggregateSet.is())
    {
                Any aSupplier = m_xAggregateSet->getPropertyValue(PROPERTY_FORMATSSUPPLIER);
                if (aSupplier.getValueType().getTypeClass() != TypeClass_VOID)
        {
            DBG_ASSERT(isAReference(aSupplier, static_cast<XNumberFormatsSupplier*>(NULL)),
                "OFormattedModel::write : invalid formats supplier !");
                        xSupplier = *(Reference<XNumberFormatsSupplier> *)aSupplier.getValue();
        }

        aFmtKey = m_xAggregateSet->getPropertyValue(PROPERTY_FORMATKEY);
        bVoidKey = (!xSupplier.is() || !aFmtKey.hasValue()) || (m_bLoaded && m_xOriginalFormatter.is());
            // (kein Fomatter und/oder Key) oder (loaded und faked Formatter)
    }

    _rxOutStream->writeBoolean(!bVoidKey);
    if (!bVoidKey)
    {
        // aus dem FormatKey und dem Formatter persistente Angaben basteln

        Any aKey = m_xAggregateSet->getPropertyValue(PROPERTY_FORMATKEY);
        sal_Int32 nKey = aKey.hasValue() ? getINT32(aKey) : 0;

        Reference<XNumberFormats>  xFormats = xSupplier->getNumberFormats();

        ::rtl::OUString         sFormatDescription;
        LanguageType    eFormatLanguage = LANGUAGE_DONTKNOW;

        static const ::rtl::OUString s_aLocaleProp( RTL_CONSTASCII_USTRINGPARAM( "Locale" ));
        Reference<com::sun::star::beans::XPropertySet>  xFormat = xFormats->getByKey(nKey);
        if (hasProperty(s_aLocaleProp, xFormat))
        {
            Any aLocale = xFormat->getPropertyValue(s_aLocaleProp);
            DBG_ASSERT(isA(aLocale, static_cast<Locale*>(NULL)), "OFormattedModel::write : invalid language property !");
            if (isA(aLocale, static_cast<Locale*>(NULL)))
            {
                Locale* pLocale = (Locale*)aLocale.getValue();
                eFormatLanguage = MsLangId::convertLocaleToLanguage( *pLocale );
            }
        }

        static const ::rtl::OUString s_aFormatStringProp( RTL_CONSTASCII_USTRINGPARAM( "FormatString" ));
        if (hasProperty(s_aFormatStringProp, xFormat))
            xFormat->getPropertyValue(s_aFormatStringProp) >>= sFormatDescription;

        _rxOutStream->writeUTF(sFormatDescription);
        _rxOutStream->writeLong((sal_Int32)eFormatLanguage);
    }

    // version 2 : write the properties common to all OEditBaseModels
    writeCommonEditProperties(_rxOutStream);

    // version 3 : write the effective value property of the aggregate
    // Due to a bug within the UnoControlFormattedFieldModel implementation (our default aggregate) this props value isn't correctly read
    // and this can't be corrected without being incompatible.
    // so we have our own handling.

    // and to be a little bit more compatible we make the following section skippable
    {
        Reference< XDataOutputStream > xOut(_rxOutStream, UNO_QUERY);
        OStreamSection aDownCompat(xOut);

        // a sub version within the skippable block
        _rxOutStream->writeShort(0x0000);

        // version 0: the effective value of the aggregate
                Any aEffectiveValue;
        if (m_xAggregateSet.is())
        {
            try { aEffectiveValue = m_xAggregateSet->getPropertyValue(PROPERTY_EFFECTIVE_VALUE); } catch(Exception&) { }
        }

        {
            OStreamSection aDownCompat2(xOut);
            switch (aEffectiveValue.getValueType().getTypeClass())
            {
                case TypeClass_STRING:
                    _rxOutStream->writeShort(0x0000);
                    _rxOutStream->writeUTF(::comphelper::getString(aEffectiveValue));
                    break;
                case TypeClass_DOUBLE:
                    _rxOutStream->writeShort(0x0001);
                    _rxOutStream->writeDouble(::comphelper::getDouble(aEffectiveValue));
                    break;
                default:    // void and all unknown states
                    DBG_ASSERT(!aEffectiveValue.hasValue(), "FmXFormattedModel::write : unknown property value type !");
                    _rxOutStream->writeShort(0x0002);
                    break;
            }
        }
    }
}

//------------------------------------------------------------------------------
void OFormattedModel::read(const Reference<XObjectInputStream>& _rxInStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException)
{
    OEditBaseModel::read(_rxInStream);
    sal_uInt16 nVersion = _rxInStream->readShort();

    Reference<XNumberFormatsSupplier>  xSupplier;
    sal_Int32 nKey = -1;
    switch (nVersion)
    {
        case 0x0001 :
        case 0x0002 :
        case 0x0003 :
        {
            sal_Bool bNonVoidKey = _rxInStream->readBoolean();
            if (bNonVoidKey)
            {
                // den String und die Language lesen ....
                ::rtl::OUString sFormatDescription = _rxInStream->readUTF();
                LanguageType eDescriptionLanguage = (LanguageType)_rxInStream->readLong();

                // und daraus von einem Formatter zu einem Key zusammenwuerfeln lassen ...
                xSupplier = calcFormatsSupplier();
                    // calcFormatsSupplier nimmt erst den vom Model, dann einen von der starform, dann einen ganz neuen ....
                Reference<XNumberFormats>  xFormats = xSupplier->getNumberFormats();

                if (xFormats.is())
                {
                    Locale aDescriptionLanguage(
                            MsLangId::convertLanguageToLocale(eDescriptionLanguage));

                    nKey = xFormats->queryKey(sFormatDescription, aDescriptionLanguage, sal_False);
                    if (nKey == (sal_Int32)-1)
                    {   // noch nicht vorhanden in meinem Formatter ...
                        nKey = xFormats->addNew(sFormatDescription, aDescriptionLanguage);
                    }
                }
            }
            if ((nVersion == 0x0002) || (nVersion == 0x0003))
                readCommonEditProperties(_rxInStream);

            if (nVersion == 0x0003)
            {   // since version 3 there is a "skippable" block at this position
                Reference< XDataInputStream > xIn(_rxInStream, UNO_QUERY);
                OStreamSection aDownCompat(xIn);

                /*sal_Int16 nSubVersion =*/ _rxInStream->readShort();

                // version 0 and higher : the "effective value" property
                                Any aEffectiveValue;
                {
                    OStreamSection aDownCompat2(xIn);
                    switch (_rxInStream->readShort())
                    {
                        case 0: // String
                            aEffectiveValue <<= _rxInStream->readUTF();
                            break;
                        case 1: // double
                            aEffectiveValue <<= (double)_rxInStream->readDouble();
                            break;
                        case 2:
                            break;
                        case 3:
                            OSL_FAIL("FmXFormattedModel::read : unknown effective value type !");
                    }
                }

                // this property is only to be set if we have no control source : in all other cases the base class did a
                // reset after it's read and this set the effective value to a default value
                if (m_xAggregateSet.is() && (m_aControlSource.getLength() == 0))
                {
                    try
                    {
                        m_xAggregateSet->setPropertyValue(PROPERTY_EFFECTIVE_VALUE, aEffectiveValue);
                    }
                    catch(Exception&)
                    {
                    }
                }
            }
        }
        break;
        default :
            OSL_FAIL("OFormattedModel::read : unknown version !");
            // dann bleibt das Format des aggregierten Sets, wie es bei der Erzeugung ist : void
            defaultCommonEditProperties();
            break;
    }

    if ((nKey != -1) && m_xAggregateSet.is())
    {
                m_xAggregateSet->setPropertyValue(PROPERTY_FORMATSSUPPLIER, makeAny(xSupplier));
                m_xAggregateSet->setPropertyValue(PROPERTY_FORMATKEY, makeAny((sal_Int32)nKey));
    }
    else
    {
        setPropertyToDefault(PROPERTY_FORMATSSUPPLIER);
        setPropertyToDefault(PROPERTY_FORMATKEY);
    }
}

//------------------------------------------------------------------------------
sal_Int16 OFormattedModel::getPersistenceFlags() const
{
    return (OEditBaseModel::getPersistenceFlags() & ~PF_HANDLE_COMMON_PROPS);
    // a) we do our own call to writeCommonEditProperties
}

//.........................................................................
}
//.........................................................................





}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
