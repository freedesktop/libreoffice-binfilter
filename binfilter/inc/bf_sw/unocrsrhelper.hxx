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
#ifndef _UNOCRSRHELPER_HXX
#define _UNOCRSRHELPER_HXX


#include <com/sun/star/beans/XPropertyState.hpp>
#include <unocrsr.hxx>
namespace binfilter {

class SfxItemSet;
struct SfxItemPropertyMap;

class SwPaM;


namespace com{ namespace sun{ namespace star{
    namespace uno{
        class Any;
    }
}}}


namespace SwUnoCursorHelper
{
    sal_Bool                    getCrsrPropertyValue(const SfxItemPropertyMap* pMap
                                        , SwPaM& rPam
                                        , ::com::sun::star::uno::Any *pAny
                                        , ::com::sun::star::beans::PropertyState& eState
                                        , const SwTxtNode* pNode = 0 );

    void                        GetCurPageStyle(SwPaM& rPaM, String &rString);

    inline sal_Bool             IsStartOfPara(SwPaM& rUnoCrsr)
                                        { return rUnoCrsr.GetPoint()->nContent == 0;}
    inline sal_Bool             IsEndOfPara(SwPaM& rUnoCrsr)
                                        { return rUnoCrsr.GetCntntNode() &&
                                            rUnoCrsr.GetPoint()->nContent == rUnoCrsr.GetCntntNode()->Len();}

    void                        resetCrsrPropertyValue(const SfxItemPropertyMap* pMap, SwPaM& rPam);
    void                        InsertFile(SwUnoCrsr* pUnoCrsr,
                                    const String& rFileName,
                                    const String& rFilterName,
                                    const String& rFilterOption,
                                    const String& rPassword);

    void                        getNumberingProperty(
                                    SwPaM& rPam,
                                    ::com::sun::star::beans::PropertyState& eState,
                                    ::com::sun::star::uno::Any *pAny );

    void                        setNumberingProperty(
                                    const ::com::sun::star::uno::Any& rValue,
                                    SwPaM& rPam);

    sal_Int16                   IsNodeNumStart(
                                    SwPaM& rPam,
                                    ::com::sun::star::beans::PropertyState& eState);

}

} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
