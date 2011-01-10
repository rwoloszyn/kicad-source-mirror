/********************************************/
/* Definitions for the EESchema program:	*/
/********************************************/

#ifndef CLASS_SCREEN_H
#define CLASS_SCREEN_H

#include "macros.h"
#include "sch_item_struct.h"
#include "class_base_screen.h"


class LIB_PIN;
class SCH_COMPONENT;
class SCH_SHEET_PATH;

/* Max number of sheets in a hierarchy project: */
#define NB_MAX_SHEET 500


class SCH_SCREEN : public BASE_SCREEN
{
    /**
     * Function addConnectedItemsToBlock
     * add items connected at \a aPosition to the block pick list.
     * <p>
     * This method tests all connectable unselected items in the screen that are connected to
     * \a aPosition and adds them to the block selection pick list.  This is used when a block
     * drag is being performed to ensure connections to items in the block are not lost.
     *</p>
     * @param aPosition = The connection point to test.
     */
    void addConnectedItemsToBlock( const wxPoint& aPosition );

public:
    int       m_RefCount;     ///< Number of sheets referencing this screen.
                              ///< Delete when it goes to zero.

    SCH_SCREEN( KICAD_T aType = SCH_SCREEN_T );
    ~SCH_SCREEN();

    /**
     * Function GetDrawItems().
     *
     * @return - A pointer to the first item in the linked list of draw items.
     */
    virtual SCH_ITEM* GetDrawItems() const { return (SCH_ITEM*) BASE_SCREEN::GetDrawItems(); }

    virtual void SetDrawItems( SCH_ITEM* aItem ) { BASE_SCREEN::SetDrawItems( aItem ); }

    /**
     * Function GetCurItem
     * returns the currently selected SCH_ITEM, overriding BASE_SCREEN::GetCurItem().
     * @return SCH_ITEM* - the one selected, or NULL.
     */
    SCH_ITEM* GetCurItem() const {  return (SCH_ITEM*) BASE_SCREEN::GetCurItem(); }

    /**
     * Function SetCurItem
     * sets the currently selected object, m_CurrentItem.
     * @param aItem Any object derived from SCH_ITEM
     */
    void SetCurItem( SCH_ITEM* aItem )
    {
        BASE_SCREEN::SetCurItem( (BASE_SCREEN*) aItem );
    }


    virtual wxString GetClass() const
    {
        return wxT( "SCH_SCREEN" );
    }

    /**
     * Free all the items from the schematic associated with the screen.
     *
     * This does not delete any sub hierarchies.
     */
    void         FreeDrawList();

    void         Place( SCH_EDIT_FRAME* frame, wxDC* DC ) { };

    /**
     * Remove \a aItem from the schematic associated with this screen.
     *
     * @param aItem - Item to be removed from schematic.
     */
    void         RemoveFromDrawList( SCH_ITEM* aItem );

    bool         CheckIfOnDrawList( SCH_ITEM* st );

    void         AddToDrawList( SCH_ITEM* st );

    bool         SchematicCleanUp( wxDC* DC = NULL );

    SCH_ITEM*    ExtractWires( bool CreateCopy );

    /* full undo redo management : */
    // use BASE_SCREEN::PushCommandToUndoList( PICKED_ITEMS_LIST* aItem )
    // use BASE_SCREEN::PushCommandToRedoList( PICKED_ITEMS_LIST* aItem )

    /**
     * Function ClearUndoORRedoList
     * free the undo or redo list from List element
     *  Wrappers are deleted.
     *  data pointed by wrappers are deleted if not in use in schematic
     *  i.e. when they are copy of a schematic item or they are no more in use (DELETED)
     * @param aList = the UNDO_REDO_CONTAINER to clear
     * @param aItemCount = the count of items to remove. < 0 for all items
     * items are removed from the beginning of the list.
     * So this function can be called to remove old commands
     */
    virtual void ClearUndoORRedoList( UNDO_REDO_CONTAINER& aList, int aItemCount = -1 );

    /**
     * Function Save
     * writes the data structures for this object out to \a aFile in "*.sch" format.
     *
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool         Save( FILE* aFile ) const;

    /**
     * Clear the state flags of all the items in the screen.
     */
    void ClearDrawingState();

    int CountConnectedItems( const wxPoint& aPos, bool aTestJunctions ) const;

    LIB_PIN* GetPin( const wxPoint& aPosition, SCH_COMPONENT** aComponent = NULL );

    /**
     * Function ClearAnnotation
     * clears the annotation for the components in \a aSheetPath on the screen.
     * @param aSheetPath The sheet path of the component annotation to clear.  If NULL then
     *                   the entire heirarchy is cleared.
     */
    void ClearAnnotation( SCH_SHEET_PATH* aSheetPath );

    /**
     * Function GetHierarchicalItems
     * adds all schematica sheet and component object in the screen to \a aItems.
     * @param aItems Hierarchical item list.
     */
    void GetHierarchicalItems( std::vector <SCH_ITEM*> aItems );

    /**
     * Function SelectBlockItems
     * creates a list of items found when a block command is initiated.  The items selected
     * depend on the block commad.  If the drag block command is issued, than any items
     * connected to items in the block are also selected.
     */
    void SelectBlockItems();

    /**
     * Function UpdatePickList
     * adds all the items in the screen within the block selection rectangle to the pick list.
     * @return The number of items in the pick list.
     */
    int UpdatePickList();

    virtual void AddItem( SCH_ITEM* aItem ) { BASE_SCREEN::AddItem( (EDA_ITEM*) aItem ); }
    virtual void InsertItem(  EDA_ITEMS::iterator aIter, SCH_ITEM* aItem )
    {
        BASE_SCREEN::InsertItem( aIter, (EDA_ITEM*) aItem );
    }
};


/********************************************************/
/* Class to handle the list of *screens* in a hierarchy */
/********************************************************/

// screens are unique, and correspond to .sch files.
class SCH_SCREENS
{
private:
    std::vector< SCH_SCREEN* > m_screens;
    unsigned int               m_index;

public:
    SCH_SCREENS();
    ~SCH_SCREENS();
    int GetCount() const { return m_screens.size(); }
    SCH_SCREEN* GetFirst();
    SCH_SCREEN* GetNext();
    SCH_SCREEN* GetScreen( unsigned int aIndex );

    /**
     * Function ClearAnnotation
     * clears the annotation for all components in the hierarchy.
     */
    void ClearAnnotation();

    /**
     * Function SchematicCleanUp
     * merges and breaks wire segments in the entire schematic hierarchy.
     */
    void SchematicCleanUp();

    /**
     * Function ReplaceDuplicateTimeStamps
     * test all sheet and component objects in the schematic for duplicate time stamps
     * an replaces them as neccessary.  Time stamps must be unique in order for complex
     * hierarcies know which components go to which sheets.
     * @return The number of duplicate time stamps replaced.
     */
    int ReplaceDuplicateTimeStamps();

    /**
     * Function SetDate
     * sets the date string for every screen to \a aDate.
     * @see GetDate()
     * @param aDate The date string to set for each screen.
     */
    void SetDate( const wxString& aDate );

private:
    void        AddScreenToList( SCH_SCREEN* aScreen );
    void        BuildScreenList( EDA_ITEM* aItem );
};

#endif /* CLASS_SCREEN_H */
