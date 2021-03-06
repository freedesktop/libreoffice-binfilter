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

#ifndef _SW_APPLET_IMPL_HXX
#define _SW_APPLET_IMPL_HXX

#include <bf_svtools/bf_solar.h>

#define SWHTML_OPTTYPE_IGNORE 0
#define SWHTML_OPTTYPE_TAG 1
#define SWHTML_OPTTYPE_PARAM 2
#define SWHTML_OPTTYPE_SIZE 3

#include <bf_tools/string.hxx>

#include <bf_svtools/htmlkywd.hxx>
#include <bf_sfx2/frameobj.hxx>
#include <vcl/wrkwin.hxx>
#include <bf_so3/svstor.hxx>
#include <bf_so3/applet.hxx>
#include <bf_so3/plugin.hxx>
#include <bf_svtools/itemset.hxx>
namespace binfilter {

class SfxItemSet;

class SwApplet_Impl
{
    SvAppletObjectRef xApplet;      // das aktuelle Applet
    SfxItemSet        aItemSet;

public:
    SwApplet_Impl( SfxItemSet& rSet ): aItemSet ( rSet) {}

    sal_Bool CreateApplet();
    SvAppletObject* GetApplet() { return &xApplet; }
    SfxItemSet& GetItemSet() { return aItemSet; }
};
} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
