/*************************************************************************
 *
 *  $RCSfile: drawsh.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mwu $ $Date: 2003-11-06 07:29:41 $
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

#ifndef SC_DRAWSH_HXX
#define SC_DRAWSH_HXX

#ifndef _SFX_SHELL_HXX //autogen
#include <bf_sfx2/shell.hxx>
#endif
#include "shellids.hxx"
#ifndef _SFXMODULE_HXX //autogen
#include <bf_sfx2/module.hxx>
#endif

#ifndef _SVDMARK_HXX //autogen
#include <bf_svx/svdmark.hxx>
#endif

#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif
namespace binfilter {

class SvxNameDialog;
class ScViewData;


class ScDrawShell : public SfxShell
{
//STRIP001  ScViewData* pViewData;

//STRIP001     DECL_LINK( NameObjectHdl, SvxNameDialog* );

//STRIP001 protected:
//STRIP001  ScViewData* GetViewData()   { return pViewData; }

public:
    TYPEINFO();
    SFX_DECL_INTERFACE(SCID_DRAW_SHELL);

    ScDrawShell(ScViewData* pData){DBG_ASSERT(0, "STRIP");} //STRIP001 ScDrawShell(ScViewData* pData);
//STRIP001                  ~ScDrawShell();

    void    StateDisableItems( SfxItemSet &rSet ){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001    void    StateDisableItems( SfxItemSet &rSet );

    void    ExecDrawAttr(SfxRequest& rReq){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    ExecDrawAttr(SfxRequest& rReq);
    void    GetDrawAttrState(SfxItemSet &rSet){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    GetDrawAttrState(SfxItemSet &rSet);
    void    GetAttrFuncState(SfxItemSet &rSet){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    GetAttrFuncState(SfxItemSet &rSet);

    void    ExecDrawFunc(SfxRequest& rReq){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    ExecDrawFunc(SfxRequest& rReq);
    void    GetDrawFuncState(SfxItemSet &rSet){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    GetDrawFuncState(SfxItemSet &rSet);
    void    GetState(SfxItemSet &rSet){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    GetState(SfxItemSet &rSet);

    void    ExecFormText(SfxRequest& rReq){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    ExecFormText(SfxRequest& rReq);     // StarFontWork
    void    GetFormTextState(SfxItemSet& rSet){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    GetFormTextState(SfxItemSet& rSet);

    void    ExecuteHLink(SfxRequest& rReq){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001   void    ExecuteHLink(SfxRequest& rReq);     // Hyperlink
    void    GetHLinkState(SfxItemSet& rSet){DBG_ASSERT(0, "STRIP");} //STRIP001 //STRIP001  void    GetHLinkState(SfxItemSet& rSet);

//STRIP001  void    ExecuteLineDlg( SfxRequest& rReq, USHORT nTabPage = 0xffff );
//STRIP001  void    ExecuteAreaDlg( SfxRequest& rReq, USHORT nTabPage = 0xffff );
//STRIP001  void    ExecuteTextAttrDlg( SfxRequest& rReq, USHORT nTabPage = 0xffff );

//STRIP001  BOOL    AreAllObjectsOnLayer(USHORT nLayerNo,const SdrMarkList& rMark);
};



} //namespace binfilter
#endif


