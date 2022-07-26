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
 * Range.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : Range.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : Range

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V0.1-001  2002-04-24  JAM  Alpha Release
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V3.3-012  05/13/2004  JJO  Use typename for .NET compiler



//****************************************************************************//
#include "Range.h"
#include "IoSeparator.h"
#include <math.h>
#include <iomanip>
#include <string>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
template < class T >
Range< T >::Range() { ; }

//------------------------------------------------------------------------------
template < class T >
Range< T >::Range(  T initial, T final, T increment  )
    : m_begin( initial, increment ),
      m_nSamples( static_cast< unsigned >( ( final - initial ) / increment ) + 1 )
{
    if ( (final < initial && 0 < increment) || 0 == increment ) {
        ThrowBadRange();
    }
}

//------------------------------------------------------------------------------
template < class T >
Range< T >::~Range()
{ ; }

//==============================================================================
// functions

//==============================================================================
// operators
//------------------------------------------------------------------------------
template < class T >
bool Range< T >::operator==( const Range< T >& mtx ) const
{
    return mtx.m_begin    == m_begin
           && mtx.m_nSamples == m_nSamples;
}

//------------------------------------------------------------------------------
template < class T >
bool Range< T >::operator!=( const Range& mtx ) const
{
    return !operator==( mtx );
}

//------------------------------------------------------------------------------
template < class T >
void Range< T >::ThrowBadRange()
{
    throw std::runtime_error( "Range< T >: Bad Range!" );
}

// ============================================================================
// insertion operator
template < class T >
std::ostream&
operator<<( std::ostream& os, const Range< T > &rng )
{
    //WARNING_FIX:
    //1>src\Range.cpp(87): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'unsigned int', possible loss of data
    long long int w = os.width();
    os << std::setw( w ) << rng.GetInitial() << ioSeparator
       << std::setw( w ) << rng.GetFinal()   << ioSeparator
       << std::setw( w ) << rng.GetIncrement();
    return os;
}

template < class T >
std::istream&
operator>>( std::istream& is, Range< T > &rng )
{
    T initial, final, increment;
    is >> initial >> final >> increment;
    rng = Range< T >( initial, final, increment );
    return is;
}

// ============================================================================
// explicit instantiation
template class Range< double >;
template std::ostream& operator<<( std::ostream&, const Range< double >& );
template std::istream& operator>>( std::istream&, Range< double >& );

template class Range< Angle >;
template std::ostream& operator<<( std::ostream&, const Range< Angle >& );
template std::istream& operator>>( std::istream&, Range< Angle >& );

// ============================================================================
}
// A3 namespace end
// ============================================================================
