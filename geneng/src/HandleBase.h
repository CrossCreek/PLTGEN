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
 * HandleBase.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : HandleBase.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  02/27/1999  JAM  new general engineering library
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  new operators
//              V1.3      07/27/2000  JAM  moved from global to A3 namespace
//                                         namespace directive no longer
//                                         applied to std
//              V2.0      04/29/2002  JAM  1st A3 library build

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
//  NOTES :

//****************************************************************************//

#ifndef HANDLEBASE_H
#define HANDLEBASE_H __FILE__ " " __DATE__

#include <iosfwd>

// HDLDB enables tracking to support debugging
#ifdef HDLDB
#include <string>
#include <set>
#endif

// ============================================================================
// A3 namespace start
namespace A3
{
// Utility classes to support debugging
// To use, HDLDB must be defined
// Don't define or undefine HDLDB for release s/w to improve performance
#   ifdef HDLDB
class HdlInfo
{
public:
    // HdlInfo: construct empty
    HdlInfo() : m_ptr(NULL), m_ptrRoot(NULL), m_cnt(0) {;}

    // HdlInfo: construct with handled pointer and set ref count
    template <typename T>
    HdlInfo(T* ptr, int cnt=0)
        : m_ptr(ptr), m_ptrRoot(ptr), m_cnt(cnt), m_sPtrNm(typeid(ptr).name()),
          m_sObjNm(typeid(*ptr).name()) {;}

    // HdlInfo: construct with handled pointer and set ref count
    template <typename T, typename U>
    HdlInfo(T* ptr, U* ptrF, int cnt=0)
        : m_ptr(ptr), m_ptrRoot(ptrF), m_cnt(cnt), m_sPtrNm(typeid(ptr).name()),
          m_sObjNm(typeid(*ptr).name()) {;}

    bool operator==(const HdlInfo& hi) const
    {
        return m_ptr == hi.m_ptr && m_sPtrNm == hi.m_sPtrNm;
    }

    bool operator<(const HdlInfo& hi) const
    {
        return m_ptr != hi.m_ptr
               ? m_ptr < hi.m_ptr
               : m_sPtrNm < hi.m_sPtrNm;
    }

    // operator--: increment reference count
    HdlInfo& operator++()
    {
        ++m_cnt;
        return *this;
    }
    // operator--: decrement reference count
    HdlInfo& operator--()
    {
        --m_cnt;
        return *this;
    }
    // operator--: increment reference count
    HdlInfo operator++(int)
    {
        HdlInfo hi(*this);
        m_cnt++;
        return hi;
    }
    // operator--: decrement reference count
    HdlInfo operator--(int)
    {
        HdlInfo hi(*this);
        m_cnt--;
        return hi;
    }

    // operator--: increment reference count
    const HdlInfo& operator++() const
    {
        ++m_cnt;
        return *this;
    }
    // operator--: decrement reference count
    const HdlInfo& operator--() const
    {
        --m_cnt;
        return *this;
    }
    // operator--: increment reference count
    const HdlInfo operator++(int) const
    {
        HdlInfo hi(*this);
        m_cnt++;
        return hi;
    }
    // operator--: decrement reference count
    const HdlInfo operator--(int) const
    {
        HdlInfo hi(*this);
        m_cnt--;
        return hi;
    }

    // returns handled pointer
    void* Pointer() const
    {
        return m_ptr;
    }
    // returns handled pointer
    void* PointerRoot() const
    {
        return m_ptrRoot;
    }
    // Count: returns number of references to pointer
    int Count() const
    {
        return m_cnt;
    }
    // NamePtr: returns name of handled pointer
    const std::string& NamePtr() const
    {
        return m_sPtrNm;
    }
    // NameObj: returns name of object
    const std::string& NameObj() const
    {
        return m_sObjNm;
    }

private:
    // handled pointer
    void* m_ptr;
    // from pointer
    void* m_ptrRoot;
    // number of references to pointer
    mutable int m_cnt;
    // type of handled pointer
    std::string m_sPtrNm;
    // type of object
    std::string m_sObjNm;
};

class HdlSet
{
public:
    // Optional insertion of a new ptr, Increment does this automatically
    // doesn't change reference count if ptr is already an element
    template <typename T>
    static void Insert(T* ptr);

    // If there is a root prt, then use this before incrementing
    // doesn't change reference count if ptr is already an element
    template <typename T, typename U>
    static void Insert(T* ptr, U* ptrF);

    // Increment: increment the debugging reference counter
    // for the handled object (ptr)
    template <typename T>
    static void Increment(T* ptr);

    // Decrement: decrement the debugging reference counter
    // for the handled object (ptr)
    template <typename T>
    static void Decrement(T* ptr);

    // Report address, ref count, pointer type, object type
    // of each handled object in the set
    static std::ostream& ReportHandles(std::ostream& os);

private:
    static std::set<HdlInfo> m_setHdl;
};

// Optional insertion of a new ptr, Increment does this automatically
// doesn't change reference count if ptr is already an element
template <typename T>
void HdlSet::Insert(T* p)
{
    m_setHdl.insert(p);
}

// If there is a root prt, then use this before incrementing
// doesn't change reference count if ptr is already an element
template <typename T, typename U>
void HdlSet::Insert(T* p, U* f)
{
    m_setHdl.insert(HdlInfo(p,f));
}

// HdlSet::Increment: increment the debugging reference counter
// for the handled object
template <typename T>
void HdlSet::Increment(T* p)
{
    if (!p) {
        return;
    }

    std::set<HdlInfo>::iterator itr(m_setHdl.insert(p).first);
    ++(*itr);
}

// HdlSet::Decrement: decrement the debugging reference counter
// for the handled object
template <typename T>
void HdlSet::Decrement(T* p)
{
    if (!p) {
        return;
    }

    std::set<HdlInfo>::iterator itr(m_setHdl.lower_bound(p));

    if (m_setHdl.end()!=itr && 0<itr->Count()) {
        --(*itr);
    }
}
// -------------------------------------------------------------------------
#   else
class HdlSet
{
public:
    template <typename T>
    static void Insert(T* ptr) {;}

    template <typename T, typename U>
    static void Insert(T* ptr, U* ptrF) {;}

    template <typename T>
    static void Increment(T* ptr) {;}

    template <typename T>
    static void Decrement(T* ptr) {;}

    static std::ostream& ReportHandles(std::ostream& os);

private:
};
#   endif

// -------------------------------------------------------------------------
namespace Hdl
{
void ThrowNullPointer(const char* sHdlType);

void ThrowBadCast(const char* sHdlType, const char* strDestType,
                  const char* strSrcType);
}

}
// A3 namespace end
// ============================================================================

#endif // HANDLEBASE_H
