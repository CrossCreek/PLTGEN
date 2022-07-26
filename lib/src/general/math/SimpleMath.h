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
 * SimpleMath.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef SIMPLE_MATH_H
#define SIMPLE_MATH_H "SimpleMath"

#include <iostream>
#include <cmath>

#include "general/array/Array2D.h"
#include "general/utility/Constant.h"

using namespace std;

/**
 * SimpleMath is the class used to perform simple math functions
 * and conversions.
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class SimpleMath
{
public:

    virtual ~SimpleMath();

    inline bool     operator == (const SimpleMath &compareMe) const;
    inline bool     operator != (const SimpleMath &compareMe) const;

    static int   Choose(int available, int needed);
    static int   Factorial(int number);
    static int   FractionalPart(double num);
    static int   Round(double value);
    static int   RoundUp(double value);
    static int   Truncate(double num);

    static int Abs(int num);
    static double Abs(double num);
    static double ArcCosine(double x);
    static double ArcSine(double x);
    static double ArcTangent(double x);
    static double ArcTangent(double y, double x);
    static double ConvertAngle(double value,
                               const ANGLE_UNIT &convertFromType,
                               const ANGLE_UNIT &convertToType);
    static double ConvertDistance(double value,
                                  const DISTANCE_UNIT &convertFromType,
                                  const DISTANCE_UNIT &convertToType);
    static double ConvertDistanceToDegrees(double value,
                                           const DISTANCE_UNIT &convertFromType);
    static double Interpolate(double lowerX,
                              double upperX,
                              double currentX,
                              double lowerY,
                              double upperY);
    static double Log10(double num);
    static int Maximum(const Array<int> *ptrIntArray);
    static double Maximum(const Array<double> *ptrDoubleArray);
    static int Minimum(const Array<int> *ptrIntArray);
    static double Minimum(const Array<double> *ptrDoubleArray);
    static double Percent(int numerator, int denominator);
    static double Percent(double numerator, double denominator);
    static double Power(double coefficient, double exponent);
    static double Remainder(double number);
    static double Remainder(double x, double y);
    static double SquareRoot(double number);

    static Array2D<int>* GetPossibleCombinations(int numberAvailable,
            int numberNeeded);

protected:

private:

    SimpleMath();

    static inline string GetClassName();

    const static double ARC_TOLERANCE_s;
};

inline
bool
SimpleMath::operator == (const SimpleMath &compareMe) const
{
    return(true);
}

inline
bool
SimpleMath::operator != (const SimpleMath &compareMe) const
{
    return(false);
}

inline
string
SimpleMath::GetClassName()
{
    return(SIMPLE_MATH_H);
}

#endif
