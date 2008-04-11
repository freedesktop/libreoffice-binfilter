/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: checkit.hxx,v $
 * $Revision: 1.4 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _CHECKIT_HXX
#define _CHECKIT_HXX

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_I18N_XINPUTSEQUENCECHECKER_HPP_
#include <com/sun/star/i18n/XInputSequenceChecker.hpp>
#endif
namespace binfilter {

/*************************************************************************
 * Input strings with length > MAX_SEQUENCE_CHECK_LEN are not checked.
 *************************************************************************/

#define MAX_SEQUENCE_CHECK_LEN 5

/*************************************************************************
 *                      class SwCheckIt
 *
 * Wrapper for the XInputSequenceChecker
 *************************************************************************/

class SwCheckIt
{
public:
    ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XInputSequenceChecker > xCheck;

    SwCheckIt();
};

extern SwCheckIt* pCheckIt;

} //namespace binfilter
#endif

