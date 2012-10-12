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

#include <bf_svtools/itemiter.hxx>
#include <bf_svtools/whiter.hxx>


#include "shellio.hxx"
#include "wrt_fn.hxx"

#include "node.hxx"
namespace binfilter {

/*N*/ Writer& Out( const SwNodeFnTab pTab, SwNode& rNode, Writer & rWrt )
/*N*/ {
/*N*/   // es muss ein CntntNode sein !!
/*N*/   SwCntntNode * pCNd = rNode.GetCntntNode();
/*N*/   if( !pCNd )
/*?*/       return rWrt;
/*N*/
/*N*/   USHORT nId = RES_TXTNODE;
/*N*/   switch( pCNd->GetNodeType() )
/*N*/   {
/*N*/   case ND_TEXTNODE:   nId = RES_TXTNODE;  break;
/*?*/   case ND_GRFNODE:    nId = RES_GRFNODE;  break;
/*?*/   case ND_OLENODE:    nId = RES_OLENODE;  break;
/*?*/   default:
/*?*/       OSL_ENSURE( FALSE, "was fuer ein Node ist es denn nun?" );
/*N*/   }
/*N*/   FnNodeOut pOut;
/*N*/   if( 0 != ( pOut = pTab[ nId - RES_NODE_BEGIN ] ))
/*N*/       (*pOut)( rWrt, *pCNd );
/*N*/   return rWrt;
/*N*/ }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */