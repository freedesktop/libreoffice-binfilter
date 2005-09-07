/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: csvsplits.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 19:40:05 $
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

// ============================================================================

#ifndef _SC_CSVSPLITS_HXX
#define _SC_CSVSPLITS_HXX

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#include <vector>
namespace binfilter {


// ============================================================================

/** Constant for an invalid vector index. */
const sal_uInt32 CSV_VEC_NOTFOUND   = ~0UL;
/** Constant for an invalid ruler position. */
const sal_Int32 CSV_POS_INVALID     = -1;


// ----------------------------------------------------------------------------

/** A vector of column splits that supports inserting, removing and moving splits. */
class ScCsvSplits
{
private:
    typedef ::std::vector< sal_Int32 >      ScSplitVector;
    typedef ScSplitVector::iterator         iterator;
    typedef ScSplitVector::const_iterator   const_iterator;

    ScSplitVector               maVec;          /// The split containter.

//STRIP001 public:
//STRIP001     // *** access by position *** ---------------------------------------------
//STRIP001
//STRIP001     /** Inserts a new split at position nPos into the vector.
//STRIP001         @return  true = split inserted (nPos was valid and empty). */
//STRIP001     bool                        Insert( sal_Int32 nPos );
//STRIP001     /** Removes a split by position.
//STRIP001         @return  true = split found and removed. */
//STRIP001     bool                        Remove( sal_Int32 nPos );
//STRIP001     /** Removes a range of splits in the given position range. */
//STRIP001     void                        RemoveRange( sal_Int32 nPosStart, sal_Int32 nPosEnd );
//STRIP001     /** Removes all elements from the vector. */
//STRIP001     void                        Clear();
//STRIP001
//STRIP001     /** Returns true if at position nPos is a split. */
//STRIP001     bool                        HasSplit( sal_Int32 nPos ) const;
//STRIP001
//STRIP001     // *** access by index *** ------------------------------------------------
//STRIP001
//STRIP001     /** Searches for a split at position nPos.
//STRIP001         @return  the vector index of the split. */
//STRIP001     sal_uInt32                  GetIndex( sal_Int32 nPos ) const;
//STRIP001     /** Returns index of the first split greater than or equal to nPos. */
//STRIP001     sal_uInt32                  LowerBound( sal_Int32 nPos ) const;
//STRIP001     /** Returns index of the last split less than or equal to nPos. */
//STRIP001     sal_uInt32                  UpperBound( sal_Int32 nPos ) const;
//STRIP001
//STRIP001     /** Returns the number of splits. */
//STRIP001     inline sal_uInt32           Count() const
//STRIP001                                     { return maVec.size(); }
//STRIP001     /** Returns the position of the specified split. */
//STRIP001     sal_Int32                   GetPos( sal_uInt32 nIndex ) const;
//STRIP001     /** Returns the position of the specified split. */
//STRIP001     inline sal_Int32            operator[]( sal_uInt32 nIndex ) const
//STRIP001                                     { return GetPos( nIndex ); }
//STRIP001
//STRIP001 private:
//STRIP001     /** Returns the vector index of an iterator. */
//STRIP001     sal_uInt32                  GetIterIndex( const_iterator aIter ) const;
};


// ============================================================================

} //namespace binfilter
#endif

