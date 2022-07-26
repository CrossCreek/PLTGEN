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
 * AutoUpdate.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : AutoUpdate.cpp

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

//              V4.5-001   2009-07-01 JAM  Added Handle.h include, required
//                                         since HandleConst.h not longer
//                                         includes Handle.h

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//****************************************************************************//

//=============================================================================
#include "AutoUpdate.h"
#include "Handle.h"
#include <sstream>

namespace A3
{

// AutoUpdate::SetDestroyedBeforeDependents()
// called if this object is destroyed before its dependents
// were unregistered or destroyed.
// (i.e., this objects is a data source for other objects that still exist)
// The normal sequence is that dependents should be unregistered or
// destroyed first.
// If an object is destroyed before its dependent unregistered or destroyed,
// an exception is not thown unless the depended object calls a destoyed
// objects non-static function
void AutoUpdate::SetDestroyedBeforeDependents()
{
    // create a new handle to a string that will hold the class
    // name of this object
    Handle< std::string > hdlNmCallerAWOL( new std::string );
    *hdlNmCallerAWOL = typeid( *this ).name();
    // create a new handle to a string that will hold a message
    // that this object was destroyed before its dependents were
    // unregistered for destoyed.
    Handle< std::string > hdlMsgCallerAWOL( new std::string );
    *hdlMsgCallerAWOL =  "AutoUpdate object destroyed before its dependents ";
    *hdlMsgCallerAWOL += "were unregistered or destroyed.";

    // give each depended this class name and message
    if ( m_setToBeCalled.size() ) {
        std::set< AutoUpdate* >::iterator itrSet
            = m_setToBeCalled.begin();
        std::set< AutoUpdate* >::iterator iedSet
            = m_setToBeCalled.end();

        for ( ; itrSet != iedSet; ++itrSet ) {
            (*itrSet)->m_hdlNmCallerAWOL  = hdlNmCallerAWOL;
            (*itrSet)->m_hdlMsgCallerAWOL = hdlMsgCallerAWOL;
            (*itrSet)->EraseCaller( this );
        }
    }
}

/*** Not used--difference assignement process
void AutoUpdate::SetReassignedWithDependents()
{
    Handle< std::string > hdlNmCallerAWOL( new std::string );
    *hdlNmCallerAWOL = typeid( *this ).name();

    Handle< std::string > hdlMsgCallerAWOL( new std::string );
    *hdlMsgCallerAWOL =  "AutoUpdate object reassigned before dependents ";
    *hdlMsgCallerAWOL += "were unregistered or destroyed.";

    if ( m_setToBeCalled.size() ) {
        std::set< AutoUpdate* >::iterator itrSet
            = m_setToBeCalled.begin();
        std::set< AutoUpdate* >::iterator iedSet
            = m_setToBeCalled.end();

        for ( ; itrSet != iedSet; itrSet++ ) {
            (*itrSet)->m_hdlNmCallerAWOL  = hdlNmCallerAWOL;
            (*itrSet)->m_hdlMsgCallerAWOL = hdlMsgCallerAWOL;
            (*itrSet)->EraseCaller( this );
        }
    }

    m_setToBeCalled.clear();
}
***/

void AutoUpdate::ThrowCannotUpdate() const
{
    std::ostringstream msg;
    msg << "An object on which this AutoUpdate object depends was\n"
        << "destroyed or reassigned.  This object cannot be updated.\n"
        << "This object is\n"
        << typeid( *this ).name()
        << std::endl
        << "The destroyed or reassigned object is\n"
        << *m_hdlNmCallerAWOL
        << "\n the message is\n"
        << *m_hdlMsgCallerAWOL
        << std::endl << std::ends;
    std::logic_error err( msg.str() );
    throw err;
}

void
AutoUpdate::ThrowCannotBeMutuallyDependent( AutoUpdate* pObjectToBeCalled ) const
{
    std::ostringstream msg;
    msg << "An object on which an AutoUpdate object depends register \n"
        << "as a dependent.  Objects cannot be mutually dependent.\n"
        << "This object is\n"
        << typeid( *this ).name()
        << std::endl
        << "The object to be register as a dependent is\n"
        << typeid( *pObjectToBeCalled ).name() << std::endl << std::ends;
    std::logic_error err( msg.str() );
    throw err;
}

}
//=============================================================================

