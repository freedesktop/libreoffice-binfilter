/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: fmtfordr.hxx,v $
 * $Revision: 1.8 $
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
#ifndef _FMTFORDR_HXX
#define _FMTFORDR_HXX

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
namespace binfilter {

//Die FillOrder ---------------------------------

enum SwFillOrder
{
    SW_FILL_ORDER_BEGIN,
    ATT_TOP_DOWN = SW_FILL_ORDER_BEGIN,
    ATT_BOTTOM_UP,
    ATT_LEFT_TO_RIGHT,
    ATT_RIGHT_TO_LEFT,
    SW_FILL_ORDER_END
};

class SwFmtFillOrder: public SfxEnumItem
{
public:
    SwFmtFillOrder( SwFillOrder = ATT_TOP_DOWN );
    inline SwFmtFillOrder &operator=( const SwFmtFillOrder &rCpy );

    // "pure virtual Methoden" vom SfxPoolItem
    virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual SfxPoolItem*    Create(SvStream &, USHORT nVer) const;
    virtual SvStream&       Store(SvStream &, USHORT nIVer) const;
    virtual USHORT          GetValueCount() const{DBG_BF_ASSERT(0, "STRIP"); return 0;} //STRIP001 virtual USHORT           GetValueCount() const;

    const SwFillOrder GetFillOrder() const { return SwFillOrder(GetValue()); }
    void  SetFillOrder( const SwFillOrder eNew ) { SetValue( USHORT(eNew) ); }
};

inline SwFmtFillOrder &SwFmtFillOrder::operator=( const SwFmtFillOrder &rCpy )
{
    SetValue( rCpy.GetValue() );
    return *this;
}

#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtFillOrder &SwAttrSet::GetFillOrder(BOOL bInP) const
    { return (const SwFmtFillOrder&)Get( RES_FILL_ORDER,bInP); }

inline const SwFmtFillOrder &SwFmt::GetFillOrder(BOOL bInP) const
    { return aSet.GetFillOrder(bInP); }
#endif

} //namespace binfilter
#endif

