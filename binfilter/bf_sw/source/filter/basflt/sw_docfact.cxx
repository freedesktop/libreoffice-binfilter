/*************************************************************************
 *
 *  $RCSfile: sw_docfact.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mwu $ $Date: 2003-11-06 07:52:16 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#pragma hdrstop

#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif

#ifndef _SHELLIO_HXX
#include <shellio.hxx>
#endif
#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _NODE_HXX
#include <node.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
namespace binfilter {


/******************************************************************************
 *  Methode     :   SwDocFac::SwDocFac( SwDoc *pDoc )
 *  Beschreibung:
 *  Erstellt    :   OK 01-24-94 11:32am
 *  Aenderung   :   OK 01-24-94 11:32am
 ******************************************************************************/


/*N*/ SwDocFac::SwDocFac( SwDoc *pDc )
/*N*/   : pDoc( pDc )
/*N*/ {
/*N*/   if( pDoc )
/*N*/       pDoc->AddLink();
/*N*/ }

/******************************************************************************
 *  Methode     :   SwDocFac::~SwDocFac()
 *  Beschreibung:
 *  Erstellt    :   OK 01-24-94 11:33am
 *  Aenderung   :   OK 01-24-94 11:33am
 ******************************************************************************/


/*N*/ SwDocFac::~SwDocFac()
/*N*/ {
/*N*/   if( pDoc && !pDoc->RemoveLink() )
/*?*/       delete pDoc;
/*N*/ }

/******************************************************************************
 *  Methode     :   SwDoc *SwDocFac::GetDoc()
 *  Beschreibung:   Diese Methode legt immer einen Drucker an.
 *  Erstellt    :   OK 01-24-94 11:34am
 *  Aenderung   :   OK 01-24-94 11:34am
 ******************************************************************************/


/*N*/ SwDoc *SwDocFac::GetDoc()
/*N*/ {
/*N*/   if( !pDoc )
/*N*/   {
/*N*/       pDoc = new SwDoc;
/*N*/       pDoc->AddLink();
/*N*/   }
/*N*/   return pDoc;
/*N*/ }


/******************************************************************************
 *  Erstellt    :   JP 01-27-94 11:37am
 *  Aenderung   :   JP 01-27-94 11:37am
 ******************************************************************************/

//STRIP001 ULONG SwDocFac::Count() const
//STRIP001 {
//STRIP001  // minus den Grund-SectionNodes
//STRIP001  return pDoc ? pDoc->GetNodes().GetEndOfContent().GetIndex() - 10 : 0;
//STRIP001 }


}
