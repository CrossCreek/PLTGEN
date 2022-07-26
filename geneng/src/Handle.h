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
 * Handle.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Handle.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  02/27/1999  JAM  new general engineering library
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  new operators
//              V1.3-016  01/26/2000  JAM  created a more flexible conversion
//                                         constructor
//              V1.3      07/27/2000  JAM  moved from global to A3 namespace
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

//              V5.0      2010-06-01  JAM  moved release version of
//                                         ReportHandles to Handle.cpp

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CLASS

//  Name:       Handle<T> =>NOT DESIGNED TO BE INHERITED<=
//  Purpose:    The Handle class family manages the lifetime of objects
//              dynamically created on the heap.  It stops memory leaks,
//              memory corruption, and access to uninitialized memory.
//              Ref: The C++ Programming Language, 3th Edition by B. Stroustrup
//              pp. 782-786
//

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  MEMBER FUNCTIONS

//  Public Member:  Handle()
//  Purpose:        default constructor safely creates an uninitialized handle
//                  if a function tries to access the object controlled by
//                  this handle, a runtime error is thrown
//  Return Type:    n/a
//  Return Value:   n/a

//  Public Member:  Handle(void*)
//  Purpose:        conversion constructor accepts a pointer to the object
//                  be managed
//  Return Type:    n/a
//  Return Value:   n/a
//  Argument Type:  void*
//  Argument Value: address of the object to be managed

//  Public Member:  ~Handle()
//  Purpose:        destructor decrement reference count and deletes object
//                  if neccessary
//  Return Type:    n/a
//  Return Value:   n/a

//  Public Member:  Ptr()
//  Public Member:  Ptr() const
//  Purpose:        returns a pointer to the managed object
//  Return Type:    T*
//  Return Value:   pointer to the managed object

//  Public Member:  Ref()
//  Public Member:  Ref() const
//  Purpose:        returns a reference to the managed object
//  Return Type:    T&
//  Return Value:   reference to the managed object

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  MEMBER OPERATORS

//  Name:           operator->()
//  Name:           operator->() const
//  Purpose:        access operator to the managed object's member functions
//  Return Type:    T*
//  Return Value:   member function return value

//  Name:           operator T*()
//  Name:           operator T*() const
//  Purpose:        returns a pointer to the managed object
//  Return Type:    T*
//  Return Value:   pointer to the managed object

//  Name:           operator*()
//  Name:           operator*() const
//  Purpose:        dereferences pointer to managed object
//  Return Type:    T&
//  Return Value:   reference to the managed object

//  Name:           operator=(const Handle<T>&)
//  Purpose:        assignment; releases current object, shares new object
//  Return Type:    Handle<T>&
//  Return Value:   *this
//  Argument Type:  const Handle<T>&
//  Argument Value: source handle

//  Name:           operator==(const Handle<T>&) const
//  Purpose:        test for equality
//  Return Type:    bool
//  Return Value:   true if handles match, otherwise false
//  Argument Type:  const Handle<T>&
//  Argument Value: handle to be compared

//  Name:           operator!=(const Handle<T>&) const
//  Purpose:        test for inequality
//  Return Type:    bool
//  Return Value:   false if handles match, otherwise true
//  Argument Type:  const Handle<T>&
//  Argument Value: handle to be compared

//  Name:           operator<(const Handle<T>&) const
//  Purpose:        less than operator
//  Return Type:    bool
//  Return Value:   true of this handle is less than the argument handle
//  Argument Type:  const Handle<T>&
//  Argument Value: handle to be compared

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  NOTES :

//****************************************************************************//

#ifndef HANDLE_H
#define HANDLE_H "Handle V4.5-001"

#include "HandleBase.h"
#include <typeinfo>

// ============================================================================
// A3 namespace start
namespace A3
{

template <typename T> class HandleConst;

template <typename T>
class Handle
{
public:
    Handle();


    Handle(T* pT);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit Handle(T* pT);

    Handle(const Handle<T>& hdl)
        : m_ptr(NULL), m_cnt(NULL)
    {
        if (!hdl.IsNullObject()) {
            m_ptr = hdl.m_ptr;
            m_cnt = hdl.m_cnt;
            ++(*m_cnt);
#                   ifdef HDLDB
            HdlSet::Insert(m_ptr, hdl.m_ptr);
            HdlSet::Increment(m_ptr);
#                   endif
        }
    }

    template <typename U>
    Handle(const Handle<U>& hdl)
        : m_ptr(NULL), m_cnt(NULL)
    {
        if (!hdl.IsNullObject()) {
            T* pT = dynamic_cast<T*>(hdl.m_ptr);

            if (!pT)
                Hdl::ThrowBadCast(typeid(*this).name(),
                                  typeid(m_ptr).name(),
                                  typeid(hdl.m_ptr).name());

            m_ptr = pT;
            m_cnt = hdl.m_cnt;
            ++(*m_cnt);
#                   ifdef HDLDB
            HdlSet::Insert(m_ptr, hdl.m_ptr);
            HdlSet::Increment(m_ptr);
#                   endif
        }
    }

    ~Handle();

    Handle& operator=(const Handle& hdl);

    bool operator==(const Handle& hdl) const;
    bool operator!=(const Handle& hdl) const;
    bool operator<(const Handle& hdl) const;

    const T* operator->() const;
    T* operator->();
    operator const T*() const;
    operator T*();
    const T& operator*() const;
    T& operator*();

    bool IsNullObject() const;
    unsigned GetReferenceCount() const;

    const T* Ptr() const;
    T* Ptr();
    const T& Ref() const;
    T& Ref();

protected:
    void Assert() const
    {
        if (!m_ptr) {
            Hdl::ThrowNullPointer(typeid(*this).name());
        }
    }

private:
    T* m_ptr;
    /*mutable*/ unsigned* m_cnt;

    template <typename U> friend class HandleConst;
    template <typename U> friend class Handle;
};

// Handle()
// default constructor safely creates an uninitialized handle
// if a function tries to access the object controlled by this handle,
// a runtime error is thrown
template <typename T>
inline
Handle<T>::Handle()
    : m_ptr(NULL), m_cnt(NULL) { ; }

// Handle(void* pT)
// conversion constructor accepts a pointer to the object to
// be managed
template <typename T>
inline
Handle<T>::Handle(T* pT)
    : m_ptr(pT), m_cnt(pT ? new unsigned(1) : NULL)
{
#       ifdef HDLDB
    HdlSet::Increment(m_ptr);
#       endif
}

// ~Handle()
// decrements the reference count and destoys the
// managed objected if appropriate
template <typename T>
inline
Handle<T>::~Handle()
{
#       ifdef HDLDB
    HdlSet::Decrement(m_ptr);
#       endif

    if (m_cnt && --(*m_cnt) == 0) {
        delete m_ptr;
        delete m_cnt;
    }
}

// operator=(const Handle<T>& hdl)
// releases current object, shares new object
template <typename T>
inline Handle<T>&
Handle<T>::operator=(const Handle<T>& hdl)
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
template <typename T>
inline T*
Handle<T>::operator->()
{
    Assert();
    return static_cast<T*>(m_ptr);
}

// operator T*()
// returns a pointer to the managed object
template <typename T>
inline
Handle<T>::operator T*()
{
    Assert();
    return static_cast<T*>(m_ptr);
}

// operator*()
// returns a reference to the managed object
template <typename T>
inline T&
Handle<T>::operator*()
{
    Assert();
    return *static_cast<T*>(m_ptr);
}

// Ptr()
// returns a pointer to the managed object
template <typename T>
inline T*
Handle<T>::Ptr()
{
    Assert();
    return static_cast<T*>(m_ptr);
}

// Ref()
// returns a reference to the managed object
template <typename T>
inline T&
Handle<T>::Ref()
{
    Assert();
    return *static_cast<T*>(m_ptr);
}

// operator->() const
// access operator to const member functions of the managed object
template <typename T>
inline const T*
Handle<T>::operator->() const
{
    Assert();
    return static_cast<T*>(m_ptr);
}

// operator const T*() const
// returns a const pointer to the managed object
template <typename T>
inline
Handle<T>::operator const T*() const
{
    Assert();
    return static_cast<T*>(m_ptr);
}

// operator*() const
// returns a const reference to the managed object
template <typename T>
inline const T&
Handle<T>::operator*() const
{
    Assert();
    return *static_cast<T*>(m_ptr);
}

// Ptr() const
// returns a const pointer to the managed object
template <typename T>
inline const T*
Handle<T>::Ptr() const
{
    Assert();
    return static_cast<T*>(m_ptr);
}

// Ref() const
// returns a const reference to the managed object
template <typename T>
inline const T&
Handle<T>::Ref() const
{
    Assert();
    return *static_cast<T*>(m_ptr);
}

template <typename T>
inline bool
Handle<T>::operator==(const Handle<T>& hdl) const
{
    return m_ptr == hdl.m_ptr;
}

template <typename T>
inline bool
Handle<T>::operator!=(const Handle<T>& hdl) const
{
    return m_ptr != hdl.m_ptr;
}

template <typename T>
inline bool
Handle<T>::operator<(const Handle<T>& hdl) const
{
    return m_ptr < hdl.m_ptr;
}

template <typename T>
inline bool
Handle<T>::IsNullObject() const
{
    return m_ptr == NULL;
}

// GetReferenceCount() const
// returns the reference count
template <typename T>
inline unsigned
Handle<T>::GetReferenceCount() const
{
    return *m_cnt;
}

// =========================================================================
// specialization helper function
bool isStdStream(std::ostream* os);

// =========================================================================
// std::ostream specialization to support cout and cerr
// class Handle< std::ostream > : public HandleBase
template <>
class Handle<std::basic_ostream<char,std::char_traits<char> > >
{
public:
    typedef std::basic_ostream<char,std::char_traits<char> > ptr_type;

    Handle();
    Handle(std::ostream* pT);

    Handle(const Handle<std::ostream>& hdl)
        : m_ptr(NULL), m_cnt(NULL)
    {
        if (!hdl.IsNullObject()) {
            m_ptr = hdl.m_ptr;
            m_cnt = hdl.m_cnt;
            ++(*m_cnt);
#                   ifdef HDLDB
            HdlSet::Insert(m_ptr, hdl.m_ptr);
            HdlSet::Increment(m_ptr);
#                   endif
        }
    }

    template <typename U>
    Handle(const Handle<U>& hdl)
        : m_ptr(NULL), m_cnt(NULL)
    {
        if (!hdl.IsNullObject()) {
            ptr_type* pT = dynamic_cast<ptr_type*>(hdl.m_ptr);

            if (!pT)
                Hdl::ThrowBadCast(typeid(*this).name(),
                                  typeid(m_ptr).name(),
                                  typeid(hdl.m_ptr).name());

            m_ptr = pT;
            m_cnt = hdl.m_cnt;
            ++(*m_cnt);
#                   ifdef HDLDB
            HdlSet::Insert(m_ptr, hdl.m_ptr);
            HdlSet::Increment(m_ptr);
#                   endif
        }
    }

    ~Handle();

    Handle& operator=(const Handle& hdl);

    bool operator==(const Handle& hdl) const;
    bool operator!=(const Handle& hdl) const;
    bool operator<(const Handle& hdl) const;

    std::ostream* operator->();
    operator std::ostream*();
    std::ostream& operator*();

    const std::ostream* operator->() const;
    operator const std::ostream*() const;
    const std::ostream& operator*() const;

    std::ostream* Ptr();
    const std::ostream* Ptr() const;
    std::ostream& Ref();
    const std::ostream& Ref() const;

    bool IsNullObject() const;
    unsigned GetReferenceCount() const;

protected:
    void Assert() const
    {
        if (!m_ptr) {
            Hdl::ThrowNullPointer(typeid(*this).name());
        }
    }

private:
    std::ostream* m_ptr;
    /*mutable*/ unsigned* m_cnt;
};
} // A3 namespace end
#endif // Handle_H
