/*************************************************************************
 *
 *  $RCSfile: caption.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: mwu $ $Date: 2003-11-06 07:54:54 $
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
#ifndef _CAPTION_HXX
#define _CAPTION_HXX

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _GLOBNAME_HXX //autogen
#include <tools/globname.hxx>
#endif
namespace binfilter {

enum SwCapObjType
{
    FRAME_CAP, GRAPHIC_CAP, TABLE_CAP, OLE_CAP
};

/*--------------------------------------------------------------------
    Beschreibung:
 --------------------------------------------------------------------*/

class InsCaptionOpt
{
private:
    BOOL         bUseCaption;
    SwCapObjType eObjType;
    SvGlobalName aOleId;
    String       sCategory;
    USHORT       nNumType;
    String       sCaption;
    USHORT       nPos;
    USHORT       nLevel;
    sal_Unicode  cSeparator;
    BOOL         bIgnoreSeqOpts;    // wird nicht gespeichert
    BOOL         bCopyAttributes;   //          -""-

public:
    InsCaptionOpt(const SwCapObjType eType = FRAME_CAP, const SvGlobalName* pOleId = 0);
//STRIP001  InsCaptionOpt(const InsCaptionOpt&);
    ~InsCaptionOpt();

    inline BOOL&            UseCaption()                    { return bUseCaption; }
    inline BOOL             UseCaption() const              { return bUseCaption; }

    inline SwCapObjType     GetObjType() const              { return eObjType; }
    inline void             SetObjType(const SwCapObjType eType) { eObjType = eType; }

    inline const SvGlobalName&  GetOleId() const                { return aOleId; }
    inline void             SetOleId(const SvGlobalName &rId)   { aOleId = rId; }

    inline const String&    GetCategory() const             { return sCategory; }
    inline void             SetCategory(const String& rCat) { sCategory = rCat; }

    inline USHORT           GetNumType() const              { return nNumType; }
    inline void             SetNumType(const USHORT nNT)    { nNumType = nNT; }

    inline const String&    GetCaption() const              { return sCaption; }
    inline void             SetCaption(const String& rCap)  { sCaption = rCap; }

    inline USHORT           GetPos() const                  { return nPos; }
    inline void             SetPos(const USHORT nP)         { nPos = nP; }

    inline USHORT           GetLevel() const                { return nLevel; }
    inline void             SetLevel(const USHORT nLvl)     { nLevel = nLvl; }

    inline sal_Unicode      GetSeparator() const                { return cSeparator; }
    inline void             SetSeparator(const sal_Unicode cSep){ cSeparator = cSep; }

    inline BOOL&            IgnoreSeqOpts()                 { return bIgnoreSeqOpts; }
    inline BOOL             IgnoreSeqOpts() const           { return bIgnoreSeqOpts; }

    inline BOOL&            CopyAttributes()                { return bCopyAttributes; }
    inline BOOL             CopyAttributes() const          { return bCopyAttributes; }

//STRIP001  BOOL                    operator==( const InsCaptionOpt& rOpt ) const;
//STRIP001  InsCaptionOpt&          operator= ( const InsCaptionOpt& rOpt );
    inline BOOL             operator< ( const InsCaptionOpt & rObj ) const
                                                { return aOleId < rObj.aOleId; }
//STRIP001  friend SvStream&        operator>>( SvStream& rIStream, InsCaptionOpt& rCapOpt );
//STRIP001  friend SvStream&        operator<<( SvStream& rOStream, const InsCaptionOpt& rCapOpt );
};

} //namespace binfilter
#endif


