/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: sch_stlpool.cxx,v $
 * $Revision: 1.6 $
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

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "stlpool.hxx"
#include "stlsheet.hxx"
namespace binfilter {


/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/

/*N*/ SchStyleSheetPool::SchStyleSheetPool(SfxItemPool& rPool) :
/*N*/   SfxStyleSheetPool(rPool),
/*N*/   pActualStyleSheet(NULL)
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

/*N*/ SchStyleSheetPool::~SchStyleSheetPool()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Vorlage erzeugen
|*
\************************************************************************/

/*N*/ SfxStyleSheetBase* SchStyleSheetPool::Create(const String& rName,
/*N*/                                            SfxStyleFamily eFamily,
/*N*/                                            USHORT nMask )
/*N*/ {
/*N*/   return new SchStyleSheet(rName, *this, eFamily, nMask);
/*N*/ }


/*************************************************************************
|*
|* Vorlage nach Vorbild erzeugen
|*
\************************************************************************/




}
