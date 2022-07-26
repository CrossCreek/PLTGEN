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
 * IteratorMatrix.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : IteratorMatrix.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : IteratorMatrix

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V0.1-001  2002-04-24  JAM  Alpha Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings
//              Change: changed distance_type to difference_type, added
//              preprocessor check to use distance_type with VC++ V6
//              Reason: distance_type was non-standard MS implementation, need
//              preprocessor check for VC++ compiler backward compatability
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros

//              V4.5-001 2009-07-01  JAM
//              replaced use of "int" in iterator definitions with
//              difference_type to eliminate 64-bit warnings

//  NOTES : Template base class



//****************************************************************************//

//==============================================================================
#ifndef ITERATORMATRIX_H
#define ITERATORMATRIX_H	"IteratorMatrix V4.3-009"

#if defined(_MSC_VER) && (_MSC_VER < 1300)
#   pragma warning( disable : 4786 ) // disables long template name warning
#	define difference_type distance_type
#endif

//------------------------------------------------------------------------------
// required headers
#include <iterator>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// foward declaration
template < class T > class ConstIteratorMatrix;

//==============================================================================
// iterators
//------------------------------------------------------------------------------
template < class T >
class IteratorMatrix
    : public std::iterator< std::random_access_iterator_tag, T >
{
public:
    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::difference_type
    difference_type;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::iterator_category
    iterator_category;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::value_type
    value_type;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::pointer
    pointer;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::reference
    reference;


    // default behavior of copy constructor and assignment operator
    // is desirable in this case, we want to share pointers
    IteratorMatrix()
        : m_pCur( NULL ), m_stride( 0 ) { ; }

    IteratorMatrix( T* ar, difference_type start, difference_type stride )
        : m_pCur( ar + start ), m_stride( stride ) { ; }

    IteratorMatrix& operator++()
    {
        m_pCur += m_stride;
        return *this;
    }

    IteratorMatrix& operator--()
    {
        m_pCur -= m_stride;
        return *this;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    IteratorMatrix  operator++( int )
    {
        IteratorMatrix itr( *this );
        m_pCur += m_stride;
        return itr;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    IteratorMatrix  operator--( int )
    {
        IteratorMatrix itr( *this );
        m_pCur -= m_stride;
        return itr;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    IteratorMatrix& operator+=( int i )
    {
        m_pCur += i * m_stride;
        return *this;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    IteratorMatrix& operator-=( int i )
    {
        m_pCur -= i * m_stride;
        return *this;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    IteratorMatrix  operator+( int i )
    {
        IteratorMatrix itr( *this );
        itr.m_pCur += i * m_stride;
        return itr;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    IteratorMatrix  operator-( int i )
    {
        IteratorMatrix itr( *this );
        itr.m_pCur -= i * m_stride;
        return itr;
    }

    // reconsider this implementation
    difference_type  operator-( const IteratorMatrix& itr ) const
    {
        return (m_pCur - itr.m_pCur ) / m_stride;
    }

    T& operator[]( difference_type i )
    {
        return *( m_pCur + i * m_stride );
    }

    T& operator*()
    {
        return *m_pCur;
    }

    T* operator->()
    {
        return m_pCur;
    }

    const T* operator->() const
    {
        return m_pCur;
    }

    bool operator==( const IteratorMatrix& itr ) const
    {
        return itr.m_pCur == m_pCur;
    }

    bool operator!=( const IteratorMatrix& itr ) const
    {
        return itr.m_pCur != m_pCur;
    }

    bool operator<( const IteratorMatrix& itr ) const
    {
        return m_pCur < itr.m_pCur;
    }

    bool operator>( const IteratorMatrix& itr ) const
    {
        return m_pCur > itr.m_pCur;
    }

    bool operator<=( const IteratorMatrix& itr ) const
    {
        return m_pCur <= itr.m_pCur;
    }

    bool operator>=( const IteratorMatrix& itr ) const
    {
        return m_pCur >= itr.m_pCur;
    }

    friend class ConstIteratorMatrix< T >;

private:
    T* m_pCur;
    difference_type m_stride;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template < class T >
class ConstIteratorMatrix
    : public std::iterator< std::random_access_iterator_tag, T >
{
public:
    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::difference_type
    difference_type;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::iterator_category
    iterator_category;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::value_type
    value_type;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::pointer
    pointer;

    typedef typename
    std::iterator<std::random_access_iterator_tag,T>::reference
    reference;

    // default behavior of copy constructor and assignment operator
    // is desirable in this case, we want to share pointers
    ConstIteratorMatrix()
        : m_pCur( NULL ), m_stride( 0 ) { ; }

    ConstIteratorMatrix( const T* ar, difference_type start, difference_type stride )
        : m_pCur( ar + start ), m_stride( stride ) { ; }

    ConstIteratorMatrix( const ConstIteratorMatrix& i )
        : m_pCur( i.m_pCur ), m_stride( i.m_stride ) { ; }


    ConstIteratorMatrix( const IteratorMatrix< T >& i ) : m_pCur( i.m_pCur ), m_stride( i.m_stride ) { ; }
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit ConstIteratorMatrix( const IteratorMatrix< T >& i ) : m_pCur( i.m_pCur ), m_stride( i.m_stride ) { ; }

    ConstIteratorMatrix& operator++()
    {
        m_pCur += m_stride;
        return *this;
    }

    ConstIteratorMatrix& operator--()
    {
        m_pCur -= m_stride;
        return *this;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    ConstIteratorMatrix  operator++( int )
    {
        ConstIteratorMatrix itr( *this );
        m_pCur += m_stride;
        return itr;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    ConstIteratorMatrix  operator--( int )
    {
        ConstIteratorMatrix itr( *this );
        m_pCur -= m_stride;
        return itr;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    ConstIteratorMatrix& operator+=( int i )
    {
        m_pCur += i * m_stride;
        return *this;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    ConstIteratorMatrix& operator-=( int i )
    {
        m_pCur -= i * m_stride;
        return *this;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    ConstIteratorMatrix  operator+( int i ) const
    {
        ConstIteratorMatrix itr( *this );
        itr.m_pCur += i * m_stride;
        return itr;
    }
    //RAGUSA 4/30/2018 For 64bit processing, removed reference to difference_type in
    //favor direct reference of "int" to fix compiler errors
    ConstIteratorMatrix  operator-( int i ) const
    {
        ConstIteratorMatrix itr( *this );
        itr.m_pCur -= i * m_stride;
        return itr;
    }

    // reconsider this implementation
    difference_type operator-( const ConstIteratorMatrix& itr ) const
    {
        return (m_pCur - itr.m_pCur ) / m_stride;
    }

    const T& operator[] ( difference_type i ) const
    {
        return *( m_pCur + i * m_stride );
    }

    const T& operator*() const
    {
        return *m_pCur;
    }

    const T* operator->() const
    {
        return m_pCur;
    }

    bool operator==( const ConstIteratorMatrix& itr ) const
    {
        return itr.m_pCur == m_pCur;
    }

    bool operator!=( const ConstIteratorMatrix& itr ) const
    {
        return itr.m_pCur != m_pCur;
    }

    bool operator<( const ConstIteratorMatrix& itr ) const
    {
        return m_pCur < itr.m_pCur;
    }

private:
    const T* m_pCur;
    difference_type m_stride;
};

// ============================================================================
}
// A3 namespace end
// ============================================================================
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#	undef difference_type
#endif

#endif  // ITERATORMATRIX_H
