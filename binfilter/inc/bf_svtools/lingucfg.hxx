/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: lingucfg.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2008-03-12 12:16:01 $
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

#ifndef _SVTOOLS_LINGUCFG_HXX_
#define _SVTOOLS_LINGUCFG_HXX_

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUES_HPP_
#include <com/sun/star/beans/PropertyValues.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif

#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_OPTIONS_HXX
#include <bf_svtools/options.hxx>
#endif

namespace binfilter
{

class SvtLinguConfigItem;

//////////////////////////////////////////////////////////////////////

struct  SvtLinguOptions
{
    ::com::sun::star::uno::Sequence< rtl::OUString >    aActiveDics;
    ::com::sun::star::uno::Sequence< rtl::OUString >    aActiveConvDics;

    BOOL                                                bROActiveDics;
    BOOL                                                bROActiveConvDics;

    // Hyphenator service specific options
    INT16   nHyphMinLeading,
            nHyphMinTrailing,
            nHyphMinWordLength;

    BOOL    bROHyphMinLeading,
            bROHyphMinTrailing,
            bROHyphMinWordLength;

    // misc options (non-service specific)
    INT16   nDefaultLanguage;
    INT16   nDefaultLanguage_CJK;
    INT16   nDefaultLanguage_CTL;

    BOOL    bRODefaultLanguage;
    BOOL    bRODefaultLanguage_CJK;
    BOOL    bRODefaultLanguage_CTL;

    // spelling options (non-service specific)
    BOOL    bIsSpellSpecial;
    BOOL    bIsSpellInAllLanguages;
    BOOL    bIsSpellAuto;
    BOOL    bIsSpellHideMarkings;
    BOOL    bIsSpellReverse;

    BOOL    bROIsSpellSpecial;
    BOOL    bROIsSpellInAllLanguages;
    BOOL    bROIsSpellAuto;
    BOOL    bROIsSpellHideMarkings;
    BOOL    bROIsSpellReverse;

    // hyphenation options (non-service specific)
    BOOL    bIsHyphSpecial;
    BOOL    bIsHyphAuto;

    BOOL    bROIsHyphSpecial;
    BOOL    bROIsHyphAuto;

    // common to SpellChecker, Hyphenator and Thesaurus service
    BOOL    bIsGermanPreReform;
    BOOL    bIsUseDictionaryList;
    BOOL    bIsIgnoreControlCharacters;

    BOOL    bROIsGermanPreReform;
    BOOL    bROIsUseDictionaryList;
    BOOL    bROIsIgnoreControlCharacters;

    // SpellChecker service specific options
    BOOL    bIsSpellWithDigits,
            bIsSpellUpperCase,
            bIsSpellCapitalization;

    BOOL    bROIsSpellWithDigits,
            bROIsSpellUpperCase,
            bROIsSpellCapitalization;

    // text conversion specific options
    BOOL    bIsIgnorePostPositionalWord;
    BOOL    bIsAutoCloseDialog;
    BOOL    bIsShowEntriesRecentlyUsedFirst;
    BOOL    bIsAutoReplaceUniqueEntries;
    BOOL    bIsDirectionToSimplified;
    BOOL    bIsUseCharacterVariants;
    BOOL    bIsTranslateCommonTerms;
    BOOL    bIsReverseMapping;

    BOOL    bROIsIgnorePostPositionalWord;
    BOOL    bROIsAutoCloseDialog;
    BOOL    bROIsShowEntriesRecentlyUsedFirst;
    BOOL    bROIsAutoReplaceUniqueEntries;
    BOOL    bROIsDirectionToSimplified;
    BOOL    bROIsUseCharacterVariants;
    BOOL    bROIsTranslateCommonTerms;
    BOOL    bROIsReverseMapping;

    // check value need to determine if the configuration needs to updatet
    // or not (used for a quick check if data files have been changed/added
    // or deleted
    INT32   nDataFilesChangedCheckValue;
    BOOL    bRODataFilesChangedCheckValue;

    SvtLinguOptions();
};

//////////////////////////////////////////////////////////////////////

class  SvtLinguConfig: public Options
{

    // disallow copy-constructor and assignment-operator for now
    SvtLinguConfig( const SvtLinguConfig & );
    SvtLinguConfig & operator = ( const SvtLinguConfig & );

    // returns static object
     SvtLinguConfigItem &   GetConfigItem();

    SvtLinguConfigItem &   GetConfigItem() const    { return const_cast< SvtLinguConfig * >( this )->GetConfigItem(); }

public:
    SvtLinguConfig();
    virtual ~SvtLinguConfig();

    //
    // borrowed from utl::ConfigItem
    //
    com::sun::star::uno::Sequence< rtl::OUString >
        GetNodeNames( const rtl::OUString &rNode );
    //
    com::sun::star::uno::Sequence< com::sun::star::uno::Any >
        GetProperties(
            const com::sun::star::uno::Sequence< rtl::OUString > &rNames );
    //
    sal_Bool
        ReplaceSetProperties(
            const rtl::OUString &rNode,
            com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue > rValues );

    com::sun::star::uno::Any
            GetProperty( const rtl::OUString &rPropertyName ) const;
    com::sun::star::uno::Any
            GetProperty( INT32 nPropertyHandle ) const;

    BOOL    SetProperty( const rtl::OUString &rPropertyName,
                         const com::sun::star::uno::Any &rValue );
    BOOL    SetProperty( INT32 nPropertyHandle,
                         const com::sun::star::uno::Any &rValue );

    BOOL    GetOptions( SvtLinguOptions &rOptions ) const;
    BOOL    SetOptions( const SvtLinguOptions &rOptions );

    BOOL    IsReadOnly( const rtl::OUString &rPropertyName ) const;
    BOOL    IsReadOnly( INT32 nPropertyHandle ) const;
};

//////////////////////////////////////////////////////////////////////

}

#endif

