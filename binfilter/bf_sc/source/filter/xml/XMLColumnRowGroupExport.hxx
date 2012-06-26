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

#ifndef _SC_XMLCOLUMNROWGROUPEXPORT_HXX
#define _SC_XMLCOLUMNROWGROUPEXPORT_HXX

#ifndef __SGI_STL_LIST
#include <list>
#endif
#include <rtl/ustring.hxx>

#include <sal/types.h>
namespace binfilter {

struct ScMyColumnRowGroup
{
    sal_Int32   nField;
    sal_Int16   nLevel;
    sal_Bool    bDisplay : 1;

    ScMyColumnRowGroup();
    sal_Bool operator< (const ScMyColumnRowGroup& rGroup) const;
};

typedef std::list <ScMyColumnRowGroup> ScMyColumnRowGroupVec;
typedef std::list <sal_Int32> ScMyFieldGroupVec;

class ScXMLExport;
class ScMyOpenCloseColumnRowGroup
{
    ScXMLExport&                rExport;
    const ::rtl::OUString           rName;
    ScMyColumnRowGroupVec       aTableStart;
    ScMyFieldGroupVec           aTableEnd;

    void OpenGroup(const ScMyColumnRowGroup& rGroup);
    void CloseGroup();
public:
    ScMyOpenCloseColumnRowGroup(ScXMLExport& rExport, sal_uInt32 nToken);
    ~ScMyOpenCloseColumnRowGroup();

    void NewTable();
    void AddGroup(const ScMyColumnRowGroup& aGroup, const sal_Int32 nEndField);
    sal_Bool IsGroupStart(const sal_Int32 nField);
    void OpenGroups(const sal_Int32 nField);
    sal_Bool IsGroupEnd(const sal_Int32 nField);
    void CloseGroups(const sal_Int32 nField);
    sal_Int32 GetLast();
    void Sort();
};

} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
