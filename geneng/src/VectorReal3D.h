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
 * VectorReal3D.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorReal3D.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : VectorReal3D

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  new general engineering library
//                                         derived from fcntlib and SE
//              V1.2-026  09/01/1999  JAM  additional i/o support for
//                                         SST reporting
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.3-009  2007-12-10  JAM  Corrected end() iterators
//                                         Deleted inline from all class decl
//                                         Deleted Vecotr
//                                         Reduced use of virtual
//                                         Added size function
//                                         Minor restructuring

//              V5.0      2007-12-18  JAM  Changed class to template such that
//                                         typedef Vector3DFP<double> VectorReal3D
//                                         The new template class Vector3DFP
//                                         is the new base for EulerAngles and
//                                         EulerAxis classes
//                                         The new template class reduces code
//                                         redundancy

//  NOTES :
//  The VectorReal3D class family provides for the representation of vectors
//  in 3-space and common vector algebra functions. E.g., vector addition,
//  scalar-vector multiplication, dot product, cross product, etc. The class
//  family provides STL-type iterators and functions for use with STL-type
//  containers and algorithms. VectorReal3D is the base class for this class
//  family, providing storage for vector elements, common vector element
//  access functions and operators that are independent of vector
//  representations. Although VectorReal3D is a concrete class, typically
//  one of the subclasses is used to provide a specific vector representation.
//  The VectorReal3D class family provides for casting from one
//  vector representation to other.

//  UNITS:
//  Function Arg & Return: any consistent
//  Stream I/O:            any consistent
//  Initialization Files:  N/A

//------------------------------------------------------------------------------
//  Public Class Interface
//  Constructors
//  VectorReal3D();
//  Default constructor; initializes all elements with 0.0.
//
//  VectorReal3D(double value);
//  Conversion constructor; initializes all elements with value.
//
//  VectorReal3D(double element0, double element1, double element2);
//  Alternative constructor; initializes elements with argument list values.
//
//  The VectorReal3D class doesn’t contain dynamically allocated resources, a
//  defined copy constructor and assignment operator is not necessary.
//
//  Typedefs
//  typedef iterator
//  A type that provides a random-access iterator that can read a const element
//  in a vector.
//
//  typedef const_iterator
//  A type that provides a random-access iterator that can read or modify any
//  element in a vector.
//
//  Member Functions
//  virtual void
//  Set(double element1, double element2, double element3);
//  Sets class elements to argument list values.
//
//  virtual void
//  Get(double& element1, double& element2, double& element3) const;
//  Assigns class element values to argument list parameters.
//
//  iterator begin();
//  const_iterator begin() const;
//  Returns a random-access iterator to the first element.
//
//  iterator end();
//  const_iterator end() const;
//  Returns a random-access iterator to one-pass-last element.
//
//  virtual const char *GetVersion() const;
//  Return class name and version.
//
//  Operators
//  bool operator==(const VectorReal3D& vec1) const;
//  Returns true if all element-wise comparisons between this and vec1 are
//  true, else returns false.
//
//  bool operator!=(const VectorReal3D& vec1) const;
//  Returns !operator==(vec1).
//  External Functions
//  Ordinary Functions
//  None.
//
//  Operators
//  std::ostream&
//  operator<<(std::ostream& os, const VectorReal3D& v);
//  Inserts class elements into the stream in sequence from front to back.
//
//  std::istream&
//  operator>>(std::istream& os, VectorReal3D& v);
//  Assigns extracted elements from the stream to the class from front to back.
//
//  Manipulators
//  ioSeparator
//  For the insertion operator, selects the characters used to delineate each
//  field.  For example, a vector with elements set to 1, 2, 3 and ioSeparator
//  set to “, “; the output may look like:
//  1.000000, 2.000000, 3.000000
//  See the ioSeparator description for more information.

//------------------------------------------------------------------------------
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#ifndef VECTORREAL3D_H
#define VECTORREAL3D_H __FILE__ " " __DATE__

#define VECTORREAL3D_SZ_ARRAY 3
#define VECTORREAL3D_SZ_ARRAY_BYTES (VECTORREAL3D_SZ_ARRAY*sizeof(value_type))

#include "constants.h"
#include <iosfwd>
#include <algorithm>
#include <memory.h>

// ============================================================================
// A3 namespace start
namespace A3
{
// vector types
enum VecType {POSITION = 0x01, VELOCITY = 0x02, ACCELERATION = 0x04};

// forward declaration
template <typename FPT> class Vector3DFP;

typedef Vector3DFP<double> VectorReal3D;

template <typename FPT>
std::istream&
operator>>(std::istream& is, Vector3DFP<FPT> &vec);

template <typename FPT>
std::ostream&
operator<<(std::ostream& os, const Vector3DFP<FPT> &vec);

// -------------------------------------------------------------------------
// Vector3DFP class definition
template <typename FPT>
class Vector3DFP
{
public:
    typedef FPT value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef size_t size_type;
    typedef int difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef const value_type* const_pointer;
    typedef const value_type& const_reference;

    // constructors and destructors
    Vector3DFP();

    // sets all els to value


    explicit Vector3DFP(value_type value);

    Vector3DFP(value_type el0, value_type el1, value_type el2);

    template <typename ITR>
    Vector3DFP(ITR b, ITR e)
    {
        size_type sz(std::distance(b,e));

        if (VECTORREAL3D_SZ_ARRAY != sz) {
            ThrowBadSize(sz);
        }

        std::copy(b, e, m_v);
    }

    virtual ~Vector3DFP() {;}

    // access functions
    virtual void Set(value_type el1, value_type el2, value_type el3);
    virtual void Get(value_type& el1, value_type& el2,
                     value_type& el3) const;

    // operators
    value_type& operator[](size_type index);
    value_type  operator[](size_type index) const;

    bool operator==(const Vector3DFP& vec) const;
    bool operator!=(const Vector3DFP& vec) const;

    // begin & end iterators, size
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    size_type size() const;

protected:
    static void ThrowBadIndex(size_type index);
    static void ThrowBadSize(size_type size);

    value_type m_v[VECTORREAL3D_SZ_ARRAY];

    // -----------------------------------------------------------------
    // OpenGL Support
    // copies from a 3 element array (pArr) into Vector3DFP
    // function created to support SST Visualization
    // avoid using this function, there is no bounds checking on pArr
    virtual void Set(const_pointer pArr);

    // copies Vector3DFP elements into a 3 element array (pArr)
    // function created to support SST Visualization
    // avoid using this function, there is no bounds checking on pArr
    virtual void Get(pointer pArr) const;
};

// -------------------------------------------------------------------------
// constructors
template <typename FPT>
inline
Vector3DFP<FPT>::Vector3DFP()
{
    std::fill(m_v, m_v+VECTORREAL3D_SZ_ARRAY, static_cast<value_type>(0.0));
}

template <typename FPT>
inline
Vector3DFP<FPT>::Vector3DFP(value_type value)
{
    std::fill(m_v, m_v+VECTORREAL3D_SZ_ARRAY, value);
}

template <typename FPT>
inline
Vector3DFP<FPT>::Vector3DFP(value_type el0, value_type el1, value_type el2)
{
    Set(el0, el1, el2);
}

// -------------------------------------------------------------------------
// access functions
template <typename FPT>
inline void
Vector3DFP<FPT>::Set(value_type el0, value_type el1, value_type el2)
{
    m_v[0] = el0;
    m_v[1] = el1;
    m_v[2] = el2;
}

template <typename FPT>
inline void
Vector3DFP<FPT>::Get(value_type& el0, value_type& el1,
                     value_type& el2) const
{
    el0 = m_v[0];
    el1 = m_v[1];
    el2 = m_v[2];
}

// copies from a 3 element array (pArr) into Vector3DFP
// function created to support SST Visualization
// avoid using this function, there is no bounds checking on pArr
template <typename FPT>
inline void
Vector3DFP<FPT>::Set(const_pointer pArr)
{
    // memcopy used for speed to support graphics
    memcpy(m_v, pArr, VECTORREAL3D_SZ_ARRAY_BYTES);
}

// copies Vector3DFP elements into a 3 element array (pArr)
// function created to support SST Visualization
// avoid using this function, there is no bounds checking on pArr
template <typename FPT>
inline void
Vector3DFP<FPT>::Get(pointer pArr) const
{
    // memcopy used for speed to support graphics
    memcpy(pArr, m_v, VECTORREAL3D_SZ_ARRAY_BYTES);
}

// -------------------------------------------------------------------------
// operators
template <typename FPT>
inline bool
Vector3DFP<FPT>::operator==(const Vector3DFP& vec) const
{
    return !memcmp(m_v, vec.m_v, VECTORREAL3D_SZ_ARRAY_BYTES);
}

template <typename FPT>
inline bool
Vector3DFP<FPT>::operator!=(const Vector3DFP& vec) const
{
    return !operator==(vec);
}

template <typename FPT>
inline typename Vector3DFP<FPT>::value_type&
Vector3DFP<FPT>::operator[](size_type i)
{
#   ifdef _DEBUG

    if (i >= VECTORREAL3D_SZ_ARRAY) {
        ThrowBadIndex(i);
    }

#   endif // _DEBUG
    return m_v[i];
}

template <typename FPT>
inline typename Vector3DFP<FPT>::value_type
Vector3DFP<FPT>::operator[](size_type i) const
{
#   ifdef _DEBUG

    if (i >= VECTORREAL3D_SZ_ARRAY) {
        ThrowBadIndex(i);
    }

#   endif // _DEBUG
    return m_v[i];
}

// -------------------------------------------------------------------------
// begin & end iterators, size
template <typename FPT>
inline typename Vector3DFP<FPT>::iterator
Vector3DFP<FPT>::begin()
{
    return m_v;
}

template <typename FPT>
inline typename Vector3DFP<FPT>::const_iterator
Vector3DFP<FPT>::begin() const
{
    return m_v;
}

template <typename FPT>
inline typename Vector3DFP<FPT>::iterator
Vector3DFP<FPT>::end()
{
    return m_v+VECTORREAL3D_SZ_ARRAY;
}

template <typename FPT>
inline typename Vector3DFP<FPT>::const_iterator
Vector3DFP<FPT>::end() const
{
    return m_v+VECTORREAL3D_SZ_ARRAY;
}

template <typename FPT>
inline typename Vector3DFP<FPT>::size_type
Vector3DFP<FPT>::size() const
{
    return VECTORREAL3D_SZ_ARRAY;
}
}
// A3 namespace end
// ============================================================================
#endif // VECTORREAL3D_H
