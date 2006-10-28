/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: polyob3d.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2006-10-28 03:53:40 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#ifndef _E3D_POLYOB3D_HXX
#define _E3D_POLYOB3D_HXX

#ifndef _E3D_OBJ3D_HXX
#include <bf_svx/obj3d.hxx>
#endif
#ifndef _POLY3D_HXX
#include <bf_svx/poly3d.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
\************************************************************************/

class E3dPolyObj : public E3dObject
{
 protected:
    PolyPolygon3D   aPolyPoly3D;
    PolyPolygon3D   aPolyNormals3D;
    PolyPolygon3D   aPolyTexture3D;
    Vector3D        aNormal;

    BOOL    bDoubleSided : 1;
    BOOL    bBackSideVisible : 1;
    BOOL    bLighted : 1;
    BOOL    bOwnAttrs;
    BOOL    bOwnStyle;
    long    nObjectnumber;  // default ist -1;

    // [FG]: Zwecks schreiben des Formates der 3.1 Version
    //       Diese Funktionen werden nur von ReadData31 und WriteData31
    //       aufgerufen.
    void WriteData31(SvStream& rOut) const;
    void ReadData31(const SdrObjIOHeader& rHead, SvStream& rIn);

public:
    TYPEINFO();
    E3dPolyObj(const PolyPolygon3D& rPoly3D, FASTBOOL bDblSided = FALSE,
                                             FASTBOOL bLight = TRUE);
    E3dPolyObj(const PolyPolygon3D& rPoly3D, const PolyPolygon3D& rVector3D,
                                             FASTBOOL bDblSided = FALSE,
                                             FASTBOOL bLight = TRUE);
    E3dPolyObj(const PolyPolygon3D& rPoly3D, const PolyPolygon3D& rVector3D,
                                             const PolyPolygon3D& rNormal3D,
                                             FASTBOOL bDblSided = FALSE,
                                             FASTBOOL bLight = TRUE);
    E3dPolyObj(const Vector3D& rP1, const Vector3D& rP2);
    E3dPolyObj();
    virtual ~E3dPolyObj();

    virtual UINT16  GetObjIdentifier() const;
    void SetPolyPolygon3D(const PolyPolygon3D& rNewPolyPoly3D);
    const PolyPolygon3D& GetPolyPolygon3D() const { return aPolyPoly3D; }

    void SetPolyNormals3D(const PolyPolygon3D& rNewPolyPoly3D);
    const PolyPolygon3D& GetPolyNormals3D() const { return aPolyNormals3D; }

    void SetPolyTexture3D(const PolyPolygon3D& rNewPolyPoly3D);
    const PolyPolygon3D& GetPolyTexture3D() const { return aPolyTexture3D; }

    // TakeObjName...() ist fuer die Anzeige in der UI, z.B. "3 Rahmen selektiert".

    virtual const   Rectangle& GetBoundRect() const;

    virtual void WriteData(SvStream& rOut) const;
    virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);



    virtual SdrObjGeoData *NewGeoData() const;
    virtual void          SaveGeoData(SdrObjGeoData &rGeo) const;
    virtual void          RestGeoData(const SdrObjGeoData &rGeo);
    virtual void          SetPage(SdrPage *pNewPage);
    virtual void          SetModel(SdrModel *pNewModel);
    virtual SdrLayerID    GetLayer() const;
    virtual void          NbcSetLayer(SdrLayerID nLayer);
    virtual SfxStyleSheet *GetStyleSheet() const;

    BOOL OwnAttrs() const { return bOwnAttrs; }
    BOOL &OwnAttrs() { return bOwnAttrs; }
    BOOL OwnStyle() const { return bOwnStyle; }
    BOOL &OwnStyle() { return bOwnStyle; }

    BOOL DoubleSided () const { return bDoubleSided; }
    void SetDoubleSided (BOOL bNewDoubleSided) { bDoubleSided = bNewDoubleSided; }

    long  GetObjectnumber () const { return nObjectnumber; }
    void  SetObjectnumber (long value) { nObjectnumber = value; }

    const Vector3D& GetNormal() { return aNormal; }
};

}//end of namespace binfilter
#endif          // _E3D_POLYOB3D_HXX
