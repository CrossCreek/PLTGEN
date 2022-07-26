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
 * TestValue.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : TestValue.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : TestValue

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V1.3-016  01/26/2000  JAM  eliminated default string value
//                                         in constructor to be compatible
//                                         with MS V5.0 compiler
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V5.0      09/27/2010  JAM  replaced namespace global
//                                         testAsinAcosArg function obj
//                                         with function

//  NOTES : TestValue class family.  These classes perform two tasks:
//          1. Test a condition and return true if the test passes else
//             false for the test fails
//          2. Test a condition and return if the test passes else
//             throw a standard exception if the test fails.

//          Class hierarchy:
//              TestValue
//                  TestBounds< class T >
//                  TestLowBound< class T >
//                  TestHighBound< class T >
//                  TestStream
//                  TestErrno

//          Contents derive from SE and SST Ver 1.0.




//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CLASS

//  Name:       TestValue< T >
//  Purpose:    Template base class for the TestValue class family.
//              See note above.

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  MEMBER FUNCTIONS

//  Public Member:  TestValue()
//  Purpose:        default constructor
//  Return Type:    n/a
//  Return Value:   n/a

//  Public Member:  TestValue( const string& )
//  Purpose:        conversion constructor, instantiates this class with the
//                  message that will be use if the Assert function throws
//                  an exception
//  Return Type:    n/a
//  Return Value:   n/a
//  Argument Type:  const string&
//  Argument Value: exception message

//  Public Member:  bool Test( const T& ) const
//  Purpose:        virtual function use to test a value against a set of
//                  constraints
//  Return Type:    bool
//  Return Value:   true if constraints are satisifed, false if at least one
//                  constraint is violated
//  Argument Type:  const T&
//  Argument Value: value to be evaluated

//  Public Member:  void Assert( const T& ) const
//  Purpose:        function use to assert a value against a set of
//                  constraints, function throughs an exception if the
//                  assertion fails
//  Return Type:    void
//  Return Value:   n/a

//  Public Member:  void SetMessage( const string& )
//  Purpose:        sets the message that will be use if the Assert
//                  function throws an exception
//  Return Type:    void
//  Return Value:   n/a
//  Argument Type:  const string&
//  Argument Value: exception message

//  Public Member:  const string& GetMessage() const
//  Purpose:        returns the message that will be use if the Assert
//                  function throws an exception
//  Return Type:    void
//  Return Value:   n/a

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  OPERATORS

//  Public Member:  bool operator==( const TestValue& ) const
//  Purpose:        test for equality
//  Return Type:    bool
//  Return Value:   true if TestValue objects are equivalent, false otherwise
//  Argument Type:  const TestValue&
//  Argument Value: TestValue object to be compared to

//  Public Member:  bool operator!=( const TestValue& ) const
//  Purpose:        test for inequality
//  Return Type:    bool
//  Return Value:   false if TestValue objects are equivalent, true otherwise
//  Argument Type:  const TestValue&
//  Argument Value: TestValue object to be compared to


//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  FUNCTION OBJECTS

//  Name:           testAsinAcosArg( double )
//  Purpose:        If the argument is slighly greater than 1 then set it to 1,
//                  if it is slighly less than -1 then set it to -1, if it is
//                  between -1 and 1 then return it as is, else throw an
//                  out_of_range exception.
//                  The user may set the argument tolerance by setting the
//                  upper bound such that an argument that is between
//                  [-upper bound, upper bound] is an acceptable value
//                  and will not cause an exception.
//                  If the upper bound is explicitly set, it must be greater
//                  than or equal to 1.
//  Return Type:    double
//  Return Value:   value between [-1, 1]
//  Argument Type:  double
//  Argument Value: sin or cos of an angle
//  Source Class:   TestAsinAcosArg

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

#ifndef TESTVALUE_H
#define TESTVALUE_H  "TestValue V2.0"

// ---------------------------------------------------------------------------
// headers
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <typeinfo>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// TestValue: base class
template < class T >
class TestValue
{
public :
    inline TestValue();



    explicit inline TestValue( const std::string& strMessage );

    virtual ~TestValue() {;}

    // base class always returns false
    // derived classes should specialize this function
    // to return false only when value to out of range
    inline virtual bool Test( const T& value ) const;

    // base class always assert
    // derived classes should specialize this function
    // to assert only when value to out of range
    inline virtual void Assert( const T& value ) const;

    inline virtual void SetMessage( const std::string& strMessage );

    inline virtual const std::string& GetMessage() const;

    inline bool operator==( const TestValue& value ) const;
    inline bool operator!=( const TestValue& value ) const;

    inline virtual const char *GetVersion() const;

protected :
    inline virtual void AddMessage( const std::string& strMessage );

private:
    std::string m_strMsg;
    std::string m_strAddMsg;
};

// ---------------------------------------------------------------------------
// TestValue inline constructors
template < class T >
inline
TestValue<T>::TestValue() { ; }

template < class T >
inline
TestValue<T>::TestValue( const std::string& strMessage )
    : m_strMsg( strMessage ) { ; }

// ---------------------------------------------------------------------------
// TestValue inline functions
template < class T >
inline bool
TestValue<T>::Test( const T& ) const
{
    return false;
}

template < class T >
inline void
TestValue<T>::Assert( const T& value ) const
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown from\n" << typeid( *this ).name() << std::endl;

    if ( !m_strMsg.empty() ) {
        stmMsg << m_strMsg << std::endl;
    }

    if ( !m_strAddMsg.empty() ) {
        stmMsg << m_strAddMsg << std::endl;
    }

    stmMsg << "Test Value = " << value << std::endl << std::ends;
    std::out_of_range oor( stmMsg.str() );
    throw oor;
}

template < class T >
inline void
TestValue<T>::SetMessage( const std::string& strMessage )
{
    m_strMsg = strMessage;
    m_strAddMsg.erase();
}

template < class T >
inline void
TestValue<T>::AddMessage( const std::string& strMessage )
{
    m_strAddMsg = strMessage;
}

template < class T >
inline const std::string&
TestValue<T>::GetMessage() const
{
    return m_strMsg;
}

template < class T >
inline const char*
TestValue<T>::GetVersion() const
{
    return TESTVALUE_H;
}

template < class T >
inline bool
TestValue<T>::operator==( const TestValue& value ) const
{
    return m_strMsg == value.m_strMsg;
}

template < class T >
inline bool
TestValue<T>::operator!=( const TestValue& value ) const
{
    return !operator==( value );
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// TestBounds
// determines if a value is within two bounding values, inclusive
// accounts for wraparound (i.e. min > max)
template < class T >
class TestBounds : public TestValue<T>
{
public :
    inline TestBounds();



    explicit inline TestBounds( const std::string& strMessage );

    inline TestBounds( const T& minValue, const T& maxValue );
    inline TestBounds( const T& minValue, const T& maxValue,
                       const std::string& strMessage );

    inline virtual bool Test( const T& testValue ) const;
    inline virtual void Assert( const T& testValue ) const;

    inline const T& GetLowBound() const;
    inline const T& GetHighBound() const;
    inline void GetBounds( T& minValue, T& maxValue ) const;

    inline bool operator==( const TestBounds& testValue ) const;
    inline bool operator!=( const TestBounds& testValue ) const;

protected:

private :
    bool m_bMinMaxSet;
    T m_mnVal, m_mxVal;

};

// ---------------------------------------------------------------------------
// constructors
template < class T >
inline
TestBounds<T>::TestBounds()
    : m_bMinMaxSet( false )
{
    TestValue<T>::AddMessage( "Boundry values not set.  Cannot test value!" );
}

template < class T >
inline
TestBounds<T>::TestBounds( const std::string& strMessage )
    : TestValue<T>( strMessage ),
      m_bMinMaxSet( false )
{
    TestValue<T>::AddMessage( "Boundry values not set.  Cannot test value!" );
}

template < class T >
inline
TestBounds<T>::TestBounds( const T& minValue, const T& maxValue )
    : m_bMinMaxSet( true ),
      m_mnVal( minValue ),
      m_mxVal( maxValue )
{
    std::ostringstream stmMsg;
    stmMsg << "Lower Bound = " << m_mnVal << std::endl
           << "Upper Bound = " << m_mxVal << std::ends;
    TestValue<T>::AddMessage( stmMsg.str() );
}

template < class T >
inline
TestBounds<T>::TestBounds( const T& minValue, const T& maxValue,
                           const std::string& strMessage )
    : TestValue<T>( strMessage ),
      m_bMinMaxSet( true ),
      m_mnVal( minValue ),
      m_mxVal( maxValue )
{
    std::ostringstream stmMsg;
    stmMsg << "Lower Bound = " << m_mnVal << std::endl
           << "Upper Bound = " << m_mxVal << std::ends;
    TestValue<T>::AddMessage( stmMsg.str() );
}

// ---------------------------------------------------------------------------
// inline functions
template < class T >
inline bool
TestBounds<T>::Test( const T& testValue ) const
{
    if ( !m_bMinMaxSet ) {
        Assert( testValue );
    }

    bool isInBounds = false;

    if ( m_mnVal <= m_mxVal )
        isInBounds = (testValue >= m_mnVal && testValue <= m_mxVal)
                     ? true : false;
    else
        isInBounds = (testValue < m_mnVal && testValue > m_mxVal)
                     ? false : true;

    return isInBounds;
}

template < class T >
inline void
TestBounds<T>::Assert( const T& testValue ) const
{
    if ( !Test( testValue ) ) {
        TestValue<T>::Assert( testValue );
    }
}

template < class T >
inline const T&
TestBounds<T>::GetLowBound() const
{
    return m_mnVal;
}

template < class T >
inline const T&
TestBounds<T>::GetHighBound() const
{
    return m_mxVal;
}

template < class T >
inline void
TestBounds<T>::GetBounds( T& minValue, T& maxValue ) const
{
    minValue = m_mnVal;
    maxValue = m_mxVal;
}

template < class T >
inline bool
TestBounds<T>::operator==( const TestBounds<T>& testValue ) const
{
    return TestValue<T>::operator==( testValue )
           && m_mnVal      == testValue.m_mnVal
           && m_mxVal      == testValue.m_mxVal
           && m_bMinMaxSet == testValue.m_bMinMaxSet;
}

template < class T >
inline bool
TestBounds<T>::operator!=( const TestBounds<T>& testValue ) const
{
    return !operator==( testValue );
}

// ---------------------------------------------------------------------------
// i/o operators
template < class T >
std::ostream& operator<<( std::ostream& os, const TestBounds<T>& tb )
{
    int w = os.width();
    os << std::setw(w)
       << tb.GetLowBound()
       << ' ' << std::setw(w)
       << tb.GetHighBound();
    return os;
}

template < class T >
std::istream& operator>>( std::istream& is, TestBounds<T>& tb )
{
    std::string strMsg = tb.GetMessage();
    T lowBound, highBound;
    is >> lowBound >> highBound;
    tb = TestBounds<T>( lowBound, highBound, strMsg );
    return is;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// TestLowBound
// pass: value is greater than or equal lower bound
// fail: value is less than lower bound
template < class T >
class TestLowBound : public TestValue<T>
{
public :
    inline TestLowBound();



    explicit inline TestLowBound( const std::string& strMessage );



    explicit inline TestLowBound( const T& minValue );

    inline TestLowBound( const T& minValue,
                         const std::string& strMessage = std::string() );

    inline virtual bool Test( const T& testValue ) const;
    inline virtual void Assert( const T& testValue ) const;

    inline const T& GetLowBound() const;

    inline bool operator==( const TestLowBound& testValue ) const;
    inline bool operator!=( const TestLowBound& testValue ) const;

protected:

private :
    T m_mnVal;
    bool m_bMnSet;
};

// ---------------------------------------------------------------------------
// constructors
template < class T >
inline
TestLowBound<T>::TestLowBound()
    : m_bMnSet( false )
{
    TestValue<T>::AddMessage( "Boundry value not set.  Cannot test value!" );
}

template < class T >
inline
TestLowBound<T>::TestLowBound( const std::string& strMessage )
    : TestValue<T>( strMessage ),
      m_bMnSet( false )
{
    TestValue<T>::AddMessage( "Boundry value not set.  Cannot test value!" );
}

template < class T >
inline
TestLowBound<T>::TestLowBound( const T& minValue )
    : m_bMnSet( true ),
      m_mnVal( minValue )
{
    std::ostringstream stmMsg;
    stmMsg << "Low Bound = " << m_mnVal << std::ends;
    TestValue<T>::AddMessage( stmMsg.str() );
}

template < class T >
inline
TestLowBound<T>::TestLowBound( const T& minValue,
                               const std::string& strMessage )
    : TestValue<T>( strMessage ),
      m_bMnSet( true ),
      m_mnVal( minValue )
{
    std::ostringstream stmMsg;
    stmMsg << "Low Bound = " << m_mnVal << std::ends;
    TestValue<T>::AddMessage( stmMsg.str() );
}

// ---------------------------------------------------------------------------
// inline functions
template < class T >
inline bool
TestLowBound<T>::Test( const T& testValue ) const
{
    if ( !m_bMnSet ) {
        Assert( testValue );
    }

    return testValue >= m_mnVal ? true : false;
}

template < class T >
inline void
TestLowBound<T>::Assert( const T& testValue ) const
{
    if ( !Test( testValue ) ) {
        TestValue<T>::Assert( testValue );
    }
}

template < class T >
inline const T& TestLowBound<T>::GetLowBound() const
{
    return m_mnVal;
}

template < class T >
inline bool
TestLowBound<T>::operator==( const TestLowBound<T>& testValue ) const
{
    return TestValue<T>::operator==( testValue )
           && m_mnVal  == testValue.m_mnVal
           && m_bMnSet == testValue.m_bMnSet;
}

template < class T >
inline bool
TestLowBound<T>::operator!=( const TestLowBound<T>& testValue ) const
{
    return !operator==( testValue );
}

// ---------------------------------------------------------------------------
// i/o operators
template < class T >
std::ostream& operator<<( std::ostream& os, const TestLowBound<T>& tb )
{
    os << tb.GetLowBound();
    return os;
}

template < class T >
std::istream& operator>>( std::istream& is, TestLowBound<T>& tb )
{
    std::string strMsg = tb.GetMessage();
    T lowBound;
    is >> lowBound;
    tb = TestLowBound<T>( lowBound, strMsg );
    return is;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// TestHighBound
// pass: value is less than or equal lower bound
// fail: value is greater than lower bound
template < class T >
class TestHighBound : public TestValue<T>
{
public :
    inline TestHighBound();



    explicit inline TestHighBound( const std::string& strMessage );



    explicit inline TestHighBound( const T& maxValue );


    inline TestHighBound( const T& maxValue,
                          const std::string& strMessage = std::string() );

    inline virtual bool Test( const T& testValue ) const;
    inline virtual void Assert( const T& testValue ) const;

    inline const T& GetHighBound() const;

    inline bool operator==( const TestHighBound& testValue ) const;
    inline bool operator!=( const TestHighBound& testValue ) const;

protected:

private :
    T m_mxVal;
    bool m_bMxSet;
};

// ---------------------------------------------------------------------------
// constructors
template < class T >
inline
TestHighBound<T>::TestHighBound()
    : m_bMxSet( false )
{
    TestValue<T>::AddMessage( "Boundry value not set.  Cannot test value!" );
}

template < class T >
inline
TestHighBound<T>::TestHighBound( const std::string& strMessage )
    : TestValue<T>( strMessage ),
      m_bMxSet( false )
{
    TestValue<T>::AddMessage( "Boundry value not set.  Cannot test value!" );
}

template < class T >
inline
TestHighBound<T>::TestHighBound( const T& maxValue )
    : m_bMxSet( true ),
      m_mxVal( maxValue )
{
    std::ostringstream stmMsg;
    stmMsg << "High Bound = " << m_mxVal << std::ends;
    TestValue<T>::AddMessage( stmMsg.str() );
}

template < class T >
inline
TestHighBound<T>::TestHighBound( const T& maxValue,
                                 const std::string& strMessage )
    : TestValue<T>( strMessage ),
      m_bMxSet( true ),
      m_mxVal( maxValue )
{
    std::ostringstream stmMsg;
    stmMsg << "High Bound = " << m_mxVal << std::ends;
    TestValue<T>::AddMessage( stmMsg.str() );
}

// ---------------------------------------------------------------------------
// inline functions
template < class T >
inline bool
TestHighBound<T>::Test( const T& testValue ) const
{
    if ( !m_bMxSet ) {
        Assert( testValue );
    }

    return testValue >= m_mxVal ? true : false;
}

template < class T >
inline void
TestHighBound<T>::Assert( const T& testValue ) const
{
    if ( !Test( testValue ) ) {
        TestValue<T>::Assert( testValue );
    }
}

template < class T >
inline const T& TestHighBound<T>::GetHighBound() const
{
    return m_mxVal;
}

template < class T >
inline bool
TestHighBound<T>::operator==( const TestHighBound<T>& testValue ) const
{
    return TestValue<T>::operator==( testValue )
           && m_mxVal  == testValue.m_mxVal
           && m_bMxSet == testValue.m_bMxSet;
}

template < class T >
inline bool
TestHighBound<T>::operator!=( const TestHighBound<T>& testValue ) const
{
    return !operator==( testValue );
}

// ---------------------------------------------------------------------------
// i/o operators
template < class T >
std::ostream& operator<<( std::ostream& os, const TestHighBound<T>& tb )
{
    os << tb.GetHighBound();
    return os;
}

template < class T >
std::istream& operator>>( std::istream& is, TestHighBound<T>& tb )
{
    std::string strMsg = tb.GetMessage();
    T highBound;
    is >> highBound;
    tb = TestHighBound<T>( highBound, strMsg );
    return is;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// TestAsinAcosArg
// If the argument is slighly greater than 1 then set it to 1,
// if it is slighly less than -1 then set it to -1, if it is
// between -1 and 1 then return it as is, else throw an
// out_of_range exception.
// The user may set the argument tolerance by setting the
// upper bound such that an argument that is between
// [-upper bound, upper bound] is an acceptable value
// and will not cause an exception.
// If the upper bound is explicitly set, it must be greater
// than or equal to 1.
class TestAsinAcosArg : protected TestValue< double >
{
public:



    explicit inline TestAsinAcosArg();

    // set the upper and lower bound to +/-upperBound
    // upperBound must be >= 1



    explicit inline TestAsinAcosArg( double upperBound );


    inline TestAsinAcosArg( double upperBound,
                            const std::string& strMessage );
    virtual ~TestAsinAcosArg() {;}

    inline double operator()( double sinCosValue ) const;

    // returns the upper bound
    inline double GetTolerance();

    // set the upper and lower bound to +/-upperBound
    // upperBound must be >= 1
    void SetTolerance( double upperBound );

private:
    double m_sinCosTol;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TestAsinAcosArg function
inline
double testAsinAcosArg(double sinCosValue)
{
    static const TestAsinAcosArg test;
    return test(sinCosValue);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TestAsinAcosArg inline functions
inline
TestAsinAcosArg::TestAsinAcosArg()
    : m_sinCosTol( 1.00000001 )
{
    std::ostringstream stmMsg;
    stmMsg << "Argument sinCosValue is out of range."
           << "Range = +/-" << m_sinCosTol << std::ends;
    TestValue<double>::AddMessage( stmMsg.str() );
}

inline
TestAsinAcosArg::TestAsinAcosArg( double upperBound )
    : m_sinCosTol( upperBound )
{
    std::ostringstream stmMsg;
    stmMsg << "Argument sinCosValue is out of range."
           << "Range = +/-" << m_sinCosTol << std::ends;
    TestValue<double>::AddMessage( stmMsg.str() );
}

inline
TestAsinAcosArg::TestAsinAcosArg( double upperBound,
                                  const std::string& strMessage )
    : TestValue< double >( strMessage ),
      m_sinCosTol( upperBound )
{
    std::ostringstream stmMsg;
    stmMsg << "Argument sinCosValue is out of range."
           << "Range = +/-" << m_sinCosTol << std::ends;
    TestValue<double>::AddMessage( stmMsg.str() );
}

inline double
TestAsinAcosArg::operator()( double sinCosValue ) const
{
    if ( sinCosValue > m_sinCosTol || sinCosValue < -m_sinCosTol ) {
        Assert( sinCosValue );
    }

    return sinCosValue > 1.0 ? 1.0 : sinCosValue < -1.0 ? -1.0 : sinCosValue;
}

inline double TestAsinAcosArg::GetTolerance()
{
    return m_sinCosTol;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // TESTVALUE_H
