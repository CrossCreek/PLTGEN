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
 * OrbitRate.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef ORBIT_RATE_H
#define ORBIT_RATE_H "OrbitRate -- Version 1.14, 00/11/07"

#include <iostream>
#include <string>

using namespace std;

/**
 * Data class to hold orbital rate information
 *
 * Name                     Description (units)
 * -------------            ----------------------------------------------------
 * nodalPrecessionRate_a    mean right ascension rate (rad/sec)
 * apsidalRotationRate_a    mean argument of perigee rate (rad/sec)
 * anomalisticMeanMotion_a  anomalistic mean motion (rad/sec)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class OrbitRate
{
public:

    OrbitRate();
    OrbitRate(const double &nodalPrecessionRate,
              const double &apsidalRotationRate,
              const double &anomalisticMeanMotion);
    OrbitRate(const OrbitRate &copyMe);
    virtual ~OrbitRate();

    bool            operator == (const OrbitRate &compareMe) const;
    bool            operator != (const OrbitRate &compareMe) const;

    static inline string GetVersion();

    inline void SetNodalPrecessionRate(const double &nodalPrecessionRate);
    inline void SetApsidalRotationRate(const double &apsidalRotationRate);
    inline void SetAnomalisticMeanMotion(const double &anomalisticMeanMotion);

    inline double GetNodalPrecessionRate() const;
    inline double GetApsidalRotationRate() const;
    inline double GetAnomalisticMeanMotion() const;

protected:

private:

    static const string CLASS_NAME_s;

    double              nodalPrecessionRate_a;
    double              apsidalRotationRate_a;
    double              anomalisticMeanMotion_a;
};

inline
double
OrbitRate::GetNodalPrecessionRate() const
{
    return(nodalPrecessionRate_a);
}

inline
double
OrbitRate::GetApsidalRotationRate() const
{
    return(apsidalRotationRate_a);
}

inline
double
OrbitRate::GetAnomalisticMeanMotion() const
{
    return(anomalisticMeanMotion_a);
}

inline
void
OrbitRate::SetNodalPrecessionRate(const double &nodalPrecessionRate)
{
    nodalPrecessionRate_a = nodalPrecessionRate;
}

inline
void
OrbitRate::SetApsidalRotationRate(const double &apsidalRotationRate)
{
    apsidalRotationRate_a = apsidalRotationRate;
}

inline
void
OrbitRate::SetAnomalisticMeanMotion(const double &anomalisticMeanMotion)
{
    anomalisticMeanMotion_a = anomalisticMeanMotion;
}

inline
string
OrbitRate::GetVersion()
{
    return(ORBIT_RATE_H);
}

#endif
