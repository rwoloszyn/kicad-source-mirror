/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 1992-2011 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file  class_board_item.h
 * @brief Classes BOARD_ITEM and BOARD_CONNECTED_ITEM.
 */

#ifndef BOARD_ITEM_STRUCT_H
#define BOARD_ITEM_STRUCT_H


#include <base_struct.h>
#include <gr_basic.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <gr_basic.h>
#include <layers_id_colors_and_visibility.h>

/// Abbrevation for fomatting internal units to a string.
#define FMT_IU     BOARD_ITEM::FormatInternalUnits
#define FMT_ANGLE  BOARD_ITEM::FormatAngle

class BOARD;
class EDA_DRAW_PANEL;


/**
 * Enum STROKE_T
 * is the set of shapes for segments (graphic segments and tracks) which are often
 * in the .m_Shape member
 */
enum STROKE_T
{
    S_SEGMENT = 0,  ///< usual segment : line with rounded ends
    S_RECT,         ///< segment with non rounded ends
    S_ARC,          ///< Arcs (with rounded ends)
    S_CIRCLE,       ///< ring
    S_POLYGON,      ///< polygon (not yet used for tracks, but could be in microwave apps)
    S_CURVE,        ///< Bezier Curve
    S_LAST          ///< last value for this list
};


/**
 * Class BOARD_ITEM
 * is a base class for any item which can be embedded within the BOARD
 * container class, and therefore instances of derived classes should only be
 * found in Pcbnew or other programs that use class BOARD and its contents.
 * The corresponding class in Eeschema is SCH_ITEM.
 */
class BOARD_ITEM : public EDA_ITEM
{
    // These are made private here so they may not be used.
    // Instead everything derived from BOARD_ITEM is handled via DLIST<>'s
    // use of DHEAD's member functions.
    void SetNext( EDA_ITEM* aNext )       { Pnext = aNext; }
    void SetBack( EDA_ITEM* aBack )       { Pback = aBack; }

protected:
    LAYER_NUM m_Layer;

public:

    BOARD_ITEM( BOARD_ITEM* aParent, KICAD_T idtype ) :
        EDA_ITEM( aParent, idtype )
        , m_Layer( FIRST_LAYER )
    {
    }

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    virtual const wxPoint& GetPosition() const = 0;

    virtual void SetPosition( const wxPoint& aPos ) = 0;

    /**
     * A value of wxPoint(0,0) which can be passed to the Draw() functions.
     */
    static wxPoint ZeroOffset;

    BOARD_ITEM* Next() const { return (BOARD_ITEM*) Pnext; }
    BOARD_ITEM* Back() const { return (BOARD_ITEM*) Pback; }
    BOARD_ITEM* GetParent() const { return (BOARD_ITEM*) m_Parent; }

    /**
     * Function GetLayer
     * returns the layer this item is on.
     */
    LAYER_NUM GetLayer() const { return m_Layer; }

    /**
     * Function SetLayer
     * sets the layer this item is on.
     * @param aLayer The layer number.
     * is virtual because some items (in fact: class DIMENSION)
     * have a slightly different initialization
     */
    virtual void SetLayer( LAYER_NUM aLayer )
    {
        // trap any invalid layers, then go find the caller and fix it.
        // wxASSERT( unsigned( aLayer ) < unsigned( NB_PCB_LAYERS ) );
        m_Layer = aLayer;
    }

    /**
     * Function Draw
     * BOARD_ITEMs have their own color information.
     */
    virtual void Draw( EDA_DRAW_PANEL* panel, wxDC* DC,
                       GR_DRAWMODE aDrawMode, const wxPoint& offset = ZeroOffset ) = 0;

    /**
     * Function IsOnLayer
     * tests to see if this object is on the given layer.  Is virtual so
     * objects like D_PAD, which reside on multiple layers can do their own
     * form of testing.
     * @param aLayer The layer to test for.
     * @return bool - true if on given layer, else false.
     */
    virtual bool IsOnLayer( LAYER_NUM aLayer ) const
    {
        return m_Layer == aLayer;
    }

    /**
     * Function IsTrack
     * tests to see if this object is a track or via (or microvia).
     * form of testing.
     * @return bool - true if a track or via, else false.
     */
    bool IsTrack() const
    {
        return ( Type() == PCB_TRACE_T ) || ( Type() == PCB_VIA_T );
    }

    /**
     * Function IsLocked
     * @return bool - true if the object is locked, else false
     */
    virtual bool IsLocked() const
    {
        return false;   // only MODULEs can be locked at this time.
    }

    /**
     * Function UnLink
     * detaches this object from its owner.  This base class implementation
     * should work for all derived classes which are held in a DLIST<>.
     */
    virtual void UnLink();

    /**
     * Function DeleteStructure
     * deletes this object after UnLink()ing it from its owner.
     */
    void DeleteStructure()
    {
        UnLink();
        delete this;
    }

    /**
     * Function ShowShape
     * converts the enum STROKE_T integer value to a wxString.
     */
    static wxString ShowShape( STROKE_T aShape );

    // Some geometric transforms, that must be rewritten for derived classes
    /**
     * Function Move
     * move this object.
     * @param aMoveVector - the move vector for this object.
     */
    virtual void Move( const wxPoint& aMoveVector )
    {
        wxMessageBox( wxT( "virtual BOARD_ITEM::Move used, should not occur" ), GetClass() );
    }

    /**
     * Function Rotate
     * Rotate this object.
     * @param aRotCentre - the rotation point.
     * @param aAngle - the rotation angle in 0.1 degree.
     */
    virtual void Rotate( const wxPoint& aRotCentre, double aAngle )
    {
        wxMessageBox( wxT( "virtual BOARD_ITEM::Rotate used, should not occur" ), GetClass() );
    }

    /**
     * Function Flip
     * Flip this object, i.e. change the board side for this object
     * @param aCentre - the rotation point.
     */
    virtual void Flip( const wxPoint& aCentre )
    {
        wxMessageBox( wxT( "virtual BOARD_ITEM::Flip used, should not occur" ), GetClass() );
    }

    /**
     * Function GetBoard
     * returns the BOARD in which this BOARD_ITEM resides, or NULL if none.
     */
    virtual BOARD* GetBoard() const;

    /**
     * Function GetLayerName
     * returns the name of the PCB layer on which the item resides.
     *
     * @return wxString containing the layer name associated with this item.
     */
    wxString GetLayerName() const;

    virtual bool HitTest( const wxPoint& aPosition )
    {
        return EDA_ITEM::HitTest( aPosition );
    }

    /**
     * Function HitTest
     * tests if the \a aRect intersects or contains this object (depending on \a aContained).
     *
     * @param aRect A reference to an EDA_RECT object containg the area to test.
     * @param aContained Test if \a aRect contains this object completly.
     * @param aAccuracy Increase the item bounding box by this amount.
     * @return bool - True if \a aRect contains this object completly or if \a aRect intersects
     *  the object and \a aContained is False, otherwise false.
     */
    virtual bool HitTest( const EDA_RECT& aRect, bool aContained = true, int aAccuracy = 0) const
    {
        return false;   // derived classes should override this function
    }


    /**
     * Function FormatInternalUnits
     * converts \a aValue from board internal units to a string appropriate for writing to file.
     *
     * @note Internal units for board items can be either deci-mils or nanometers depending
     *       on how KiCad is build.
     * @param aValue A coordinate value to convert.
     * @return A std::string object containing the converted value.
     */
    static std::string FormatInternalUnits( int aValue );

    /**
     * Function FormatAngle
     * converts \a aAngle from board units to a string appropriate for writing to file.
     *
     * @note Internal angles for board items can be either degrees or tenths of degree
     *       on how KiCad is built.
     * @param aAngle A angle value to convert.
     * @return A std::string object containing the converted angle.
     */
    static std::string FormatAngle( double aAngle );

    static std::string FormatInternalUnits( const wxPoint& aPoint );

    static std::string FormatInternalUnits( const wxSize& aSize );

    /// @copydoc VIEW_ITEM::ViewGetLayers()
    virtual void ViewGetLayers( int aLayers[], int& aCount ) const;
};

#endif /* BOARD_ITEM_STRUCT_H */
