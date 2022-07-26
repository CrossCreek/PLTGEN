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
 * Trek.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TREK_H
#define TREK_H "Trek"

#include "Journey.h"
#include "TrekNode.h"

#include "general/exception/NullPointerException.h"

using namespace std;

class InputDataElement;

/**
 * This class represents a trek based on a series of points read in from the trek.input file.
 * This is for other vehicle modeling (such as airplane and ground vehicles)
 *
 * Name                           Description (units)
 * -------------                  ----------------------------------------------
 * ptrTrekFilename_a*             string containing Trek Input Filename
 * ptrTrekNodeList_a*             pointer to a list of node(s) in trek (only one
 *                                  node if stationary)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class Trek : public Journey
{
public:

    Trek();
    Trek(const Trek &copyMe);
    virtual ~Trek();

    Trek&  operator =  (const Trek &copyMe);
    bool   operator == (const Trek &compareMe);

    void      PrintAttributes(ostream &outfile) const;

    TrekNode* GetFirstTrekNode() const;
    TrekNode* GetLastTrekNode() const;

    void CreateTrek();

    void SetInitialLayover(int initialLayover);
    void SetLatitude(double latitude);
    void SetLongitude(double longitude);
    void SetAltitude(double altitude);
    void SetTrekFileName(shared_ptr<string> ptrfileName);

    inline const shared_ptr<string>     GetTrekFileName() const;

    inline const LinkList<TrekNode>* GetTrekNodeList() const;

protected:

private:

    void ExtractTrekInfo(InputDataElement *ptrInputDataElement);

    void RetrieveTrekNodes();

    static inline string GetClassName();

    shared_ptr<string>  ptrTrekFilename_a;
    LinkList<TrekNode>  *ptrTrekNodeList_a;

    int                  initialLayover_a;
    double               latitude_a;
    double               longitude_a;
    double               altitude_a;
};

inline
const shared_ptr<string>
Trek::GetTrekFileName() const
{
    return(ptrTrekFilename_a);
}

inline
const LinkList<TrekNode>*
Trek::GetTrekNodeList() const
{
    return(ptrTrekNodeList_a);
}

inline
string
Trek::GetClassName()
{
    return(TREK_H);
}

#endif
