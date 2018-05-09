#if !defined(FINALIZE_C)
#define FINALIZE_C 

/** \file Implementaion of the finalize function for the Astro-H HXI & SGD 
          First Fits File to Second FITS File tool "fff2sff". This derives
          from James Peachey's "ahdemo" Build 0.0 example.
    \author David B. Hon
    \date 2012
*/

#include "ahhxisgd.h" // all includes needed and local function declarations -- namespace ahhxisgd
AHHXISGD_H(finalize)

namespace {
  const string cvsId = "$Name:  $ $Id: finalize.cxx,v 1.6 2012/08/16 17:58:02 dhon Exp $";
}

namespace ahhxisgd {
  // doxygen tag to generate diagram of func. invocations
  /// \callgraph
  void finalize(AhFitsFilePtr infile, AhFitsFilePtr outfile, int* checkpoint) {
    /* ahFitsClose is a function in ahfits that flushes output, updates the checksum and closes the file. */
    /* Close the output file first. */
    // C++: handle exceptions here because the clean-up code needs to continue even if there are errors.
    if( outfile && outfile->m_filename != "" && outfile->m_filename != "none" ) {
      try {
       AH_INFO(ahlog::LOW) <<  " close output file: " << outfile->m_filename << endl;
       ahFitsClose(outfile);
       }
      catch (const exception & x) {
        setStatus(1);
        AH_INFO(ahlog::LOW) << "finalize: error while closing output file: " << x.what() << endl;
      }
    }

    /* Close infile regardless of whether an error occurred above. */
    // C++: handle exceptions here because the clean-up code needs to continue even if there are errors.
    if( infile && infile->m_filename != "" && infile->m_filename != "none" ) {
      try {
        AH_INFO(ahlog::LOW) <<  " close input file: " << infile->m_filename << endl;
        ahFitsClose(infile);
      }
      catch (const exception & x) {
        setStatus(1);
        AH_INFO(ahlog::LOW) << "finalize: error while closing input file(s): " << x.what() << endl;
      }
    }
  }
} // namespace ahhxisgd

#endif // FINALIZE_C
