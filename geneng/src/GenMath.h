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
 * GenMath.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief General purpose math and bit functions
///
/// @details Provides factorial, dB, secant solver, and bit setting/testing
/// functions
///
/// @author J. McCormick
/// @version V5.0 Comment update only
/// @date 2010-02-26
///
/// @author J. McCormick
/// @version V4.5 Added New secant root finder template
/// and areAnyBitsOn function
/// @date 2009-07-01
///
/// @author J. McCormick
/// @version V2.0 moved from global to A3 namespace, moved remaining includes
/// from header to source, stopped using cmath since Microsoft doesn't include
/// it in the std
/// @date 2002-04-29
///
/// @author J. McCormick
/// @version V1.3 removed unneeded includes
/// @date 1999-12-29
///
/// @author J. McCormick
/// @version V1.2-003 migration to the new general engineering
/// contents were derived from fcntlib and old SE libraries
/// @date 1999-08-31
///
/// @par Units
/// - Function Arg & Return: N/A
/// - Stream I/O:            N/A
/// - Initialization Files:  N/A
// =============================================================================
#ifndef GENMATH_H
#define GENMATH_H   "GenMath V4.5"

#include <stdexcept>
#include <bitset>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// @brief Convert a ratio to dB
/// @param [in] a ratio
/// @return dB
double toDB(double a);

/// @brief Convert a number from dB to a ration
/// @param [in] a dB
/// @return ratio
double fromDB(double a);

/// @brief Compute factorial
/// @param integral number, [0, 13)
/// @return factorial
unsigned factorial( unsigned number );

// -------------------------------------------------------------------------
/// @brief Less than functor for ordering bitsets in a sequence container
struct LessBitset {
    /// @brief Define ordering of bitsets in a sequence container
    /// @param [in] a left bitset to be compared
    /// @param [in] b right bitset to be compared
    /// @return true if a < b, else false
    template <size_t I>
    bool operator()(const std::bitset<I>& a, const std::bitset<I>& b) const
    {
        return a.to_ulong()<b.to_ulong();
    }
};

/// @brief Set bits, FLAGTYPE must be an integral type
/// @param [in] bitsOn of interest
/// @param [in,out] flags integral object to be set
template < class BITTYPE, class FLAGTYPE >
inline void setBitsOn( BITTYPE bitsOn, FLAGTYPE& flags )
{
    flags |= bitsOn ;
}

/// @brief Reset bits, FLAGTYPE must be an integral type
/// @param [in] bitsOff of interest
/// @param [in,out] flags integral object to be reset
template < class BITTYPE, class FLAGTYPE >
inline void setBitsOff( BITTYPE bitsOff, FLAGTYPE& flags )
{
    flags = (flags | bitsOff) ^ bitsOff;
}

/// @brief Check bit state
/// @param [in] bits of interest
/// @param [in] flags integral object to check
/// @return true if ALL bits are on, false otherwise
template < class BITTYPE, class FLAGTYPE >
inline bool areBitsOn( BITTYPE bits, FLAGTYPE flags )
{
    return (flags & bits) == bits;
}

/// @brief Check bit state
/// @param [in] bits of interest
/// @param [in] flags integral object to check
/// @return true if any bits are on, false otherwise
template < class BITTYPE, class FLAGTYPE >
inline bool areAnyBitsOn( BITTYPE bits, FLAGTYPE flags )
{
    return 0 != (flags & bits);
}

// -------------------------------------------------------------------------
/// @brief Secant solver root finder
///
/// Solves for <i>x</i> such that <i>y = f(x)</i>
/// Ref: <i>Numerical Analysis, 4th ed., 1988, Burden & Faires</i>
/// @param [in] f unary function to be evaluated,
/// UFResult = operator()(UFArg)
/// @param [in] y value to find x for
/// @param [in] tol tolerance
/// @param [in, out] x initial estimate, returns a solution within tolerance
/// else an exception is thrown
/// @param [in, out] itrn on input, maximum iterations allowed,
/// an exception thrown if the function doesn't converge before this limit,
/// on output, the number of iterations that was required for convergence
template <class UnaryFctn, class UFArg, class UFResult>
inline void
solveSecant(UnaryFctn& f, const UFResult& y, const UFArg& tol,
            UFArg& x, unsigned itrn)
{
    const UFArg fctr(2.0);
    const UFArg tol2(tol*tol);
    const unsigned imax(itrn);
    UFArg x0(x);
    UFArg x1(x+fctr*tol);
    UFResult q0(f(x0)-y);
    UFResult q1(f(x1)-y);
    x = x1-q1*(x1-x0)/(q1-q0);
    UFArg dx(x-x1);
    UFArg d2(dx*dx);

    for (itrn=0; itrn<imax && d2>tol2; ++itrn) {
        x0 = x1;
        q0 = q1;
        x1 = x;
        q1 = f(x1)-y;
        x = q1!=q0 ? x1-q1*(x1-x0)/(q1-q0) : x1;
        dx = x-x1;
        d2 = dx*dx;
    }

    if (d2>tol2) {
        throw std::range_error("solveSecant failed to converge");
    }
}

// -------------------------------------------------------------------------
/// @brief Numerical differentiation, 3-point method
///
/// Solves <i>y' = f'(x)</i> given <i>y = f(x)</i>
/// Yes, it only evaluates 2-points but it's still considered a 3-point
/// method.
/// Ref: <i>Numerical Analysis, 4th ed., 1988, Burden & Faires</i>
/// @param [in] f unary function to evaluate
/// UFResult = operator()(UFArg)
/// @param [in] x function argument
/// @param [in] dx delta about x to be evaluated
/// @param [out] dydx approx. value of derivative, f'(x)
template <class UnaryFctn, class UFArg, class UFResult>
inline void
numDif3P(UnaryFctn& f, const UFArg& x, const UFArg& dx, UFResult& dydx)
{
    static const UFArg a0(2.0);
    dydx = (f(x+dx) - f(x-dx)) / static_cast<UFResult>(a0*dx);
}

/// @brief Numerical differentiation, 3-point method
///
/// Solves for <i>y' = f'(x)</i> given <i>y = f(x)</i>
/// Uses forward-difference or backward-difference method depending on
/// the sign of <i>dx</i>.
/// Ref: <i>Numerical Analysis, 4th ed., 1988, Burden & Faires</i>
/// @param [in] f unary function to evaluate
/// UFResult = operator()(UFArg)
/// @param [in] x function argument
/// @param [in] dx delta about x to be evaluated, positive (+) for
/// forward-difference method, negative (-) for backward-difference
/// @param [out] dydx approx. value of derivative, f'(x)
template <class UnaryFctn, class UFArg, class UFResult>
inline void
numDif3PFB(UnaryFctn& f, const UFArg& x, const UFArg& dx, UFResult& dydx)
{
    static const UFArg a0(2.0);
    static const UFResult r0(-3.0);
    static const UFResult r1(4.0);
    dydx = (r0*f(x) + r1*f(x+dx) - f(x+a0*dx))
           / static_cast<UFResult>(a0*dx);
}

/// @brief Numerical differentiation, 5-point method
///
/// Solves <i>y' = f'(x)</i> given <i>y = f(x)</i>
/// Yes, it only evaluates 4-points but it's still considered a 5-point
/// method.
/// Ref: <i>Numerical Analysis, 4th ed., 1988, Burden & Faires</i>
/// @param [in] f unary function to evaluate
/// UFResult = operator()(UFArg)
/// @param [in] x function argument
/// @param [in] dx delta about x to be evaluated
/// @param [out] dydx approx. value of derivative, f'(x)
template <class UnaryFctn, class UFArg, class UFResult>
inline void
numDif5P(UnaryFctn& f, const UFArg& x, const UFArg& dx, UFResult& dydx)
{
    static const UFArg a0(12.0);
    static const UFArg a1(2.0);
    static const UFResult r0(8.0);
    dydx = (f(x-a1*dx) - r0*f(x-dx) + r0*f(x+dx) - f(x+a1*dx))
           / static_cast<UFResult>(a0*dx);
}

/// @brief Numerical differentiation, 5-point method
///
/// Solves for <i>y' = f'(x)</i> given <i>y = f(x)</i>
/// Uses forward-difference or backward-difference method depending on
/// the sign of <i>dx</i>.
/// Ref: <i>Numerical Analysis, 4th ed., 1988, Burden & Faires</i>
/// @param [in] f unary function to evaluate
/// UFResult = operator()(UFArg)
/// @param [in] x function argument
/// @param [in] dx delta about x to be evaluated, positive (+) for
/// forward-difference method, negative (-) for backward-difference
/// @param [out] dydx approx. value of derivative, f'(x)
template <class UnaryFctn, class UFArg, class UFResult>
inline void
numDif5PFB(UnaryFctn& f, const UFArg& x, const UFArg& dx, UFResult& dydx)
{
    static const UFArg a0(12.0);
    static const UFArg a2(2.0);
    static const UFArg a3(3.0);
    static const UFArg a4(4.0);
    static const UFResult r0(-25.0);
    static const UFResult r1(48.0);
    static const UFResult r2(36.0);
    static const UFResult r3(16.0);
    static const UFResult r4(3.0);
    dydx = (r0*f(x) + r1*f(x+dx) - r2*f(x+a2*dx) + r3*f(x+a3*dx) - r4*f(x+a4*dx))
           / static_cast<UFResult>(a0*dx);
}

} // A3
#endif // GENMATH_H
