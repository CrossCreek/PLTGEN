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
 * Handle.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Handle.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  02/27/1999  JAM  new general engineering library
//                                         derived from fcntlib and SE
//              V1.4      07/27/2000  JAM  moved from global to A3 namespace
//                                         namespace directive no longer
//                                         applied to std
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: added #include <typeinfo>
//              Reason: older MS compilers didn't have this standard header,
//              newer MS compilers do have it, GNU requires it

//              V4.5-001  2009-07-01  JAM  eliminated common base class,
//                                         pointer no longer store a void
//                                         which makes debugging easier but
//                                         is no longer compatible with early
//                                         C++ compilers, added multiple
//                                         inheritance support which is not
//                                         compatible with early C++ compilers,
//                                         and added tracking for debugging--use
//                                         the HDLDB macro

//  NOTES :
//  The Handle class family manages the lifetime of objects dynamically
//  created on the heap.  Stops memory leaks, memory corruption, and
//  access to uninitialized memory.
//  see The C++ Programming Language, 3th Edition by B. Stroustrup
//  pp. 782-786

//****************************************************************************//
#include "Handle.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

// HDLDB enables tracking to support debugging
#ifdef HDLDB
#include <algorithm>
#include <iomanip>
#include <iterator>
#endif

// =============================================================================
// A3 namespace start
namespace A3
{

#   ifdef HDLDB
std::set<HdlInfo> HdlSet::m_setHdl;

// operator<<(std::ostream&, const HdlInfo&): Report address, ref count,
// pointer type,object type of HdlInfo handled object
std::ostream& operator<<(std::ostream& os, const HdlInfo& hi)
{
    os << std::setbase(std::ios_base::hex)
       << std::setw(8)  << hi.Pointer()  << ' '
       << std::setw(8)  << hi.PointerRoot() << ' '
       << std::setbase(std::ios_base::dec)
       << std::setw(6)  << hi.Count() << ' '
       << hi.NamePtr() << "\n" << std::string(25,' ')
       << hi.NameObj();
    return os;
}

// HdlSet::ReportHandles: Report address, ref count, pointer type,
// object type of each handled object in the set
std::ostream& HdlSet::ReportHandles( std::ostream& os )
{
    os << std::setw(8)  << "Address" << ' '
       << std::setw(8)  << "Root" << ' '
       << std::setw(6)  << "Count" << ' '
       << "Pointer Type" << "\n" << std::string(25,' ')
       << "Actual Type" << '\n';
    std::set<HdlInfo>::const_iterator
    itr(m_setHdl.begin()), etr(m_setHdl.end());
    std::ostream_iterator<HdlInfo> osItr(os, "\n");
    std::copy(itr, etr, osItr);
    os << std::endl;
    return os;
}

#   else
std::ostream& HdlSet::ReportHandles( std::ostream& os )
{
    return os;
}
#   endif

// -------------------------------------------------------------------------
namespace Hdl
{
void ThrowNullPointer(const char* sHdlType)
{
    std::ostringstream stmMsg;
    stmMsg <<"Exception thrown by " <<sHdlType
           <<"\nApplication tried to use the NULL pointer stored "
           <<"in this Handle!\n" <<std::ends;
    std::runtime_error except( stmMsg.str() );
    throw except;
}

void ThrowBadCast(const char* sHdlType, const char* strDestType,
                  const char* strSrcType)
{
    std::ostringstream stmMsg;
    stmMsg <<"Exception thrown by " <<sHdlType
           <<"\nBAD CAST\n"
           <<"Tried to cast " <<strSrcType <<std::endl
           <<"as " <<strDestType <<'\n' <<std::ends;
    std::invalid_argument except( stmMsg.str() );
    throw except;
}
}

// -------------------------------------------------------------------------
bool isStdStream( std::ostream* pOs )
{
    return pOs && (&std::cerr==pOs || &std::cout==pOs || &std::clog==pOs);
}

// =========================================================================
// std::ostream specialization to support cout and cerr
// class Handle< std::ostream > : public HandleBase
// =========================================================================
Handle<std::ostream>::Handle()
    : m_ptr(NULL), m_cnt(NULL)
{
}

Handle<std::ostream>::Handle(std::ostream* pT)
    : m_ptr(pT), m_cnt(pT ? new unsigned(1) : NULL)
{
    // ensure that standard std::ostreams are never deleted
    if (isStdStream(pT)) {
        ++m_cnt;
    }

#       ifdef HDLDB
    HdlSet::Increment(m_ptr);
#       endif
}

Handle<std::ostream>::~Handle()
{
#       ifdef HDLDB
    HdlSet::Decrement(m_ptr);
#       endif

    if (m_cnt && --(*m_cnt) == 0) {
        delete m_ptr;
        delete m_cnt;
    }
}

// operator=(const Handle< std::ostream >& hdl)
// releases current object, shares new object
// releases current object, shares new object
Handle<std::ostream>&
Handle<std::ostream>::operator=(const Handle<std::ostream>& hdl)
{
    if (m_ptr != hdl.m_ptr) {
#           ifdef HDLDB
        HdlSet::Decrement(m_ptr);
#           endif

        if (m_cnt && --(*m_cnt) == 0) {
            delete m_ptr;
            delete m_cnt;
        }

        m_ptr = hdl.m_ptr;
        m_cnt = hdl.m_cnt;

        if (m_cnt) {
            (*m_cnt)++;
        }

#           ifdef HDLDB
        HdlSet::Increment(m_ptr);
#           endif
    }

    return *this;
}

// operator->()
// access operator to the managed object's member functions
std::ostream*
Handle<std::ostream>::operator->()
{
    Assert();
    return static_cast<std::ostream*>(m_ptr);
}

// operator std::ostream*()
// returns a pointer to the managed object
Handle<std::ostream>::operator std::ostream*()
{
    Assert();
    return static_cast< std::ostream*>(m_ptr);
}

// operator*()
// returns a reference to the managed object
std::ostream&
Handle<std::ostream>::operator*()
{
    Assert();
    return *static_cast<std::ostream*>(m_ptr);
}

// Ptr()
// returns a pointer to the managed object
std::ostream*
Handle<std::ostream>::Ptr()
{
    Assert();
    return static_cast<std::ostream*>(m_ptr);
}

// Ref()
// returns a reference to the managed object
std::ostream&
Handle<std::ostream>::Ref()
{
    Assert();
    return *static_cast<std::ostream*>(m_ptr);
}

// operator->() const
// access operator to const member functions of the managed object
const std::ostream*
Handle<std::ostream>::operator->() const
{
    Assert();
    return static_cast<std::ostream*>(m_ptr);
}

// operator const std::ostream*() const
// returns a const pointer to the managed object
Handle<std::ostream>::operator const std::ostream*() const
{
    Assert();
    return static_cast<std::ostream*>(m_ptr);
}

// operator*() const
// returns a const reference to the managed object
const std::ostream&
Handle<std::ostream>::operator*() const
{
    Assert();
    return *static_cast<std::ostream*>(m_ptr);
}

// Ptr() const
// returns a const pointer to the managed object
const std::ostream*
Handle<std::ostream>::Ptr() const
{
    Assert();
    return static_cast<std::ostream*>(m_ptr);
}

// Ref() const
// returns a const reference to the managed object
const std::ostream&
Handle< std::ostream >::Ref() const
{
    Assert();
    return *static_cast<std::ostream*>(m_ptr);
}

bool
Handle<std::ostream>::operator==(const Handle<std::ostream>& hdl) const
{
    return m_ptr == hdl.m_ptr;
}

bool
Handle<std::ostream>::operator!=(const Handle<std::ostream>& hdl) const
{
    return m_ptr != hdl.m_ptr;
}

bool
Handle<std::ostream>::operator<(const Handle<std::ostream>& hdl) const
{
    return m_ptr < hdl.m_ptr;
}

bool
Handle<std::ostream>::IsNullObject() const
{
    return m_ptr == NULL;
}

// GetReferenceCount() const
// returns the reference count
unsigned
Handle<std::ostream>::GetReferenceCount() const
{
    return *m_cnt;
}
}
// A3 namespace end
// =============================================================================
