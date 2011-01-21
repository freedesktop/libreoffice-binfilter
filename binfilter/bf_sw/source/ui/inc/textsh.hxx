/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef _SWTEXTSH_HXX
#define _SWTEXTSH_HXX

#include <basesh.hxx>
class Button;
namespace binfilter {


class SvxHyperlinkItem;

class SwTextShell: public SwBaseShell
{


public:
    SFX_DECL_INTERFACE(SW_TEXTSHELL);
    TYPEINFO();


    void    GetState(SfxItemSet &);

    void    ExecInsert(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    StateInsert(SfxItemSet&);
    void    ExecEnterNum(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecBasicMove(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecMove(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecMovePage(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecMoveCol(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecMoveLingu(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecMoveMisc(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    StateField(SfxItemSet &);
    void    ExecIdx(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");}  ;
    void    GetIdxState(SfxItemSet &);
    void    ExecGlossary(SfxRequest &);

    void    ExecCharAttr(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecCharAttrArgs(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecParaAttr(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecParaAttrArgs(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecAttr(SfxRequest &);
    void    ExecDB(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;
    void    ExecTransliteration(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;

    void    GetAttrState(SfxItemSet &);

             SwTextShell(SwView &rView);
    virtual ~SwTextShell();
};

} //namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
