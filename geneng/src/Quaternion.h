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
 * Quaternion.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief Quaternion class
///
/// @details Provides a quaternion representation with related functions
/// to support coordinate transformation, the current implementation supports
/// quaternions of norm 1, i.e. quaternions that represent a rotation.
///
/// @author J. McCormick
/// @version V4.5 initial release
/// @date 2009-07-01
///
/// @author J. McCormick
/// @version V5.0 added euler axis & angle constructor & function, negated
/// direction of rotation from DCM transformamtion for consistency with
/// CoorTrans class family and negated again in Simulation such that the
/// quaternion output doesn't change (code was changed for improved internal
/// consistency only)
/// @date 2009-12-18
///
/// @par Units
/// - Function Arg & Return: N/A
/// - Stream I/O:            N/A
/// - Initialization Files:  N/A
// =============================================================================
#ifndef QUATERNION_H
/// Include guard macro
#define QUATERNION_H    __FILE__ " " __DATE__

#include "MatrixBase.h"
#include "VectorCartesian.h"
#include <iosfwd>

/// Astrodynamics, Allocation and Availability
namespace A3
{
template <typename FPT> class MatrixFP;

/// Quaternion representation of rotation
///
/// 1st three elements represent the vector component, the last element
/// represents the scalar
class Quaternion
{
public:
    typedef double* iterator;
    typedef const double* const_iterator;
    typedef int size_type;
    typedef int difference_type;
    typedef double  value_type;
    typedef double* pointer;
    typedef double& reference;
    typedef const double* const_pointer;
    typedef const double& const_reference;

    Quaternion();

    /// Copy constructor
    /// @param [in] q quaternion to be copied
    Quaternion(const Quaternion& q);

    /// Alternate constructor for DCM conversion
    /// @param [in] m valid Direction Cosine Matrix



    explicit Quaternion(const MatrixFP<double>& m);

    /// Alternate constructor for Euler Axis conversion
    /// @param [in] u rotation unit vector, an exception is thrown if
    /// the vector is NOT an unit vector
    /// @param [in] a rotation angle
    Quaternion(const VectorCartesian& u, const Angle& a);

    /// Alternate constructor providing an iterator i/f to another
    /// vector (sz=3, scalar = 0), quaternion (sz=4) or DCM (sz=9)
    /// if quaternion iterators are provided, the 1st three elements
    /// must represent the vector component, the last represents the scalar
    /// @param [in] b begin iterator
    /// @param [in] e end iterator
    template<typename ITR>
    Quaternion(ITR b, ITR e);

    /// Begin iterator
    /// @return iterator to the 0th element of vector component
    iterator begin()
    {
        return m_q;
    }

    /// End iterator
    /// @return iterator to 1 past the scalar component
    iterator end()
    {
        return m_q+4;
    }

    /// Constant begin iterator
    /// @return constant iterator to the 0th element of vector component
    const_iterator begin() const
    {
        return m_q;
    }

    /// Constant end iterator
    /// @return constant iterator to 1 past the scalar component
    const_iterator end() const
    {
        return m_q+4;
    }

    /// Subscript operator
    /// @return element reference at a specified position
    double& operator[](size_type i)
    {
        return m_q[i];
    }

    /// Subscript operator
    /// @return const element reference at a specified position
    double operator[](size_type i) const
    {
        return m_q[i];
    }

    /// Checks validity
    /// @return true if quaternion is valid quaternion of norm 1,
    /// else false
    bool IsValid() const;

    /// Return Euler Axis
    /// @param [out] u rotation axis
    /// @param [out] a rotation angle
    void GetEulerAxis(VectorCartesian& u, Angle& a);

    /// Conjugate (inverse if norm = 1) quaternion
    /// @param [out] invQ inverse quaternion
    void GetConjugate(Quaternion& invQ) const;

    /// Conjugate (inverse if norm = 1) this quaternion
    void ConjugateThis();

    /// Quaternion multiplication
    /// @param [in] q quaternion
    /// @return quaternion such that <em>return = *this * q</em>
    Quaternion operator*(const Quaternion& q) const;

    /// Quaternion multiplication
    /// @param [in] b quaternion
    /// @param [out] c quaternion, <em>c = *this * b</em>
    void Multiply(const Quaternion& b, Quaternion& c) const;

    /// Quaternion-Scalar multiplication
    /// @param [in] b scalar
    /// @param [out] c quaternion, <em>c = *this * b</em>
    void Multiply(double b, Quaternion& c) const;

    /// Quaternion addition
    /// @param [in] b quaternion
    /// @param [out] c quaternion, <em>c = *this + b</em>
    void Add(const Quaternion& b, Quaternion& c) const;

    /// Vector Transformation
    /// @details Quaternion must be norm 1, this requirement is not
    /// tested in this fuction; if the user is unsure that the
    /// quaternion is norm 1, call and check the return value of
    /// IsValid() before calling this function
    /// @param [in] vi original vector
    /// @param [out] vt transformed vector
    void Transform(const VectorCartesian& vi,
                   VectorCartesian& vt) const;

    /// Vector Transformation using Inverse Quaternion
    /// @details Quaternion must be norm 1, this requirement is not
    /// tested in this fuction; if the user is unsure that the
    /// quaternion is norm 1, call and check the return value of
    /// IsValid() before calling this function
    /// @param [in] vi original vector
    /// @param [out] vt transformed vector
    void TransformInverse(const VectorCartesian& vi,
                          VectorCartesian& vt) const;

    /// Dot Product
    /// @param [in] q quaternion to dot with
    /// @return dot product
    double DotProduct(const Quaternion& q) const;

protected:
    void FromDcm(const MatrixBase<double>& mtx);

private:
    double m_q[4];

    static const size_type s_szQuaternion;
    static const size_type s_szDcm;
    static const size_type s_szDcmDiag;
    static const size_type s_szVector;
};

template<typename ITR>
Quaternion::Quaternion(ITR b, ITR e)
{
    if (s_szQuaternion == std::distance(b, e)) {
        std::copy(b, e, begin());
    } else if (s_szVector == std::distance(b, e)) {
        std::copy(b, e, begin());
        m_q[3] = 0.0;
    } else if (s_szDcm == std::distance(b, e)) {
        MatrixBase<double> m(b, e, s_szDcmDiag, 0);
        FromDcm(m);
    } else {
        throw std::runtime_error("Quaternion(ITR,ITR) bad distance");
    }
}

std::ostream& operator<<(std::ostream& os, const Quaternion& q);
std::istream& operator>>(std::istream& is, Quaternion& q);
} // A3
#endif  // QUATERNION_H

