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
#pragma optimize("e",off)
#pragma hdrstop
#endif

#define ITEMID_FONTLIST         0
#define ITEMID_POSTURE          0
#define ITEMID_WEIGHT           0
#define ITEMID_UNDERLINE        0
#define ITEMID_CROSSEDOUT       0
#define ITEMID_SHADOWED         0
#define ITEMID_AUTOKERN         0
#define ITEMID_WORDLINEMODE     0
#define ITEMID_CONTOUR          0
#define ITEMID_PROPSIZE         0
#define ITEMID_CHARSETCOLOR     0
#define ITEMID_KERNING          0
#define ITEMID_CASEMAP          0
#define ITEMID_ESCAPEMENT       0
#define ITEMID_LANGUAGE         0
#define ITEMID_NOLINEBREAK      0
#define ITEMID_NOHYPHENHERE     0
#define ITEMID_BLINK            0

#define ITEMID_FONT        EE_CHAR_FONTINFO
#define ITEMID_COLOR       EE_CHAR_COLOR
#define ITEMID_FONTHEIGHT  EE_CHAR_FONTHEIGHT
#define ITEMID_FONTWIDTH   EE_CHAR_FONTWIDTH

#include <bf_svtools/whiter.hxx>
#include <bf_svx/eeitem.hxx>

#include "schattr.hxx"
#ifndef _SVX_CHRTITEM_HXX //autogen
#define ITEMID_DOUBLE           0
#define ITEMID_CHARTTEXTORDER   SCHATTR_TEXT_ORDER
#define ITEMID_CHARTTEXTORIENT  SCHATTR_TEXT_ORIENT
#define ITEMID_CHARTDATADESCR   SCHATTR_DATADESCR_DESCR

#include <bf_svtools/eitem.hxx>
#endif

#include <bf_svx/fhgtitem.hxx>
#include <bf_svx/svxids.hrc>
#include <globfunc.hxx>

#include "math.h"
#include "float.h"

#include <bf_svx/fontitem.hxx>
#include <bf_svx/wghtitem.hxx>
#include <bf_svx/udlnitem.hxx>
#include <bf_svx/crsditem.hxx>
#include <bf_svx/postitem.hxx>
#include <bf_svx/cntritem.hxx>
#include <bf_svx/shdditem.hxx>
#include <bf_svx/akrnitem.hxx>
#include <bf_svx/wrlmitem.hxx>

#include <algorithm>
#include <functional>

/*************************************************************************
|*
|* Objekt attributieren
|*
\************************************************************************/
namespace binfilter {
/*N*/ SdrObject *SetObjectAttr (SdrObject  *pObj,
/*N*/                         UINT16     nID,
/*N*/                         BOOL       bProtect,
/*N*/                         BOOL       bResize,
/*N*/                         SfxItemSet *pAttr)
/*N*/ {
/*N*/   pObj->InsertUserData (new SchObjectId (nID));
/*N*/   pObj->SetMoveProtect (bProtect);
/*N*/   pObj->SetResizeProtect (bResize);
/*N*/   if (pAttr)
/*N*/       pObj->SetItemSet(*pAttr);
/*N*/
/*N*/   return pObj;
/*N*/ }

/*************************************************************************
|*
|* Objektgruppe erzeugen
|*
\************************************************************************/

/*N*/ SdrObjList *CreateGroup (SdrObjList &rObjList,
/*N*/                        UINT16     nID,
/*N*/                        ULONG      nIndex)
/*N*/ {
/*N*/   SdrObjGroup *pGroup = (SdrObjGroup *) SetObjectAttr (new SchObjGroup, nID, TRUE, TRUE, 0);
/*N*/
/*N*/   rObjList.NbcInsertObject((SdrObject *) pGroup, nIndex);
/*N*/   return pGroup->GetSubList();
/*N*/ }

/*************************************************************************
|*
|* Objektgruppe erzeugen
|*
\************************************************************************/

/*N*/ SchObjGroup *CreateSimpleGroup (UINT16 nID,
/*N*/                               BOOL   bProtect,
/*N*/                               BOOL   bResize)
/*N*/ {
/*N*/   return (SchObjGroup *) SetObjectAttr (new SchObjGroup, nID, bProtect, bResize, 0);
/*N*/ }

/*************************************************************************
|*
|* Passe untere Grenze an den Wertebereich an
|*
\************************************************************************/

/*N*/ double SizeBounds (double dMinValue,
/*N*/                  double dMaxValue,
/*N*/                  BOOL   bIsMax)
/*N*/ {
/*N*/   if( (dMinValue == DBL_MIN) ||
/*N*/       (dMaxValue == DBL_MIN) ||
/*N*/       (dMinValue == dMaxValue) ||
/*N*/       (dMinValue == 0.0) )
/*?*/       return 0.0;
/*N*/
/*N*/   return bIsMax? dMaxValue : dMinValue;
/*N*/
/*N*/   // BM: I removed some very strange code here. It
/*N*/   // calculated a kind of log10 but the charts didn't
/*N*/   // really use the value calculated.
/*N*/ }

/*************************************************************************
|*
|* Erhoehe einen Wert mit Log.
|*
\************************************************************************/

/*N*/ void IncValue(double &rValue,
/*N*/             double fInc,
/*N*/             BOOL   bLogarithm)
/*N*/ {
/*N*/   if (bLogarithm)
/*?*/       rValue *= fInc;
/*N*/   else
/*N*/       rValue += fInc;
/*N*/ }

/*************************************************************************
|*
|* Vermindere einen Wert mit Log.
|*
\************************************************************************/

/*N*/ void DecValue(double &rValue,
/*N*/             double fInc,
/*N*/             BOOL   bLogarithm)
/*N*/ {
/*N*/   if (bLogarithm) rValue /= fInc;
/*N*/   else rValue -= fInc;
/*N*/ }

/*************************************************************************
|*
|* Konvertiert in echte RGB-Farben.
|*
\************************************************************************/

/*N*/ Color RGBColor(ColorData nColorName)
/*N*/ {
/*N*/   Color aColor(nColorName);
/*N*/
/*N*/   return Color(aColor.GetRed(), aColor.GetGreen(), aColor.GetBlue());
/*N*/ }

/*************************************************************************
|*
|* "Stapelt" den angegebenen String.
|*
\************************************************************************/

/*N*/ String StackString( const String& aString )
/*N*/ {
/*N*/   String aStackStr;
/*N*/   xub_StrLen nLen = aString.Len();
/*N*/
/*N*/   if( nLen )
/*N*/   {
/*N*/       // '\n' is interpreted as newline by the outliner
/*N*/       aStackStr.Fill( nLen * 2 - 1, (sal_Unicode)('\n') );
/*N*/
/*N*/       for( xub_StrLen posSrc=0, posDest=0;
/*N*/            posSrc < nLen;
/*N*/            posSrc++, posDest += 2 )
/*N*/           aStackStr.SetChar( posDest, aString.GetChar( posSrc ));
/*N*/   }
/*N*/
/*N*/   return aStackStr;
/*N*/ }

/*************************************************************************
|*
|* Position des ungedrehten Rechtecks entsprechend der Ausrichtung anpassen
|*
\************************************************************************/

/*N*/ void AdjustRect(Rectangle          &rRect,
/*N*/               ChartAdjust        eAdjust)
/*N*/ {
/*N*/   Point aPos = rRect.TopLeft();
/*N*/   Size aSize = rRect.GetSize();
/*N*/
/*N*/   switch (eAdjust)
/*N*/   {
/*N*/       case CHADJUST_TOP_LEFT:
/*N*/
/*N*/           break;
/*N*/
/*N*/       case CHADJUST_TOP_CENTER:
/*N*/
/*N*/           aPos.X() -= aSize.Width() / 2;
/*N*/           break;
/*N*/
/*N*/       case CHADJUST_TOP_RIGHT:
/*N*/
/*N*/           aPos.X() -= aSize.Width();
/*N*/           break;
/*N*/
/*N*/       case CHADJUST_CENTER_LEFT:
/*N*/
/*N*/           aPos.Y() -= aSize.Height() / 2;
/*N*/           break;
/*N*/
/*N*/       case CHADJUST_CENTER_CENTER:
/*N*/
/*?*/           aPos.X() -= aSize.Width() / 2;
/*?*/           aPos.Y() -= aSize.Height() / 2;
/*?*/           break;
/*N*/
/*N*/       case CHADJUST_CENTER_RIGHT:
/*N*/
/*N*/           aPos.X() -= aSize.Width();
/*N*/           aPos.Y() -= aSize.Height() / 2;
/*N*/           break;
/*N*/
/*N*/       case CHADJUST_BOTTOM_LEFT:
/*N*/
/*?*/           aPos.Y() -= aSize.Height();
/*?*/           break;
/*N*/
/*N*/       case CHADJUST_BOTTOM_CENTER:
/*N*/
/*N*/           aPos.X() -= aSize.Width() / 2;
/*N*/           aPos.Y() -= aSize.Height();
/*N*/           break;
/*N*/
/*N*/       case CHADJUST_BOTTOM_RIGHT:
/*?*/           aPos.X() -= aSize.Width();
/*?*/           aPos.Y() -= aSize.Height();
/*N*/
/*N*/   }
/*N*/
/*N*/   rRect.SetPos(aPos);
/*N*/ }

/*************************************************************************
|*
|* Position des gedrehten Rechtecks entsprechend der Ausrichtung anpassen
|*
\************************************************************************/

/*N*/ Size AdjustRotatedRect(const Rectangle       &rOldRect,
/*N*/                      ChartAdjust        eAdjust,
/*N*/                      const Rectangle          &rNewRect)
/*N*/ {
/*N*/
/*N*/   Size aMovement;
/*N*/   Point aOld;
/*N*/   Point aNew;
/*N*/   Point aDifference;
/*N*/
/*N*/   switch (eAdjust)
/*N*/   {
/*?*/       case CHADJUST_TOP_LEFT:
/*?*/
/*?*/           aOld = rOldRect.TopLeft();
/*?*/           aNew = rNewRect.TopLeft();
/*?*/           break;
/*?*/
/*?*/       case CHADJUST_TOP_CENTER:
/*?*/
/*?*/           aOld = rOldRect.TopCenter();
/*?*/           aNew = rNewRect.TopCenter();
/*?*/           break;
/*?*/
/*?*/       case CHADJUST_TOP_RIGHT:
/*?*/
/*?*/           aOld = rOldRect.TopRight();
/*?*/           aNew = rNewRect.TopRight();
/*?*/           break;
/*?*/
/*?*/       case CHADJUST_CENTER_LEFT:
/*?*/
/*N*/           aOld = rOldRect.LeftCenter();
/*N*/           aNew = rNewRect.LeftCenter();
/*N*/           break;
/*?*/
/*?*/       case CHADJUST_CENTER_CENTER:
/*?*/
/*?*/           aOld = rOldRect.Center();
/*?*/           aNew = rNewRect.Center();
/*?*/           break;
/*?*/
/*?*/       case CHADJUST_CENTER_RIGHT:
/*?*/
/*?*/           aOld = rOldRect.RightCenter();
/*?*/           aNew = rNewRect.RightCenter();
/*?*/           break;
/*?*/
/*?*/       case CHADJUST_BOTTOM_LEFT:
/*?*/
/*?*/           aOld = rOldRect.BottomLeft();
/*?*/           aNew = rNewRect.BottomLeft();
/*?*/           break;
/*?*/
/*?*/       case CHADJUST_BOTTOM_CENTER:
/*?*/
/*?*/           aOld = rOldRect.BottomCenter();
/*?*/           aNew = rNewRect.BottomCenter();
/*?*/           break;
/*?*/
/*?*/       case CHADJUST_BOTTOM_RIGHT:
/*?*/
/*?*/           aOld = rOldRect.BottomRight();
/*?*/           aNew = rNewRect.BottomRight();
/*?*/           break;
/*N*/   }
/*N*/
/*N*/   aDifference = ( aOld - aNew);
/*N*/   aMovement =  Size(aDifference.X(),aDifference.Y());
/*N*/
/*N*/   return aMovement;
/*N*/ }
//Umrechnung Textgr��e in Gr��e des BoundRects rotierter Texte
/*N*/ Size GetRotatedTextSize(const Size& rSize,const long nDegrees)
/*N*/ {
/*N*/   if(nDegrees)
/*N*/   {
/*?*/       double fDeg,fSin,fCos;
/*?*/       fDeg=CDEG2RAD(nDegrees);
/*?*/       fSin=fabs(sin(fDeg));
/*?*/       fCos=fabs(cos(fDeg));
/*?*/       Size aRetSize(
/*?*/       (long)( (double)rSize.Width()*fCos + (double)rSize.Height()*fSin ),
/*?*/       (long)( (double)rSize.Width()*fSin + (double)rSize.Height()*fCos )
/*?*/           );
/*?*/       return aRetSize;
/*N*/   }
/*N*/   Size aRetSize(rSize);
/*N*/   return aRetSize;
/*N*/ }
/*************************************************************************
|*
|* die Rotation des Textes in Grad zurueckgeben, falls kein DEGREE-item
|* vorhanden, (z.B. 4.0-Chart) so wird das Item ergaenzt
|*
\************************************************************************/
/*N*/ long GetTextRotation(SfxItemSet &rAttr) //Wrapper, falls eOrient noch nicht ermittelt
/*N*/ {
/*N*/   SvxChartTextOrient eOrient
/*N*/       = ((const SvxChartTextOrientItem&)rAttr.Get(SCHATTR_TEXT_ORIENT)).GetValue();
/*N*/   return GetTextRotation(rAttr,eOrient);
/*N*/ }

/*N*/ long GetTextRotation(SfxItemSet &rAttr,SvxChartTextOrient eOrient)
/*N*/ {
/*N*/
/*N*/   const SfxPoolItem* pPoolItem = NULL;
/*N*/   long nDegrees = 0;
/*N*/
/*N*/     // the attribute is set: use it
/*N*/   if( rAttr.GetItemState( SCHATTR_TEXT_DEGREES, TRUE, &pPoolItem ) == SFX_ITEM_SET )
/*N*/   {
/*?*/         nDegrees = ((const SfxInt32Item*)pPoolItem)->GetValue();
/*?*/         return nDegrees;
/*N*/   }
/*N*/
/*N*/     // otherwise use orientation to set default rotation
/*N*/   switch( eOrient )
/*N*/   {
/*N*/       case CHTXTORIENT_BOTTOMTOP:
/*N*/             nDegrees = 9000;    // 90 deg
/*N*/           break;
/*N*/
/*N*/       case CHTXTORIENT_TOPBOTTOM:
/*?*/             nDegrees = 27000;   // 270 deg
/*?*/           break;
/*N*/
/*N*/       case CHTXTORIENT_STANDARD:
/*N*/       case CHTXTORIENT_STACKED:
/*N*/           break;
/*N*/
/*N*/       default:
/*N*/           break;
/*N*/   }
/*N*/
/*N*/   return nDegrees;
/*N*/ }


/*************************************************************************
|*
|* Ausrichtung entsprechend der Orientierung anpassen
|*
\************************************************************************/

/*N*/ void SetAdjust(ChartAdjust        &rAdjust,
/*N*/              SvxChartTextOrient eOrient)
/*N*/ {
/*N*/   switch (eOrient)
/*N*/   {
/*N*/       case CHTXTORIENT_BOTTOMTOP:
/*N*/           switch (rAdjust)
/*N*/           {
/*N*/               case CHADJUST_TOP_LEFT:
/*N*/                   rAdjust = CHADJUST_TOP_RIGHT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_TOP_CENTER:
/*N*/                   rAdjust = CHADJUST_CENTER_RIGHT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_TOP_RIGHT:
/*N*/                   rAdjust = CHADJUST_BOTTOM_RIGHT;
/*N*/
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_CENTER_LEFT:
/*N*/                   rAdjust = CHADJUST_TOP_CENTER;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_CENTER_RIGHT:
/*N*/                   rAdjust = CHADJUST_BOTTOM_CENTER;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_BOTTOM_LEFT:
/*N*/                   rAdjust = CHADJUST_TOP_LEFT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_BOTTOM_CENTER:
/*N*/                   rAdjust = CHADJUST_CENTER_LEFT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_BOTTOM_RIGHT:
/*N*/                   rAdjust = CHADJUST_BOTTOM_LEFT;
/*N*/                   break;
/*N*/               default:
/*N*/                   break;
/*N*/           }
/*N*/           break;
/*N*/
/*N*/       case CHTXTORIENT_TOPBOTTOM:
/*N*/           switch (rAdjust)
/*N*/           {
/*N*/               case CHADJUST_TOP_LEFT:
/*N*/                   rAdjust = CHADJUST_BOTTOM_LEFT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_TOP_CENTER:
/*N*/                   rAdjust = CHADJUST_CENTER_LEFT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_TOP_RIGHT:
/*N*/                   rAdjust = CHADJUST_TOP_LEFT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_CENTER_LEFT:
/*N*/                   rAdjust = CHADJUST_BOTTOM_CENTER;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_CENTER_RIGHT:
/*N*/                   rAdjust = CHADJUST_TOP_CENTER;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_BOTTOM_LEFT:
/*N*/                   rAdjust = CHADJUST_BOTTOM_RIGHT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_BOTTOM_CENTER:
/*N*/                   rAdjust = CHADJUST_CENTER_RIGHT;
/*N*/                   break;
/*N*/
/*N*/               case CHADJUST_BOTTOM_RIGHT:
/*N*/                   rAdjust = CHADJUST_TOP_RIGHT;
/*N*/                   break;
/*N*/               default:
/*N*/                   break;
/*N*/           }
/*N*/           break;
/*N*/       default:
/*N*/           break;
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|* Textobjekt positionieren
|*
\************************************************************************/
/*N*/ void SetTextPos(SdrTextObj  &rTextObj,
/*N*/               const Point &rPos,SfxItemSet* pAttr)
/*N*/ {
/*N*/   SchObjectAdjust    *pObjAdjust = GetObjectAdjust(rTextObj);
/*N*/   ChartAdjust         eAdjust    = pObjAdjust->GetAdjust();
/*N*/   double              fVal;
/*N*/   SvxChartTextOrient  eOrient    = pObjAdjust->GetOrient();
/*N*/
/*N*/   switch (eOrient)
/*N*/   {
/*N*/       case CHTXTORIENT_BOTTOMTOP:
/*N*/       case CHTXTORIENT_TOPBOTTOM:
/*N*/       {
/*N*/           long nAng = 36000 - rTextObj.GetRotateAngle();
/*N*/           fVal = nAng * nPi180;
/*N*/           rTextObj.NbcRotate(rPos, nAng, sin(fVal), cos(fVal));
/*N*/           break;
/*N*/       }
/*N*/       default:
/*N*/           break;
/*N*/   }
/*N*/
/*N*/   Rectangle aRect = rTextObj.GetLogicRect();
/*N*/   aRect.SetPos(rPos);
/*N*/   AdjustRect(aRect, eAdjust);
/*N*/   rTextObj.NbcSetLogicRect(aRect);
/*N*/
/*N*/   switch (eOrient)
/*N*/   {
/*N*/       case CHTXTORIENT_BOTTOMTOP:
/*N*/       case CHTXTORIENT_TOPBOTTOM:
/*N*/       {
/*N*/           long nDegrees = GetTextRotation(*pAttr,eOrient);
/*N*/           Rectangle aOldBoundRect=rTextObj.GetBoundRect();
/*N*/           fVal = nDegrees * nPi180;
/*N*/           rTextObj.NbcRotate(rPos, nDegrees, sin(fVal), cos(fVal));
/*N*/           Rectangle aNewBoundRect=rTextObj.GetBoundRect();
/*N*/           rTextObj.NbcMove( AdjustRotatedRect(aOldBoundRect, eAdjust, aNewBoundRect));
/*N*/       }
/*N*/       default:
/*N*/           break;
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|* Textobjekt-Groesse entsprechend der Ausrichtung anpassen
|*
\************************************************************************/

/*N*/ void AdjustTextSize(SdrTextObj &rTextObj,
/*N*/                   const Size &rTextSize)
/*N*/ {
/*N*/   Rectangle aRect = rTextObj.GetLogicRect();
/*N*/
/*N*/   if (aRect.GetSize() != rTextSize)
/*N*/   {
/*N*/       SchObjectAdjust    *pObjAdjust = GetObjectAdjust(rTextObj);
/*N*/       ChartAdjust        eAdjust     = pObjAdjust->GetAdjust();
/*N*/       SvxChartTextOrient eOrient     = pObjAdjust->GetOrient();
/*N*/
/*N*/       SetAdjust(eAdjust, eOrient);
/*N*/
/*N*/       Point aOldPos = aRect.TopLeft();
/*N*/
/*N*/       switch (eAdjust)
/*N*/       {
/*N*/           case CHADJUST_TOP_CENTER:
/*N*/               aRect.Left() = aRect.Left() + aRect.GetWidth() / 2 - rTextSize.Width() / 2;
/*N*/               aRect.Right() = aRect.Left() + rTextSize.Width();
/*N*/               aRect.Bottom() = aRect.Top() + rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           case CHADJUST_TOP_RIGHT:
/*N*/               aRect.Left() = aRect.Right() - rTextSize.Width();
/*N*/               aRect.Bottom() = aRect.Top() + rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           case CHADJUST_CENTER_LEFT:
/*N*/               aRect.Right() = aRect.Left() + rTextSize.Width();
/*N*/               aRect.Top() = aRect.Top() + aRect.GetHeight() / 2 - rTextSize.Height() / 2;
/*N*/               aRect.Bottom() = aRect.Top() + rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           case CHADJUST_CENTER_CENTER:
/*N*/               aRect.Left() = aRect.Left() + aRect.GetWidth() / 2 - rTextSize.Width() / 2;
/*N*/               aRect.Right() = aRect.Left() + rTextSize.Width();
/*N*/               aRect.Top() = aRect.Top() + aRect.GetHeight() / 2 - rTextSize.Height() / 2;
/*N*/               aRect.Bottom() = aRect.Top() + rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           case CHADJUST_CENTER_RIGHT:
/*N*/               aRect.Left() = aRect.Right() - rTextSize.Width();
/*N*/               aRect.Top() = aRect.Top() + aRect.GetHeight() / 2 - rTextSize.Height() / 2;
/*N*/               aRect.Bottom() = aRect.Top() + rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           case CHADJUST_BOTTOM_LEFT:
/*N*/               aRect.Right() = aRect.Left() + rTextSize.Width();
/*N*/               aRect.Top() = aRect.Bottom() - rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           case CHADJUST_BOTTOM_CENTER:
/*N*/               aRect.Left() = aRect.Left() + aRect.GetWidth() / 2 - rTextSize.Width() / 2;
/*N*/               aRect.Right() = aRect.Left() + rTextSize.Width();
/*N*/               aRect.Top() = aRect.Bottom() - rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           case CHADJUST_BOTTOM_RIGHT:
/*N*/               aRect.Left() = aRect.Right() - rTextSize.Width();
/*N*/               aRect.Top() = aRect.Bottom() - rTextSize.Height();
/*N*/               break;
/*N*/
/*N*/           default:
/*N*/               aRect.Right() = aRect.Left() + rTextSize.Width();
/*N*/               aRect.Bottom() = aRect.Top() + rTextSize.Height();
/*N*/               break;
/*N*/       }
/*N*/
/*N*/       Point aNewPos = aRect.TopLeft();
/*N*/
/*N*/       if (aNewPos != aOldPos)
/*N*/       {
/*N*/           long nArc = rTextObj.GetRotateAngle();
/*N*/
/*N*/           if (nArc)
/*N*/           {
/*N*/               double fVal = nArc * nPi180;
/*N*/               RotatePoint(aNewPos, aOldPos, sin(fVal), cos(fVal));
/*N*/               aRect.SetPos(aNewPos);
/*N*/           }
/*N*/       }
/*N*/
/*N*/       rTextObj.SetLogicRect(aRect);
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|* Ausgabegroesse ermitteln
|*
\************************************************************************/

/*N*/ Size GetOutputSize(SdrTextObj& rTextObj)
/*N*/ {
/*N*/   return (rTextObj.GetBoundRect().GetSize());
/*N*/ }

/*************************************************************************
|*
|* Erstelle eine Gruppe und die Sublist fuer ein Chart
|*
\************************************************************************/

/*N*/ void CreateChartGroup( SchObjGroup* &pGroup,
/*N*/                      SdrObjList*  &pList )
/*N*/ {
/*N*/   pGroup = new SchObjGroup;
/*N*/   pList  = pGroup->GetSubList();
/*N*/   pGroup->InsertUserData( new SchObjectId( CHOBJID_DIAGRAM ));
/*N*/ }



/*************************************************************************
|*
|* Beseitige alle Statistikattrs
|*
\************************************************************************/


/*N*/ const long nOffX = SCHATTR_AXIS_AUTO_MIN - SCHATTR_X_AXIS_AUTO_MIN;
/*N*/ const long nOffY = SCHATTR_AXIS_AUTO_MIN - SCHATTR_Y_AXIS_AUTO_MIN;
/*N*/ const long nOffZ = SCHATTR_AXIS_AUTO_MIN - SCHATTR_Z_AXIS_AUTO_MIN;

//alte in neue Achsenattr konvertieren
/*N*/ void AxisAttrOld2New(SfxItemSet &rDestSet,BOOL bClear,long nId)
/*N*/ {
/*N*/
/*N*/   double f;
/*N*/   BOOL b;
/*N*/   SfxItemSet aSet(rDestSet); //Kopie
/*N*/
/*N*/   SfxWhichIter aIter (aSet);
/*N*/   USHORT nWhich = aIter.FirstWhich ();
/*N*/
/*N*/   while (nWhich)
/*N*/   {
/*N*/       switch(nWhich)
/*N*/       {
/*N*/           case SCHATTR_X_AXIS_AUTO_MIN:
/*N*/           case SCHATTR_X_AXIS_AUTO_MAX:
/*N*/           case SCHATTR_X_AXIS_AUTO_STEP_MAIN:
/*N*/           case SCHATTR_X_AXIS_AUTO_STEP_HELP:
/*N*/           case SCHATTR_X_AXIS_LOGARITHM:
/*N*/           case SCHATTR_X_AXIS_AUTO_ORIGIN:
/*N*/
/*N*/               if(nId==CHOBJID_DIAGRAM_X_AXIS)
/*N*/               {
/*N*/                   b=((const SfxBoolItem&)rDestSet.Get(nWhich)).GetValue();
/*N*/                   rDestSet.Put(SfxBoolItem(nWhich+USHORT(nOffX),b));
/*N*/                   if(bClear)
/*N*/                       rDestSet.ClearItem(nWhich);
/*N*/               }
/*N*/               break;
/*N*/
/*N*/           case SCHATTR_X_AXIS_MIN:
/*N*/           case SCHATTR_X_AXIS_MAX:
/*N*/           case SCHATTR_X_AXIS_STEP_MAIN:
/*N*/           case SCHATTR_X_AXIS_STEP_HELP:
/*N*/           case SCHATTR_X_AXIS_ORIGIN:
/*N*/
/*N*/               if(nId==CHOBJID_DIAGRAM_X_AXIS)
/*N*/               {
/*N*/                   f=((const SvxDoubleItem&)rDestSet.Get(nWhich)).GetValue();
/*N*/                   rDestSet.Put(SvxDoubleItem(f,nWhich+USHORT(nOffX)));
/*N*/                   if(bClear)
/*N*/                       rDestSet.ClearItem(nWhich);
/*N*/               }
/*N*/               break;
/*N*/
/*N*/           case SCHATTR_Y_AXIS_AUTO_MIN:
/*N*/           case SCHATTR_Y_AXIS_AUTO_MAX:
/*N*/           case SCHATTR_Y_AXIS_AUTO_STEP_MAIN:
/*N*/           case SCHATTR_Y_AXIS_AUTO_STEP_HELP:
/*N*/           case SCHATTR_Y_AXIS_LOGARITHM:
/*N*/           case SCHATTR_Y_AXIS_AUTO_ORIGIN:
/*N*/
/*N*/               if(nId==CHOBJID_DIAGRAM_Y_AXIS)
/*N*/               {
/*N*/                   b=((const SfxBoolItem&)rDestSet.Get(nWhich)).GetValue();
/*N*/                   rDestSet.Put(SfxBoolItem(nWhich+USHORT(nOffY),b));
/*N*/                   if(bClear)
/*N*/                       rDestSet.ClearItem(nWhich);
/*N*/               }
/*N*/               break;
/*N*/
/*N*/           case SCHATTR_Y_AXIS_MIN:
/*N*/           case SCHATTR_Y_AXIS_MAX:
/*N*/           case SCHATTR_Y_AXIS_STEP_MAIN:
/*N*/           case SCHATTR_Y_AXIS_STEP_HELP:
/*N*/           case SCHATTR_Y_AXIS_ORIGIN:
/*N*/
/*N*/               if(nId==CHOBJID_DIAGRAM_Y_AXIS)
/*N*/               {
/*N*/                   f=((const SvxDoubleItem&)rDestSet.Get(nWhich)).GetValue();
/*N*/                   rDestSet.Put(SvxDoubleItem(f,nWhich+USHORT(nOffY)));
/*N*/                   if(bClear)
/*N*/                       rDestSet.ClearItem(nWhich);
/*N*/               }
/*N*/               break;
/*N*/
/*N*/           case SCHATTR_Z_AXIS_AUTO_MIN:
/*N*/           case SCHATTR_Z_AXIS_AUTO_MAX:
/*N*/           case SCHATTR_Z_AXIS_AUTO_STEP_MAIN:
/*N*/           case SCHATTR_Z_AXIS_AUTO_STEP_HELP:
/*N*/           case SCHATTR_Z_AXIS_LOGARITHM:
/*N*/           case SCHATTR_Z_AXIS_AUTO_ORIGIN:
/*N*/
/*N*/               if(nId==CHOBJID_DIAGRAM_Z_AXIS)
/*N*/               {
/*N*/                   b=((const SfxBoolItem&)rDestSet.Get(nWhich)).GetValue();
/*N*/                   rDestSet.Put(SfxBoolItem(nWhich+USHORT(nOffZ),b));
/*N*/                   if(bClear)
/*N*/                       rDestSet.ClearItem(nWhich);
/*N*/               }
/*N*/               break;
/*N*/
/*N*/           case SCHATTR_Z_AXIS_MIN:
/*N*/           case SCHATTR_Z_AXIS_MAX:
/*N*/           case SCHATTR_Z_AXIS_STEP_MAIN:
/*N*/           case SCHATTR_Z_AXIS_STEP_HELP:
/*N*/           case SCHATTR_Z_AXIS_ORIGIN:
/*N*/
/*N*/               if(nId==CHOBJID_DIAGRAM_Z_AXIS)
/*N*/               {
/*N*/                   f=((const SvxDoubleItem&)rDestSet.Get(nWhich)).GetValue();
/*N*/                   rDestSet.Put(SvxDoubleItem(f,nWhich+USHORT(nOffZ)));
/*N*/                   if(bClear)
/*N*/                       rDestSet.ClearItem(nWhich);
/*N*/               }
/*N*/               break;
/*N*/       }
/*N*/       nWhich = aIter.NextWhich();
/*N*/   }
/*N*/ }

/*************************************************************************
|*
|* Pruefe zwei ItemSets und vernichte paarweise verschiedene Items
|*
\************************************************************************/


/*N*/ void IntersectSets( const SfxItemSet &  rSource, SfxItemSet &  rDest )
/*N*/ {
/*N*/     SfxWhichIter aIter( rSource );
/*N*/     SfxItemState aSrcState;
/*N*/
/*N*/     for( USHORT nWhich = aIter.FirstWhich(); nWhich != 0; nWhich = aIter.NextWhich() )
/*N*/     {
/*N*/         aSrcState = rSource.GetItemState( nWhich );
/*N*/
/*N*/         if( // one item is (may be) set but the other one isn't
/*N*/             ( aSrcState != rDest.GetItemState( nWhich ) )
/*N*/             ||
/*N*/             // both items are set, but their content differs
/*N*/             // (if aSrcState is set it follows that also aDestState is set)
/*N*/             ( ( aSrcState == SFX_ITEM_SET )
/*N*/                 &&
/*N*/               ( rSource.Get( nWhich ) != rDest.Get( nWhich ) ) ) )
/*N*/         {
/*N*/             rDest.InvalidateItem( nWhich );
/*N*/         }
/*N*/     }
/*N*/ }

/*************************************************************************
|*
|* Setze je nach Ausrichtungsinformation den Punkt um den ausgerichtet wird
|*
|* Das wird hauptsaechlich von chtmode4 aufgerufen um zu wissen welche
|* Position des Textes man sich merken muss, um dne wird dann mit moeglicherweise
|* veraenderter Schriftgroesse ausgegeben.
|*
\************************************************************************/

/*N*/ Point SetPointOfRectangle (const Rectangle& rRect, ChartAdjust eAdjust)
/*N*/ {
/*N*/   switch (eAdjust)
/*N*/   {
/*?*/       case CHADJUST_TOP_LEFT:
/*?*/            return (rRect.TopLeft());
/*?*/       case CHADJUST_TOP_RIGHT:
/*?*/            return (rRect.TopRight());
/*N*/       case CHADJUST_TOP_CENTER:
/*N*/            return (rRect.TopCenter());
/*N*/       case CHADJUST_CENTER_LEFT:
/*N*/            return (rRect.LeftCenter());
/*?*/       case CHADJUST_CENTER_RIGHT:
/*?*/            return (rRect.RightCenter());
/*?*/       case CHADJUST_CENTER_CENTER:
/*?*/            return (rRect.Center());
/*?*/       case CHADJUST_BOTTOM_LEFT:
/*?*/            return (rRect.BottomLeft());
/*?*/       case CHADJUST_BOTTOM_CENTER:
/*N*/            return (rRect.BottomCenter());
/*?*/       case CHADJUST_BOTTOM_RIGHT:
/*?*/            return (rRect.BottomRight());
/*?*/       default:
/*?*/            ;
/*?*/   }
/*?*/   Point aPoint(-1,-1);
/*?*/   return aPoint;  // Das ist das Default f�r die Plazierungsinformation von Chart-Texten
/*N*/ }

//!!! Es werden NICHT ALLE Attr ausgewertet, nur Gr��en-relevante!
/*N*/ void ItemsToFont(const SfxItemSet& rSet,Font& rFont)
/*N*/ {
/*N*/   const SfxPoolItem* pItem = NULL;
/*N*/   if( rSet.GetItemState( EE_CHAR_FONTINFO, TRUE, &pItem ))
/*N*/   {
/*N*/       SvxFontItem* pFontItem = (SvxFontItem*)pItem;
/*N*/
/*N*/       rFont.SetStyleName(pFontItem->GetStyleName() );
/*N*/       rFont.SetName(     pFontItem->GetFamilyName());
/*N*/       rFont.SetCharSet(  pFontItem->GetCharSet());
/*N*/       rFont.SetFamily(   pFontItem->GetFamily());
/*N*/       rFont.SetPitch(    pFontItem->GetPitch());
/*N*/   }
/*N*/
/*N*/   //  Scale the font's horizontal size like the vertical size.  Assume that the original size is
/*N*/   //  7pt.  The scaling is done here because the item EE_CHAR_FONTWIDTH holds a horizontal scaling
/*N*/   //  factor.  The horizontal size can therefore not be stored there.  But as the font is scaled
/*N*/   //  uniformly, the horizontal size depends uniquely on the vertical size.
/*N*/   long nFontHeight = static_cast<const SvxFontHeightItem&>(rSet.Get(EE_CHAR_FONTHEIGHT)).GetHeight();
/*N*/   long nFontWidth = 0;    // #89001# use default font width
/*N*/   rFont.SetSize (Size (nFontWidth, nFontHeight));
/*N*/
/*N*/   rFont.SetWeight( ((const SvxWeightItem&)rSet.Get( EE_CHAR_WEIGHT )).GetWeight() );
/*N*/   rFont.SetUnderline( ((const SvxUnderlineItem&)rSet.Get( EE_CHAR_UNDERLINE )).GetUnderline() );
/*N*/   rFont.SetStrikeout( ((const SvxCrossedOutItem&)rSet.Get( EE_CHAR_STRIKEOUT )).GetStrikeout() );
/*N*/   rFont.SetItalic( ((const SvxPostureItem&)rSet.Get( EE_CHAR_ITALIC )).GetPosture() );
/*N*/   rFont.SetOutline( ((const SvxContourItem&)rSet.Get( EE_CHAR_OUTLINE )).GetValue() );
/*N*/   rFont.SetShadow( ((const SvxShadowedItem&)rSet.Get( EE_CHAR_SHADOW )).GetValue() );
/*N*/   rFont.SetKerning( ((const SvxAutoKernItem&)rSet.Get( EE_CHAR_PAIRKERNING )).GetValue() );
/*N*/   rFont.SetWordLineMode( ((const SvxWordLineModeItem&)rSet.Get( EE_CHAR_WLM )).GetValue() );
/*N*/ }


} //namespace binfilter
#ifdef DBG_UTIL

// ==================== DEBUG SfxItemSets ====================

#include <bf_svtools/itempool.hxx>
#include <bf_svtools/itemiter.hxx>

#include <cstdio>       // for snprintf
#include <cstring>      // for strncat
namespace binfilter {
/*N*/ void Dbg_DebugItems( SfxItemSet& rSet, ChartModel* pModel )
/*N*/ {
/*N*/   SfxItemPool *pItemPool=&( pModel->GetItemPool() );
/*N*/
/*N*/   char pBuf[ 512 ] = "";
/*N*/   char pSmallBuf[ 128 ] = "";
/*N*/
/*N*/   const USHORT* pRanges = rSet.GetRanges();
/*N*/   for( long n = 0; pRanges[ n ] && n<32; n+=2 )
/*N*/   {
/*N*/       snprintf( pSmallBuf, sizeof(pSmallBuf), "[%" SAL_PRIdINT32 "; %" SAL_PRIdINT32 "] ", (sal_uInt32)pRanges[ n ], (sal_uInt32)pRanges[ n+1 ] );
/*N*/       strncat( pBuf, pSmallBuf, sizeof(pBuf) - strlen(pBuf) - 1 );
/*N*/   }
/*N*/
/*N*/   OSL_TRACE( "SCH:ItemDBG - Ranges: %s", pBuf );
/*N*/
/*N*/   pBuf[ 0 ] = '\0';
/*N*/
/*N*/   long nInv = 0, nCnt = 0, nCnv = 0, nCns = 0;
/*N*/   SfxItemIter aIterator( rSet );
/*N*/
/*N*/   const SfxPoolItem* pItem;
/*N*/   USHORT nWhich, nNewWhich;
/*N*/
/*N*/   pItem = aIterator.FirstItem();
/*N*/   while( pItem )
/*N*/   {
/*N*/       if( ! IsInvalidItem( pItem ) )
/*N*/       {
/*N*/           nWhich= pItem->Which();
/*N*/
/*N*/           nCnt++;
/*N*/           if( nWhich < SCHATTR_END )
/*N*/               nCns++;
/*N*/           if( nCnt < 100 )
/*N*/           {
/*N*/               snprintf( pSmallBuf, sizeof(pSmallBuf), "%" SAL_PRIdINT32 ", ", (sal_uInt32) nWhich );
/*N*/               strncat( pBuf, pSmallBuf, sizeof(pBuf) - strlen(pBuf) - 1 );
/*N*/           }
/*N*/
/*N*/           nNewWhich = pItemPool->GetWhich( nWhich );
/*N*/           if( nWhich != nNewWhich )
/*?*/               nCnv++;
/*N*/       }
/*N*/       else
/*?*/           nInv++;
/*N*/
/*N*/       pItem = aIterator.NextItem();
/*N*/   }
/*N*/
/*N*/   OSL_TRACE( "SCH:ItemDBG - List: %s", pBuf );
/*N*/
/*N*/   long nColor = -1;
/*N*/   long nMat = -1;
/*N*/   const SfxPoolItem *pPoolItem;
/*N*/
/*N*/   if( SFX_ITEM_SET == rSet.GetItemState(XATTR_FILLCOLOR,TRUE,&pPoolItem ) )
/*N*/   {
/*N*/       Color aColor( ( ( const XFillColorItem* ) pPoolItem )->GetValue() );
/*N*/       nColor=aColor.GetRGBColor();
/*N*/   }
/*N*/   if( SFX_ITEM_SET == rSet.GetItemState( SDRATTR_3DOBJ_MAT_COLOR, TRUE, &pPoolItem ) )
/*N*/   {
/*N*/       Color aNew(  ((const SvxColorItem*) pPoolItem )->GetValue()   );
/*N*/       nMat=aNew.GetRGBColor();
/*N*/   }
/*N*/
/*N*/
/*N*/   long r=COLORDATA_RED(nColor),g=COLORDATA_GREEN(nColor),b=COLORDATA_BLUE(nColor);
/*N*/
/*N*/   OSL_TRACE( "SCH:ItemDBG - Info: this=%lx, #=%ld, WID-able=%ld, invalid=%ld", (long)pModel, nCnt, nCnv, nInv );
/*N*/   OSL_TRACE( "... Chart=%ld, RGB=(%d, %d, %d), Mat=%ld", nCns, r, g, b, nMat );
/*N*/ }
} //namespace binfilter
#endif



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
