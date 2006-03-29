// $Id$
#ifndef MTRAND_H
#define MTRAND_H

/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// Mersenne Twister random number generator -- a C++ class MTRand
// Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
// Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

// The Mersenne Twister is an algorithm for generating random numbers.  It
// was designed with consideration of the flaws in various other generators.
// The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
// are far greater.  The generator is also fast; it avoids multiplication and
// division, and it benefits from caches and pipelines.  For more information
// see the inventors' web page at http://www.math.keio.ac.jp/~matumoto/emt.html

// Reference
// M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
// Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
// Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// Copyright (C) 2000 - 2003, Richard J. Wagner
// All rights reserved.                          
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. The names of its contributors may not be used to endorse or promote 
//      products derived from this software without specific prior written 
//      permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// The original code included the following notice:
//
//     When you use this, send an email to: matumoto@math.keio.ac.jp
//     with an appropriate reference to your work.
//
// It would be nice to CC: rjwagner@writeme.com and Cokus@math.washington.edu
// when you write.

// Not thread safe (unless auto-initialization is avoided and each thread has
// its own MTRand object)
	
/********************************************************************************************

>	class MTRand 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com> (adapted from original - see above)
	Created:	04/11/2005

	Purpose:	This is the ``Mersenne Twister'' random number generator MT19937, which
				generates pseudorandom integers uniformly distributed in 0..(2^32 - 1)
				starting from any odd seed in 0..(2^32 - 1). 

	SeeAlso:	http://www.math.keio.ac.jp/~matumoto/emt.html is the homepage where you can find
				out all about this algorithm.  This particular adaptation is taken from the recode
				by Martin Hinsche http://www.usf.uos.de/~mhinsch/index.en.html.

********************************************************************************************/

// Two constants for GetNextRandomNumberScaled
// Latter casting suppresses compiler warnings - the sign does not actually matter as
// it's only used for UINT32 & UINT32 subtraction
#define MTRAND_MAX 4294967296.0
#define MTRAND_MED ((UINT32)(2147483648ULL))

class MTRand : public CCObject
{
// Data
protected:
	enum { N = 624 };       // length of state vector
	enum { M = 397 };  // period parameter
	
	UINT32 state[N];   // internal state
	UINT32 *pNext;     // next value to get from state
	INT32 left;          // number of values left before reload needed

	static UINT32 lastSeed;

//Methods
public:
	MTRand( const UINT32& oneSeed );  // initialize with a simple uint32
	MTRand();  // auto-initialize with /dev/urandom or time() and clock()
	
	// Do NOT use for CRYPTOGRAPHY without securely hashing several returned
	// values together, otherwise the generator state can be learned after
	// reading 624 consecutive values.
	
	UINT32 rand();                       // integer in [0,2^32-1]
	UINT32 operator()() { return rand(); }  // same as rand()
	
	// Re-seeding functions with same behavior as initializers
	void seed( const UINT32 oneSeed );

	// returns the next random number scaled so it is between the values given
	UINT32 GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue);
	
	UINT32 GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue, UINT32 Median);

protected:
	void initialize( const UINT32 oneSeed );
	void reload();
	UINT32 hiBit( const UINT32& u ) const { return u & 0x80000000UL; }
	UINT32 loBit( const UINT32& u ) const { return u & 0x00000001UL; }
	UINT32 loBits( const UINT32& u ) const { return u & 0x7fffffffUL; }
	UINT32 mixBits( const UINT32& u, const UINT32& v ) const
		{ return hiBit(u) | loBits(v); }
	UINT32 twist( const UINT32& m, const UINT32& s0, const UINT32& s1 ) const
		{ return m ^ (mixBits(s0,s1)>>1) ^ (-(INT32)loBit(s1) & 0x9908b0dfUL); }
};


inline MTRand::MTRand( const UINT32& oneSeed )
	{ seed(lastSeed = oneSeed); }

inline MTRand::MTRand()
	{ seed(lastSeed += 2); }


inline UINT32 MTRand::rand()
{
	// Pull a 32-bit integer from the generator state
	// Every other access function simply transforms the numbers extracted here
	
	if( left == 0 ) reload();
	--left;
		
	register UINT32 s1;
	s1 = *pNext++;
	s1 ^= (s1 >> 11);
	s1 ^= (s1 <<  7) & 0x9d2c5680UL;
	s1 ^= (s1 << 15) & 0xefc60000UL;
	return ( s1 ^ (s1 >> 18) );
}


inline void MTRand::seed( const UINT32 oneSeed )
{
	// Seed the generator with a simple UINT32
	initialize(oneSeed);
	reload();
}


inline void MTRand::initialize( const UINT32 seed )
{
	// Initialize generator state with seed
	// See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	// In previous versions, most significant bits (MSBs) of the seed affect
	// only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
//	register UINT32 *s = state;
//	register UINT32 *r = state;
//	register INT32 i = 1;
//	*s++ = seed & 0xffffffffUL;
//	for( ; i < N; ++i )
//	{
//		*s++ = ( 1812433253UL * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffUL;
//		r++;
//	}

	// This is the original seeding code to ensure that the number sequences are the same
	register UINT32 x = (seed | 1U) & 0xFFFFFFFFU, *s = state;
	register INT32    j;

	for(left=0, *s++=x, j=N; --j;
		*s++ = (x*=69069U) & 0xFFFFFFFFU);
}


inline void MTRand::reload()
{
	// Generate N new values in state
	// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
	register UINT32 *p = state;
	register INT32 i;
	for( i = N - M; i--; ++p )
		*p = twist( p[M], p[0], p[1] );
	for( i = M; --i; ++p )
		*p = twist( p[M-N], p[0], p[1] );
	*p = twist( p[M-N], p[0], state[0] );

	left = N, pNext = state;
}

#endif	// MTRAND_H
