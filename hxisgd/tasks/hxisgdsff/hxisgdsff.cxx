#if !defined(HXISGDSFF_C)
#define HXISGDSFF_C 

/** \file Implementaion of the Astro-H "hxisgdsff" tool that creates either an HXI or SGD
    Second FITS file (SFF) from the instrument First FITS file (FFF) -- supported by 
    Astro-H (AH) library infrastructure (libahgen, libahfits, libahlog, etc.).
    Use case scenario: Lorella's example 2-D to 1-D variable-length column-cell extraction and row (column) augmentation
    Use case scenario: Jame's "ahdemo" example variable-length column-cell extraction and row (column) augmentation
    Use case scenario: A generalized 2-D to 1-D variable-length slice (lists-of-lists) row (column) augmentation
    Use case scenario: A specialitzation of the general case with slice parameters provided from a hash of std::bitset.
    Use case scenario: slice parameters provided from std::bitset of HXI and/or SGD FFF telemetry flags.
    \author David B. Hon
    \date 2012
*/

// attempt to get more prose into doxygen main page:
//#include "ReadMe.h"
// Local includes.
#include "ahhxisgd.h"
AHHXISGD_H(hxisgdsff)

#include "ahsgd/ahSGDtyps.h"
AHSGDTYPS_H(hxisgdsff)

#include "ahhxi/ahHXItyps.h"
AHHXITYPS_H(hxisgdsff)

#include "ahtlm/ahBits.h"
AHBITS_H(hxisgdsff)

#include "ahtlm/ahSlice.h"
AHSLICE_H(hxisgdsff)

#include "ahtlm/ahTLM.h"
AHTLM_H(hxisgdsff)

#include "ahtlm/ahTLMIO.h"
AHTLMIO_H(hxisgdsff)

#include "ahtlm/ahTLMinfoCache.h"
AHTLMINFOCACHE_H(hxisgdsff)


// all includes needed and local function declarations -- namespace ahhxisgd 
// note that stdexcept is included, which itself includes string and exception..
#include "ahlog/ahlog.h"
#include "ahgen/ahgen.h"
#include "ahfits/ahfits.h"

using namespace ahhxisgd;
using namespace ahfits;
using namespace ahgen;
using namespace ahlog;
using namespace std;

/* Astro-H standardized main function. Developers would be required
   to follow this basic structure, including certain required function calls as noted below.
*/
// doxygen tag to generate diagram of func. invocations
/// \callgraph
int main(int argc, char ** argv) {
  const string cvsId = "$Name:  $ $Id: hxisgdsff.cxx,v 1.10 2012/08/16 18:03:37 dhon Exp $";

  // default number of checkpoints -- optional
  int checkpoint = 0; // 0 or 1 will iterate doWork once only
  bool clobber = false, precheck = false;

  /// this will change during build 1 development to make use of ahhxisgd::AppRuntime struct? 
  /// Names of input and output files.  
  /// "" or "none" indicate internal test of template driven output?
  string infile_name = "", outfile_name = "", template_name = "";

  /* AhFitsFilePtr is a typedef struct from libahfits that handles FITS file access
     (fitsfile pointers, buffers, current row, etc.). */

  /* Input & Output file structures. */
  AhFitsFilePtr infile = 0, outfile = 0;


  try {
    /* Global initializations. Implemented in libahgen. Set up logging streams, open parameter file,
       handle universal parameters like "debug" and "chatter", etc. REQUIRED IN ALL APPLICATIONS. */
    ahhxisgd::startUp(argc, argv);

    /* Get tool-specific parameters. Implemented in this application with support from libahgen.  */
    ahhxisgd::getParFITS(infile_name, outfile_name, template_name, &clobber, &precheck);

    /* Perform application-specific initializations based on parameters. Implemented in application with
       support from libahgen. Open the input file(s). Create the output file. Position input and output
       file streams appropriately. */
    /// hon:
    // ahhxisgd::initialize(0, outfile_name, template_name, 0, &outfile);
    /// note this follows ahdemo build 0 -- the template here is meant for the
    /// output file only: 
    ahhxisgd::initialize(infile_name, outfile_name, template_name, &infile, &outfile);

    // if precheck is true, perform preCheck and only enter doWork if the result of the precheck
    // is ok/good/true, otherwise shutdown ...
    if( precheck ) {
      precheck = ahhxisgd::preCheck(infile_name, outfile_name, template_name);
      if( ! precheck ) {
        ahhxisgd::finalize(infile, outfile);
        ahhxisgd::shutDown();
        return ahgen::getStatus();
      }
    }
    /* Do the work of this application, which processes the input file line-by-line to unpack the ASIC
       data as it is encoded in FFF, repack it as it is encoded in SFF, and write it to the output file. */
    ahhxisgd::doWork(infile, outfile);
  }
  catch (const exception & x) {
    // C++ exception: this is a standard C++ exception. It includes a function "what()" that returns a
    // string describing the error. For now, just flag the error, and write that messsage to the error stream.
    ahgen::setStatus(1);
    AH_ERR << " -- hxisgdsff: caught error: " << x.what() << endl;
  }
  catch( ... ) {
    ahgen::setStatus(3);
    AH_ERR << " -- hxisgdsff: caught unknown exception somewhere during startUp through doWork..." << endl;
  }

  /* Perform finalizations in parallel to initialize function. Implemented in application with support
     from libahgen.  Close/flush files, etc. Call this regardless of whether an error occurred above. */
  try {
    ahhxisgd::finalize(infile, outfile);
  }
  catch( const std::exception & x ) {
    ahgen::setStatus(2);
    AH_ERR << x.what() << std::endl;
  }
  catch( ... ) {
    ahgen::setStatus(3);
    AH_ERR << " -- hxisgdsff: caught unknown exception at finalize ..." << endl;
  }

  /* Perform global clean-up in parallel to startUp function. Implemented in libahgen. Close parameter file,
     close logging streams, etc. REQUIRED IN ALL APPLICATIONS. */
  ahhxisgd::shutDown();

  /* Return the global status. Passing a 0 will not affect the status if it was already set to 1. */
  return ahgen::getStatus();
} // main -- hxisgdsff.cxx

#endif  // HXISGDSFF_C
