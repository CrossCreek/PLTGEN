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
 * MatrixFP.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : MatrixFP.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : MatrixFP

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V0.1-001  2002-02-10  JAM  Alpha Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: changed 0.0 to static_cast<FPT>(0.0)
//              Reason: stops implicit cast generated warning
//              Change: deleted unused variable nElem
//              Reason: stops unused variable warning
//              Change: added scope resolution to  template base class member
//              function calls that don't depend on a template parameter
//              Reason: compiler needs explicit scope resolution to find base
//              class template function
//              Change: changed 0.0 to static_cast<FPT>(0.0)
//              Reason: stops implicit cast generated warning
//              Change: changed fabs(…) to static_cast<FPT>(fabs(…))
//              Reason: avoids implicit cast warnings with certain compiler
//              settings
//              Change: changed 0.0 to static_cast<FPT>(0.0)
//              Reason: stops implicit cast generated warning
//              Change: changed 0.0 to static_cast<FPT>(0.0)
//              Reason: stops implicit cast generated warning

//              V4.4-001  2008-08-29  JAM  Added following A3 global fctns
//                                         operator>>(std::istream,
//                                                    MatrixFP<FPT>)
//                                         Multiply(FPT, const MatrixFP<FPT>&
//                                         operator*(FPT, const MatrixFP<FPT>&)

//                                         Added following MatrixFP mbr fctns
//                                         InverseThis()
//                                         GetTrace() const
//                                         operator*(const MatrixFP&) const
//                                         operator*(const VectorCartesian&) const
//                                         operator*(FPT) const
//                                         operator/(FPT) const
//                                         operator*=(FPT)
//                                         operator/=(FPT)
//                                         operator+(const MatrixFP&) const
//                                         operator-(const MatrixFP&) const
//                                         operator=(const MatrixFP&)
//                                         operator+=(const MatrixFP&)
//                                         operator-=(const MatrixFP&)
//                                         Multiply(FPT) const
//                                         Divide(FPT) const
//                                         MultiplyEq(FPT)
//                                         DivideEq(FPT)

//              V4.5-001  2009-07-01  JAM  dropped support for MS CPP Ver 4/5
//                                         compilers
//                                         added standard typedef's
//                                         added MatrixOption class
//                                         to select initialization options
//                                         vice some integral type and
//                                         eliminated option to define a
//                                         square matrix using order
//                                         only to resolve potential ambiguity
//                                         matching constructor signiture
//                                         added Multiply and TransposeMultiply
//                                         functions

//              V5.0      2010-1-27  JAM   added specialize DCM class and
//                                         Euler Axis class i/f;
//                                         replaced old delimiter object with
//                                         the new & improved object in the
//                                         matrix overloaded insertion operator

//  NOTES : Floating Point 2-D Matrix Template Class



//****************************************************************************//
#ifndef MATRIXFP_H
#define MATRIXFP_H	__FILE__ " " __DATE__

//------------------------------------------------------------------------------
// required headers
#include "MatrixBase.h"
#include "VectorCartesian.h"
#include "Quaternion.h"
#include "EulerElemRotation.h"
#include "EnumMap.h"
#include "GenMath.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <functional>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
//  forward declarations
template <class FPT>
class MatrixFP;

template <class FPT>
std::ostream&
operator<<(std::ostream& os, const MatrixFP<FPT>& mtx);

template <class FPT>
std::istream&
operator>>(std::istream& is, MatrixFP<FPT>& mtx);

// ============================================================================
namespace MatrixExcept
{
void ThrowNoInverse(const std::string &who);
void ThrowColRowMismatch(const std::string &who,
                         size_type szColA, size_type szRowB);
void ThrowMustBeSquare(const std::string &who,
                       size_type szRow, size_type szCol);
void ThrowMustBe3X3(const std::string &who,
                    size_type szRow, size_type szCol);
void ThrowMustBeSameSize(const std::string &who,
                         size_type szRowA, size_type szColA,
                         size_type szRowB, size_type szColB);
void ThrowConflictingOptions();
};

// Matrix Option Enumerators and Labels
// The MatrixOption class is used to select initialization options
// vice some integral type to avoid ambiguity matching constructor
// signature
class MatrixOption : public EnumMap<std::bitset<8>, LessBitset>
{
public:
    typedef std::bitset<8> Tag;

    static const Tag NONE;
    static const Tag INITZERO;
    static const Tag IDENTITY;
    static const Tag RESERVE_SCRATCH;
    static const Tag ALL_BITS;

    MatrixOption();

private:
};

// ============================================================================
template <class FPT>
class MatrixFP : public MatrixBase<FPT>
{
public:
    typedef typename MatrixBase<FPT>::iterator iterator;
    typedef typename MatrixBase<FPT>::const_iterator const_iterator;
    typedef typename MatrixBase<FPT>::size_type size_type;
    typedef typename MatrixBase<FPT>::difference_type difference_type;
    typedef typename MatrixBase<FPT>::value_type value_type;
    typedef typename MatrixBase<FPT>::pointer pointer;
    typedef typename MatrixBase<FPT>::reference reference;
    typedef typename MatrixBase<FPT>::const_pointer const_pointer;
    typedef typename MatrixBase<FPT>::const_reference const_reference;
    typedef MatrixOption::Tag Option;

    //----------------------------------------------------------------------
    // constructors & destructors
    MatrixFP();
    MatrixFP(const MatrixFP& mtx);

    // quaternion conversion
    // applicable Options: RESERVE_SCRATCH
    MatrixFP(const Quaternion& q, Option optn = MatrixOption::NONE);

    // construct DCM from Euler Angle
    MatrixFP(const EulerElemRotation& eer,
             Option optn = MatrixOption::NONE);

    // START: NOT TESTED
    // Euler axis conversion
    // applicable Options: RESERVE_SCRATCH
    //MatrixFP(const VectorCartesian& u, const Angle& a,
    //         Option optn = MatrixOption::NONE);
    // END: NOT TESTED

    // row x column matrix
    // uninitialized elements
    // applicable Options: RESERVE_SCRATCH
    MatrixFP(size_type nRows, size_type nCols,
             Option optn = MatrixOption::NONE);

    // initialize all elements = element_value
    // applicable Options: RESERVE_SCRATCH
    MatrixFP(size_type nRows, size_type nCols, FPT element_value,
             Option optn = MatrixOption::NONE);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // MatrixBase initialize with iterators
    // in class definition required by VC++
    // if nRows == 0 && nCols != 0 then nRows is computed
    // if nRows != 0 && nCols == 0 then nCols is computed
    // if nRows == 0 && nCols == 0 then throw exception
    // applicable Options: RESERVE_SCRATCH
    template <class ITERATOR>
    MatrixFP(ITERATOR first, ITERATOR last,
             size_type nRows, size_type nCols,
             Option optn = MatrixOption::NONE)
        : MatrixBase<FPT>(first, last, nRows, nCols)
    {
        if ((MatrixOption::RESERVE_SCRATCH & optn).any()) {
            MatrixBase<FPT>::ReserveScratch();
        }

        if (((MatrixOption::ALL_BITS
                ^ MatrixOption::RESERVE_SCRATCH) & optn).any()) {
            MatrixExcept::ThrowConflictingOptions();
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    virtual ~MatrixFP();

    //----------------------------------------------------------------------
    // identity, matrix transpose, etc.
    void IdentityThis();

    void GetInverse(MatrixFP& mtxOut) const; // resizes mtxOut if needed
    void InverseThis(); // inverse this

    FPT GetTrace() const;

    //----------------------------------------------------------------------
    // operators
    MatrixFP operator*(const MatrixFP& mtx) const;
    VectorCartesian operator*(const VectorCartesian& vec) const;

    MatrixFP operator*(FPT scalar) const;
    MatrixFP operator/(FPT scalar) const;
    MatrixFP& operator*=(FPT scalar);
    MatrixFP& operator/=(FPT scalar);

    //----------------------------------------------------------------------
    MatrixFP operator+(const MatrixFP& mtx) const;
    MatrixFP operator-(const MatrixFP& mtx) const;

    MatrixFP& operator=(const MatrixFP& mtx);

    MatrixFP& operator+=(const MatrixFP& mtx);
    MatrixFP& operator-=(const MatrixFP& mtx);

    bool operator==(const MatrixFP& mtx) const;
    bool operator!=(const MatrixFP& mtx) const;

    //----------------------------------------------------------------------
    void Add(const MatrixFP& b, MatrixFP& c) const;
    void Subtract(const MatrixFP& b, MatrixFP& c) const;
    void Multiply(FPT s, MatrixFP& c) const;
    void Divide(FPT s, MatrixFP& c) const;

    void Multiply(const MatrixFP& b, MatrixFP& c) const;
    void Multiply(const VectorCartesian& b, VectorCartesian& c) const;
    void TransposeMultiply(const MatrixFP& b, MatrixFP& c) const;
    void TransposeMultiply(const VectorCartesian& b,
                           VectorCartesian& c) const;

    //----------------------------------------------------------------------
    // destructive matrix resize, elements left uninitialized
    // unless INITZERO or IDENTITY specificed
    virtual void Resize(size_type nRows, size_type nCols,
                        Option optn = MatrixOption::NONE);

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
};

//==============================================================================
template <class FPT>
inline void Multiply(FPT s, const MatrixFP<FPT>& a, MatrixFP<FPT>& b)
{
    b = a*s;
}

template <class FPT>
inline MatrixFP<FPT> operator*(FPT s, const MatrixFP<FPT>& a)
{
    return a*s;
}

//------------------------------------------------------------------------------
// misc functions
template <class FPT>
inline const char* MatrixFP<FPT>::GetVersion() const
{
    return MATRIXFP_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // MATRIXFP_H
