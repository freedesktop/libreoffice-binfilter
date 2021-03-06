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

#ifndef _OUTLINER_HXX
#define _OUTLINER_HXX

#include <bf_svtools/bf_solar.h>

#include <tools/gen.hxx>
#include <bf_tools/string.hxx>
#include <tools/debug.hxx>
#include <bf_svx/editdata.hxx>
#include <i18npool/lang.h>
#include <tools/color.hxx>
#include <vcl/graph.hxx>
#include <tools/link.hxx>
#include <rsc/rscsfx.hxx>

#include <tools/rtti.hxx>   // wegen typedef TypeId
#include <com/sun/star/uno/Reference.h>

#include <rtl/ref.hxx>
#include <bf_svx/svxfont.hxx>

namespace com { namespace sun { namespace star { namespace linguistic2 {
    class XSpellChecker1;
    class XHyphenator;
}}}}

class SvStream;
class Window;
class KeyEvent;
class MouseEvent;
class Pointer;
class MapMode;
class OutputDevice;
class PolyPolygon;

namespace binfilter {

class SvKeyValueIterator;
class SfxStyleSheetPool;
class SfxStyleSheet;
class SfxItemPool;
class SfxItemSet;
class SfxUndoManager;

class OutlinerEditEng;
class Outliner;
class EditView;
class EditUndo;
class ParagraphList;
class OutlinerParaObject;
class SvxBulletItem;
class SvxFont;
class SvxFieldItem;
class OLUndoRemove;
class XPolyPolygon;
class CommandEvent;
class SvxNumBulletItem;
class SvxNumberFormat;
class SvxLRSpaceItem;
class EditEngine;
class SvxForbiddenCharactersTable;

// nur interner Gebrauch!
#define PARAFLAG_DROPTARGET         0x1000
#define PARAFLAG_DROPTARGET_EMPTY   0x2000
#define PARAFLAG_HOLDDEPTH          0x4000
#define PARAFLAG_SETBULLETTEXT      0x8000

// Undo-Action-Ids
#define OLUNDO_DEPTH            EDITUNDO_USER
// #define OLUNDO_HEIGHT           EDITUNDO_USER+1
#define OLUNDO_EXPAND           EDITUNDO_USER+2
#define OLUNDO_COLLAPSE         EDITUNDO_USER+3
// #define OLUNDO_REMOVE           EDITUNDO_USER+4
#define OLUNDO_ATTR             EDITUNDO_USER+5
#define OLUNDO_INSERT           EDITUNDO_USER+6
// #define OLUNDO_MOVEPARAGRAPHS    EDITUNDO_USER+7
#define OLUNDO_CHECKPARA        EDITUNDO_USER+8

// MT 07/00: Only for internal use, oder some kind like hPara for the few
// functions where you need it outside ( eg. moving paragraphs... )

class Paragraph
{
private:
    friend class Outliner;
    friend class ParagraphList;
    friend class OutlinerView;
    friend class OutlinerParaObject;
    friend class OutlinerEditEng;
    friend class OLUndoDepth;
    friend class OutlinerUndoCheckPara;

    Paragraph& operator=(const Paragraph& rPara );

    USHORT              nFlags;
    USHORT              nDepth;
    XubString           aBulText;
    Size                aBulSize;
    BOOL                bVisible;

    BOOL                IsVisible() const { return bVisible; }
    void                SetText( const XubString& rText ) { aBulText = rText; aBulSize.Width() = -1; }
    void                Invalidate() { aBulSize.Width() = -1; }
    void                SetDepth( USHORT nNewDepth ) { nDepth = nNewDepth; aBulSize.Width() = -1; }
    const XubString&    GetText() const { return aBulText; }

                        Paragraph( USHORT nDepth );
                        ~Paragraph();

    USHORT              GetDepth() const { return nDepth; }
};

class OutlinerView
{
    friend class Outliner;

private:

    Outliner*   pOwner;
    EditView*   pEditView;

public:
    virtual     ~OutlinerView();

    EditView&   GetEditView() const { return *pEditView; }

    Outliner*   GetOutliner() const { return pOwner; }

    Window*     GetWindow() const;

    Rectangle   GetOutputArea() const;

    Rectangle   GetVisArea() const;

    ESelection  GetSelection();
    void        SetSelection( const ESelection& );
};

class DrawPortionInfo
{
public:
    const Point&    rStartPos;

    const String&   rText;
    USHORT          nTextStart;
    USHORT          nTextLen;

    const SvxFont&  rFont;

    USHORT          nPara;
    xub_StrLen      nIndex;

    const sal_Int32*     pDXArray;

    // #101498# BiDi level needs to be transported, too.
    BYTE            mnBiDiLevel;

    BYTE GetBiDiLevel() const { return mnBiDiLevel; }
    sal_Bool IsRTL() const;

    DrawPortionInfo( const Point& rPos, const String& rTxt, USHORT nTxtStart, USHORT nTxtLen,
        const SvxFont& rFnt, USHORT nPar, xub_StrLen nIdx, const sal_Int32* pDXArr, BYTE nBiDiLevel)
        :   rStartPos(rPos), rText(rTxt), rFont(rFnt), nPara(nPar), nIndex(nIdx),
            pDXArray(pDXArr), mnBiDiLevel(nBiDiLevel)
        {
            nTextStart = nTxtStart;
            nTextLen = nTxtLen;
        }
};

class EditFieldInfo
{
private:
    Outliner*           pOutliner;
    const SvxFieldItem& rFldItem;

    Color*              pTxtColor;
    Color*              pFldColor;

    String              aRepresentation;

    USHORT              nPara;
    xub_StrLen          nPos;
    BOOL                bSimpleClick;

                        EditFieldInfo();
                        EditFieldInfo( const EditFieldInfo& );

public:
                    EditFieldInfo( Outliner* pOutl, const SvxFieldItem& rFItem, USHORT nPa, xub_StrLen nPo )
                        : rFldItem( rFItem )
                    {
                        pOutliner = pOutl;
                        nPara = nPa; nPos = nPo;
                        pTxtColor = 0; pFldColor = 0; bSimpleClick = FALSE;
                    }
                    ~EditFieldInfo()
                    {
                        delete pTxtColor;
                        delete pFldColor;
                    }

    Outliner*       GetOutliner() const { return pOutliner; }

    const SvxFieldItem& GetField() const { return rFldItem; }

    Color*          GetTxtColor() const { return pTxtColor; }
    void            SetTxtColor( const Color& rColor )
                        { delete pTxtColor; pTxtColor = new Color( rColor ); }

    Color*          GetFldColor() const { return pFldColor; }
    void            SetFldColor( const Color& rColor )
                        { delete pFldColor; pFldColor = new Color( rColor ); }
    void            ClearFldColor()
                        { delete pFldColor; pFldColor = 0; }

    USHORT          GetPara() const { return nPara; }
    xub_StrLen          GetPos() const { return nPos; }

    BOOL            IsSimpleClick() const { return bSimpleClick; }
    void            SetSimpleClick( BOOL bSimple ) { bSimpleClick = bSimple; }

    const String&       GetRepresentation() const                { return aRepresentation; }
    String&             GetRepresentation()                      { return aRepresentation; }
    void                SetRepresentation( const String& rStr )  { aRepresentation = rStr; }
};

struct EBulletInfo
{
    BOOL        bVisible;
    USHORT      nType;          // see SvxNumberType
    String      aText;
    SvxFont     aFont;
    Graphic     aGraphic;
    USHORT      nParagraph;
    Rectangle   aBounds;

    EBulletInfo() : bVisible( FALSE ), nType( 0 ), nParagraph( EE_PARA_NOT_FOUND ) {}
};

#define OUTLINERMODE_DONTKNOW       0x0000
#define OUTLINERMODE_TEXTOBJECT     0x0001
#define OUTLINERMODE_TITLEOBJECT    0x0002
#define OUTLINERMODE_OUTLINEOBJECT  0x0003
#define OUTLINERMODE_OUTLINEVIEW    0x0004

#define OUTLINERMODE_USERMASK       0x00FF

#define OUTLINERMODE_SUBTITLE       (0x0100|OUTLINERMODE_TEXTOBJECT)
#define OUTLINERMODE_NOTE           (0x0200|OUTLINERMODE_TEXTOBJECT)

class Outliner
{
    friend class OutlinerView;
    friend class OutlinerEditEng;
    friend class OutlinerParaObject;
    friend class OLUndoExpand;
    friend class OutlinerUndoChangeDepth;
    friend class OutlinerUndoCheckPara;

    OutlinerEditEng*    pEditEngine;

    ParagraphList*      pParaList;

    Paragraph*          pHdlParagraph;
    Link                aDrawPortionHdl;
    Link                aExpandHdl;
    Link                aParaInsertedHdl;
    Link                aParaRemovingHdl;
    Link                aDepthChangedHdl;
    Link                aWidthArrReqHdl;
    Link                aBeginMovingHdl;
    Link                aEndMovingHdl;
    Link                aIndentingPagesHdl;
    Link                aRemovingPagesHdl;
    Link                aFieldClickedHdl;
    Link                aCalcFieldValueHdl;

    USHORT              nDepthChangedHdlPrevDepth;
    USHORT              nMaxDepth;
    USHORT              nMinDepth;
    USHORT              nFirstPage;

    USHORT              nOutlinerMode;

    BOOL                bIsExpanding; // Nur in Expand/Collaps-Hdl gueltig, mal umstellen
    BOOL                bFirstParaIsEmpty;
    BOOL                bBlockInsCallback;
    BOOL                bStrippingPortions;
    BOOL                bPasting;

    SvxNumberFormat*    pOverwriteLevel0Bullet;

    ULONG               nDummy;

#ifdef _OUTLINER_CXX

    DECL_LINK(              ParaVisibleStateChangedHdl, Paragraph* );
    DECL_LINK(              BeginMovingParagraphsHdl, MoveParagraphsInfo* );
    DECL_LINK(              EndMovingParagraphsHdl, MoveParagraphsInfo* );
    DECL_LINK(              BeginPasteOrDropHdl, PasteOrDropInfos* );
    DECL_LINK(              EndPasteOrDropHdl, PasteOrDropInfos* );
    void                    ImplCheckParagraphs( USHORT nStart, USHORT nEnd );
    BOOL                    ImplHasBullet( USHORT nPara ) const;
    const SvxNumberFormat*  ImplGetBullet( USHORT nPara ) const;
    Size                    ImplGetBulletSize( USHORT nPara );
    void                    ImplCalcBulletText( USHORT nPara, BOOL bRecalcLevel, BOOL bRecalcChilds );
    String                  ImplGetBulletText( USHORT nPara );
    void                    ImplCheckNumBulletItem( USHORT nPara );
    void                    ImplInitDepth( USHORT nPara, USHORT nDepth, BOOL bCreateUndo, BOOL bUndoAction = FALSE );
    void                    ImplSetLevelDependendStyleSheet( USHORT nPara, SfxStyleSheet* pLevelStyle = NULL );

    void                    ImplBlockInsertionCallbacks( BOOL b );

    void                ImplCheckStyleSheet( USHORT nPara, BOOL bReplaceExistingStyle );
    void                ImpRecalcBulletIndent( ULONG nPara );

    const SvxBulletItem& ImpGetBullet( ULONG nPara, USHORT& );
    void        ImpConvertOutToEdt( Paragraph* pPara, ULONG nPara );

    long        ImpCalcMaxBulletWidth( USHORT nPara, const SvxBulletItem& rBullet );
    Font        ImpCalcBulletFont( USHORT nPara ) const;
    Rectangle   ImpCalcBulletArea( USHORT nPara );
    long        ImpGetTextIndent( ULONG nPara );

    USHORT      ImplGetOutlinerMode() const { return nOutlinerMode & OUTLINERMODE_USERMASK; }
    void        ImplCheckDepth( USHORT& rnDepth ) const;
#endif

protected:
    void            ParagraphInserted( USHORT nParagraph );
    void            ParagraphDeleted( USHORT nParagraph );
    void            ParaAttribsChanged( USHORT nParagraph );

    void        PaintBullet( USHORT nPara, const Point& rStartPos,
                    const Point& rOrigin, short nOrientation,
                    OutputDevice* pOutDev );

public:

                    Outliner( SfxItemPool* pPool, USHORT nOutlinerMode );
    virtual         ~Outliner();

    void            Init( USHORT nOutlinerMode );
    USHORT          GetMode() const { return nOutlinerMode; }

    void            SetVertical( BOOL bVertical );
    BOOL            IsVertical() const;


    USHORT          GetScriptType( const ESelection& rSelection ) const;
    LanguageType    GetLanguage( USHORT nPara, USHORT nPos ) const;

    void            SetAsianCompressionMode( USHORT nCompressionMode );

    void            SetKernAsianPunctuation( BOOL bEnabled );

    void            SetText( const OutlinerParaObject& );
    void            SetText( const String& rText, Paragraph* pParagraph );
    String          GetText( Paragraph* pPara, ULONG nParaCount=1 ) const;

    OutlinerParaObject* CreateParaObject( USHORT nStartPara = 0, USHORT nParaCount = 0xFFFF ) const;

    const SfxItemSet& GetEmptyItemSet() const;

    void            SetRefMapMode( const MapMode& );
    MapMode         GetRefMapMode() const;

    void            SetBackgroundColor( const Color& rColor );

    void            SetMinDepth( USHORT nDepth, BOOL bCheckParas = FALSE );
    USHORT          GetMinDepth() const { return nMinDepth; }

    void            SetMaxDepth( USHORT nDepth, BOOL bCheckParas = FALSE );
    USHORT          GetMaxDepth() const { return nMaxDepth; }

    void            SetUpdateMode( BOOL bUpdate );
    BOOL            GetUpdateMode() const;

    void            Clear();


    ULONG           GetParagraphCount() const;
    Paragraph*      GetParagraph( ULONG nAbsPos ) const;

    BOOL            HasParent( Paragraph* pParagraph ) const;
    ULONG           GetAbsPos( Paragraph* pPara );

    USHORT          GetDepth( USHORT nPara ) const;
    void            SetDepth( Paragraph* pParagraph, USHORT nNewDepth );

    BOOL            IsVisible( Paragraph* pPara ) const { return pPara->IsVisible(); }

    void            EnableUndo( BOOL bEnable );
    BOOL            IsUndoEnabled() const;
    void            UndoActionStart( USHORT nId );
    void            UndoActionEnd( USHORT nId );
    BOOL            IsInUndo();


    Paragraph*      GetHdlParagraph() const { return pHdlParagraph; }
    BOOL            IsExpanding() const { return bIsExpanding; }

    void            SetExpandHdl( const Link& rLink ) { aExpandHdl = rLink; }
    Link            GetExpandHdl() const { return aExpandHdl; }

    virtual void    ParagraphInsertedHdl();
    void            SetParaInsertedHdl(const Link& rLink){aParaInsertedHdl=rLink;}
    Link            GetParaInsertedHdl() const { return aParaInsertedHdl; }

    virtual void    ParagraphRemovingHdl();
    void            SetParaRemovingHdl(const Link& rLink){aParaRemovingHdl=rLink;}
    Link            GetParaRemovingHdl() const { return aParaRemovingHdl; }

    virtual void    DepthChangedHdl();
    void            SetDepthChangedHdl(const Link& rLink){aDepthChangedHdl=rLink;}
    Link            GetDepthChangedHdl() const { return aDepthChangedHdl; }
    USHORT          GetPrevDepth() const { return nDepthChangedHdlPrevDepth; }

    void            SetRemovingPagesHdl(const Link& rLink){aRemovingPagesHdl=rLink;}
    Link            GetRemovingPagesHdl() const { return aRemovingPagesHdl; }
    void            SetIndentingPagesHdl(const Link& rLink){aIndentingPagesHdl=rLink;}
    Link            GetIndentingPagesHdl() const { return aIndentingPagesHdl; }
    // nur gueltig in den beiden oberen Handlern
    USHORT          GetSelPageCount() const { return nDepthChangedHdlPrevDepth; }
    // nur gueltig in den beiden oberen Handlern
    ULONG           GetFirstSelPage() const { return (ULONG)pHdlParagraph; }

    void            SetCalcFieldValueHdl(const Link& rLink ) { aCalcFieldValueHdl= rLink; }
    Link            GetCalcFieldValueHdl() const { return aCalcFieldValueHdl; }

    void            SetFieldClickedHdl(const Link& rLink ) { aFieldClickedHdl= rLink; }
    Link            GetFieldClickedHdl() const { return aFieldClickedHdl; }

    void            SetDrawPortionHdl(const Link& rLink){aDrawPortionHdl=rLink;}
    Link            GetDrawPortionHdl() const { return aDrawPortionHdl; }

    void            SetModifyHdl( const Link& rLink );
    Link            GetModifyHdl() const;

    void            SetNotifyHdl( const Link& rLink );

    Link            GetStatusEventHdl() const;

    const Size&     GetPaperSize() const;
    void            SetPaperSize( const Size& rSize );

    void            SetFirstPageNumber( USHORT n )  { nFirstPage = n; }
    USHORT          GetFirstPageNumber() const      { return nFirstPage; }

    void                ClearPolygon();

    void            SetMinAutoPaperSize( const Size& rSz );

    void            SetMaxAutoPaperSize( const Size& rSz );

    void            SetDefTab( USHORT nTab );

    BOOL            IsForceAutoColor() const;

    EBulletInfo     GetBulletInfo( USHORT nPara );

    void            StripPortions();

    // #101498#
    virtual void    DrawingText( const Point& rStartPos, const String& rText, USHORT nTextStart, USHORT nTextLen,
                        const sal_Int32* pDXArray, const SvxFont& rFont,
                        USHORT nPara, xub_StrLen nIndex, BYTE nRightToLeft);

    Size            CalcTextSize();

    void                SetStyleSheetPool( SfxStyleSheetPool* pSPool );
    SfxStyleSheetPool*  GetStyleSheetPool();

    void            SetStyleSheet( ULONG nPara, SfxStyleSheet* pStyle );
    SfxStyleSheet*  GetStyleSheet( ULONG nPara );

    void            SetParaAttribs( ULONG nPara, const SfxItemSet&, bool bApiCall = false );
    SfxItemSet      GetParaAttribs( ULONG nPara );

    // gibt ein Array mit den Bulletbreiten der n Einrueckebenen
    // zurueck. Letzter Wert muss -1 sein. Wird vom Outliner geloescht.
    Link            GetWidthArrReqHdl() const{ return aWidthArrReqHdl; }
    void            SetWidthArrReqHdl(const Link& rLink){aWidthArrReqHdl=rLink; }

    void            SetControlWord( ULONG nWord );
    ULONG           GetControlWord() const;

    Link            GetBeginMovingHdl() const { return aBeginMovingHdl; }
    void            SetBeginMovingHdl(const Link& rLink) {aBeginMovingHdl=rLink;}
    Link            GetEndMovingHdl() const {return aEndMovingHdl;}
    void            SetEndMovingHdl( const Link& rLink){aEndMovingHdl=rLink;}

    ULONG           GetLineCount( ULONG nParagraph ) const;

    SfxUndoManager& GetUndoManager();

    void            QuickSetAttribs( const SfxItemSet& rSet, const ESelection& rSel );
    void            QuickInsertField( const SvxFieldItem& rFld, const ESelection& rSel );
    void            QuickInsertLineBreak( const ESelection& rSel );

    // nur fuer EditEngine-Modus
    void            QuickInsertText( const String& rText, const ESelection& rSel );
    void            QuickDelete( const ESelection& rSel );
    void            QuickRemoveCharAttribs( USHORT nPara, USHORT nWhich = 0 );
    void            QuickFormatDoc( BOOL bFull = FALSE );

    BOOL            UpdateFields();

    virtual String  CalcFieldValue( const SvxFieldItem& rField, USHORT nPara, xub_StrLen nPos, Color*& rTxtColor, Color*& rFldColor );

    void            SetSpeller( ::com::sun::star::uno::Reference<
                            ::com::sun::star::linguistic2::XSpellChecker1 > &xSpeller );
    void            SetHyphenator( ::com::sun::star::uno::Reference<
                        ::com::sun::star::linguistic2::XHyphenator >& xHyph );

    void            SetForbiddenCharsTable( rtl::Reference<SvxForbiddenCharactersTable> xForbiddenChars );

    // Depricated
    void            SetDefaultLanguage( LanguageType eLang );

    // Depricated

    void            SetEditTextObjectPool( SfxItemPool* pPool );
    SfxItemPool*    GetEditTextObjectPool() const;

    void            SetRefDevice( OutputDevice* pRefDev );
    OutputDevice*   GetRefDevice() const;

    ULONG           GetTextHeight() const;
    Point           GetDocPosTopLeft( ULONG nParagraph );
    Point           GetDocPos( const Point& rPaperPos ) const;
    BOOL            IsTextPos( const Point& rPaperPos, USHORT nBorder = 0 );
    BOOL            IsTextPos( const Point& rPaperPos, USHORT nBorder, BOOL* pbBuuletPos );

    void            SetGlobalCharStretching( USHORT nX = 100, USHORT nY = 100 );
    void            EraseVirtualDevice();

    void            SetBigTextObjectStart( USHORT nStartAtPortionCount );
    BOOL            ShouldCreateBigTextObject() const;

    const EditEngine& GetEditEngine() const { return *((EditEngine*)pEditEngine); }

    // this is needed for StarOffice Api
    void SetLevelDependendStyleSheet( USHORT nPara );

    USHORT  GetOutlinerMode() const { return nOutlinerMode & OUTLINERMODE_USERMASK; }
};

}//end of namespace binfilter
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
