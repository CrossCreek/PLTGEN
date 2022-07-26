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
 * GenMath.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// General purpose math functions
// Refer to GenMath.h for more info.
// =============================================================================
#include "GenMath.h"
#include <stdexcept>
// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>

// ============================================================================
// A3 namespace start
namespace A3
{
// dB conversions
double toDB(double a)
{
    return 10.0 * log10(a);
}

double fromDB(double a)
{
    return pow(10.0, a / 10.0);
}

// factorial function
unsigned factorial( unsigned number )
{
    if ( number < 3 ) {
        return number;
    }

    if ( number > 12 ) {
        throw std::out_of_range( "<<factorial>> argument greater than 12" );
    }

    unsigned ftr = 0;

    for ( ftr = number--; number > 1; number-- ) {
        ftr *= number;
    }

    return ftr;
}

// -------------------------------------------------------------------------
// bit functions
template
void setBitsOn( unsigned int bitsOn, unsigned int& flags );

template
void setBitsOff( unsigned int bitsOff, unsigned int& flags );

template
bool areBitsOn( unsigned int bits, unsigned int flags );

template
bool areAnyBitsOn( unsigned int bits, unsigned int flags );

// -------------------------------------------------------------------------
// find compile time errors
namespace
{
class Trivial
{
public:
    float operator()(double f)
    {
        return float(2.0 * f + 3.0);
    }
};
}

template
void solveSecant(Trivial&, const float&, const double&, double&, unsigned);

template
void numDif3P(Trivial&, const double&, const double&, float&);

template
void numDif3PFB(Trivial&, const double&, const double&, float&);

template
void numDif5P(Trivial&, const double&, const double&, float&);

template
void numDif5PFB(Trivial&, const double&, const double&, float&);

} // A3
