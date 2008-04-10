/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: sdiocmpt.hxx,v $
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

#ifndef _SD_SDIOCMPT_HXX
#define _SD_SDIOCMPT_HXX

#ifndef _SVDIO_HXX //autogen
#include <bf_svx/svdio.hxx>
#endif
class SvStream;
namespace binfilter {


#define SDIOCOMPAT_VERSIONDONTKNOW (UINT16)0xffff

class SdIOCompat : public SdrDownCompat
{
private:
    UINT16 nVersion;

public:
                // nNewMode: STREAM_READ oder STREAM_WRITE
                // nVer:     nur beim Schreiben angeben
            SdIOCompat(SvStream& rNewStream, USHORT nNewMode,
                       UINT16 nVer = SDIOCOMPAT_VERSIONDONTKNOW);
            ~SdIOCompat() {};
    UINT16  GetVersion() const { return nVersion; }
};

} //namespace binfilter
#endif      // _SD_SDIOCMPT_HXX

