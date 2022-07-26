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
 * Range.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Range.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : Range

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V0.1-001  2002-04-24  JAM  Alpha Release
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V3.3-012  05/13/2004  JJO  Use typename for .NET compiler

//  NOTES : Template base class



//****************************************************************************//

//==============================================================================
#ifndef RANGE_H
#define RANGE_H	"Range V3.3-012  2004-05-13"

//------------------------------------------------------------------------------
// required headers
#include "IteratorRange.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// foward declaration
template< class T >
class Range;

// i/o prototype
template< class T >
std::ostream&
operator<<( std::ostream& os, const Range< T > &rng );

template< class T >
std::istream&
operator>>( std::istream& os, Range< T > &rng );

// ============================================================================
template< class T >
class Range
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    typedef ConstIteratorRange< T > const_iterator;

    //----------------------------------------------------------------------
    // constructors & destructors
    Range();

    Range( T initial, T final, T increment );

    virtual ~Range();

    //----------------------------------------------------------------------
    // access functions
    T GetInitial() const;
    T GetFinal() const;
    T GetIncrement() const;
    unsigned GetSize() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const Range& rng ) const;
    bool operator!=( const Range& rng ) const;

    //----------------------------------------------------------------------
    // stl like iterators
    inline const_iterator begin() const;
    inline const_iterator end() const;

    // nearest: returns iterator to the sample that is closest to value
    // if out of range, returns end()
    inline const_iterator nearest( T value ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    //----------------------------------------------------------------------
    static void ThrowBadRange();

    //--------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------
    const_iterator m_begin;
    unsigned m_nSamples;
};

//==============================================================================
// iterators
//------------------------------------------------------------------------------
template < class T >
inline typename Range< T >::const_iterator Range< T >::begin() const
{
    return m_begin;
}

template < class T >
inline typename Range< T >::const_iterator Range< T >::end() const
{
    // can't use m_final since it may not be a sample
    return m_begin + m_nSamples;
}

// if out of range, returns end()
template < class T >
inline typename Range< T >::const_iterator Range< T >::nearest( T value ) const
{
    T increment = m_begin.GetIncrement();
    const_iterator itr = m_begin
                         + static_cast< int >( ( value - *m_begin ) / increment );

    //CPPCHECK_FIX Performance - prefix iterator change
    if ( *itr <= value - increment / 2 ) {
        ++itr;
    }

    if ( itr < m_begin || itr >= end() ) {
        itr = end();
    }

    return itr;
}


//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------
template < class T >
inline T Range< T >::GetInitial() const
{
    return *m_begin;
}

template < class T >
inline T Range< T >::GetFinal() const
{
    return *(end() - 1);
}

template < class T >
inline T Range< T >::GetIncrement() const
{
    return m_begin.GetIncrement();
}

template < class T >
inline unsigned Range< T >::GetSize() const
{
    return m_nSamples;
}

//------------------------------------------------------------------------------
// misc functions
template < class T >
inline const char* Range< T >::GetVersion() const
{
    return RANGE_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // RANGE_H
