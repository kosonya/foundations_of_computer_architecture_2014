///////////////////////////////////////////////////////////////////////
//
// PrefetchBufferer.h
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Prefetch Buffer header file
//
// Revision history
// 9-19-2005, Eric Chung
//
//

#include <iostream>
#include <vector>

#include "CacheArray.h"

#ifndef PF_BF
#define PF_BF

using namespace nCache;

namespace nPrefetcher {

  class PrefetchBufferEntry {
    public: 
      bool theValid;
      MemoryAddress theAddress;
  };



  class DirectMapPrefetchBuffer {
    public:
      DirectMapPrefetchBuffer( uint32_t aPrefetchBufferSize, uint16_t aBlockSize );

      void insertBufferEntry(MemoryAddress aMemoryAddress);
      void deleteBufferEntry(MemoryAddress aMemoryAddress);
      bool isHit(MemoryAddress aMemoryAddress);

    private:
      MemoryAddress getBlockAddress(MemoryAddress aMemoryAddress);
      uint32_t getBufferIndex(MemoryAddress aMemoryPC);

      PrefetchBufferEntry **theBuffer;
      uint32_t		    theBufferSize;
      uint16_t		    theBlockSize;   
      uint16_t		    theBlockBits;
  };



  class FifoPrefetchBuffer {
    public:
      FifoPrefetchBuffer( uint32_t aPrefetchBufferSize, uint16_t aBlockSize );

      void insertBufferEntry(MemoryAddress aMemoryAddress);
      void deleteBufferEntry(MemoryAddress aMemoryAddress);
      bool isHit(MemoryAddress aMemoryAddress);

    private:
      MemoryAddress getBlockAddress(MemoryAddress aMemoryAddress);

      std::vector<MemoryAddress> theBuffer; 
      
      uint32_t		    theBufferSize;
      uint16_t		    theBlockSize;   
      uint16_t		    theBlockBits;
  };



  class RandomPrefetchBuffer {
    public:
      RandomPrefetchBuffer( uint32_t aPrefetchBufferSize, uint16_t aBlockSize );

      void insertBufferEntry(MemoryAddress aMemoryAddress);
      void deleteBufferEntry(MemoryAddress aMemoryAddress);
      bool isHit(MemoryAddress aMemoryAddress);

      void dumpStats();

    private:
      MemoryAddress getBlockAddress(MemoryAddress aMemoryAddress);

      //std::vector<MemoryAddress> theBuffer; 
      PrefetchBufferEntry **theBuffer;
      
      uint32_t		    theBufferSize;
      uint32_t              theCurrentBufferSize;

      uint16_t		    theBlockSize;   
      uint16_t		    theBlockBits;

      uint32_t		    theCurrRand;
      uint32_t              theMaxRand;
      uint32_t	            thePoly;
      string		    thePolyStr;

      uint64_t              theRandSum;
      uint64_t              theRandCalls;
  };

}

#endif
