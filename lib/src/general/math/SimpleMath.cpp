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
 * SimpleMath.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "general/math/SimpleMath.h"
#include "general/exception/ArithmeticException.h"
#include "general/exception/DivideByZeroException.h"
#include "general/utility/Constant.h"
#include "general/data/UnitData.h"

const double SimpleMath::ARC_TOLERANCE_s = 1.01;

/* ***************************************************************************** */

SimpleMath::SimpleMath()
{
    return;
}

/* ***************************************************************************** */

int
SimpleMath::Choose(int available, int needed)
{
    if ((available < 1) || (needed < 1) || (available < needed)) {
        throw new ArithmeticException(GetClassName(),
                                      "Choose(int, int)");
    }

    return(Factorial(available) / (Factorial(needed) * Factorial(available - needed)));
}

/* ***************************************************************************** */

int
SimpleMath::Factorial(int number)
{
    int factorial = 1;
    int count = 2;

    if (number < 0) {
        throw new ArithmeticException(GetClassName(), "Factorial(int)");
    }

    while (count <= number) {
        factorial *= count;
        ++count;
    }

    return(factorial);
}

/* ***************************************************************************** */

int
SimpleMath::FractionalPart(double num)
{
    double result = num - Truncate(num);

    while ((result - Truncate(result)) != 0.0) {
        result *= 10.0;
    }

    return((int)result);
}

/* ***************************************************************************** */

int
SimpleMath::Round(double value)
{
    return(Truncate(value + 0.5));
}

/* ***************************************************************************** */

int
SimpleMath::RoundUp(double value)
{
    int result = Truncate(value);

    if (value > (double)result) {
        ++result;
    }

    return(result);
}

/* ***************************************************************************** */

int
SimpleMath::Truncate(double num)
{
    return((int)num);
}

/* ***************************************************************************** */

int
SimpleMath::Abs(int num)
{
    int returnValue = num;

    if (num < 0) {
        returnValue = (-1)*(num);
    }

    return(returnValue);
}

/* ***************************************************************************** */

double
SimpleMath::Abs(double num)
{
    double returnValue = num;

    if (num < 0.0) {
        returnValue = (-1)*(num);
    }

    return(returnValue);
}

/* ***************************************************************************** */

double
SimpleMath::ArcCosine(double x)
{
    double arcCosine = 0.0;

    if ((x < -ARC_TOLERANCE_s) || (x > ARC_TOLERANCE_s)) {
        throw new ArithmeticException(GetClassName(), "ArcCosine(const double&)");
    }

    if (x <= -1.000) {
        arcCosine = UnitData::GetStoreHalfCircle();
    } else if (x < 1.000) {
        arcCosine = acos(x);
    }

    return(arcCosine);
}

/* ***************************************************************************** */

double
SimpleMath::ArcSine(double x)
{
    if ((x < -ARC_TOLERANCE_s) || (x > ARC_TOLERANCE_s)) {
        throw new ArithmeticException(GetClassName(), "ArcSine(const double&)");
    }

    return(asin(x));
}

/* ***************************************************************************** */

double
SimpleMath::ArcTangent(double x)
{
    return(atan(x));
}

/* ***************************************************************************** */

double
SimpleMath::ArcTangent(double y, double x)
{
    double arcTangent = 0.0;

    if ((x != 0.0) || (y != 0.0)) {
        arcTangent = atan2(y, x);
    }

    return(arcTangent);
}

/* ***************************************************************************** */

double
SimpleMath::ConvertAngle(double angle,
                         const ANGLE_UNIT &convertFromType,
                         const ANGLE_UNIT &convertToType)
{
    double returnAngle = angle;

    if (convertFromType != convertToType) {
        if (convertFromType == DEGREES) {
            if (convertToType == RADIANS) {
                returnAngle *= DEGREES_TO_RADIANS;
            } else if (convertToType == GRADIENTS) {
                returnAngle *= DEGREES_TO_GRADIENTS;
            }
        } else if (convertFromType == RADIANS) {
            if (convertToType == DEGREES) {
                returnAngle *= RADIANS_TO_DEGREES;
            } else if (convertToType == GRADIENTS) {
                returnAngle *= RADIANS_TO_GRADIENTS;
            }
        } else if (convertFromType == GRADIENTS) {
            if (convertToType == DEGREES) {
                returnAngle *= GRADIENTS_TO_DEGREES;
            } else if (convertToType == RADIANS) {
                returnAngle *= GRADIENTS_TO_RADIANS;
            }
        }
    }

    return(returnAngle);
}

/* ***************************************************************************** */

double
SimpleMath::ConvertDistance(double distance,
                            const DISTANCE_UNIT &convertFromType,
                            const DISTANCE_UNIT &convertToType)
{
    double returnDistance = distance;

    if (convertFromType != convertToType) {
        if (convertFromType == METERS) {
            if (convertToType == KILOMETERS) {
                returnDistance *= METERS_TO_KILOMETERS;
            } else if (convertToType == INCHES) {
                returnDistance *= METERS_TO_INCHES;
            } else if (convertToType == FEET) {
                returnDistance *= METERS_TO_FEET;
            } else if (convertToType == MILES) {
                returnDistance *= METERS_TO_MILES;
            } else if (convertToType == NAUTICAL_MILES) {
                returnDistance *= METERS_TO_NMI;
            }
        } else if (convertFromType == KILOMETERS) {
            if (convertToType == METERS) {
                returnDistance *= KILOMETERS_TO_METERS;
            } else if (convertToType == INCHES) {
                returnDistance *= KILOMETERS_TO_INCHES;
            } else if (convertToType == FEET) {
                returnDistance *= KILOMETERS_TO_FEET;
            } else if (convertToType == MILES) {
                returnDistance *= KILOMETERS_TO_MILES;
            } else if (convertToType == NAUTICAL_MILES) {
                returnDistance *= KILOMETERS_TO_NMI;
            }
        } else if (convertFromType == INCHES) {
            if (convertToType == METERS) {
                returnDistance *= INCHES_TO_METERS;
            } else if (convertToType == KILOMETERS) {
                returnDistance *= INCHES_TO_KILOMETERS;
            } else if (convertToType == FEET) {
                returnDistance *= INCHES_TO_FEET;
            } else if (convertToType == MILES) {
                returnDistance *= INCHES_TO_MILES;
            } else if (convertToType == NAUTICAL_MILES) {
                returnDistance *= INCHES_TO_NMI;
            }
        } else if (convertFromType == FEET) {
            if (convertToType == METERS) {
                returnDistance *= FEET_TO_METERS;
            } else if (convertToType == KILOMETERS) {
                returnDistance *= FEET_TO_KILOMETERS;
            } else if (convertToType == INCHES) {
                returnDistance *= FEET_TO_INCHES;
            } else if (convertToType == MILES) {
                returnDistance *= FEET_TO_MILES;
            } else if (convertToType == NAUTICAL_MILES) {
                returnDistance *= FEET_TO_NMI;
            }
        } else if(convertFromType == MILES) {
            if (convertToType == METERS) {
                returnDistance *= MILES_TO_METERS;
            } else if (convertToType == KILOMETERS) {
                returnDistance *= MILES_TO_KILOMETERS;
            } else if (convertToType == INCHES) {
                returnDistance *= MILES_TO_INCHES;
            } else if (convertToType == FEET) {
                returnDistance *= MILES_TO_FEET;
            } else if (convertToType == NAUTICAL_MILES) {
                returnDistance *= MILES_TO_NMI;
            }
        } else if (convertFromType == NAUTICAL_MILES) {
            if (convertToType == METERS) {
                returnDistance *= NMI_TO_METERS;
            } else if (convertToType == KILOMETERS) {
                returnDistance *= NMI_TO_KILOMETERS;
            } else if (convertToType == INCHES) {
                returnDistance *= NMI_TO_INCHES;
            } else if (convertToType == FEET) {
                returnDistance *= NMI_TO_FEET;
            } else if (convertToType == MILES) {
                returnDistance *= NMI_TO_MILES;
            }
        }
    }

    return(returnDistance);
}

/* ***************************************************************************** */

double
SimpleMath::ConvertDistanceToDegrees(double distance,
                                     const DISTANCE_UNIT &convertFromType)
{
    const double distanceNMI = ConvertDistance(distance, convertFromType,
                               NAUTICAL_MILES);
    return(distanceNMI * DEGREES_PER_NMI);
}

/* ***************************************************************************** */

double
SimpleMath::Interpolate(double lowerX,
                        double upperX,
                        double currentX,
                        double lowerY,
                        double upperY)
{
    double interpolatedY = (upperY - lowerY) / (upperX - lowerX);
    interpolatedY *= (currentX - lowerX);
    interpolatedY += lowerY;
    return(interpolatedY);
}

/* ***************************************************************************** */

double
SimpleMath::Log10(double num)
{
    if (num <= 0.0) {
        throw new ArithmeticException(GetClassName(), "Log10(const double&)");
    }

    return(log10(num));
}

/* ***************************************************************************** */

int
SimpleMath::Maximum(const Array<int> *ptrArray)
{
    int maximumValue = LONG_MIN;
    int element = 0;

    while (element < ptrArray->Size()) {
        maximumValue = std::max(maximumValue,
                                ptrArray->ElementAt(element));
        ++element;
    }

    return(maximumValue);
}

/* ***************************************************************************** */

double
SimpleMath::Maximum(const Array<double> *ptrDoubleArray)
{
    int   element = 0;
    double maximumValue = (double)LONG_MIN;

    while (element < ptrDoubleArray->Size()) {
        maximumValue = std::max(maximumValue, ptrDoubleArray->ElementAt(element));
        ++element;
    }

    return(maximumValue);
}

/* ***************************************************************************** */

int
SimpleMath::Minimum(const Array<int> *ptrArray)
{
    int minimumValue = LONG_MAX;
    int element = 0;

    while (element < ptrArray->Size()) {
        minimumValue = std::min(minimumValue,
                                ptrArray->ElementAt(element));
        ++element;
    }

    return(minimumValue);
}

/* ***************************************************************************** */

double
SimpleMath::Minimum(const Array<double> *ptrDoubleArray)
{
    int   element = 0;
    double minimumValue = DBL_MAX;

    while (element < ptrDoubleArray->Size()) {
        minimumValue = std::min(minimumValue, ptrDoubleArray->ElementAt(element));
        ++element;
    }

    return(minimumValue);
}

/* ***************************************************************************** */

double
SimpleMath::Power(double coefficient, double exponent)
{
    if ((coefficient < 0.0) && (FractionalPart(exponent) != 0)) {
        throw new ArithmeticException(GetClassName(),
                                      "Power(const double&, const double&)");
    }

    if ((coefficient == 0.00) && (exponent < 0.00)) {
        throw new ArithmeticException(GetClassName(),
                                      "Power(const double&, const double&)");
    }

    return(pow(coefficient, exponent));
}

/* ***************************************************************************** */

double
SimpleMath::Percent(int numerator, int denominator)
{
    double percent = 0.0;

    if (denominator != 0) {
        percent = ((double)numerator / (double)denominator) * 100.0;
    }

    return(percent);
}

/* ***************************************************************************** */

double
SimpleMath::Percent(double numerator, double denominator)
{
    double percent = 0.0;

    if (denominator != 0.0) {
        percent = (numerator / denominator) * 100.0;
    }

    return(percent);
}

/* ***************************************************************************** */

double
SimpleMath::Remainder(double number)
{
    const int wholeNumber = (int)number;
    return(number - (double)wholeNumber);
}

/* ***************************************************************************** */

double
SimpleMath::Remainder(double x, double y)
{
    if (y == 0.0) {
        throw new DivideByZeroException(GetClassName(),
                                        "Remainder(const double&,const double&)");
    }

    return(fmod(x, y));
}

/* ***************************************************************************** */

double
SimpleMath::SquareRoot(double number)
{
    if (number < 0.0) {
        throw new ArithmeticException(GetClassName(), "SquareRoot(const double&)");
    }

    return(sqrt(number));
}

/* ***************************************************************************** */

Array2D<int>*
SimpleMath::GetPossibleCombinations(int numberAvailable,
                                    int numberNeeded)
{
    const int     numberNotNeeded = numberAvailable - numberNeeded;
    Array2D<int> *ptrValueArray = NULL;

    try {
        int option = 0;
        int need = 0;
        int value = 0;
        int numberOfOptions = 0;
        numberOfOptions = Choose(numberAvailable, numberNeeded);
        ptrValueArray = new Array2D<int>(numberOfOptions, numberNeeded);

        if (numberOfOptions == 1) {
            while (need < numberNeeded) {
                (*ptrValueArray)[option][need] = value++;
                ++need;
            }
        } else {
            while (need < numberNeeded) {
                int pass = 0;
                int startingValue = 0;
                int startOvers = 0;
                int completeStartOvers = 0;
                int maximumValue = 0;
                int endOption = 0;
                int numberOfPasses = 0;
                option = 0;
                endOption = 0;
                startingValue = need;
                startOvers = 0;
                completeStartOvers = 0;
                maximumValue = numberNotNeeded + need;
                pass = 0;
                numberOfPasses = Choose(maximumValue, numberNotNeeded);

                while (pass < numberOfPasses) {
                    value = startingValue;

                    while (value <= maximumValue) {
                        if ((need + 1) == numberNeeded) {
                            ++endOption;
                        } else {
                            endOption += Choose((numberAvailable - value - 1), (numberNeeded - need - 1));
                        }

                        while (option < endOption) {
                            (*ptrValueArray)[option][need] = value;
                            ++option;
                        }

                        ++value;
                    }

                    if (startingValue < maximumValue) {
                        ++startingValue;
                    } else {
                        if ((need + startOvers) < maximumValue) {
                            ++startOvers;
                        } else {
                            ++completeStartOvers;
                            startOvers = completeStartOvers;
                        }

                        startingValue = need + startOvers;
                    }

                    ++pass;
                }

                while (option < numberOfOptions) {
                    (*ptrValueArray)[option][need] = maximumValue;
                    ++option;
                }

                ++need;
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetPossibleCombinations(int, int)");
        throw;
    }

    return(ptrValueArray);
}

/* ***************************************************************************** */

SimpleMath::~SimpleMath()
{
    return;
}
