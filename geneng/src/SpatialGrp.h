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
 * SpatialGrp.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  SCCS - Release %R%  Delta %I%  from %G%

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialGrp.h

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V5.0      2010-07-23  JAM  Added spatial headers

//  NOTES : This header include all spatial headers.



//****************************************************************************//

//==============================================================================
#ifndef SPATIALGRP_H
#define SPATIALGRP_H	"SpatialGrp V2.0"

//------------------------------------------------------------------------------
// spatial headers
#include "Spatial.h"
#include "SpatialGeocentric.h"
#include "SpatialGeodetic.h"
#include "SpatialMJ2J4.h"
#include "SpatialEphFileSatsim.h"
#include "SpatialEphFileHFNI.h"
#include "SpatialEphFileTrail.h"
#include "SpatialEphFilePlanning.h"
#include "SpatialStationaryEci.h"
#include "SpatialSun.h"
#include "SpatialGeocentricGrid.h"
#include <SpatialEphFilePlanningSS.h>
#include <SpatialEphFileSST.h>
#include <SpatialEphFilePost.h>
#include <SpatialEphFileGAP.h>
#include <SpatialEphFileComplex.h>
#include <SpatialEphOplFile.h>
#include <SpatialEphFileFreeform.h>
#include <SpatialIntegrator.h>
#include <SpatialJpl.h>

//------------------------------------------------------------------------------
#endif

//==============================================================================
