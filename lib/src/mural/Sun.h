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
 * Sun.h
 *
 * MURAL - Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef SUN_H
#define SUN_H "Sun"

#include <iostream>
#include "general/utility/Constant.h"

using namespace std;

class SpaceVector;

/**
 * Sun is the class that is used to represent the Sun and provide
 * information about it throughout time.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class Sun
{
public:

    Sun(const Sun &copyMe);
    virtual ~Sun();

    Sun&   operator =  (const Sun &copyMe);

    static void        PrintPositionFile(ofstream &outfile);

    static bool        IsInLight(int timeIndex,
                                 const SpaceVector &positionVector);

    static SpaceVector GetPosition(int timeIndex);
    static SpaceVector GetPosition(int timeIndex,
                                   const REFERENCE_FRAME &returnFrame);

protected:

private:

    Sun();

    static inline string GetClassName();

    static const double RADIUS_NMI_s;
};

inline
string
Sun::GetClassName()
{
    return(SUN_H);
}

#endif
