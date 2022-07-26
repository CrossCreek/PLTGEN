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
 * MatrixFP.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Floating point 2-D matrix template class
// Refer to MatrixFP.h for more info.
// =============================================================================
#include "MatrixFP.h"
#include "IoDelim.h"
#include "Angle.h"
#include <math.h>
#include <float.h>
#include <iomanip>
#include <string>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
const MatrixOption::Tag MatrixOption::NONE(0x00);
const MatrixOption::Tag MatrixOption::INITZERO(0x01);
const MatrixOption::Tag MatrixOption::IDENTITY(0x02);
const MatrixOption::Tag MatrixOption::RESERVE_SCRATCH(0x04);
const MatrixOption::Tag MatrixOption::ALL_BITS(NONE|INITZERO|IDENTITY|RESERVE_SCRATCH);

MatrixOption::MatrixOption()
{
    /*insert(value_type(NONE, "NONE"))
    ->second.insert("No options selected");
    insert(value_type(INITZERO, "INITZERO"))
    ->second.insert("Initialize Elements to Zero");
    insert(value_type(IDENTITY, "IDENTITY"))
    ->second.insert("Identity Matrix");
    insert(value_type(RESERVE_SCRATCH, "RESERVE_SCRATCH"))
    ->second.insert("Scratch space reserved");*/
}

namespace MatrixExcept
{
void ThrowColRowMismatch(const std::string &who,
                         size_type szColA, size_type szRowB)
{
    std::ostringstream oss;
    oss << who << "\n1st matrix column count must match 2nd matrix "
        << "row count.\n1st matrix column count: " << szColA
        << "\n2nd matrix row lenght: " << szRowB << '\n';
    throw std::range_error(oss.str());
}

void ThrowMustBeSquare(const std::string &who,
                       size_type szRow, size_type szCol)
{
    std::ostringstream oss;
    oss << who << "\nMatrix must be square.\n"
        << "Row count must match column count.\nRow count: "
        << szRow << "\nColumn count: " << szCol << '\n';
    throw std::range_error(oss.str());
}

void ThrowMustBe3X3(const std::string &who,
                    size_type szRow, size_type szCol)
{
    std::ostringstream oss;
    oss << who << "\nMatrix, 3-space Vector Multiplication requires "
        << "a 3x3 matrix.\nRow count: "
        << szRow << "\nColumn count: " << szCol << '\n';
    throw std::range_error(oss.str());
}

void ThrowMustBeSameSize(const std::string &who,
                         size_type szRowA, size_type szColA,
                         size_type szRowB, size_type szColB)
{
    std::ostringstream oss;
    oss << who << "\nOperation requires same size matrices"
        << "\n1st matrix row count: " << szRowA
        << "\n1st matrix column count: " << szColA << '\n'
        << "\n2st matrix row count: " << szRowB
        << "\n2st matrix column count: " << szColB << '\n';
    throw std::range_error(oss.str());
}

//----------------------------------------------------------------------
void ThrowNoInverse(const std::string &who)
{
    throw std::runtime_error(who+"\nCannot determine inverse!");
}

//----------------------------------------------------------------------
void ThrowConflictingOptions()
{
    throw std::runtime_error("MatrixFP: Conflicting Options!");
}
}

//==============================================================================
// constructors & destructors
template <class FPT>
MatrixFP<FPT>::MatrixFP() {;}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT>::MatrixFP(const MatrixFP<FPT>& mtx)
    : MatrixBase<FPT>(mtx) {;}

template <class FPT>
MatrixFP<FPT>::MatrixFP(const Quaternion& q, Option optn)
    : MatrixBase<FPT>(3, 3)
{
    if ((MatrixOption::RESERVE_SCRATCH & optn).any()) {
        this->ReserveScratch();
    }

    if (((MatrixOption::ALL_BITS
            ^ MatrixOption::RESERVE_SCRATCH) & optn).any()) {
        MatrixExcept::ThrowConflictingOptions();
    }

    FPT qq0(static_cast<FPT>(q[0]*q[0]));
    FPT qq1(static_cast<FPT>(q[1]*q[1]));
    FPT qq2(static_cast<FPT>(q[2]*q[2]));
    FPT qq3(static_cast<FPT>(q[3]*q[3]));
    FPT q01(static_cast<FPT>(2.0*q[0]*q[1]));
    FPT q02(static_cast<FPT>(2.0*q[0]*q[2]));
    FPT q03(static_cast<FPT>(2.0*q[0]*q[3]));
    FPT q12(static_cast<FPT>(2.0*q[1]*q[2]));
    FPT q13(static_cast<FPT>(2.0*q[1]*q[3]));
    FPT q23(static_cast<FPT>(2.0*q[2]*q[3]));
    iterator i(this->begin());
    // V5.0 changed internal quaternion representation in MatrixFP, Quaternion
    // and Simulation for consistency with internal CoorTrans implementation;
    // there is no change to input or output
    i[0] =  qq0-qq1-qq2+qq3;
    i[1] =  q01+q23;
    i[2] =  q02-q13;
    i[3] =  q01-q23;
    i[4] = -qq0+qq1-qq2+qq3;
    i[5] =  q12+q03;
    i[6] =  q02+q13;
    i[7] =  q12-q03;
    i[8] = -qq0-qq1+qq2+qq3;
}

template <class FPT>
MatrixFP<FPT>::MatrixFP(const EulerElemRotation& eer, Option optn)
    : MatrixBase<FPT>(3, 3)
{
    typedef EulerElemRotation EER;

    if ((MatrixOption::RESERVE_SCRATCH & optn).any()) {
        this->ReserveScratch();
    }

    if (((MatrixOption::ALL_BITS
            ^ MatrixOption::RESERVE_SCRATCH) & optn).any()) {
        MatrixExcept::ThrowConflictingOptions();
    }

    IdentityThis();
    Angle a = eer.GetAngle();
    FPT c = static_cast<FPT>(cos(a));
    FPT s = static_cast<FPT>(sin(a));
    iterator itr = begin();

    switch (eer.GetAxis()) {
        case EER::A1:
            *(itr + 4) = c;
            *(itr + 5) = -s;
            *(itr + 7) = s;
            *(itr + 8) = c;
            break;

        case EER::A2:
            *(itr + 0) = c;
            *(itr + 2) = s;
            *(itr + 6) = -s;
            *(itr + 8) = c;
            break;

        case EER::A3:
            *(itr + 0) = c;
            *(itr + 1) = -s;
            *(itr + 3) = s;
            *(itr + 4) = c;
            break;
    }
}

// START: NOT TESTED
//template <class FPT>
//MatrixFP<FPT>::MatrixFP(const VectorCartesian& u, const Angle& a,
//                        Option optn = MatrixOption::NONE)
//: MatrixBase<FPT>(3, 3)
//{
//    static const std::string FUNSIG(
//        "MatrixFP(const VectorCartesian&, const Angle&, Option) ");
//    static const double NEAR_ZERO(10.0*DBL_EPSILON);
//
//    if (NEAR_ZERO < fabs(1.0-u.MagnitudeSquared()))
//        throw std::runtime_error(FUNSIG +
//            "rotation vector must be unit vector");
//
//    FPT sa(static_cast<FPT>(sin(a)));
//    FPT ca(static_cast<FPT>(cos(a)));
//    FPT omca(static_cast<FPT>(1.0 - ca));
//
//    FPT e0(static_cast<FPT>(u[0]));
//    FPT e1(static_cast<FPT>(u[1]));
//    FPT e2(static_cast<FPT>(u[2]));
//    FPT e00(e0*e0);
//    FPT e11(e1*e1);
//    FPT e22(e2*e2);
//    FPT e01(e0*e1);
//    FPT e02(e0*e2);
//    FPT e10(e1*e0);
//    FPT e12(e1*e2);
//    FPT e20(e2*e0);
//    FPT e21(e2*e1);
//
//    iterator i(this->begin());
//
//    i[0] = ca       + e00*omca;
//    i[1] = e01*omca + e2*sa;
//    i[2] = e02*omca - e1*sa;
//    i[3] = e01*omca - e2*sa;
//    i[4] = ca       + e11*omca;
//    i[5] = e12*omca + e0*sa;
//    i[6] = e02*omca + e1*sa;
//    i[7] = e12*omca - e0*sa;
//    i[8] = ca       + e22*omca;
//}
// END: NOT TESTED

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT>::MatrixFP(size_type nRows, size_type nCols, Option optn)
    : MatrixBase<FPT>(nRows, nCols)
{
    if ((MatrixOption::RESERVE_SCRATCH & optn).any()) {
        this->ReserveScratch();
    }

    if (((MatrixOption::INITZERO | MatrixOption::IDENTITY) & optn).any()) {
        std::fill(this->begin(), this->end(), static_cast<FPT>(0.0));
    }

    if ((MatrixOption::IDENTITY & optn).any()) {
        if (nRows != nCols) {
            throw std::runtime_error("IDENITY option requires square matrix");
        }

        // std::fill(this->begin(), this->end(), static_cast<FPT>(0.0));
        std::fill(this->begin_diagonal(), this->end_diagonal(),
                  static_cast<FPT>(1.0));
    }
}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT>::MatrixFP(size_type nRows, size_type nCols,
                        FPT value, Option optn)
    : MatrixBase<FPT>(nRows, nCols, value)
{
    if ((MatrixOption::RESERVE_SCRATCH & optn).any()) {
        this->ReserveScratch();
    }

    if (((MatrixOption::INITZERO | MatrixOption::IDENTITY) & optn).any()) {
        std::fill(this->begin(), this->end(), static_cast<FPT>(0.0));
    }

    if ((MatrixOption::IDENTITY & optn).any()) {
        if (nRows != nCols) {
            throw std::runtime_error("IDENITY option requires square matrix");
        }

        // std::fill(this->begin(), this->end(), static_cast<FPT>(0.0));
        std::fill(this->begin_diagonal(), this->end_diagonal(),
                  static_cast<FPT>(1.0));
    }
}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT>::~MatrixFP()
{ ; }

//==============================================================================
template <class FPT>
void MatrixFP<FPT>::IdentityThis()
{
    if (!this->IsSquare()) {
        std::string who(typeid(*this).name());
        who += "::IdentityThis()";
        MatrixExcept
        ::ThrowMustBeSquare(who, this->size_row(), this->size_column());
    }

    std::fill(this->begin(), this->end(), static_cast<FPT>(0.0));
    std::fill(this->begin_diagonal(), this->end_diagonal(),
              static_cast<FPT>(1.0));
}

//------------------------------------------------------------------------------
template <class FPT>
void MatrixFP<FPT>::InverseThis()
{
    MatrixFP<FPT> mtx(*this);
    mtx.GetInverse(*this);
}

//------------------------------------------------------------------------------
template <class FPT>
void MatrixFP<FPT>::GetInverse(MatrixFP<FPT>& invOut) const
{
    static const std::string
    who(std::string(typeid(*this).name())
        +"::GetInverse(MatrixFP<FPT>&) const");

    if (!this->IsSquare())
        MatrixExcept
        ::ThrowMustBeSquare(who, this->size_row(), this->size_column());

    FPT piv, el;
    int i, j, k;
    bool b_isScratchReserved(this->IsScratchReserved());
    // make sure that the scratch area is size adaquately
    this->ReserveScratch();
    // copy current matrix to the scratch area
    std::copy(this->begin(),
              this->end(),
              this->begin_scratch());
    int order(static_cast<int>(this->size_row()));
    // resize if output matrix if needed
    invOut.Resize(order, order);
    // set output matrix to identity
    invOut.IdentityThis();
    iterator itrInv = invOut.begin();
    iterator itrScr(this->begin_scratch());

    // convert the matrix to upper trianglar form
    for (i = 0; i < order; i++) {
        // determine if pivoting is necessary
        k = i;
        piv = static_cast<FPT>(fabs(itrScr[order*i+i]));

        for (j = i+1; j < order; j++) {
            if (static_cast<FPT>(fabs(itrScr[order*j+i])) > piv) {
                k = j;
                piv = static_cast<FPT>(fabs(itrScr[order*j+i]));
            }
        }

        // if each element in this column is 0 then return no solution
        if (piv == 0.0) {
            MatrixExcept::ThrowNoInverse(who);
        }

        // swap rows if pivoting is needed
        if (k != i) {
            for (j = i; j < order; j++) {
                piv = itrScr[ order * i + j ];
                itrScr[ order * i + j ] = itrScr[ order * k + j ];
                itrScr[ order * k + j ] = piv;
            }

            for (j = 0; j < order; j++) {
                piv = itrInv[ order * i + j ];
                itrInv[ order * i + j ] = itrInv[ order * k + j ];
                itrInv[ order * k + j ] = piv;
            }
        }

        piv = itrScr[ order * i + i ];

        // normalize the row
        for (j = i + 1; j < order; j++) {
            itrScr[ order * i + j ] /= piv;
        }

        for (j = 0; j < order; j++) {
            itrInv[ order * i + j ] /= piv;
        }

        // clear the column
        for (j = i + 1; j < order; j++) {
            el = itrScr[ order * j + i ];

            for (k = i + 1; k < order; k++) {
                itrScr[ order * j + k ]  -= itrScr[ order * i + k ] * el;
            }

            for (k = 0; k < order; k++) {
                itrInv[ order * j + k ] -= itrInv[ order * i + k ] * el;
            }
        }
    }

    // convert the mtx to reduce row-echelon form
    for (i = order-1; i > 0; i--) {
        for (j = i - 1; j >= 0; j--) {
            el = itrScr[ order * j + i ];

            for (k = 0; k < order; k++) {
                itrInv[ order * j + k ] -= itrInv[ order * i + k ] * el;
            }
        }
    }

    if (!b_isScratchReserved) {
        this->ReleaseScratch();
    }
}

//------------------------------------------------------------------------------
// Trace
template <class FPT>
FPT MatrixFP<FPT>::GetTrace() const
{
    if (!this->IsSquare()) {
        std::string who(typeid(*this).name());
        who += "::GetTrace() const";
        MatrixExcept
        ::ThrowMustBeSquare(who, this->size_row(), this->size_column());
    }

    return std::accumulate(this->begin_diagonal(), this->end_diagonal(),
                           static_cast<FPT>(0.0));
}

//==============================================================================
// operators
template <class FPT>
MatrixFP<FPT>& MatrixFP<FPT>::operator=(const MatrixFP<FPT>& mtx)
{
    if (&mtx != this) {
        MatrixBase<FPT>::operator=(mtx);
    }

    return *this;
}

//------------------------------------------------------------------------------
template <class FPT>
bool MatrixFP<FPT>::operator==(const MatrixFP<FPT>& mtx) const
{
    return MatrixBase<FPT>::operator==(mtx);;
}

//------------------------------------------------------------------------------
template <class FPT>
bool MatrixFP<FPT>::operator!=(const MatrixFP<FPT>& mtx) const
{
    return !operator==(mtx);
}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT> MatrixFP<FPT>::operator*(const MatrixFP<FPT>& a) const
{
    MatrixFP<FPT> m;
    Multiply(a, m);
    return m;
}

//------------------------------------------------------------------------------
template <class FPT>
VectorCartesian MatrixFP<FPT>::operator*(const VectorCartesian& vec) const
{
    VectorCartesian v;
    this->Multiply(vec, v);
    return v;
}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT> MatrixFP<FPT>::operator+(const MatrixFP<FPT>& mtx) const
{
    MatrixFP<FPT> m(*this);
    m += mtx;
    return m;
}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT>& MatrixFP<FPT>::operator+=(const MatrixFP<FPT>& mtx)
{
    if (this->size_column() != mtx.size_column()
            || this->size_row() != mtx.size_row()) {
        std::string who(typeid(*this).name());
        who += "::operator+=(const MatrixFP<FPT>&)";
        MatrixExcept
        ::ThrowMustBeSameSize(who, this->size_row(), this->size_column(),
                              mtx.size_row(), mtx.size_column());
    }

    iterator itr(this->begin());
    iterator etr(this->end());
    const_iterator it1(mtx.begin());

    while (itr != etr) {
        *(itr++) += *(it1++);
    }

    return *this;
}
//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT> MatrixFP<FPT>::operator-(const MatrixFP<FPT>& mtx) const
{
    MatrixFP<FPT> m(*this);
    m -= mtx;
    return m;
}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT>& MatrixFP<FPT>::operator-=(const MatrixFP<FPT>& mtx)
{
    if (this->size_column() != mtx.size_column()
            || this->size_row() != mtx.size_row()) {
        std::string who(typeid(*this).name());
        who += "::operator-=(const MatrixFP<FPT>&)";
        MatrixExcept
        ::ThrowMustBeSameSize(who, this->size_row(), this->size_column(),
                              mtx.size_row(), mtx.size_column());
    }

    iterator itr(this->begin());
    iterator etr(this->end());
    const_iterator it1(mtx.begin());

    while (itr != etr) {
        *(itr++) -= *(it1++);
    }

    return *this;
}

//------------------------------------------------------------------------------
template <class FPT>
MatrixFP<FPT> MatrixFP<FPT>::operator*(FPT s) const
{
    MatrixFP a(*this);
    return a *= s;
}

template <class FPT>
MatrixFP<FPT> MatrixFP<FPT>::operator/(FPT s) const
{
    MatrixFP a(*this);
    return a /= s;
}

template <class FPT>
MatrixFP<FPT>& MatrixFP<FPT>::operator*=(FPT s)
{
    iterator itr(this->begin());
    iterator etr(this->end());

    while (itr != etr) {
        *(itr++) *= s;
    }

    return *this;
}

template <class FPT>
MatrixFP<FPT>& MatrixFP<FPT>::operator/=(FPT s)
{
    iterator itr(this->begin());
    iterator etr(this->end());

    while (itr != etr) {
        *(itr++) /= s;
    }

    return *this;
}

// ============================================================================
template <class FPT>
void MatrixFP<FPT>::Add(const MatrixFP& b, MatrixFP& c) const
{
    c = *this;
    c += b;
}

template <class FPT>
void MatrixFP<FPT>::Subtract(const MatrixFP& b, MatrixFP& c) const
{
    c = *this;
    c -= b;
}

template <class FPT>
void MatrixFP<FPT>::Multiply(FPT s, MatrixFP& c) const
{
    c = *this;
    c *= s;
}

template <class FPT>
void MatrixFP<FPT>::Divide(FPT s, MatrixFP& c) const
{
    c = *this;
    c /= s;
}

template <class FPT>
void MatrixFP<FPT>::Multiply(const MatrixFP& b, MatrixFP& m) const
{
    // [a][b] X [b][c] = [a][c]
    if (this->size_column() != b.size_row()) {
        std::string who(typeid(*this).name());
        who += "::Multiply(const MatrixFP<FPT>&, Matrix<FPT>&)";
        MatrixExcept
        ::ThrowColRowMismatch(who, this->size_column(), b.size_row());
    }

    typename MatrixFP<FPT>::size_type r(this->size_row());
    typename MatrixFP<FPT>::size_type c(b.size_column());
    m.Resize(r, c);
    typename MatrixFP<FPT>::iterator itrM = m.end();

    while (r--) {
        c = b.size_column();

        while (c--) {
            *(--itrM) = std::inner_product(this->begin_row(r),
                                           this->end_row(r),
                                           b.begin_column(c),
                                           static_cast<FPT>(0.0));
        }
    }
}

template <class FPT>
void MatrixFP<FPT>::Multiply(const VectorCartesian& b,
                             VectorCartesian& v) const
{
    // [3][3] X [3][1] = [3][1]
    if (this->size_row() != 3 || this->size_column() != 3) {
        std::string who(typeid(*this).name());
        who += "::Multiply(const VectorCartesian&, VectorCartesian&)";
        MatrixExcept
        ::ThrowMustBe3X3(who, this->size_row(), this->size_column());
    }

    v[0] = std::inner_product(this->begin_row(0), this->end_row(0),
                              b.begin(), 0.0);
    v[1] = std::inner_product(this->begin_row(1), this->end_row(1),
                              b.begin(), 0.0);
    v[2] = std::inner_product(this->begin_row(2), this->end_row(2),
                              b.begin(), 0.0);
}

template <class FPT>
void MatrixFP<FPT>::TransposeMultiply(const MatrixFP& b,
                                      MatrixFP& m) const
{
    // [b][a]T X [b][c] = [a][c]
    if (this->size_row() != b.size_row()) {
        std::string who(typeid(*this).name());
        who += "::TransposeMultiply(const MatrixFP<FPT>&, Matrix<FPT>&)";
        MatrixExcept
        ::ThrowColRowMismatch(who, this->size_row(), b.size_row());
    }

    typename MatrixFP<FPT>::size_type r(this->size_column());
    typename MatrixFP<FPT>::size_type c(b.size_column());
    m.Resize(r, c);
    typename MatrixFP<FPT>::iterator itrM = m.end();

    while (r--) {
        c = b.size_column();

        while (c--) {
            *(--itrM) = std::inner_product(this->begin_column(r),
                                           this->end_column(r),
                                           b.begin_column(c),
                                           static_cast<FPT>(0.0));
        }
    }
}

template <class FPT>
void MatrixFP<FPT>::TransposeMultiply(const VectorCartesian& b,
                                      VectorCartesian& v) const
{
    // [3][3] X [3][1] = [3][1]
    if (this->size_row() != 3 || this->size_column() != 3) {
        std::string who(typeid(*this).name());
        who += "::TransposeMultiply(const VectorCartesian&, VectorCartesian&)";
        MatrixExcept
        ::ThrowMustBe3X3(who, this->size_row(), this->size_column());
    }

    v[0] = std::inner_product(this->begin_column(0), this->end_column(0),
                              b.begin(), 0.0);
    v[1] = std::inner_product(this->begin_column(1), this->end_column(1),
                              b.begin(), 0.0);
    v[2] = std::inner_product(this->begin_column(2), this->end_column(2),
                              b.begin(), 0.0);
}

template <class FPT>
void MatrixFP<FPT>::Resize(size_type nRows, size_type nCols, Option optn)
{
    MatrixBase<FPT>::Resize(nRows, nCols);

    if (((MatrixOption::INITZERO | MatrixOption::IDENTITY) & optn).any()) {
        std::fill(this->begin(), this->end(), static_cast<FPT>(0.0));
    }

    if ((MatrixOption::IDENTITY & optn).any()) {
        if (nRows != nCols) {
            throw std::runtime_error("IDENITY option requires square matrix");
        }

        // std::fill(this->begin(), this->end(), static_cast<FPT>(0.0));
        std::fill(this->begin_diagonal(), this->end_diagonal(),
                  static_cast<FPT>(1.0));
    }
}

// ============================================================================
// insertion operator
template <class FPT>
std::ostream&
operator<<(std::ostream& os, const MatrixFP<FPT>& mtx)
{
    static const std::string newline("\n");
    std::string space(ioDelimGet(os));
    //WARNING_FIX:
    //1>src\MatrixFP.cpp(735): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w = os.width();
    typename MatrixFP<FPT>::size_type nCol  = mtx.size_column();
    typename MatrixFP<FPT>::size_type nElem = mtx.size();
    typename MatrixFP<FPT>::const_iterator itr = mtx.begin();
    typename MatrixFP<FPT>::size_type i(0);

    for (; i != nElem; i++) {
        os << std::setw(w) << itr[i] << (((i + 1)%nCol) ? space : newline);
    }

    return os;
}

// V4.3-010 extraction operator
template <class FPT>
std::istream&
operator>>(std::istream& is, MatrixFP<FPT>& mtx)
{
    typename MatrixFP<FPT>::iterator itr = mtx.begin();
    typename MatrixFP<FPT>::iterator etr = mtx.end();

    while (itr != etr) {
        is >> *(itr++);
    }

    return is;
}

// ============================================================================
// explicit instantiation
template class MatrixFP<Angle>;
template std::ostream& operator<<(std::ostream&, const MatrixFP<Angle>&);
template std::istream& operator>>(std::istream&, MatrixFP<Angle>&);
template void Multiply(Angle, const MatrixFP<Angle>&, MatrixFP<Angle>&);
template MatrixFP<Angle> operator*(Angle, const MatrixFP<Angle>&);

template class MatrixFP<double>;
template std::ostream& operator<<(std::ostream&, const MatrixFP<double>&);
template std::istream& operator>>(std::istream&, MatrixFP<double>&);
template void Multiply(double, const MatrixFP<double>&, MatrixFP<double>&);
template MatrixFP<double> operator*(double, const MatrixFP<double>&);

template class MatrixFP<float>;
template std::ostream& operator<<(std::ostream&, const MatrixFP<float>&);
template std::istream& operator>>(std::istream&, MatrixFP<float>&);
template void Multiply(float, const MatrixFP<float>&, MatrixFP<float>&);
template MatrixFP<float> operator*(float, const MatrixFP<float>&);

// ============================================================================
}
// A3 namespace end
// ============================================================================
