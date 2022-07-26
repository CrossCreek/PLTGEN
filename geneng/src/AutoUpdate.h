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
 * AutoUpdate.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : AutoUpdate.h

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : AutoUpdate (abstract class)

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros

//              V4.5-001   2009-07-01JAM  Added Handle.h include, required
//                                         since HandleConst.h not longer
//                                         includes Handle.h

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CLASSES

//  Name:       AutoUpdate
//  Purpose:    Abstract base class for all auto-updating class families.
//              This class defines the required interfaces for these classes
//              and provides a default implementation for a call back method
//              for auto notification of a state change in a auto-update object.

//              AutoUpdate derived class require that all parameters that are
//              needed to compute current return values be provided at the time
//              of class construction.  Invariant parameters are passed by value
//              or const reference to the constructor.  Required parameters that
//              can vary are passed as handles to the class constructor and are
//              dereferenced when an update cycle occurs.
//              AutoUpdate class will automatically update class data members
//              and return current values when an access function is called.

//  Public Member:  AutoUpdate()
//  Purpose:        default constructor
//  Return Type:    n/a
//  Return Value:   n/a
//  Argument Type:  void
//  Argument Value: n/a

//  Public Member:  void Update()
//  Purpose:        explicitly updates class data members
//  Return Type:    void
//  Return Value:   n/a
//  Argument Type:  void
//  Argument Value: n/a

//  Protected Member: virtual bool IsStale() = 0
//  Purpose:          indicates the currentness of the class
//  Return Type:      bool
//  Return Value:     false = class members are current,
//                    true  = class members are stale
//  Argument Type:    void
//  Argument Value:   n/a

//  Public Member:    void RegisterCallBack( AutoUpdate* pObjectToBeCalled )
//  Purpose:          adds class to the call back stack
//  Return Type:      void
//  Return Value:     n/a
//  Argument Type:    AutoUpdate*
//  Argument Value:   pointer to the class to be called back

//  Public Member:    void UnregisterCallBack( AutoUpdate* pObjectToBeCalled )
//  Purpose:          removes class to the call back stack
//  Return Type:      void
//  Return Value:     n/a
//  Argument Type:    AutoUpdate*
//  Argument Value:   pointer to the class to be called back

//  Protected Member: virtual void UpdateState() = 0
//  Purpose:          function to be called when a member auto-update class
//                    is updated and the class is stale
//  Return Type:      void
//  Return Value:     n/a
//  Argument Type:    const AutoUpdate*
//  Argument Value:   pointer to the updated class

//  Protected Member: virtual void CallBack( const AutoUpdate* pUpdatedObject )
//  Purpose:          function to be called when a member auto-update class
//                    is updated, use of this feature is optional
//  Return Type:      void
//  Return Value:     n/a
//  Argument Type:    const AutoUpdate*
//  Argument Value:   pointer to the updated class

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//****************************************************************************//

//=============================================================================
#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H	"AutoUpdate V4.3-009"

//-----------------------------------------------------------------------------
// disable Microsoft's browser long name warning
#if defined(_MSC_VER) && (_MSC_VER < 1300)
// disables long template class name warning
#   pragma warning( disable : 4786 )
#endif
//-----------------------------------------------------------------------------

#include <set>
#include <string>
#include <stdexcept>

#include "HandleConst.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class AutoUpdate
{
public:
    inline AutoUpdate();
    inline AutoUpdate( const AutoUpdate& au );
    inline virtual ~AutoUpdate();

    // ====================================================================
    virtual bool IsStale() const = 0;

    inline void Update();

    inline void RegisterCallBack( AutoUpdate* pObjectToBeCalled );
    inline void UnRegisterCallBack( AutoUpdate* pObjectToBeCalled );

    inline AutoUpdate& operator=( const AutoUpdate& au );

protected:
    virtual void UpdateState() = 0;

    // implement this function if your class needs to do
    // something when a member AutoUpdate class finishes
    // an update cycle, the default behavior is to do nothing
    inline virtual void CallBack( const AutoUpdate* pUpdatedObject );

    inline void ThrowIfStale( const std::string& strMessage ) const;

private:
    inline void UnRegisterDestroyedCallBack( AutoUpdate* pObjectToBeCalled );

    inline void InsertCaller( AutoUpdate* pCaller );
    inline void EraseCaller( AutoUpdate* pCaller );

    std::set< AutoUpdate* > m_setToBeCalled;
    std::set< AutoUpdate* > m_setCallers;

    // Exception Handling
    void SetDestroyedBeforeDependents();
    // void SetReassignedWithDependents();
    void ThrowCannotBeMutuallyDependent( AutoUpdate* pObjectToBeCalled ) const;
    void ThrowCannotUpdate() const;
    HandleConst< std::string > m_hdlNmCallerAWOL;
    HandleConst< std::string > m_hdlMsgCallerAWOL;
};

inline AutoUpdate::AutoUpdate()
{ ; }

inline AutoUpdate::AutoUpdate( const AutoUpdate &au )
    : m_hdlNmCallerAWOL( au.m_hdlNmCallerAWOL ),
      m_hdlMsgCallerAWOL( au.m_hdlMsgCallerAWOL )

{
    // Note, a copied AutoUpdate object doesn't have dependents
    // even if the original AutoUpdate object did.  It does depend
    // on the same objects that the original AutoUpdate object
    // depended on.
    if ( au.m_setCallers.size() ) {
        std::set< AutoUpdate* >::const_iterator itrSet
            = au.m_setCallers.begin();
        std::set< AutoUpdate* >::const_iterator iedSet
            = au.m_setCallers.end();

        while ( itrSet != iedSet ) {
            (*itrSet++)->RegisterCallBack( this );
        }
    }
}

inline AutoUpdate::~AutoUpdate()
{
    if ( m_setCallers.size() ) {
        std::set< AutoUpdate* >::iterator itrSet
            = m_setCallers.begin();
        std::set< AutoUpdate* >::iterator iedSet
            = m_setCallers.end();

        while ( itrSet != iedSet ) {
            (*itrSet++)->UnRegisterDestroyedCallBack( this );
        }
    }

    if ( m_setToBeCalled.size() ) {
        SetDestroyedBeforeDependents();
    }
}

inline AutoUpdate& AutoUpdate::operator=( const AutoUpdate& au )
{
    // Note, a reassigned AutoUpdate object
    // + keeps its original dependents,
    // + doesn't inherit new dependents from the copy-from object
    // + loses its original depends-on objects
    // + gains new depends-on objects for the copy-from object

    // dependents = to-be-called objects
    // depends-on = callers
    if ( this != &au ) {
        // not used--different assignment process
        // if ( m_setToBeCalled.size() )
        // SetReassignedWithDependents();
        if ( m_setCallers.size() ) {
            std::set< AutoUpdate* >::const_iterator itrSet
                = m_setCallers.begin();
            std::set< AutoUpdate* >::const_iterator iedSet
                = m_setCallers.end();

            while ( itrSet != iedSet ) {
                (*itrSet++)->UnRegisterDestroyedCallBack( this );
            }
        }

        m_setCallers.clear();

        if ( au.m_setCallers.size() ) {
            std::set< AutoUpdate* >::const_iterator itrSet
                = au.m_setCallers.begin();
            std::set< AutoUpdate* >::const_iterator iedSet
                = au.m_setCallers.end();

            while ( itrSet != iedSet ) {
                (*itrSet++)->RegisterCallBack( this );
            }
        }

        m_hdlNmCallerAWOL  = au.m_hdlNmCallerAWOL;
        m_hdlMsgCallerAWOL = au.m_hdlMsgCallerAWOL;
    }

    return *this;
}

inline void
AutoUpdate::Update()
{
    if ( IsStale() ) {
        if ( !m_hdlNmCallerAWOL.IsNullObject() ) {
            ThrowCannotUpdate();
        }

        UpdateState();

        if ( m_setToBeCalled.size() ) {
            std::set< AutoUpdate* >::iterator itrSet
                = m_setToBeCalled.begin();
            std::set< AutoUpdate* >::iterator iedSet
                = m_setToBeCalled.end();

            while ( itrSet != iedSet ) {
                (*itrSet++)->CallBack( this );
            }
        }
    }
}

inline void
AutoUpdate::CallBack( const AutoUpdate* ) // pUpdatedObject
{ ; }

inline void
AutoUpdate::RegisterCallBack( AutoUpdate* pObjectToBeCalled )
{
    // if the object is given an object-to-be-called that
    // it is called by (i.e. it depends on) an exception
    // is thrown
    if ( m_setCallers.end() != m_setCallers.find( pObjectToBeCalled ) ) {
        ThrowCannotBeMutuallyDependent( pObjectToBeCalled );
    }

    m_setToBeCalled.insert( pObjectToBeCalled );
    pObjectToBeCalled->InsertCaller( this );
}

inline void
AutoUpdate::UnRegisterCallBack( AutoUpdate* pObjectToBeCalled )
{
    m_setToBeCalled.erase( pObjectToBeCalled );
    pObjectToBeCalled->EraseCaller( this );
}


inline void
AutoUpdate::UnRegisterDestroyedCallBack( AutoUpdate* pObjectToBeCalled )
{
    m_setToBeCalled.erase( pObjectToBeCalled );
}

inline void
AutoUpdate::InsertCaller( AutoUpdate* pCaller )
{
    m_setCallers.insert( pCaller );
}

inline void
AutoUpdate::EraseCaller( AutoUpdate* pCaller )
{
    m_setCallers.erase( pCaller );
}

inline void
AutoUpdate::ThrowIfStale( const std::string& strMessage ) const
{
    if ( IsStale() ) {
        throw std::runtime_error( strMessage );
    }
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // AUTOUPDATE_H
