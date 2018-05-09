#if !defined(AHBITS_H)
#define AHBITS_H(arg) const char arg##AHBITS_rcsId_svnId[] = "$Id: ahBits.h,v 1.9 2012/08/16 19:42:07 dhon Exp $"; 

/** 
 * \file ahBits.h
 * \brief Astro-H HXI-SGD Telemetry (TLM) Bitset typedefs, structs, and func. defs.
 * \author David B. Hon
 * \date $Date: 2012/08/16 19:42:07 $
 *
 * Provides a variety of bit-field related functionality in support of HXI and SGD
 * telemetry (TLM) processing via extensions of the C++ stdlib bitset and other vector types. 
 * 
 */

/// make use of c/c++ stdlib items:
#include <cstdlib>
#include <cmath>
#include <bitset>
#include <map>
#include <vector>
#include <sstream>
#include <string>

#include "ahlog/ahlog.h"

using namespace ahlog;
using namespace std;

namespace ahtlm {

/** \addtogroup mod_ahtlm
 *  @{
 */

/// bit-sets (flags and such) for time and hit counts are then followed by raw data

typedef unsigned long long Uint64;
typedef unsigned int       Uint32;
typedef unsigned short     Uint16;
typedef unsigned char      Uint8;
typedef unsigned char      Ubyte;

// a general purpose arbitrary size bit field/flag:
typedef std::vector<bool> BitsAny;
typedef std::map< int, BitsAny > BitHash;
//
// these are fixed-size bit flag types:
typedef std::bitset<1> Bits1;
typedef std::bitset<2> Bits2;
typedef std::bitset<3> Bits3;
typedef std::bitset<4> Bits4;
typedef std::bitset<5> Bits5;
typedef std::bitset<6> Bits6;
typedef std::bitset<8> Bits8;
typedef std::bitset<10> Bits10;
typedef std::bitset<16> Bits16;
typedef std::bitset<24> Bits24;
typedef std::bitset<31> Bits31;
typedef std::bitset<32> Bits32;
typedef std::bitset<48> Bits48;
typedef std::bitset<64> Bits64;
typedef std::bitset<96> Bits96;

typedef std::bitset<1280> Bits1280; // 32*40 (hxi max chan*asic)
typedef std::bitset<39936> Bits39936; // 64*624 (sgd max chan*asic)

// all telemtry (TLM) is 16 bit aligned 
typedef std::map< int, Bits16 > Bit16Hash; 

typedef Bits32 HXIChanFlagsAsic;
typedef Bits1280 HXIChanFlagsOccur;
typedef std::map< int, HXIChanFlagsAsic> HXIChanFlagsHash;

typedef Bits64 SGDChanFlagsAsic;
typedef Bits39936 SGDChanFlagsOccur;
typedef std::map< int, SGDChanFlagsAsic> SGDChanFlagsHash;

/// return number of set bits
int remapOccurrenceBitFlags(HXIChanFlagsOccur& remapflags, const HXIChanFlagsHash& origchans);

int remapOccurrenceBitFlags(SGDChanFlagsOccur& remapflags, const SGDChanFlagsHash& origchans);

//
//
///////////////////////////////////// bitset tranformation and pretty print funcs.
//

/// \remarks Typedefs and functions in ahBits.h for multiple bits -- bitset <--> vector<bool> conversion, 
/// (these loop over the bool vector and bitset [] by index) betsets: 1, 2, 3, 4, 5, 6, 8, 24, 31, 32, 64,
/// and perhaps more.
/// The fancy chart and the april 2012 initial draft fits template indicated a variety of 
/// different length bit-fields. AhBits.h provides a collection host of specialized bitset functions 
/// which support all the fixed-length bit-fields found so far in astro-h hxi & sgd docs.

/// \brief Clears or sets bit-field elements of a fixed-length bitset according to bools provided
/// by variable-length vector of bools.
/// \param[in] const BitsAny& -- arbitray length vector of bools (passed by reference).
/// \param[in] int idx -- a start index (offset) into the bool vector with default == 0.
/// \param[out] HXIChanFlagsOccur& -- bitset<32> fixed length "Bits32" typedef (passed by reference).
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(HXIChanFlagsOccur& b, const BitsAny& bv, int idx= 0);

/// \brief Sets elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const HXIChanFlagsOccur& -- bitset<32> fixed length "Bits32" typedef (passed by reference).
/// \param[in] int idx -- a start index (offset) into the bool vector with default == 0.
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
bool setBitsAny(BitsAny& bv, const HXIChanFlagsOccur& b, int idx= 0);

//
// while HXI ASICs have 32 channels, SGD ASICs have 64 ...
//

/// \brief Clears or sets bit-field elements of a fixed-length bitset according to bools provided 
/// by variable-length vector of bools.
/// \param[in] const BitsAny -- arbitray length vector of bools (passed by reference).
/// \param[in] int idx -- a start index (offset) into the bool vector with default == 0 
/// \param[out] SHDChanFlagsOccur& -- bitset<64> fixed length "Bits64" typedef (passed by reference).
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(SGDChanFlagsOccur& b, const BitsAny& bv, int idx= 0);

/// \brief Sets elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const SGDChanFlagsOccur& -- bitset<64> fixed length "Bits64" typedef (passed by reference).
/// \param[in] int idx -- a start index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
bool setBitsAny(BitsAny& bv, const SGDChanFlagsOccur& b, int idx= 0);

//
// the collection of known HXI and SGD TLM bit-fields (fixed-length bitsets) ... 
//

//
// 1 bit
//
/// \brief Clears or sets single bit-field element of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits1& b -- bitset<1> fixed length "Bits1" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits1& b, const BitsAny& bv, int idx= 0);

/// \brief Sets single element of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits1& -- bitset<1> fixed length "Bits1" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
bool setBitsAny(BitsAny& bv, const Bits1& b, int idx= 0);

//
// 2 bits
//
/// \brief Clears or sets two bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits2& b -- bitset<2> fixed length "Bits2" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits2& b, const BitsAny& bv, int idx= 0);

/// \brief Sets two elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits2& -- bitset<2> fixed length "Bits2" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits2& b, int idx= 0);

//
// 3 bits
//
/// \brief Clears or sets three bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits3& b -- bitset<3> fixed length "Bits3" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits3& b, const BitsAny& bv, int idx= 0);

/// \brief Sets three elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits3& -- bitset<3> fixed length "Bits3" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits3& b, int idx= 0);

//
// 4 bits
//
/// \brief Clears or sets four bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits4& b -- bitset<4> fixed length "Bits4" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits4& b, const BitsAny& bv, int idx= 0);

/// \brief Sets four elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits4& -- bitset<4> fixed length "Bits4" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits4& b, int idx= 0); 

//
// 5 bits
//
/// \brief Clears or sets five bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits5& b -- bitset<5> fixed length "Bits5" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits5& b, const BitsAny& bv, int idx= 0);

/// \brief Sets five elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits5& -- bitset<5> fixed length "Bits5" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits5& b, int idx= 0); 

//
// 6 bits
//
/// \brief Clears or sets six bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits6& b -- bitset<6> fixed length "Bits6" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits6& b, const BitsAny& bv, int idx= 0);

/// \brief Sets six elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits6& -- bitset<6> fixed length "Bits6" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits6& b, int idx= 0); 

//
// 8 bits
//
/// \brief Clears or sets eight bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits8& b -- bitset<8> fixed length "Bits8" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits8& b, const BitsAny& bv, int idx= 0);

/// \brief Sets eight elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits8& -- bitset<8> fixed length "Bits8" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits8& b, int idx= 0); 

//
// 10 bits
//
/// \brief Clears or sets ten bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits10& b -- bitset<10> fixed length "Bits10" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits10& b, const BitsAny& bv, int idx= 0);

/// \brief Sets ten elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits10& -- bitset<10> fixed length "Bits10" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits10& b, int idx= 0); 

//
// 16 bits
//
/// \brief Clears or sets five bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits16& b -- bitset<16> fixed length "Bits16" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits16& b, const BitsAny& bv, int idx= 0);

/// \brief Sets 16 elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits16& -- bitset<16> fixed length "Bits16" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits16& b, int idx= 0); 

//
// 24 bits
//
/// \brief Clears or sets 24 bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits24& b -- bitset<24> fixed length "Bits24" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits24& b, const BitsAny& bv, int idx= 0);

/// \brief Sets 24 elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits24& -- bitset<24> fixed length "Bits24" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits24& b, int idx= 0); 

//
// 31 bits
//
/// \brief Clears or sets 31 bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits31& b -- bitset<31> fixed length "Bits31" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits31& b, const BitsAny& bv, int idx= 0);

/// \brief Sets 31 elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits31& -- bitset<31> fixed length "Bits31" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits31& b, int idx= 0); 

//
// 32 bits
//
/// \brief Clears or sets 32 bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits32& b -- bitset<32> fixed length "Bits32" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits32& b, const BitsAny& bv, int idx= 0);

/// \brief Sets 32 elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits32& -- bitset<32> fixed length "Bits32" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits32& b, int idx= 0); 

//
// 48 bits
//
/// \brief Clears or sets 48 bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits5& b -- bitset<5> fixed length "Bits5" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits48& b, const BitsAny& bv, int idx= 0);

/// \brief Sets 48 elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits48& -- bitset<48> fixed length "Bits48" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits48& b, int idx= 0);

//
// 64 bits
//
/// \brief Clears or sets 64 bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits64& b -- bitset<64> fixed length "Bits64" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits64& b, const BitsAny& bv, int idx= 0);

/// \brief Sets 64 elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits64& -- bitset<64> fixed length "Bits64" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits64& b, int idx= 0);

//
// 96 bits
//
/// \brief Clears or sets 96 bit-field elements of a fixed-length bitset according to bool provided 
/// by variable-length vector of bools, and the indicated vector index.
/// \param[in] const BitsAny -- arbitray length vector of bools
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] Bits96& b -- bitset<96> fixed length "Bits96" typedef.
/// \return bool indicates success or failure of get (perhaps an empty vector was supplied):
bool getBitSet(Bits96& b, const BitsAny& bv, int idx= 0);

/// \brief Sets 96 elements of a variable-length vector of bools according to bits provided
/// by a fixed-length bitset.
/// \param[in] const Bits96& -- bitset<96> fixed length "Bits96" typedef (passed by reference).
/// \param[in] int idx -- an index (offset) into the bool vector with default == 0 
/// \param[out] BitsAny& -- arbitray length vector of bools (passed by reference).
/// \return bool indicates success or failure of get (perhaps no bits were set in bitset)
int setBitsAny(BitsAny& bv, const Bits96& b, int idx= 0);


/// \remarks funcs for multiple bits -- bitset <--> vector<bool> display (pretty print) 

/// \brief Formats a printable text string to display the contents of TLM bit-fields
/// \param[in] Bits8 -- bitset<8> fixed length Bits8.
/// \param[in] string prefix -- optional prefix to insert before bit pattern text.
/// \param[out] string& line -- single line of text for printout.
/// \return length of text.
int bitText(string& line, const Bits8& b, string prefix= "");

/// \brief Formats a printable text string to display the contents of TLM bit-fields
/// \param[in] BitsAny -- vector<bool> variable length bit-field.
/// \param[in] string prefix -- optional prefix to insert before bit pattern text.
/// \param[out] string& line -- single line of text for printout.
/// \return length of text.
int bitText(string& line, const BitsAny& bv, string prefix= "");

/// \brief Formats a printable text string to display the contents of TLM bit-fields
/// \param[in] BitsAny -- vector<bool> variable length bit-field.
/// \param[in] int -- optional stride-width (number of bits) to format into printout text line.
/// \param[in] string prefix -- optional prefix to insert before bit pattern text.
/// \param[out] vector<string>& printbuff -- list of lines of text for printout.
/// \return number of lines of text (size of vector list).
int bitText(vector<string>& printbuff, const BitsAny& bv, int stride= 8, string prefix= "");

/** @} */

} // namespace ahtlm

#endif // AHBITS_H

