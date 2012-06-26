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
#ifndef _SVTOOLS_TEMPLDLG_HXX
#define _SVTOOLS_TEMPLDLG_HXX

#include "bf_svtools/svtdllapi.h"

#include <vcl/button.hxx>
#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>

namespace binfilter
{

struct SvtTmplDlg_Impl;

// class SvtDocumentTemplateDialog ---------------------------------------

class SvtTemplateWindow;

class  SvtDocumentTemplateDialog : public ModalDialog
{
private:
    FixedLine           aLine;
    PushButton          aManageBtn;
    PushButton          aEditBtn;
    OKButton            aOKBtn;
    CancelButton        aCancelBtn;
    HelpButton          aHelpBtn;

    SvtTmplDlg_Impl*    pImpl;

    DECL_DLLPRIVATE_LINK(           SelectHdl_Impl, SvtTemplateWindow* );
    DECL_DLLPRIVATE_LINK(           DoubleClickHdl_Impl, SvtTemplateWindow* );
    DECL_DLLPRIVATE_LINK(           NewFolderHdl_Impl, SvtTemplateWindow* );
    DECL_DLLPRIVATE_LINK(           SendFocusHdl_Impl, SvtTemplateWindow* );
    DECL_DLLPRIVATE_LINK(           OKHdl_Impl, PushButton* );
    DECL_DLLPRIVATE_LINK(           OrganizerHdl_Impl, PushButton* );
    DECL_DLLPRIVATE_LINK(           UpdateHdl_Impl, Timer* );

public:
    SvtDocumentTemplateDialog( Window* pParent );

    /** ctor for calling the dialog for <em>selection</em> only, not for <em>opening</em> a document
        <p>If you use this ctor, the dialog will behave differently in the following areas:
        <ul><li>The <em>Edit</em> button will be hidden.</li>
            <li>Upon pressing em>Open</em>, the selected file will not be opened. Instead, it's
                URL is available (see <method>GetSelectedFileURL</method>).</li>
        </ul>

    */
    struct SelectOnly { };
    SvtDocumentTemplateDialog( Window* _pParent, SelectOnly );

    ~SvtDocumentTemplateDialog();

    sal_Bool    IsFileSelected( ) const;
    String      GetSelectedFileURL( ) const;

    void        SelectTemplateFolder();

private:
    void InitImpl( );
    sal_Bool CanEnableEditBtn() const;
};

}

#endif // _SVTOOLS_TEMPLDLG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
