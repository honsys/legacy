#if !defined(DOHXITEST_C)
#define DOHXITEST_C const char doHXItest_rcsId_svnId[] = "$Id: doHXItest.cxx,v 1.18 2012/08/16 19:27:35 dhon Exp $"; 
/** \file Funcs. that generate internal test "data" and related activities.
    Some of tthe more general funcs. will be refactored into ahgen and ahfits.
    Others need to move to blobCells.cxx and blobBits.cxx.
    \author David B. Hon
    \date Feb-Mar-Apr 2012
*/

/// all includes needed and local function declarations -- namespace ahvbslice
/// note some of these funcs. may move into the ahfits or ahgen namespaces
/// and associated libs...
//
#include "ahtlm/ahSlice.h"
AHSLICE_H(doHXItest)

#include "ahtlm/ahTLM.h"
AHTLM_H(doHXItest)

#include "ahhxi/ahHXItyps.h"
AHHXITYPS_H(doHXItest)

// this is temporary -- need to reconsider location of some typedefs 
#include "ahhxisgd.h"

using namespace ahtlm;
using namespace ahhxisgd;

using namespace ahfits;
using namespace ahgen;
using namespace ahlog;
using namespace std;

namespace {
  const string cvsId = "$Name$ $Id$";

  std::map< int, vector<Uint16> > _hashChanCount; // key == occurrence Id and val == vector<int> active chan. count
  std::map< int, vector<Bits32> > _hashChanFlags; // key == occurrence Id and val == vector<Bits32> active chan. bitFlags
  std::map< int, vector<Uint16> > _hashPHAs; // key == occurrence Id and val == vector<int> pulse height amplitudes
  std::map< int, vector<Bits10> > _hashADCs; // key == occurrence Id and val == vector<int> pulse height analog-to-digital-conversions    

} // end private-local namespace of heplers

namespace ahhxi {
  // doxygen tag to generate diagram of func. invocations
  /// \callgraph
  int hxiSimActvChans(vector<Uint16>& chanCount, vector<Bits32>& chanFlags, int occurId = 1) {
    chanCount.clear(); chanFlags.clear();

    if( _hashChanCount.find(occurId) != _hashChanCount.end() ) chanCount = _hashChanCount[occurId];
    if( _hashChanFlags.find(occurId) != _hashChanFlags.end() ) chanFlags = _hashChanFlags[occurId];

    if( !chanCount.empty() && !chanFlags.empty() ) return (int)chanCount.size();

    if( chanCount.empty() ) {
      int maxchan =  occurId % 32;
      for( int i = 1; i <= maxchan; ++i ) chanCount.push_back(i);
    }
    // ensure chanIds and flagIds are congruent
    chanFlags.clear();
    int asic_cnt = (int)chanCount.size();
    for( int i = 1; i <= asic_cnt; ++i ) {
      Bits32 bits;
      for( int b = 0; b < i; ++b ) bits.set(b);
      chanFlags.push_back(bits);
    }
      
    return asic_cnt;
  }

  int hxiSimPHAs(map< int, vector<Uint16> >& pha, map<int, vector<Bits10> >& adc, int actvchan) {
    pha.clear(); adc.clear();
    return (int)pha.size();
  }

  // doxygen tag to generate diagram of func. invocations
  /// \callgraph
  int hxiSimOutputColCells(vector<string>& col_names, vector<int>& cell_sizes, HXISF& hxisf,
                           vector<Uint16>& chanCounts, vector<Bits32>& chanFlags, 
                           map<int, vector<Uint16> >& pha, map<int, vector<Bits10> >& adc,
                           int occurId) {
  // need a variety of test colunms, perhaps identified by occurent Id ...
  // simOccurrence hash key == occurrence Id and val == vector<Bits32>& actvchan,
  // int hxiTestOutputColCells(vector<string>& col_names, vector<int>& cell_sizes, HXISF& hxisf, int hitcnt, int actvchan, int occurId) {
    // hard-code default test values for 26 column hxi sff
    col_names.clear() ; cell_sizes.clear();
    int colcount = 26;
    // default initializer of stuct init does not perfom slice alloc, but does provide lenghts
    bool alloc_slice = hxiTLMof(hxisf); // so alloc_slice return == false
    AH_INFO(ahlog::LOW) << "> " << alloc_slice << " ... allocating and setting test bit-buffs ..." << endl;

    int hitcnt = hxiSimActvChans(chanCounts, chanFlags, occurId);
    int actvchan = 0; // hxiSimPHAs(pha, adc, chanCounts[asic_idx]); // hxiSimPHAs(pha, adc, occurId);
    int totalchan = 0;
    for( int i = 0; i < hitcnt; ++i ) totalchan += chanCounts[i];

    // col1 is an asci text descrition string -- a slice that simply uses c++ string type:
    // this column is temporary and strictly for use in development and test
    // need a vector<int> actvchan to provide actv chans per asic rather than this single value:
    string describe = describeOccurrence("HXI", hitcnt, totalchan, occurId);
    hxisf.col01.setSlice(describe);
    // name of first column -- which should contain a simple example of FFF blob data
    col_names.push_back(hxisf.col01.name);
    cell_sizes.push_back((int) hxisf.col01.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[0] << ", size: " << cell_sizes[0]
         << ", val: " << (int)hxisf.col01.bufptr[0] << endl;
 
    vector<double> time(1, occurId + 2012.40);
    hxisf.col02.setSlice(time); 
    col_names.push_back(hxisf.col02.name);
    cell_sizes.push_back((int) hxisf.col02.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[1] << ", size: " << cell_sizes[1]
         << ", val: " << (int)hxisf.col02.bufptr[0] << endl;

    vector<double> stime(1, occurId + 2012.32);
    hxisf.col03.setSlice(stime); 
    col_names.push_back(hxisf.col03.name);
    cell_sizes.push_back((int) hxisf.col03.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[2] << ", size: " << cell_sizes[2]
         << ", val: " << (int)hxisf.col03.bufptr[0] << endl;

    string tlm_astext = "20byte_CCSDS_PktHdr";
    TLMbyteVec ccsds_pkthdr; setByteTLMVecFromText(ccsds_pkthdr, tlm_astext);
    hxisf.col04.setSlice(ccsds_pkthdr); 
    col_names.push_back(hxisf.col04.name);
    cell_sizes.push_back((int) hxisf.col04.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[3] << ", size: " << cell_sizes[3]
         << ", val: " << (int)hxisf.col04.bufptr[0] << endl;

    vector<Uint32> time_indicator(1, occurId + 2012); 
    hxisf.col05.setSlice(time_indicator); 
    col_names.push_back(hxisf.col05.name);
    cell_sizes.push_back((int) hxisf.col05.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[4] << ", size: " << cell_sizes[4]
         << ", val: " << (int)hxisf.col05.bufptr[0] << endl;

    vector<Uint32> occurrenceId(1, occurId); 
    hxisf.col06.setSlice(occurrenceId); 
    col_names.push_back(hxisf.col06.name);
    cell_sizes.push_back((int) hxisf.col06.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[5] << ", size: " << cell_sizes[5]
         << ", val: " << (int)hxisf.col06.bufptr[0] << endl;

    vector<Uint32> localtime(1, occurId + 2012); 
    hxisf.col07.setSlice(localtime); 
    col_names.push_back(hxisf.col07.name);
    cell_sizes.push_back((int) hxisf.col07.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[6] << ", size: " << cell_sizes[6]
         << ", val: " << (int)hxisf.col07.bufptr[0] << endl;

    tlm_astext = "00000001";
    TLMbyteVec seuflag(1, 0); setBitTLMVecFromText(seuflag, tlm_astext);
    hxisf.col08.setSlice(seuflag); 
    col_names.push_back(hxisf.col08.name);
    cell_sizes.push_back((int) hxisf.col08.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[7] << ", size: " << cell_sizes[7]
         << ", val: " << hxisf.col08.bufptr[0] << endl;

    tlm_astext = "00000011";
    TLMbyteVec lenchkflag(1, 0); setBitTLMVecFromText(lenchkflag, tlm_astext);
    hxisf.col09.setSlice(lenchkflag); 
    col_names.push_back(hxisf.col09.name);
    cell_sizes.push_back((int) hxisf.col09.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[8] << ", size: " << cell_sizes[8]
         << ", val: " << (int)hxisf.col09.bufptr[0] << endl;

    tlm_astext = "00000111";
    TLMbyteVec trig0(1, 0); setBitTLMVecFromText(trig0, tlm_astext);
    hxisf.col10.setSlice(trig0); 
    col_names.push_back(hxisf.col10.name);
    cell_sizes.push_back((int) hxisf.col10.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[9] << ", size: " << cell_sizes[9]
         << ", val: " << hxisf.col10.bufptr[0] << endl;

    tlm_astext = "00001111";
    TLMbyteVec trigpat(1, 0); setBitTLMVecFromText(trigpat, tlm_astext);
    hxisf.col11.setSlice(trigpat); 
    col_names.push_back(hxisf.col11.name);
    cell_sizes.push_back((int) hxisf.col11.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[10] << ", size: " << cell_sizes[10]
         << ", val: " << (int)hxisf.col11.bufptr[0] << endl;
    
    tlm_astext = "00011111";
    TLMbyteVec precisehitpat(1, 0); setBitTLMVecFromText(precisehitpat, tlm_astext);
    hxisf.col12.setSlice(precisehitpat); 
    col_names.push_back(hxisf.col12.name);
    cell_sizes.push_back((int) hxisf.col12.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[11] << ", size: " << cell_sizes[11] 
         << ", val: " << (int)hxisf.col12.bufptr[0] << endl;

    tlm_astext = "00111111";
    TLMbyteVec fasthitpat(1, 0); setBitTLMVecFromText(fasthitpat, tlm_astext);
    hxisf.col13.setSlice(fasthitpat); 
    col_names.push_back(hxisf.col13.name);
    cell_sizes.push_back((int) hxisf.col13.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[12] << ", size: " << cell_sizes[12]
         << ", val: " << (int)hxisf.col13.bufptr[0] << endl;

    vector<Uint32> livetime(1, 2014); 
    hxisf.col14.setSlice(livetime); 
    col_names.push_back(hxisf.col14.name);
    cell_sizes.push_back((int) hxisf.col14.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[13] << ", size: " << cell_sizes[13]
         << ", val: " << (int)hxisf.col14.bufptr[0] << endl;
 
    TLMbyteVec numhits(1, hitcnt);
    hxisf.col15.setSlice(numhits); 
    col_names.push_back(hxisf.col15.name);
    cell_sizes.push_back((int) hxisf.col15.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[14] << ", size: " << cell_sizes[14]
         << ", val: " << (int)hxisf.col15.bufptr[0] << endl;

    TLMbyteVec raw_asic_data; // need a more useful set of values for more complete testing (later)
    // genTextTLM4Test(tlm_astext) ; setByteTLMVecFromText(raw_asic_data, tlm_astext)
    //tlm_astext = "aabbccddeeffgg"; // 7 asics, 2 channels/per-asic -- keep it simple 
    Bits32 chanflags;
    ahtlm::genTLM4Test(tlm_astext, hitcnt, actvchan, chanflags); // need less simple version ala vector<int>& actvchan
    setByteTLMVecFromText(raw_asic_data, tlm_astext);
    hxisf.col16.setSlice(raw_asic_data); 
    col_names.push_back(hxisf.col16.name);
    cell_sizes.push_back((int) hxisf.col16.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[15] << ", size: " << cell_sizes[15]
         << ", val: " << (int)hxisf.col16.bufptr[0] << endl;

    // the commented-out funcs. are HXI specific test (bit) pattern generators
    // signature may change and/or be augmented...
 
    vector<Uint32> asic_hdrs(hitcnt, 17); // hxiAsicHdrs(asic_hdrs, raw_asic_data)
    hxisf.col17.setSlice(asic_hdrs); 
    col_names.push_back(hxisf.col17.name);
    cell_sizes.push_back((int) hxisf.col17.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[16] << ", size: " << cell_sizes[16]
         << ", val: " << (int)hxisf.col17.bufptr[0] << endl;

    TLMbyteVec orig_asicIds(hitcnt, 18); // hxiOrigAsicIds(orig_asicIds, raw_asic_data);
    hxisf.col18.setSlice(orig_asicIds); 
    col_names.push_back(hxisf.col18.name);
    cell_sizes.push_back((int) hxisf.col18.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[17] << ", size: " << cell_sizes[17]
         << ", val: " << (int)hxisf.col18.bufptr[0] << endl;

    TLMbyteVec remapped_asicIds(hitcnt, 19); // hxiRemapAsicIds(remapped_asicIds, raw_asic_data);
    hxisf.col19.setSlice(remapped_asicIds); 
    col_names.push_back(hxisf.col19.name);
    cell_sizes.push_back((int) hxisf.col19.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[18] << ", size: " << cell_sizes[18]
         << ", val: " << (int)hxisf.col19.bufptr[0] << endl;

    vector<Uint32> orig_chanFlags(hitcnt, 20); // hxiOrigChanBitFlags(orig_chanIds, raw_asic_data);
    hxisf.col20.setSlice(orig_chanFlags); 
    col_names.push_back(hxisf.col20.name);
    cell_sizes.push_back((int) hxisf.col20.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[19] << ", size: " << cell_sizes[19]
         << ", val: " << (int)hxisf.col20.bufptr[0] << endl;

    vector<Uint16> remapped_chanIds(totalchan, 21); // hxiRemapChanIds(remapped_chanIds, raw_asic_data)
    hxisf.col21.setSlice(remapped_chanIds);
    col_names.push_back(hxisf.col21.name);
    cell_sizes.push_back((int) hxisf.col21.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[20] << ", size: " << cell_sizes[20]
         << ", val: " << (int)hxisf.col21.bufptr[0] << endl;

    TLMbyteVec actvchan_bvec(hitcnt, 22); // hxiActiveChans(actvchan_bvec, raw_asic_data);
    hxisf.col22.setSlice(actvchan_bvec); 
    col_names.push_back(hxisf.col22.name);
    cell_sizes.push_back((int) hxisf.col22.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[21] << ", size: " << cell_sizes[21]
         << ", val: " << (int)hxisf.col22.bufptr[0] << endl;

    vector<Uint16> ref_chans(hitcnt, 23); // hxiRefChans(ref_chans, raw_asic_data)
    hxisf.col23.setSlice(ref_chans);
    col_names.push_back(hxisf.col23.name);
    cell_sizes.push_back((int) hxisf.col23.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[22] << ", size: " << cell_sizes[22]
         << ", val: " << (int)hxisf.col23.bufptr[0] << endl;

    vector<Uint16> comn_noise(hitcnt, 24); // hxiNoiseChans(comn_noise, raw_asic_data)
    hxisf.col24.setSlice(comn_noise);
    col_names.push_back(hxisf.col24.name);
    cell_sizes.push_back((int) hxisf.col24.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[23] << ", size: " << cell_sizes[23]
         << ", val: " << (int)hxisf.col24.bufptr[0] << endl;

    vector<Uint16> raw_adc_pulse_heights(totalchan, 25); // hxiRawAdcPulses(raw_adc_pulse_heights, raw_asic_data);
    hxisf.col25.setSlice(raw_adc_pulse_heights); 
    col_names.push_back(hxisf.col25.name);
    cell_sizes.push_back((int) hxisf.col25.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[24] << ", size: " << cell_sizes[24]
         << ", val: " << (int)hxisf.col25.bufptr[0] << endl;

    vector<Uint16> cal_adc_pulse_heights(totalchan, 26); // hxiCalAdcPulses(cal_adc_pulse_heights, raw_asic_data);
    hxisf.col26.setSlice(cal_adc_pulse_heights); 
    col_names.push_back(hxisf.col26.name);
    cell_sizes.push_back((int) hxisf.col26.size());
    AH_INFO(ahlog::LOW) << " column: " << col_names[25] << ", size: " << cell_sizes[25]
         << ", val: " << (int)hxisf.col26.bufptr[0] << endl;

    return colcount;
  }

} // end namespace ahhxi

#endif // DOHXITEST_C
