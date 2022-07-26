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
 * OPSStateVectorParser.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef OPS_STATE_VECTOR_PARSER_H
#define OPS_STATE_VECTOR_PARSER_H "OPSStateVectorParser"

#include <fstream>
#include <string>
#include <map>
//RAGUSA 7/24/2017 - Needed to resolve shared_ptr removal from std library.
#include <memory>

using namespace std;

class SpaceVector;
class Calendar;

/**
 * Parses the OPS State Vector File and holds a map of J2000 Time to
 * position/Velcotiy state vectors
 *
 * @author Brian Fowler
 * @date 09/16/11
 */
class OPSStateVectorParser
{
public:

    /**
     * Constructor taking the file name of the ops state vector
     *
     * @param &fileName the file name of the ops state vector
     */



    explicit OPSStateVectorParser(const string &fileName);

    /**
     * Destructor
     */
    ~OPSStateVectorParser();

    /**
     * Method to Parse the State Vector File.  This method finds the closest Epoch to the Simulation day
     * and populates the std::maps with the state vectors for that day.  This will be used to look up later.
     */
    void  ParseStateVectorFile();

    /**
     * Gets the J2000 Epoch from the internal maps
     *
     * @param designator
     * @return double
     */
    double GetJ2000Epoch(const string &designator) const;
    /**
     * Gets the SpaceVector Position from the internal maps
     *
     * @param designator
     * @return SpaceVector ptr
     */
    SpaceVector GetPosition(const string &designator) const;

    /**
     * Gets the SpaceVector Velocity from the internal maps
     *
     * @param designator
     * @return SpaceVector ptr
     */
    SpaceVector GetVelocity(const string &designator) const;

    static inline void                             InitStateVectorParser(const string &fileName);
    static inline shared_ptr<OPSStateVectorParser> GetStateVectorParser();

protected:

private:

    /**
     * Method to Parse the State Vector File.  This method finds the closest Epoch to the Simulation day
     * and populates the std::maps with the state vectors for that day.  This will be used to look up later.
     *
     * The last record in the file will be used if no date is closer to the simulation start
     */
    string FindEpochDate();

    static inline string GetClassName();

    static shared_ptr<OPSStateVectorParser> ptrOPSStateVectorParser_s;

    shared_ptr<string> ptrStateVectorFileName_a;
    shared_ptr<std::map<string, double>> ptrEpochJ2000Map_a;
    shared_ptr<std::map<string, SpaceVector>> ptrPositionMap_a;
    shared_ptr<std::map<string, SpaceVector>> ptrVelocityMap_a;
};

/**
 * Inits the singleton state vector parser.
 */
inline
void
OPSStateVectorParser::InitStateVectorParser(const string &fileName)
{
    if ( ptrOPSStateVectorParser_s == NULL ) {
        ptrOPSStateVectorParser_s = shared_ptr<OPSStateVectorParser>(new OPSStateVectorParser(fileName));
    }
}

/**
 * Gets the singleton state vector parser.
 *
 * @return static OPSStateVectorParser
 */
inline
shared_ptr<OPSStateVectorParser>
OPSStateVectorParser::GetStateVectorParser()
{
    return ptrOPSStateVectorParser_s;
}

/**
 * Gets the class name
 *
 * @return string
 */
inline
string
OPSStateVectorParser::GetClassName()
{
    return(OPS_STATE_VECTOR_PARSER_H);
}

#endif

