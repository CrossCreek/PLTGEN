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
 * GRC.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : GRC.h

//  NAMESPACE : A3

//  SYSTEM    : SATSIM 2000

//  SUBSYSTEM : SST General Library

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  2000-03-29  JAM  Initial release of this software
//              V2.0      04/29/2002  JAM  1st A3 library build
//              v5.0       04/13/2010 KER add security banner


//  NOTES : Processes run control for the SatSim system
//          The full tags are used in this class (i.e. <TAG_STRING>)
//          in lieu of just the unique string (i.e. TAG_STRING) to
//          simplify searching (i.e. reduce the number of false hits),
//          scanning (i.e. its easier to spot a tag in the code if
//          it bounded by brackets) and automated comparison of tags that
//          are contained in a run control file and this class

//          This class was designed to be inherited by other classes designed
//          to read a particular section of the Run Control file.
//          Public and privated functions have been defined as virtual.

//****************************************************************************//

// CLASS PURPOSE :

// To provide the interface to process the SATSIM 2000 Run Control file

// CLASS INTERFACES :

//----------------------------------------------------------------------------//

// PUBLIC :

//----------------------------------------------------------------------------//

//  GRC(const string nmRunCotnrolFile, const string version );

//  Purpose  : Alternate class constructor to initialize data.
//             This construtor is the required constructor to use to instantiate
//             this class. Must provide the run control file name.

//  Input    :
//     nmRunControlFile -  reference to string with name of run control file
//     version          - the version the run control file must be

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines or
//             run control file name is blank.

//----------------------------------------------------------------------------//

//  inline ~GRC()

//  Purpose  : Definition for class destructor

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : No exceptions thrown

//----------------------------------------------------------------------------//

//  inline virtual void DumpRc( const string &nmDumpFile )

//  Purpose  : Dump the contents of the run control file read into memory
//             to a given file.

//  Input    :
//     nmDumpFile - reference to a string with the name of file to write

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if a problem occurs writing the file.

//----------------------------------------------------------------------------//

//  inline virtual bool FindClassification(bool bFailIsFatal = false)

//  Purpose  : Find the Classification token in the run control file

//  Input    :
//     bFailIsFatal - flag when True indicates that not finding the token
//                    is fatal, when True an exception will be thrown.
//                    (optional argument)

//  Output   : None

//  Return   : True if found, otherwise false

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline virtual bool FindSecurityBanner(bool bFailIsFatal = false)

//  Purpose  : Find the Security banner token in the run control file

//  Input    :
//     bFailIsFatal - flag when True indicates that not finding the token
//                    is fatal, when True an exception will be thrown.
//                    (optional argument)

//  Output   : None

//  Return   : True if found, otherwise false

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline virtual bool FindDescription(bool bFailIsFatal = false)

//  Purpose  : Find the Description token in the run control file

//  Input    :
//     bFailIsFatal - flag when True indicates that not finding the token
//                    is fatal, when True an exception will be thrown.
//                    (optional argument)

//  Output   : None

//  Return   : True if found, otherwise false

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline virtual string GetClassification()

//  Purpose  : Return the classification from the run control file

//  Input    : None

//  Output   : None

//  Return   : a copy of a classification string

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline virtual string GetSecurityBanner()

//  Purpose  : Return the security banner from the run control file

//  Input    : None

//  Output   : None

//  Return   : a copy of a security banner string

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline virtual string GetDescription()

//  Purpose  : Return the description from the run control file

//  Input    : None

//  Output   : None

//  Return   : a copy of a description string

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

//  inline virtual const string & GetRunControlFileNm() const

//  Purpose  : Return the run control file name

//  Input    : None

//  Output   : None

//  Return   : refernence to a run control filename string

//  Except   : No exceptions thrown.

//----------------------------------------------------------------------------//

//  inline virutal const char* GetVersion() const

//  Purpose  : Return the current version for this class

//  Input    : None

//  Output   : None

//  Return   : Content of the define GRC_H

//  Except   : No exceptions thrown

//----------------------------------------------------------------------------//

// PROTECTED :

//----------------------------------------------------------------------------//

//  virtual void InitItems()

//  Purpose  : Create default set of run control items

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : Exceptions thrown if problem with supporting routines.

//----------------------------------------------------------------------------//

// PRIVATE :

//----------------------------------------------------------------------------//

//  inline GRC(const GRC &)

//  Purpose  : Default copy constructor, in private part of class to force
//             not being able to copy this class.  Only want one copy of the
//             run control in memory and for copy would have to adjust the
//             new copy stream pointers.  Do no want that to occur.

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : No exceptions thrown

//----------------------------------------------------------------------------//

//  inline GRC & operator = (const GRC &GRCCopy)

//  Purpose  : Overloaded assignment operator in private part of class to force
//             not being able to copy this class.

//  Input    : None

//  Output   : None

//  Return   : Nothing

//  Except   : No exceptions thrown

//****************************************************************************//

#ifndef GRC_H
#define GRC_H  "GRC Version 2.0"

#include <fstream>
#include <string>       // For STL string

#include <sstream>

#include "GRCItem.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class GRC
{
public:
    // Constructor and Destructor

    explicit GRC( const std::string &nmRunControlFile );

    virtual ~GRC();

    // Public member functions
    inline void DumpRc( const std::string &nmDumpFile );

    inline bool FindClassification(bool bFailIsFatal = false);

    inline bool FindSecurityBanner(bool bFailIsFatal = false);

    inline bool FindDescription(bool bFailIsFatal = false);

    inline std::string GetClassification();

    inline std::string GetSecurityBanner();

    inline std::string GetDescription();

    // inline std::istream& GetRcStream();
    inline std::istream& operator()();

    inline const std::string & GetRunControlFileNm() const;

    inline const char* GetVersion() const;

    // No public data members

protected:

    // Protected member functions
    virtual void InitItems();

    // Protected data members
    GRCItem m_itmRunControl;     // Run control start and end positions

private:

    // Private member functions

    // Disable copying this class with copy constructor in private section

    inline GRC(const GRC &);

    // Disable using assignment operator to copy the class

    inline GRC& operator=(const GRC &GRCCopy);

    // Private data members

    std::string m_strRunControlFileNm;   // Run control filename

    std::stringstream m_rcStream;

    // Common information for all modules
    GRCItem m_itmClassification;
    GRCItem m_itmSecurityBanner;
    GRCItem m_itmDescription;

};

//----------------------------------------------------------------------------//

inline void GRC::DumpRc( const std::string &nmDumpFile )
{
    std::ofstream dumpFile( nmDumpFile.c_str() );
    dumpFile << m_rcStream.rdbuf();
    dumpFile.close();
}

//----------------------------------------------------------------------------//

inline bool GRC::FindClassification(bool bFailIsFatal)
{
    return m_itmClassification.FindFirst(bFailIsFatal);
}

//----------------------------------------------------------------------------//

inline bool GRC::FindSecurityBanner(bool bFailIsFatal)
{
    return m_itmSecurityBanner.FindFirst(bFailIsFatal);
}

//----------------------------------------------------------------------------//

inline bool GRC::FindDescription(bool bFailIsFatal)
{
    return m_itmDescription.FindFirst(bFailIsFatal);
}

//----------------------------------------------------------------------------//

inline std::string GRC::GetClassification()
{
    std::string classification;

    if (this->FindClassification()) {
        (*this)() >> classification;
    } else {
        classification = "";
    }

    return classification;
}

//----------------------------------------------------------------------------//

inline std::string GRC::GetSecurityBanner()
{
    std::string securityBanner;

    if ( this->FindSecurityBanner() ) {
        char ch[2] = " ";

        while ( (*this)().get( ch[0] ) && ch[0] != '<' ) {
            securityBanner += ch;
        }
    } else {
        securityBanner = "";
    }

    return securityBanner;
}

//----------------------------------------------------------------------------//

inline std::string GRC::GetDescription()
{
    std::string description;

    if ( this->FindDescription() ) {
        char ch[2] = " ";

        while ( (*this)().get( ch[0] ) && ch[0] != '<' ) {
            description += ch;
        }
    } else {
        description = "";
    }

    return description;
}

//----------------------------------------------------------------------------//

// inline std::istream & GRC::GetRcStream()
inline std::istream& GRC::operator()()
{
    return m_rcStream;
}

//----------------------------------------------------------------------------//

inline const std::string & GRC::GetRunControlFileNm() const
{
    return m_strRunControlFileNm;
}

//----------------------------------------------------------------------------//

inline const char* GRC::GetVersion() const
{
    return GRC_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
