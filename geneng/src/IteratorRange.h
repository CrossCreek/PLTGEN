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
 * IteratorRange.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : IteratorRange.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : IteratorRange

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V0.1-001  2002-04-24  JAM  Alpha Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: changed distance_type to difference_type, added
//              preprocessor check to use distance_type with VC++ V6
//              Reason: distance_type was non-standard MS implementation, need
//              preprocessor check for VC++ compiler backward compatability
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings

//  NOTES : Template base class



//****************************************************************************//

//==============================================================================
#ifndef ITERATORRANGE_H
#define ITERATORRANGE_H	"IteratorRange V4.3-009"

#if defined(_MSC_VER) && (_MSC_VER < 1300)
#   pragma warning( disable : 4786 ) // disables long template name warning
#	define difference_type distance_type
#endif

//------------------------------------------------------------------------------
// required headers
#include <iterator>
#include "Angle.h"
#include "Normalize.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//==============================================================================
// iterators
//------------------------------------------------------------------------------
template < class T >
class ConstIteratorRange
    : public std::iterator< std::random_access_iterator_tag, T >
{
public:
    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::difference_type
    difference_type;

    // --------------------------------------------------------------------
    // constructors
    // default behavior of copy constructor and assignment operator
    // is desirable in this case, we want to share pointers
    ConstIteratorRange()
        : m_initial( 0 ), m_increment( 0 ),  m_idx( 0 ) { ; }

    ConstIteratorRange( T initial, T increment )
        : m_initial( initial ), m_increment( increment ), m_idx( 0 ) { ; }

    // --------------------------------------------------------------------
    // standard random access iterator operators
    ConstIteratorRange& operator++()
    {
        m_idx++;
        return *this;
    }

    ConstIteratorRange& operator--()
    {
        m_idx--;
        return *this;
    }

    ConstIteratorRange  operator++( int )
    {
        ConstIteratorRange itr( *this );
        m_idx++;
        return itr;
    }

    ConstIteratorRange  operator--( int )
    {
        ConstIteratorRange itr( *this );
        m_idx--;
        return itr;
    }

    ConstIteratorRange& operator+=( int i )
    {
        m_idx += i;
        return *this;
    }

    ConstIteratorRange& operator-=( int i )
    {
        m_idx -= i;
        return *this;
    }

    ConstIteratorRange  operator+( int i ) const
    {
        ConstIteratorRange itr( *this );
        itr.m_idx += i;
        return itr;
    }

    ConstIteratorRange  operator-( int i ) const
    {
        ConstIteratorRange itr( *this );
        itr.m_idx -= i;
        return itr;
    }

    difference_type operator-( const ConstIteratorRange& itr ) const
    {
        return m_idx - itr.m_idx;
    }

    T operator[] ( int i )
    {
        return m_initial + i * m_increment;
    }

    T operator*() const
    {
        return m_initial + m_idx * m_increment;
    }

    bool operator==( const ConstIteratorRange& itr ) const
    {
        return itr.m_initial == m_initial
               && itr.m_increment   == m_increment
               && itr.m_idx     == m_idx ;
    }

    bool operator!=( const ConstIteratorRange& itr ) const
    {
        return !operator==( itr );
    }

    bool operator<( const ConstIteratorRange& itr ) const
    {
        return m_idx < itr.m_idx;
    }

    bool operator>( const ConstIteratorRange& itr ) const
    {
        return m_idx > itr.m_idx;
    }

    bool operator<=( const ConstIteratorRange& itr ) const
    {
        return m_idx <= itr.m_idx;
    }

    bool operator>=( const ConstIteratorRange& itr ) const
    {
        return m_idx >= itr.m_idx;
    }

    // --------------------------------------------------------------------
    // access functions
    T GetInitial() const
    {
        return m_initial;
    }
    T GetIncrement() const
    {
        return m_increment;
    }
    int GetIndex() const
    {
        return m_idx;
    }

private:
    // --------------------------------------------------------------------
    T m_initial;
    T m_increment;
    int m_idx;
};

//------------------------------------------------------------------------------
// Angle Specialization
template <>
class ConstIteratorRange< Angle >
    : public std::iterator< std::random_access_iterator_tag, Angle >
{
public:
    typedef
    std::iterator<std::random_access_iterator_tag,Angle>::difference_type
    difference_type;

    // --------------------------------------------------------------------
    // constructors
    // default behavior of copy constructor and assignment operator
    // is desirable in this case, we want to share pointers
    ConstIteratorRange()
        : m_initial( 0 ), m_increment( 0 ), m_idx( 0 ) { ; }

    ConstIteratorRange( Angle initial, Angle increment )
        : m_initial( initial ), m_increment( increment ), m_idx( 0 ) { ; }

    // --------------------------------------------------------------------
    // standard random access iterator operators
    ConstIteratorRange& operator++()
    {
        m_idx++;
        return *this;
    }

    ConstIteratorRange& operator--()
    {
        m_idx--;
        return *this;
    }

    ConstIteratorRange  operator++( int )
    {
        ConstIteratorRange itr( *this );
        m_idx++;
        return itr;
    }

    ConstIteratorRange  operator--( int )
    {
        ConstIteratorRange itr( *this );
        m_idx--;
        return itr;
    }

    ConstIteratorRange& operator+=( int i )
    {
        m_idx += i;
        return *this;
    }

    ConstIteratorRange& operator-=( int i )
    {
        m_idx -= i;
        return *this;
    }

    ConstIteratorRange  operator+( int i ) const
    {
        ConstIteratorRange itr( *this );
        itr.m_idx += i;
        return itr;
    }

    ConstIteratorRange  operator-( int i ) const
    {
        ConstIteratorRange itr( *this );
        itr.m_idx -= i;
        return itr;
    }

    difference_type operator-( const ConstIteratorRange& itr ) const
    {
        return m_idx - itr.m_idx;
    }

    Angle operator[] ( int i ) const
    {
        return normalizePlusMinusPi( m_initial + i * m_increment );
    }

    Angle operator*() const
    {
        return normalizePlusMinusPi( m_initial + m_idx * m_increment );
    }

    bool operator==( const ConstIteratorRange& itr ) const
    {
        return itr.m_initial == m_initial
               && itr.m_increment   == m_increment
               && itr.m_idx     == m_idx ;
    }

    bool operator!=( const ConstIteratorRange& itr ) const
    {
        return !operator==( itr );
    }

    bool operator<( const ConstIteratorRange& itr ) const
    {
        return m_idx < itr.m_idx;
    }

    bool operator>( const ConstIteratorRange& itr ) const
    {
        return m_idx > itr.m_idx;
    }

    bool operator<=( const ConstIteratorRange& itr ) const
    {
        return m_idx <= itr.m_idx;
    }

    bool operator>=( const ConstIteratorRange& itr ) const
    {
        return m_idx >= itr.m_idx;
    }

    // --------------------------------------------------------------------
    // access functions
    Angle GetInitial() const
    {
        return m_initial;
    }
    Angle GetIncrement() const
    {
        return m_increment;
    }
    int GetIndex() const
    {
        return m_idx;
    }

private:
    Angle m_initial;
    Angle m_increment;
    int m_idx;
};

// ============================================================================
}
// A3 namespace end
// ============================================================================
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#	undef difference_type
#endif

#endif  // ITERATORRANGE_H
