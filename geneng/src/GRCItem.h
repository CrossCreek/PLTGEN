/*=============================================================================================
 * CLASSIFICATION: Unclassified
 * Government Purpose Rights
 * Contract No: 12-C-0158
 * Contractor Name: Perspecta
 * Contractor Address: 15052 Conference Center Dr. Chantilly, VA 20151
 * Expiration Date: 31 Aug. 2023
 * The Government's rights to use, modify, reproduce, release, perform, display, or disclose these
 * technical data and computer software are restricted by paragraph (b)(1) of clause N52.227-002,
 * Rights in Technical Data and Computer Software: Noncommercial Items, contained in the contract
 * identified above. No restrictions apply after the expiration date shown above. Any reproduction of
 * technical data or computer software, or portions thereof marked with this legend,must also reproduce the markings.
 *
 *
 * GRCItem.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : GRCItem.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  2000-03-29  JAM  Initial release of this software
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: changed static_cast<std::streampos> to static_cast<std::streamoff>
//              Reason: avoids implicit cast warnings with certain compiler settings

//              V4.5-001  2009-07-01  JAM  Moved static variable initialization
//                                         to Initialize.cpp

//              V5.0      2010-07-15  JAM  Moved class static defs

//  NOTES : This class was designed to part-of Not inherited.
//          The public functions and destructor are Not virtual.
//          It is not reccommended to inherit this class.

//          An item can be one of three things, The whole run control file,
//          a block of data (section) in the run control file or just a
//          single item entry.  Single item entries are within blocks and
//          blocks can be within other blocks or the whole run control file.
//          For example : The run control file is a parent to a block and
//          a block is a parent to a single item entry.

//****************************************************************************//

// CLASS PURPOSE :

// To find blocks (sections) or individual single items within the run control
// file based on given token (tag) enclosed in less than and greater than symbols.
//

// <START_TAG>  <END_TAG>   For a single item entry the end tag is optional


// CLASS INTERFACES :

//----------------------------------------------------------------------------//

// PUBLIC :

//----------------------------------------------------------------------------//

//  inline GRCItem()

//  Purpose  : Default class constructor to initialize data

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : No exceptions thrown.

//----------------------------------------------------------------------------//

//  GRCItem(istream& isRunControl, std::streampos startPosition,
//         std::streampos endPosition );

//  Purpose  : Alternate class constructor to initialize data.
//             This constructor is used to create an item representing all of
//             run control, this item does not have a parent
//             this must be the first item created, the root item.
//             Should only be used once.

//  Input    :
//     isRunControl  - input stream to run control file
//     startPosition - start of run control file in the input stream
//     endPosition   - end of run control file in the input stream

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  GRCItem(const char* startToken, const char* endToken,
//         const GRCItem& parentBlock );

//  Purpose  : Alternate class constructor to initialize data
//             This constructor is used to create an item representing a block
//             or section of the run control file.  The parent is the whole
//             run control file or another block outside this block.

//  Input    :
//     startToken  - pointer to text for start token for this block
//     endToken    - pointer to text for end token for this block
//     parentBlock - item reference for the parent item for this item

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  GRCItem(const char* token, const GRCItem& parentBlock );

//  Purpose  : Alternate class constructor to initialize data
//             This constructor is used to create an item representing a
//             single data entry.  The parent is a block that this item
//             is within

//  Input    :
//     token       - pointer to text for this single item, no end token
//     parentBlock - item reference to the parent item for this item

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline ~GRCItem()

//  Purpose  : Definition for class destructor

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : No exceptions thrown

//----------------------------------------------------------------------------//

//  unsigned int Count(void);

//  Purpose  : For the current item this class represents (block or single item)
//             counts the number of occurences for this item (uses tags)
//             within the parent item for this item.

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines or
//             problem with stream s_pIsRunControl.

//----------------------------------------------------------------------------//

//  bool FindFirst(bool bFailIsFatal = false)

//  Purpose  : Find the first item that this class represents within the
//             parent item for this item, use tags item created with

//  Input    :
//     bFailIsFatal - when True, an exception is thrown if the item is NOT found
//                    (Optional argument)

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines or
//             token not found and bFailIsFatal is True

//----------------------------------------------------------------------------//

//  bool FindNext(bool bFailIsFatal = false)

//  Purpose  : Find the next item that this class represents within the
//             parent item for this item, use tags item created with.
//             Can use this routine to find the first item if desired.

//  Input    :
//     bFailIsFatal - when True, an exception is thrown if the item is NOT found
//                    (Optional argument)

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines or
//             problem with stream s_pIsRunControl or
//             token not found and bFailIsFatal is True

//----------------------------------------------------------------------------//

//  std::streampos GetCurrentEnd(void) const

//  Purpose   : For this item return the end input stream pointer for the
//              end of the item in the run control file.  This is the byte
//              before the end tag if an end tag exists for the item, otherwise
//              just the last byte for the item's information.

//  Input     : None

//  Output    : None

//  Return    : The end input stream pointer

//  Except    : No exceptions thrown.

//----------------------------------------------------------------------------//

//  std::streampos GetCurrentStart(void) const

//  Purpose   : For this item return the start input stream pointer for the
//              start of the item in the run control file.  This is the first
//              byte ater the start tag.

//  Input     : None

//  Output    : None

//  Return    : The start input stream pointer

//  Except    : No exceptions thrown.

//----------------------------------------------------------------------------//

//  inline const char* GetVersion(void) const

//  Purpose  : Return the current version for this class

//  Input    : None

//  Output   : None

//  Return   : Content of the define GRCITEM_H

//  Except   : No exceptions thrown

//----------------------------------------------------------------------------//

//  void Reset(void)

//  Purpose  : Still a mystery but necessary.  See Joe.

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  void Rewind(void)

//  Purpose  : set the input stream pointer back to the begining of the
//             block or single item entry.

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem stream s_pIsRunControl

//----------------------------------------------------------------------------//

//  void Set(istream& isRunControl, std::streampos startPosition,
//           std::streampos endPosition);

//  Purpose  : When default contructor is used to instantiate the class
//             object use this function to create the root item.
//             This set is used to create an item representing all of
//             run control, this item does not have a parent
//             this must be the first item created, the root item.
//             Should only be used once.

//  Input    :
//     isRunControl  - input stream to run control file
//     startPosition - start of run control file in the input stream
//     endPosition   - end of run control file in the input stream

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines

//----------------------------------------------------------------------------//

//  void Set(const char* startToken, const char* endToken,
//           const GRCItem& parentBlock);

//  Purpose  : When default contructor is used to instantiate the class
//             object use this function to create the block item.
//             This set is used to create an item representing a block
//             or section of the run control file.  The parent is the whole
//             run control file or another block outside this block.

//  Input    :
//     startToken  - pointer to text for start token for this block
//     endToken    - pointer to text for end token for this block
//     parentBlock - item reference for the parent item for this item

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines

//----------------------------------------------------------------------------//

//  void Set(const char* token, const GRCItem& parentBlock);

//  Purpose  : When default contructor is used to instantiate the class
//             object use this function to create the single item entry.
//             This set is used to create an item representing a
//             single data entry.  The parent is a block that this item
//             is within

//  Input    :
//     token       - pointer to text for this single item, no end token
//     parentBlock - item reference to the parent item for this item

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines or
//             s_pIsRunControl = NULL

//----------------------------------------------------------------------------//

//  void SetPosition(const GRCItem& rcItem);

//  Purpose  : Take control of the input stream for the item and move it

//  Input    :
//     rcItem - Item to change input stream for

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline bool WasLastFindSuccessful(void) const

//  Purpose   : Check if last search for a token within the item was successful

//  Input     : None

//  Output    : None

//  Return    : True, if last search was successful, otherwise false

//  Except    : No exceptions thrown.

//----------------------------------------------------------------------------//

// PROTECTED :

//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//

// PRIVATE :

//----------------------------------------------------------------------------//

//  inline void Initialize(void)

//  Purpose  : Initialize class data members

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : No exceptions thrown

//----------------------------------------------------------------------------//

//  static std::streampos FindToken(const char* token, std::streampos posStartSearch,
//                             std::streampos posEndSearch);

//  Purpose  : Used to actually find a token within the given start and
//             input stream positions, the work horse routine for the class

//  Input    :
//     token          - the token or tag to search for
//     posStartSearch - start search position in the input stream
//     posEndSearch   - end search position in the input stream

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines or
//             problem with stream s_pIsRunControl

//****************************************************************************//

#ifndef GRCITEM_H
#define GRCITEM_H  "GRCItem V4.3-009"

#include <iostream>
#include <string>       // For STL string

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class GRCItem
{
public:

    // Default constructor and Destructor

    inline GRCItem();
    inline ~GRCItem();

    // Alternate constructors

    // Constructor for the whole run control (root) item

    GRCItem( std::istream& isRunControl, std::streampos startPosition,
             std::streampos endPosition );

    // Constructor for a block item

    GRCItem( const char* startToken, const char* endToken,
             const GRCItem& parentBlock );

    // Constructor for a single item entry

    GRCItem( const char* token, const GRCItem& parentBlock );

    // Public member functions

    // Count number of this item in item's parent block

    unsigned int Count( void );

    // Find the first item of this item's type in parent block

    bool FindFirst( bool bFailIsFatal = false );

    // Find next item of this item's type in parent block

    bool FindNext( bool bFailIsFatal = false );

    // Get end input stream position for this item, before tag

    inline std::streampos GetCurrentEnd( void ) const;

    // Get start input stream position for this item, after tag

    inline std::streampos GetCurrentStart( void ) const;

    // See Joe, resets something

    void Reset( void );

    // Set input stream pointer to the start of the item

    void Rewind( void );

    // Create a whole run control item when default constructor is used

    void Set( std::istream& isRunControl,
              std::streampos startPosition, std::streampos endPosition );

    // Create a block item when the default constructor is used

    void Set( const char* startToken, const char* endToken,
              const GRCItem& parentBlock );

    // Create a single item entry when the default constructor is used

    void Set( const char* token, const GRCItem& parentBlock );

    // Last token search for item in parent block successful

    inline bool WasLastFindSuccessful(void) const;

    // Change the position of the input stream for the item

    void SetPosition( const GRCItem& rcItem );

    // Return current version of this class

    inline const char* GetVersion(void) const;

    // No public data members

protected:

    // No protected member functions

    // No protected data members

private:

    // Private member functions

    // Initialize data members

    inline void Initialize( void );

    // Find given token in given input stream range

    static std::streampos FindToken( const std::string& token,
                                     std::streampos posStartSearch,
                                     std::streampos posEndSearch,
                                     bool bFailIsFatal = false );

    // Private data members

    std::string m_strStartToken;     // Start token (tag) for this item
    std::string m_strEndToken;       // End token (tag) for this item

    std::streampos m_currentStart;   // Start input stream position in
    // run control for this item
    std::streampos m_currentEnd;     // End input stream position in
    // run control for this item

    const GRCItem* m_parentBlock;     // Pointer to the parent item for this item
    // Read only

    bool m_bLastFindSuccessful;      // True, if last token search for item
    // was successful
    bool m_bBlockType;               // True if GRCItem represents a data block,
    // False if GRCItem represents a data item

    static std::istream* s_pIsRunControl; // Copy of the run control file in memory
    // as an input string stream

    static const std::streampos s_posNotFound;  // the stream position that is
    // returned when the token could
    // not be found

    static const std::string s_enabledToken;
    static const std::string s_strEnabledOn;
    static const std::string s_strEnabledOff;
};

//----------------------------------------------------------------------------//

GRCItem::GRCItem()
{
    // Initialize data members
    Initialize();
}

//----------------------------------------------------------------------------//

inline GRCItem::~GRCItem()
{
    // Nothing has to be destructed for GRCItem class local data members
}

//----------------------------------------------------------------------------//

inline void GRCItem::Initialize(void)
{
    // Initialize local data members
    m_parentBlock         = NULL;
    m_currentStart        = 0;
    m_currentEnd          = 0;
    m_bLastFindSuccessful = false;
    m_bBlockType          = false;
}

//----------------------------------------------------------------------------//

inline std::streampos GRCItem::GetCurrentEnd(void) const
{
    return m_currentEnd;
}

//----------------------------------------------------------------------------//

inline std::streampos GRCItem::GetCurrentStart(void) const
{
    return m_currentStart;
}

//----------------------------------------------------------------------------//

inline const char* GRCItem::GetVersion(void) const
{
    return GRCITEM_H;
}

//----------------------------------------------------------------------------//

inline bool GRCItem::WasLastFindSuccessful(void) const
{
    return m_bLastFindSuccessful;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // GRCITEM_H
