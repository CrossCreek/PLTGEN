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
 * SpatialEphFileGAP.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//==============================================================================
//                         U N C L A S S I F I E D
//==============================================================================
// UNITS 030109
// Function Arg & Return: Standard GenEngr (see constants.h)
// Stream I/O:            N/A
// Initialization Files:  nmi, s

// ============================================================================
//  AutoUpdate Enabled
//  On copy and assignment, resources are SHARED (i.e., NOT duplicated)

//  FILE      : SpatialEphFileGAP.h

//  NAMESPACE : A3

//  MODULE(s) : SpatialEphFileGAP

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V01.00    2007-07-25  JAM  Initial Release for EMAF
//              V4.4-001  2008-10-03  JAM  1st SATSIM build
//                                         Changed from TimeClockVT to TimeClock
//                                         Changed units to nmi and s
//                                         Moved RefTime from file
//                                         to constructor
//              V4.5-001  2009-07-01  JAM  Deleted usused local test variables
//                                         Applied new/more eff mtx multiply fcn

//  NOTES :

//==============================================================================
#ifndef SPATIALEPHFILEGAP_H
#define SPATIALEPHFILEGAP_H	"SpatialEphFileGAP V4.4-001  2008-10-03"

//------------------------------------------------------------------------------
// required headers
#include "SpatialTimeDep.h"
#include "CoorTransEciEcf.h"
#include <map>

// =============================================================================
// A3 namespace start
namespace A3
{
// given 2 ECF way pts, GreatArcPath returns the ECF at some time between
// the way pts
void GreatArcPath(double tmRel0, const VectorCartesian& posEcf0,
                  double tmRel1, const VectorCartesian& posEcf1,
                  double tmRel,
                  VectorCartesian& posEcf, VectorCartesian& velEcf);

// -------------------------------------------------------------------------
class SpatialEphFileGAP : public SpatialTimeDep
{
public:
    // constructors & destructors



    explicit SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                       Handle< CoorTransBaseEci > hCoorTrans );

    SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                       const TimeJ2000& refTime,
                       const std::string& sFileNm );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                       Handle< CoorTransBaseEci > hCoorTrans,
                       const TimeJ2000& refTime,
                       const std::string& sFileNm );

    SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                       const TimeJ2000& refTime,
                       std::istream& isWayPts );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFileGAP( HandleConst< TimeJ2000 > hTime,
                       Handle< CoorTransBaseEci > hCoorTrans,
                       const TimeJ2000& refTime,
                       std::istream& isWayPts );

    virtual ~SpatialEphFileGAP();

    virtual bool IsStale() const;

    virtual const char* GetVersion() const;

protected:
    // file functions
    void ReadExtWayPtFile();
    void ReadExtWayPt( std::istream& isWayPts );
    virtual std::istream& Initialize( std::istream& is );
    virtual void UpdateState();

private:
    TimeJ2000 m_epoch;
    std::map< double, VectorCartesian > m_waypts;

    CoorTransEciEcf  m_coorTransEciEcf;

    std::string m_sFileNm;

    VectorCartesian  m_vecEcfPosition;
    VectorCartesian  m_vecEcfVelocity;
};

//==============================================================================
// inline functions
inline bool SpatialEphFileGAP::IsStale() const
{
    return SpatialTimeDep::IsStale() || m_coorTransEciEcf.IsStale();
}

inline const char* SpatialEphFileGAP::GetVersion() const
{
    return SPATIALEPHFILEGAP_H;
}
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALEPHFILEGAP_H
