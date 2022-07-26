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
 * RCX.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
// UNITS 030109
// Function Arg & Return: N/A
// Stream I/O:            N/A
// Initialization Files:  N/A

// ============================================================================
//  FILE      : RCX.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0      04/29/2002  JAM  initial build
//              V1.1      2007-09-05  JAM  added ASSERT option for Find failure
//                                         if Find fails and
//                                         OPTION=NULL then return end()
//                                         OPTION=ASSERT then throw exception
//              V4.4-001  2008-10-03  JAM  1st SATSIM build

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CLASS

//  Name:       RCX
//  Purpose:    Navigate XML tree, extract attributes and data

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  MEMBER FUNCTIONS

//  Public Member:  RCX()
//  Purpose:        empty default constructor
//  Return Type:    n/a
//  Return Value:   n/a

//  Public Member:  RCX(std::istream&)
//  Purpose:        calls Process(std::istream&) to read and parse an XML tree
//  Return Type:    n/a
//  Return Value:   n/a

//  Public Member:  Process(std::istream&)
//  Purpose:        searches for XML root node, reads and parses the XML tree
//  Return Type:    bool
//  Return Value:   true if root node found, else false

//  Public Member:  GetTag() const
//  Purpose:        return node name
//  Return Type:    const std::string&
//  Return Value:   name of node

//  Public Member:  GetData() const
//  Purpose:        return node data
//  Return Type:    const std::string&
//  Return Value:   string containing node data

//  Public Member:  GetHdlDataStream() const
//  Purpose:        provides handle to a stream that may be used to extract
//                  node data
//  Return Type:    Handle< std::istream >
//  Return Value:   stream handle for extracting data

//  Public Member:  GetNAttributes() const
//  Purpose:        return count of node attributes
//  Return Type:    unsigned
//  Return Value:   count of node attributes

//  Public Member:  GetAttribute(const std::string&) const
//  Purpose:        return value of attribute
//  Return Type:    std::string
//  Return Value:   attribute value
//  Argument Type:  const std::string&
//  Argument Value: name of attribute

//  Public Member:  Size() const
//  Purpose:        return count of nodes beneath this node
//  Return Type:    unsigned
//  Return Value:   count of nodes

//  Public Member:  Size(const std::string&) const
//  Purpose:        return count of nodes beneath this node with specified tag
//  Return Type:    unsigned
//  Return Value:   count of nodes with tag
//  Argument Type:  const std::string&
//  Argument Value: tag of interest

//  Public Member:  Size(const std::string&, const std::string&) const
//  Purpose:        return count of nodes beneath this node with specified tag
//                  and attribute name
//  Return Type:    unsigned
//  Return Value:   count of nodes with tag
//  Argument Type:  const std::string&
//  Argument Value: tag of interest
//  Argument Type:  const std::string&
//  Argument Value: attribute name of interest

//  Public Member:  FindNode(const std::string&, OPTION=NONE)
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag,
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, iterator,
//                  OPTION=NONE)
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node, start search at specified iterator
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                  OPTION=NONE)
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                           iterator, OPTION=NONE)
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node, start search
//                  at specified iterator
//  Return Type:    iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, OPTION=NONE) const
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, iterator,
//                  OPTION=NONE) const
//  Purpose:        return iterator to node matching tag one level beneath
//                  this node, start search at specified iterator
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                  OPTION=NONE) const
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNode(const std::string&, const std::string&,
//                           iterator, OPTION=NONE) const
//  Purpose:        return iterator to node matching tag and containing
//                  attribute name one level beneath this node, start search
//                  at specified iterator
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  matching tag
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: node tag to find
//  Argument Type:  const std::string&
//  Argument Value: attribute name required
//  Argument Type:  iterator
//  Argument Value: iterator to search start
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNodeId(const std::string&, OPTION=NONE)
//  Purpose:        return iterator to node matching id="name",
//                  searches tree beneath this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to node  with id "name"
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: id "name" to be found
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  FindNodeId(const std::string&, OPTION=NONE) const
//  Purpose:        return iterator to node matching id="name"
//                  searches tree beneath this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to node  with id "name"
//                  end() or exception if node is not found
//  Argument Type:  const std::string&
//  Argument Value: id "name" to be found
//  Argument Type:  OPTION
//  Argument Value: find failure returns end() if OPTION=NONE (default)
//                  find failure throws exception if OPTION=ASSERT

//  Public Member:  begin()
//  Purpose:        return iterator to 1st node one level beneath this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  end()
//  Purpose:        return iterator that addresses the location succeeding the
//                  last node one level beneath this node
//  Return Type:    iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  begin() const
//  Purpose:        return iterator to 1st node one level beneath this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  end() const
//  Purpose:        return iterator that addresses the location succeeding the
//                  last node one level beneath this node
//  Return Type:    const_iterator
//  Return Value:   random-access iterator to 1st node

//  Public Member:  SizeTotal() const
//  Purpose:        count of nodes in entire XML tree beneath this node
//  Return Type:    unsigned
//  Return Value:   count of nodes
//  Argument Type:
//  Argument Value:

//  Public Member:  SizeTotal(const std::string&) const
//  Purpose:        count of nodes in entire XML tree beneath this node
//                  matching tag
//  Return Type:    unsigned
//  Return Value:   count of nodes
//  Argument Type:  const std::string&
//  Argument Value: tag

//  Public Member:  SizeTotal(const std::string&, const std::string&) const
//  Purpose:        count of nodes in entire XML tree beneath this node
//                  matching tag and type
//  Return Type:    unsigned
//  Return Value:   count of nodes
//  Argument Type:  const std::string&
//  Argument Value: tag
//  Argument Type:  const std::string&
//  Argument Value: type

//  Public Member:  ReportState(std::ostream&)
//  Purpose:        dump XML tree to stream
//  Return Type:    void
//  Return Value:   n/a
//  Argument Type:  std::ostream&
//  Argument Value: stream receiving XML tree

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  NOTES :

//****************************************************************************//
#ifndef RCX_H
#define RCX_H "RCX.h Version 1.1"

#include "Handle.h"
#include <iosfwd>
#include <string>
#include <map>
#include <vector>

// A3 namespace begin
namespace A3
{

class RCX
{
public:
    typedef std::vector< RCX > ContNodes;
    typedef ContNodes::iterator iterator;
    typedef ContNodes::const_iterator const_iterator;

    enum OPTION { NONE, ASSERT };

    RCX();



    explicit RCX( std::istream& is );

    bool Process( std::istream& is );

    const std::string& GetTag() const;
    const std::string& GetData() const;
    Handle< std::istream > GetHdlDataStream() const;

    unsigned GetNAttributes() const;
    std::string GetAttribute( const std::string& tag ) const;

    unsigned Size() const;
    unsigned Size( const std::string& sTagName ) const;
    unsigned Size( const std::string& sTagName,
                   const std::string& sType  ) const;

    iterator FindNode( const std::string& sTagName,
                       OPTION opt=NONE );

    iterator FindNode( const std::string& sTagName,
                       iterator itrStart,
                       OPTION opt=NONE );

    iterator FindNode( const std::string& sTagName,
                       const std::string& sType,
                       OPTION opt=NONE );

    iterator FindNode( const std::string& sTagName,
                       const std::string& sType,
                       iterator itrStart,
                       OPTION opt=NONE );

    const_iterator
    FindNode( const std::string& sTagName,
              OPTION opt ) const; // opt=NONE ) const;

    const_iterator
    FindNode( const std::string& sTagName,
              const_iterator itrStart,
              OPTION opt ) const; // opt=NONE ) const;

    const_iterator
    FindNode( const std::string& sTagName,
              const std::string& sType,
              OPTION opt ) const; // opt=NONE ) const;

    const_iterator
    FindNode( const std::string& sTagName,
              const std::string& sType,
              const_iterator itrStart,
              OPTION opt ) const; // opt=NONE ) const;

    iterator FindNodeId( const std::string& sId,
                         OPTION opt=NONE );

    const_iterator FindNodeId( const std::string& sId,
                               OPTION opt ) const; // opt=NONE ) const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    unsigned SizeTotal() const;
    unsigned SizeTotal( const std::string& sTagName ) const;
    unsigned SizeTotal( const std::string& sTagName,
                        const std::string& sType  ) const;

    void ReportState( std::ostream& os );

protected:

private:
    typedef std::map< std::string, std::string >  ContAttributes;

    static void ParseNodes( std::istream& is,  std::streampos dataEnd,
                            std::string& sData, ContNodes& vNodes );

    static void ParseAttributes( const std::string& sAttributes,
                                 ContAttributes& mAttributes );

    static void FindBlock( std::istream&  is,
                           std::streampos& blockStart,
                           std::streampos& blockEnd,
                           std::streampos& dataStart,
                           std::streampos& dataEnd,
                           std::string&    sTagName,
                           std::string&    sAttribData );

    ContAttributes m_mAttributes;
    ContNodes m_vNodes;

    std::string m_sTagName;
    std::string m_sData;
};

}
// A3 namespace end
#endif // RCX_H
