///////////////////////////////////////////////////////////////////////
//
// CompetitionPrefetcher.h
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Competition Prefetcher header file
//
// Revision history
// 9-9-2005, Eric Chung
//
//

#include <iostream>

#ifndef PREFETCHER
#define PREFETCHER

#include "CacheArray.h"
#include "CacheController.h"

using namespace nCache;

namespace nPrefetcher {

  class Prefetcher {
    public:
      Prefetcher( uint16_t aBlockSize );

      void updatePrefetcher(MemoryMessageType aMemoryMessageType, 
                            MemoryAddress aAddress,
			    bool isHitL1,
			    bool isHitL2,
			    bool isHitPrefetch,
			    int numPrefetchQueueFree);

      void evictionL2(MemoryAddress aMemoryAddress, EvictType aEvictType);
      bool isPrefetchReady();
      MemoryMessage getPrefetch();

      void resetStats();
      void dumpStats();

    private:
      MemoryAddress getBlockAddress(MemoryAddress aMemoryAddress);
      void sendPrefetch(MemoryAddress aMemoryAddress, PrefetchType aPrefetchType);

      uint16_t		      theBlockSize;   
      uint16_t		      theBlockBits;

      bool		      theIsPrefetchReady;
      MemoryMessage  	      thePrefetchOutputEntry;

      bool                    isEven;

      //-----------------------------------------------------
      // Statistics counters

      uint32_t		      thePrefetchesRequested;
  };

}

#endif
