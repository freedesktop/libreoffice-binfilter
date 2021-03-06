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

#include <unolingu.hxx>

#include <cppuhelper/implbase1.hxx> // helper for implementations

#include <tools/debug.hxx>
#include <tools/urlobj.hxx>
#include <bf_svtools/pathoptions.hxx>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/linguistic2/XAvailableLocales.hpp>
#include <comphelper/processfactory.hxx>

#include <cppuhelper/implbase1.hxx> // helper for implementations

#include <i18npool/mslangid.hxx>
#include <bf_svtools/lingucfg.hxx>
#include <vcl/msgbox.hxx>
#include <tools/shl.hxx>

#include <dialmgr.hxx>
#include "dialogs.hrc"
#include <legacysmgr/legacy_binfilters_smgr.hxx>

namespace binfilter {
using namespace ::rtl;
using namespace ::comphelper;
using namespace ::com::sun::star;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::linguistic2;

#define A2OU(x) OUString::createFromAscii(x)

///////////////////////////////////////////////////////////////////////////


/*N*/ static Reference< XLinguServiceManager > GetLngSvcMgr_Impl()
/*N*/ {
/*N*/   Reference< XLinguServiceManager > xRes;
/*N*/   Reference< XMultiServiceFactory >  xMgr = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/   if (xMgr.is())
/*N*/   {
/*N*/       xRes = Reference< XLinguServiceManager > ( xMgr->createInstance(
/*N*/               OUString( RTL_CONSTASCII_USTRINGPARAM(
/*N*/                   "com.sun.star.linguistic2.LinguServiceManager" ) ) ), UNO_QUERY ) ;
/*N*/   }
/*N*/   return xRes;
/*N*/ }

///////////////////////////////////////////////////////////////////////////

//! Dummy implementation in order to avoid loading of lingu DLL.
//! The dummy accesses the real implementation (and thus loading the DLL)
//! when it needs to be done only.
/*N*/ class HyphDummy_Impl :
/*N*/     public cppu::WeakImplHelper1< XHyphenator >
/*N*/ {
/*N*/     Reference< XHyphenator >     xHyph;      // the real one...
/*N*/
/*N*/     void    GetHyph_Impl();
/*N*/
/*N*/ public:
/*N*/
/*N*/     // XSupportedLocales
/*N*/     virtual ::com::sun::star::uno::Sequence<
/*N*/           ::com::sun::star::lang::Locale > SAL_CALL
/*N*/       getLocales()
/*N*/           throw(::com::sun::star::uno::RuntimeException);
/*N*/     virtual sal_Bool SAL_CALL
/*N*/         hasLocale( const ::com::sun::star::lang::Locale& rLocale )
/*N*/           throw(::com::sun::star::uno::RuntimeException);
/*N*/
/*N*/     // XHyphenator
/*N*/     virtual ::com::sun::star::uno::Reference<
/*N*/           ::com::sun::star::linguistic2::XHyphenatedWord > SAL_CALL
/*N*/         hyphenate( const ::rtl::OUString& rWord,
/*N*/                 const ::com::sun::star::lang::Locale& rLocale,
/*N*/               sal_Int16 nMaxLeading,
/*N*/                 const ::com::sun::star::beans::PropertyValues& rProperties )
/*N*/           throw(::com::sun::star::lang::IllegalArgumentException,
/*N*/                 ::com::sun::star::uno::RuntimeException);
/*N*/     virtual ::com::sun::star::uno::Reference<
/*N*/           ::com::sun::star::linguistic2::XHyphenatedWord > SAL_CALL
/*N*/         queryAlternativeSpelling( const ::rtl::OUString& rWord,
/*N*/                 const ::com::sun::star::lang::Locale& rLocale,
/*N*/               sal_Int16 nIndex,
/*N*/                 const ::com::sun::star::beans::PropertyValues& rProperties )
/*N*/           throw(::com::sun::star::lang::IllegalArgumentException,
/*N*/                 ::com::sun::star::uno::RuntimeException);
/*N*/     virtual ::com::sun::star::uno::Reference<
/*N*/           ::com::sun::star::linguistic2::XPossibleHyphens > SAL_CALL
/*N*/       createPossibleHyphens(
/*N*/                 const ::rtl::OUString& rWord,
/*N*/                 const ::com::sun::star::lang::Locale& rLocale,
/*N*/                 const ::com::sun::star::beans::PropertyValues& rProperties )
/*N*/           throw(::com::sun::star::lang::IllegalArgumentException,
/*N*/                 ::com::sun::star::uno::RuntimeException);
/*N*/ };


/*N*/ void HyphDummy_Impl::GetHyph_Impl()
/*N*/ {
/*N*/     if (!xHyph.is())
/*N*/     {
/*N*/         Reference< XLinguServiceManager > xLngSvcMgr( GetLngSvcMgr_Impl() );
/*N*/         if (xLngSvcMgr.is())
/*N*/             xHyph = xLngSvcMgr->getHyphenator();
/*N*/     }
/*N*/ }


uno::Sequence< lang::Locale > SAL_CALL HyphDummy_Impl::getLocales()
    throw(uno::RuntimeException)
{
    DBG_BF_ASSERT(0, "STRIP");  // VIRTUAL
    return uno::Sequence< lang::Locale >();
}


/*N*/ sal_Bool SAL_CALL
/*N*/     HyphDummy_Impl::hasLocale( const lang::Locale& rLocale )
/*N*/         throw(uno::RuntimeException)
/*N*/ {
/*N*/     GetHyph_Impl();
/*N*/     BOOL bRes = FALSE;
/*N*/     if (xHyph.is())
/*N*/         bRes = xHyph->hasLocale( rLocale );
/*N*/     return bRes;
/*N*/ }


/*N*/ uno::Reference< linguistic2::XHyphenatedWord > SAL_CALL
/*N*/     HyphDummy_Impl::hyphenate(
/*N*/             const ::rtl::OUString& rWord,
/*N*/             const lang::Locale& rLocale,
/*N*/             sal_Int16 nMaxLeading,
/*N*/             const beans::PropertyValues& rProperties )
/*N*/         throw(lang::IllegalArgumentException,
/*N*/               uno::RuntimeException)
/*N*/ {
/*N*/     GetHyph_Impl();
/*N*/     uno::Reference< linguistic2::XHyphenatedWord > xRes;
/*N*/     if (xHyph.is())
/*N*/         xRes = xHyph->hyphenate( rWord, rLocale, nMaxLeading, rProperties );
/*N*/     return xRes;
/*N*/ }


/*N*/ uno::Reference< linguistic2::XHyphenatedWord > SAL_CALL
/*N*/     HyphDummy_Impl::queryAlternativeSpelling(
/*N*/             const ::rtl::OUString& rWord,
/*N*/             const lang::Locale& rLocale,
/*N*/             sal_Int16 nIndex,
/*N*/             const PropertyValues& rProperties )
/*N*/         throw(lang::IllegalArgumentException,
/*N*/               uno::RuntimeException)
/*N*/ {
/*N*/     GetHyph_Impl();
/*N*/     uno::Reference< linguistic2::XHyphenatedWord > xRes;
/*N*/     if (xHyph.is())
/*N*/         xRes = xHyph->queryAlternativeSpelling( rWord, rLocale, nIndex, rProperties );
/*N*/     return xRes;
/*N*/ }


/*N*/ uno::Reference< linguistic2::XPossibleHyphens > SAL_CALL
/*N*/     HyphDummy_Impl::createPossibleHyphens(
/*N*/             const ::rtl::OUString& rWord,
/*N*/             const lang::Locale& rLocale,
/*N*/             const beans::PropertyValues& rProperties )
/*N*/         throw(lang::IllegalArgumentException,
/*N*/               uno::RuntimeException)
/*N*/ {
/*?*/     GetHyph_Impl();
/*?*/     uno::Reference< linguistic2::XPossibleHyphens > xRes;
/*?*/     if (xHyph.is())
/*?*/         xRes = xHyph->createPossibleHyphens( rWord, rLocale, rProperties );
/*?*/     return xRes;
/*N*/ }


///////////////////////////////////////////////////////////////////////////


/*N*/ typedef cppu::WeakImplHelper1 < XEventListener > LinguMgrAppExitLstnrBaseClass;

/*N*/ class LinguMgrAppExitLstnr : public LinguMgrAppExitLstnrBaseClass
/*N*/ {
/*N*/   Reference< XComponent >         xDesktop;
/*N*/
/*N*/ public:
/*N*/   LinguMgrAppExitLstnr();
/*N*/   virtual ~LinguMgrAppExitLstnr();
/*N*/
/*N*/   virtual void    AtExit() = 0;
/*N*/
/*N*/
/*N*/   // lang::XEventListener
/*N*/     virtual void  SAL_CALL disposing(const EventObject& rSource)
/*N*/           throw( RuntimeException );
/*N*/ };

/*N*/ LinguMgrAppExitLstnr::LinguMgrAppExitLstnr()
/*N*/ {
/*N*/   // add object to frame::Desktop EventListeners in order to properly call
/*N*/   // the AtExit function at appliction exit.
/*N*/
/*N*/   Reference< XMultiServiceFactory >  xMgr = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/   if ( xMgr.is() )
/*N*/   {
/*N*/       xDesktop = Reference< XComponent > ( xMgr->createInstance(
/*N*/               OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.frame.Desktop" ) ) ), UNO_QUERY ) ;
/*N*/       if (xDesktop.is())
/*N*/           xDesktop->addEventListener( this );
/*N*/   }
/*N*/ }

/*N*/ LinguMgrAppExitLstnr::~LinguMgrAppExitLstnr()
/*N*/ {
/*N*/   if (xDesktop.is())
/*N*/   {
/*?*/       xDesktop->removeEventListener( this );
/*?*/       xDesktop = NULL;    //! release reference to desktop
/*N*/   }
/*N*/   DBG_ASSERT(!xDesktop.is(), "reference to desktop should be realeased");
/*N*/ }

/*N*/ void LinguMgrAppExitLstnr::disposing(const EventObject& rSource)
/*N*/       throw( RuntimeException )
/*N*/ {
/*N*/   if (xDesktop.is()  &&  rSource.Source == xDesktop)
/*N*/   {
/*N*/       xDesktop->removeEventListener( this );
/*N*/       xDesktop = NULL;    //! release reference to desktop
/*N*/
/*N*/       AtExit();
/*N*/   }
/*N*/ }

///////////////////////////////////////////////////////////////////////////

/*N*/ class LinguMgrExitLstnr : public LinguMgrAppExitLstnr
/*N*/ {
/*N*/ public:
/*N*/   virtual void    AtExit();
/*N*/ };

/*N*/ void LinguMgrExitLstnr::AtExit()
/*N*/ {
/*N*/   // release references
/*N*/   LinguMgr::xLngSvcMgr    = 0;
/*N*/   LinguMgr::xSpell        = 0;
/*N*/   LinguMgr::xHyph         = 0;
/*N*/   LinguMgr::xThes         = 0;
/*N*/   LinguMgr::xDicList      = 0;
/*N*/   LinguMgr::xProp         = 0;
/*N*/   LinguMgr::xIgnoreAll    = 0;
/*N*/   LinguMgr::xChangeAll    = 0;
/*N*/
/*N*/   LinguMgr::bExiting      = sal_True;
/*N*/
/*N*/   //TL:TODO: MBA fragen wie ich ohne Absturz hier meinen Speicher
/*N*/   //  wieder freibekomme...
/*N*/   //delete LinguMgr::pExitLstnr;
/*N*/   LinguMgr::pExitLstnr    = 0;
/*N*/ }

///////////////////////////////////////////////////////////////////////////


// static member initialization
/*N*/ LinguMgrExitLstnr *           LinguMgr::pExitLstnr    = 0;
/*N*/ sal_Bool                      LinguMgr::bExiting      = sal_False;
/*N*/ Reference< XLinguServiceManager > LinguMgr::xLngSvcMgr    = 0;
/*N*/ Reference< XSpellChecker1 >   LinguMgr::xSpell        = 0;
/*N*/ Reference< XHyphenator >      LinguMgr::xHyph         = 0;
/*N*/ Reference< XThesaurus >       LinguMgr::xThes         = 0;
/*N*/ Reference< XDictionaryList >  LinguMgr::xDicList      = 0;
/*N*/ Reference< XPropertySet >         LinguMgr::xProp         = 0;
/*N*/ Reference< XDictionary1 >         LinguMgr::xIgnoreAll    = 0;
/*N*/ Reference< XDictionary1 >         LinguMgr::xChangeAll    = 0;

/*N*/ Reference< XHyphenator > LinguMgr::GetHyphenator()
/*N*/ {
/*N*/   return xHyph.is() ? xHyph : GetHyph();
/*N*/ }

/*N*/ Reference< XDictionaryList > LinguMgr::GetDictionaryList()
/*N*/ {
/*N*/   return xDicList.is() ? xDicList : GetDicList();
/*N*/ }

/*N*/ Reference< XHyphenator > LinguMgr::GetHyph()
/*N*/ {
/*N*/   if (bExiting)
/*N*/       return 0;
/*N*/
/*N*/   if (!pExitLstnr)
/*N*/       pExitLstnr = new LinguMgrExitLstnr;
/*N*/
/*N*/     //! use dummy implementation in order to avoid loading of lingu DLL
/*N*/     xHyph = new HyphDummy_Impl;
/*N*/
/*N*/   return xHyph;
/*N*/ }

/*N*/ Reference< XDictionaryList > LinguMgr::GetDicList()
/*N*/ {
/*N*/   if (bExiting)
/*N*/       return 0;
/*N*/
/*N*/   if (!pExitLstnr)
/*N*/       pExitLstnr = new LinguMgrExitLstnr;
/*N*/
/*N*/   Reference< XMultiServiceFactory >  xMgr( ::legacy_binfilters::getLegacyProcessServiceFactory() );
/*N*/   if (xMgr.is())
/*N*/   {
/*N*/       xDicList = Reference< XDictionaryList > ( xMgr->createInstance(
/*N*/                     A2OU("com.sun.star.linguistic2.DictionaryList") ), UNO_QUERY );
/*N*/   }
/*N*/   return xDicList;
/*N*/ }





///////////////////////////////////////////////////////////////////////////

//TL:TODO: remove argument or provide SvxGetIgnoreAllList with the same one

///////////////////////////////////////////////////////////////////////////

#include <com/sun/star/linguistic2/XHyphenatedWord.hpp>

///////////////////////////////////////////////////////////////////////////

//TL:TODO: soll mal den rictigen R�ckgabetyp bekommen!

/*N*/ LanguageType SvxLocaleToLanguage( const Locale& rLocale )
/*N*/ {
/*N*/   //  empty Locale -> LANGUAGE_NONE
/*N*/   if ( rLocale.Language.getLength() == 0 )
/*N*/       return LANGUAGE_NONE;
/*N*/
/*N*/   return MsLangId::convertLocaleToLanguage( rLocale );
/*N*/ }

/*N*/ Locale& SvxLanguageToLocale( Locale& rLocale, LanguageType eLang )
/*N*/ {
/*N*/   if ( eLang != LANGUAGE_NONE /* &&  eLang != LANGUAGE_SYSTEM */)
/*N*/       MsLangId::convertLanguageToLocale( eLang, rLocale );
/*N*/   else
/*N*/       rLocale = Locale();
/*N*/
/*N*/   return rLocale;
/*N*/ }

/*N*/ Locale SvxCreateLocale( LanguageType eLang )
/*N*/ {
/*N*/   Locale aLocale;
/*N*/   if ( eLang != LANGUAGE_NONE /* &&  eLang != LANGUAGE_SYSTEM */)
/*N*/       MsLangId::convertLanguageToLocale( eLang, aLocale );
/*N*/
/*N*/   return aLocale;
/*N*/ }


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
