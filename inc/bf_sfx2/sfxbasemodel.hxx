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

#ifndef _SFX_SFXBASEMODEL_HXX_
#define _SFX_SFXBASEMODEL_HXX_

#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#include <com/sun/star/document/XEventBroadcaster.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/view/XPrintable.hpp>
#include <com/sun/star/view/XPrintJobBroadcaster.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/frame/XLoadable.hpp>
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <cppuhelper/weak.hxx>
#include <com/sun/star/script/XStarBasicAccess.hpp>
#include <tools/link.hxx>
#include <com/sun/star/document/XViewDataSupplier.hpp>

#include <bf_svtools/lstner.hxx>
#include <tools/debug.hxx> //for strip

namespace binfilter {

// Some defines to write better code :-)
#define XCHILD                  ::com::sun::star::container::XChild
#define XNAMECONTAINER          ::com::sun::star::container::XNameContainer
#define XCONTROLLER             ::com::sun::star::frame::XController
#define XEVENTLISTENER          ::com::sun::star::lang::XEventListener
#define XINTERFACE              ::com::sun::star::uno::XInterface
#define XCOMPONENT              ::com::sun::star::lang::XComponent
#define XMODEL                  ::com::sun::star::frame::XModel
#define XMODIFIABLE             ::com::sun::star::util::XModifiable
#define XMODIFIABLE             ::com::sun::star::util::XModifiable
#define XMODIFYBROADCASTER      ::com::sun::star::util::XModifyBroadcaster
#define XMODIFYLISTENER         ::com::sun::star::util::XModifyListener
#define XCLOSEABLE              ::com::sun::star::util::XCloseable
#define XCLOSEBROADCASTER       ::com::sun::star::util::XCloseBroadcaster
#define XCLOSELISTENER          ::com::sun::star::util::XCloseListener
#define XPRINTABLE              ::com::sun::star::view::XPrintable
#define XPRINTJOBBROADCASTER    ::com::sun::star::view::XPrintJobBroadcaster
#define XSTORABLE               ::com::sun::star::frame::XStorable
#define XLOADABLE               ::com::sun::star::frame::XLoadable
#define XTYPEPROVIDER           ::com::sun::star::lang::XTypeProvider
#define XSTARBASICACCESS        ::com::sun::star::script::XStarBasicAccess
#define NOSUCHELEMENTEXCEPTION  ::com::sun::star::container::NoSuchElementException
#define ELEMENTEXISTEXCEPTION   ::com::sun::star::container::ElementExistException
#define XNAMEREPLACE            ::com::sun::star::container::XNameReplace

#define DOCEVENTOBJECT          ::com::sun::star::document::EventObject
#define XDOCEVENTLISTENER       ::com::sun::star::document::XEventListener
#define XDOCUMENTINFO           ::com::sun::star::document::XDocumentInfo
#define XDOCUMENTINFOSUPPLIER   ::com::sun::star::document::XDocumentInfoSupplier
#define XEVENTBROADCASTER       ::com::sun::star::document::XEventBroadcaster
#define XEVENTSSUPPLIER         ::com::sun::star::document::XEventsSupplier

#define NOSUPPORTEXCEPTION      ::com::sun::star::lang::NoSupportException
#define RUNTIMEEXCEPTION        ::com::sun::star::uno::RuntimeException
#define CLOSEVETOEXCEPTION      ::com::sun::star::util::CloseVetoException

#define ANY                     ::com::sun::star::uno::Any
#define EVENTOBJECT             ::com::sun::star::lang::EventObject
#define PROPERTYVALUE           ::com::sun::star::beans::PropertyValue
#define REFERENCE               ::com::sun::star::uno::Reference
#define SEQUENCE                ::com::sun::star::uno::Sequence
#define MUTEX                   ::osl::Mutex
#define OUSTRING                ::rtl::OUString
#define OWEAKOBJECT             ::cppu::OWeakObject
#define UNOTYPE                 ::com::sun::star::uno::Type
#define XVIEWDATASUPPLIER       ::com::sun::star::document::XViewDataSupplier
#define XINDEXACCESS            ::com::sun::star::container::XIndexAccess
#define XTRANSFERABLE           ::com::sun::star::datatransfer::XTransferable
#define DATAFLAVOR              ::com::sun::star::datatransfer::DataFlavor

//________________________________________________________________________________________________________
//  forwards
//________________________________________________________________________________________________________
class   SfxPrinter;
class   SfxViewShell;
class   SfxObjectShell                      ;
class   SfxEventHint;
struct  IMPL_SfxBaseModel_DataContainer     ;   // impl. struct to hold member of class SfxBaseModel

//________________________________________________________________________________________________________
//  class declarations
//________________________________________________________________________________________________________

struct IMPL_SfxBaseModel_MutexContainer
{
    MUTEX m_aMutex ;
} ;

/**_______________________________________________________________________________________________________
    @short      -
    @descr      -
    @implements XChild
                XComponent
                XDocumentInfoSupplier
                XEventListener
                XModel
                XModifiable
                XPrintable
                XStorable
                ::document::XEventBroadcaster
                ::document::XEventsSupplier
                XCloseable
                XCloseBroadcaster
    @base       IMPL_MutexContainer
                 SfxListener
*/

class SfxBaseModel  :   public XTYPEPROVIDER
                    ,   public XCHILD
                    ,   public XDOCUMENTINFOSUPPLIER
                    ,   public XEVENTBROADCASTER
                    ,   public XEVENTLISTENER
                    ,   public XEVENTSSUPPLIER
                    ,   public XMODEL
                    ,   public XMODIFIABLE
                    ,   public XPRINTABLE
                    ,   public XPRINTJOBBROADCASTER
                    ,   public XSTORABLE
                    ,   public XLOADABLE
                    ,   public XSTARBASICACCESS
                    ,   public XVIEWDATASUPPLIER
                    ,   public XCLOSEABLE           // => XCLOSEBROADCASTER
                    ,   public XTRANSFERABLE
                    ,   public IMPL_SfxBaseModel_MutexContainer
                    ,   public SfxListener
                    ,   public OWEAKOBJECT
{

//________________________________________________________________________________________________________
//  public methods
//________________________________________________________________________________________________________

public:

    //____________________________________________________________________________________________________
    //  constructor/destructor
    //____________________________________________________________________________________________________

    SfxBaseModel( SfxObjectShell *pObjektShell = NULL ) ;

    virtual ~SfxBaseModel() ;

    //____________________________________________________________________________________________________
    //  XInterface
    //____________________________________________________________________________________________________

    /**___________________________________________________________________________________________________
        @short      give answer, if interface is supported
        @descr      The interfaces are searched by type.
        @seealso    XInterface
        @param      "rType" is the type of searched interface.
        @return     Any     information about found interface
        @onerror    A RuntimeException is thrown.
    */
    virtual ANY SAL_CALL queryInterface( const UNOTYPE& rType ) throw( RUNTIMEEXCEPTION ) ;

    /**___________________________________________________________________________________________________
        @short      increment refcount
        @descr      -
        @seealso    XInterface
        @seealso    release()
        @param      -
        @return     -
        @onerror    A RuntimeException is thrown.
    */
    virtual void SAL_CALL acquire() throw() ;

    /**___________________________________________________________________________________________________
        @short      decrement refcount
        @descr      -
        @seealso    XInterface
        @seealso    acquire()
        @param      -
        @return     -
        @onerror    A RuntimeException is thrown.
    */
    virtual void SAL_CALL release() throw() ;

    //____________________________________________________________________________________________________
    //  XTypeProvider
    //____________________________________________________________________________________________________

    /**___________________________________________________________________________________________________
        @short      get information about supported interfaces
        @descr      -
        @seealso    XTypeProvider
        @param      -
        @return     Sequence of types of all supported interfaces
        @onerror    A RuntimeException is thrown.
    */
    virtual SEQUENCE< UNOTYPE > SAL_CALL getTypes() throw( RUNTIMEEXCEPTION ) ;

    /**___________________________________________________________________________________________________
        @short      get implementation id
        @descr      This ID is neccessary for UNO-caching. If there no ID, cache is disabled.
                    Another way, cache is enabled.
        @seealso    XTypeProvider
        @param      -
        @return     ID as Sequence of byte
        @onerror    A RuntimeException is thrown.
    */
    virtual SEQUENCE< sal_Int8 > SAL_CALL getImplementationId() throw( RUNTIMEEXCEPTION ) ;

    //____________________________________________________________________________________________________
    //  XStarBasicAccess
    //____________________________________________________________________________________________________

    /**___________________________________________________________________________________________________
        @seealso    XStarBasicAccess
    */
    virtual REFERENCE< XNAMECONTAINER > SAL_CALL getLibraryContainer() throw( RUNTIMEEXCEPTION );

    /**___________________________________________________________________________________________________
        @seealso    XStarBasicAccess
    */
    virtual void SAL_CALL createLibrary( const OUSTRING& LibName, const OUSTRING& Password,
        const OUSTRING& ExternalSourceURL, const OUSTRING& LinkTargetURL )
            throw(ELEMENTEXISTEXCEPTION, RUNTIMEEXCEPTION);

    /**___________________________________________________________________________________________________
        @seealso    XStarBasicAccess
    */
    virtual void SAL_CALL addModule( const OUSTRING& LibraryName, const OUSTRING& ModuleName,
        const OUSTRING& Language, const OUSTRING& Source )
            throw( NOSUCHELEMENTEXCEPTION, RUNTIMEEXCEPTION);

    /**___________________________________________________________________________________________________
        @seealso    XStarBasicAccess
    */
    virtual void SAL_CALL addDialog( const OUSTRING& LibraryName, const OUSTRING& DialogName,
        const ::com::sun::star::uno::Sequence< sal_Int8 >& Data )
            throw(NOSUCHELEMENTEXCEPTION, RUNTIMEEXCEPTION);

    //____________________________________________________________________________________________________
    //  XChild
    //____________________________________________________________________________________________________
    virtual REFERENCE< XINTERFACE > SAL_CALL getParent() throw( RUNTIMEEXCEPTION ) ;

    virtual void SAL_CALL setParent( const REFERENCE< XINTERFACE >& xParent ) throw(    NOSUPPORTEXCEPTION  ,
                                                                                        RUNTIMEEXCEPTION    ) ;

    //____________________________________________________________________________________________________
    //  XComponent
    //____________________________________________________________________________________________________
    virtual void SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL addEventListener(const REFERENCE< XEVENTLISTENER >& aListener) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeEventListener(const REFERENCE< XEVENTLISTENER >& aListener ) throw (::com::sun::star::uno::RuntimeException);

    //____________________________________________________________________________________________________
    //  XDocumentInfoSupplier
    //____________________________________________________________________________________________________
    virtual REFERENCE< XDOCUMENTINFO > SAL_CALL getDocumentInfo() throw (::com::sun::star::uno::RuntimeException);

    //____________________________________________________________________________________________________
    //  XEventListener
    //____________________________________________________________________________________________________
    virtual void SAL_CALL disposing( const EVENTOBJECT& aEvent ) throw (::com::sun::star::uno::RuntimeException);

    //____________________________________________________________________________________________________
    //  XModel
    //____________________________________________________________________________________________________
    virtual sal_Bool SAL_CALL attachResource(   const   OUSTRING&                   sURL    ,
                                                const   SEQUENCE< PROPERTYVALUE >&  aArgs   )
        throw (::com::sun::star::uno::RuntimeException);

    virtual OUSTRING SAL_CALL getURL() throw (::com::sun::star::uno::RuntimeException);

    virtual SEQUENCE< PROPERTYVALUE > SAL_CALL getArgs() throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL connectController( const REFERENCE< XCONTROLLER >& xController ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL disconnectController( const REFERENCE< XCONTROLLER >& xController ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL lockControllers() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL unlockControllers() throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasControllersLocked() throw (::com::sun::star::uno::RuntimeException);
    virtual REFERENCE< XCONTROLLER > SAL_CALL getCurrentController() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setCurrentController( const REFERENCE< XCONTROLLER >& xController )
        throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::uno::RuntimeException);

    virtual REFERENCE< XINTERFACE > SAL_CALL getCurrentSelection() throw (::com::sun::star::uno::RuntimeException);

    //____________________________________________________________________________________________________
    //  XModifiable
    //____________________________________________________________________________________________________
    virtual sal_Bool SAL_CALL isModified() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setModified( sal_Bool bModified )
        throw (::com::sun::star::beans::PropertyVetoException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addModifyListener( const REFERENCE< XMODIFYLISTENER >& xListener ) throw( RUNTIMEEXCEPTION ) ;
    virtual void SAL_CALL removeModifyListener(const REFERENCE< XMODIFYLISTENER > & xListener) throw( RUNTIMEEXCEPTION ) ;

    //____________________________________________________________________________________________________
    //  XCloseable
    //____________________________________________________________________________________________________
    virtual void SAL_CALL close( sal_Bool bDeliverOwnership ) throw (CLOSEVETOEXCEPTION, RUNTIMEEXCEPTION);

    //____________________________________________________________________________________________________
    //  XCloseBroadcaster
    //____________________________________________________________________________________________________
    virtual void SAL_CALL addCloseListener( const REFERENCE< XCLOSELISTENER >& xListener ) throw (RUNTIMEEXCEPTION);
    virtual void SAL_CALL removeCloseListener( const REFERENCE< XCLOSELISTENER >& xListener ) throw (RUNTIMEEXCEPTION);

    //____________________________________________________________________________________________________
    //  XPrintJobBroadcaster
    //____________________________________________________________________________________________________
    virtual void SAL_CALL addPrintJobListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XPrintJobListener >& )throw (::com::sun::star::uno::RuntimeException) {DBG_BF_ASSERT(0, "STRIP");}
    virtual void SAL_CALL removePrintJobListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XPrintJobListener >& ) throw (::com::sun::star::uno::RuntimeException) {DBG_BF_ASSERT(0, "STRIP");}

    //____________________________________________________________________________________________________
    //  XPrintable
    //____________________________________________________________________________________________________
    virtual SEQUENCE< PROPERTYVALUE > SAL_CALL getPrinter() throw (::com::sun::star::uno::RuntimeException) {DBG_BF_ASSERT(0, "STRIP");SEQUENCE< PROPERTYVALUE > aFlavorSeq( 0 );return aFlavorSeq;}
    virtual void SAL_CALL setPrinter( const SEQUENCE< PROPERTYVALUE >& )
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException) {DBG_BF_ASSERT(0, "STRIP");}

    virtual void SAL_CALL print( const SEQUENCE< PROPERTYVALUE >& seqOptions )
        throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

    //____________________________________________________________________________________________________
    //  XStorable
    //____________________________________________________________________________________________________
    virtual sal_Bool SAL_CALL hasLocation() throw (::com::sun::star::uno::RuntimeException);
    virtual OUSTRING SAL_CALL getLocation() throw (::com::sun::star::uno::RuntimeException);

    virtual sal_Bool SAL_CALL isReadonly() throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL store() throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL storeAsURL(   const   OUSTRING&                   sURL            ,
                                        const   SEQUENCE< PROPERTYVALUE >&  seqArguments    )
        throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException) ;
    virtual void SAL_CALL storeToURL(   const   OUSTRING&                   sURL            ,
                                        const   SEQUENCE< PROPERTYVALUE >&  seqArguments    )
        throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    //____________________________________________________________________________________________________
    //  XLoadable
    //____________________________________________________________________________________________________
    virtual void SAL_CALL initNew()
        throw (::com::sun::star::frame::DoubleInitializationException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException,
               ::com::sun::star::uno::Exception); // #dochnoetig# {DBG_BF_ASSERT(0, "STRIP");}

    virtual void SAL_CALL load( const   SEQUENCE< PROPERTYVALUE >&  seqArguments )
        throw (::com::sun::star::frame::DoubleInitializationException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException,
                ::com::sun::star::uno::Exception); // #dochnoetig# {DBG_BF_ASSERT(0, "STRIP");}

    //____________________________________________________________________________________________________
    //  XTransferable
    //____________________________________________________________________________________________________
    virtual ANY SAL_CALL getTransferData( const DATAFLAVOR& aFlavor )
        throw (::com::sun::star::datatransfer::UnsupportedFlavorException,
               ::com::sun::star::io::IOException,
               ::com::sun::star::uno::RuntimeException);

    virtual SEQUENCE< DATAFLAVOR > SAL_CALL getTransferDataFlavors()
    throw (::com::sun::star::uno::RuntimeException);

    virtual sal_Bool SAL_CALL isDataFlavorSupported( const DATAFLAVOR& )
        throw (::com::sun::star::uno::RuntimeException) {DBG_BF_ASSERT(0, "STRIP");return false;}

    //____________________________________________________________________________________________________
    //  XEventsSupplier
    //____________________________________________________________________________________________________
    virtual REFERENCE< XNAMEREPLACE > SAL_CALL getEvents() throw( RUNTIMEEXCEPTION );

    //____________________________________________________________________________________________________
    //  XEventBroadcaster
    //____________________________________________________________________________________________________
    virtual void SAL_CALL addEventListener( const REFERENCE< XDOCEVENTLISTENER >& xListener ) throw( RUNTIMEEXCEPTION );
    virtual void SAL_CALL removeEventListener( const REFERENCE< XDOCEVENTLISTENER >& xListener ) throw( RUNTIMEEXCEPTION );

    //____________________________________________________________________________________________________
    //  SfxListener
    //____________________________________________________________________________________________________
    void Notify(            SfxBroadcaster& aBC     ,
                    const   SfxHint&        aHint   ) ;

    //____________________________________________________________________________________________________
    //  public IMPL?
    //____________________________________________________________________________________________________
    void changing() ;

    SfxObjectShell* GetObjectShell() const ;

    sal_Bool impl_isDisposed() const ;

    ::com::sun::star::uno::Reference < ::com::sun::star::container::XIndexAccess > SAL_CALL getViewData() throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL setViewData( const ::com::sun::star::uno::Reference < ::com::sun::star::container::XIndexAccess >& aData ) throw (::com::sun::star::uno::RuntimeException);

    /** calls all XEventListeners */
    void notifyEvent( const ::com::sun::star::document::EventObject& aEvent ) const;

    /** returns true if someone added a XEventListener to this XEventBroadcaster */
    sal_Bool hasEventListeners() const;

protected:
    virtual void impl_setPrinter(const SEQUENCE< PROPERTYVALUE >&,SfxPrinter*&,sal_uInt16&,SfxViewShell*&){DBG_BF_ASSERT(0, "STRIP");}
//________________________________________________________________________________________________________
//  private methods
//________________________________________________________________________________________________________

private:
    void impl_store(            SfxObjectShell*             pObjectShell    ,
                        const   OUSTRING&                   sURL            ,
                        const   SEQUENCE< PROPERTYVALUE >&  seqArguments    ,
                                sal_Bool                    bSaveTo         ) ;
    void    postEvent_Impl( const SfxEventHint& rHint );
    String  getEventName_Impl( long nID );

//________________________________________________________________________________________________________
//  private variables and methods
//________________________________________________________________________________________________________

private:
    IMPL_SfxBaseModel_DataContainer*    m_pData ;

    DECL_LINK( LoadDone_Impl, void* );

} ; // class SfxBaseModel

}//end of namespace binfilter
#endif // _SFX_SFXBASEMODEL_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
