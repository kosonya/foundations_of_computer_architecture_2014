///////////////////////////////////////////////////////////////////////
//
// Wiring.cpp
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Wiring for connecting instantiated cache modules together
//
// Revision history
// 8-21-2005, Eric Chung
//
//

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstring>
#include <stdio.h>

#include "PrefetchBuffer.h"
#include "CacheController.h"
#include "CacheArray.h"


#ifdef STRIDED
#include "StridePrefetcher.h"
#endif

#ifdef COMPETITION
#include "CompetitionPrefetcher.h"
#endif

#ifdef EXAMPLE
#include "ExamplePrefetcher.h"
#endif


#define PF_DELAY 20

using namespace std;
using namespace nCache;
using namespace nPrefetcher;

int main(int argc, char *argv[]) {
  assert(argc < 3);

  //--------------------------------------
  // 0. Read configuration file

  char configbuffer[256];
  char *configFileName = "config.default";

  if(argc == 2) configFileName = argv[1];
  else if(argc == 1) configFileName = "config.default";
  else {
    cout << "Neither config.default or other config file specified" << endl;
    return 0;
  }

  ifstream configstream(configFileName);
  if(!configstream.is_open()) {
    cout << "Bad config file" << endl;      
    return 0;
  }

  int L1Dsize, L1Dblocksize, L1Dassoc, L1Isize, L1Iblocksize, L1Iassoc, L2Usize, L2Ublocksize, L2Uassoc, resetStatistics;

  configstream.getline(configbuffer, 100); assert(!configstream.eof());
  sscanf(configbuffer, "%d %d %d", &L1Isize, &L1Iblocksize, &L1Iassoc);

  configstream.getline(configbuffer, 100); assert(!configstream.eof());
  sscanf(configbuffer, "%d %d %d", &L1Dsize, &L1Dblocksize, &L1Dassoc);

  configstream.getline(configbuffer, 100); assert(!configstream.eof());
  sscanf(configbuffer, "%d %d %d", &L2Usize, &L2Ublocksize, &L2Uassoc);

  configstream.getline(configbuffer, 100); 
  sscanf(configbuffer, "%d", &resetStatistics);

  int thePrefetchQueueSize = 32;
  int thePrefetchDelay = PF_DELAY;
  int thePrefetchBufferSize = AUX_SIZE;

  // Instantiate the cache hierarchy
  CacheController theCacheController( L1Dsize, L1Dblocksize, L1Dassoc, L1Isize, L1Iblocksize, L1Iassoc, L2Usize, L2Ublocksize, L2Uassoc, thePrefetchQueueSize, thePrefetchDelay, thePrefetchBufferSize);


  // Instantiate the prefetcher, if there is one
  #ifdef ENABLE_PF
  Prefetcher thePrefetcher( L2Ublocksize );
  #endif



  //--------------------------------------
  // 1. Read trace file

  char buffer[31];
  unsigned long long counter = 0;

  MemoryMessageType aMemoryMessageType;
  MemoryAddress aMemoryAddress;
  MemoryAddress theLastPC = 0;

  // Try to make this as fast as possible

  int rc = scanf("%d %x", &aMemoryMessageType, &aMemoryAddress); 
  while(rc == 2) {
    if(counter == resetStatistics) {
      theCacheController.resetAllStats();
      #ifdef ENABLE_PF
      thePrefetcher.resetStats();
      #endif
    }

    //----------------------------------------------------------
    // Create a memory message from the "CPU"

    MemoryMessage *aMemoryMessage = new MemoryMessage; 
    aMemoryMessage->theSource = CPU;
    aMemoryMessage->theMemoryAddress = aMemoryAddress;
    aMemoryMessage->theMemoryMessageType = aMemoryMessageType;

    #ifdef DEBUG
    cout.flags(ios::dec);
    cout << "Processing reference " << counter << endl;
    cout.flags(ios::hex);
    #endif

    theCacheController.processMemoryMessage(aMemoryMessage);

    delete aMemoryMessage;

    #ifdef ENABLE_PF
    if(theCacheController.didNormalEvict()) {
      thePrefetcher.evictionL2(theCacheController.getNormalEvictAddress(), ByNormal);
    }

    if(theCacheController.didPrefetchEvict()) {
      thePrefetcher.evictionL2(theCacheController.getPrefetchEvictAddress(), ByPrefetch);
    }

    thePrefetcher.updatePrefetcher(aMemoryMessageType,
     				   aMemoryAddress,
				   theCacheController.isHitL1(),
				   theCacheController.isHitL2(),
				   theCacheController.isHitPrefetch(),
				   theCacheController.getNumPrefetchQueueFree()
				  );
  

    if(thePrefetcher.isPrefetchReady()) {
      MemoryMessage aPrefetchRequest = thePrefetcher.getPrefetch();

      MemoryMessage *aPrefetch = new MemoryMessage;
      aPrefetch->theMemoryMessageType = aPrefetchRequest.theMemoryMessageType;
      aPrefetch->theMemoryAddress = aPrefetchRequest.theMemoryAddress;
      aPrefetch->theSource = aPrefetchRequest.theSource;
      aPrefetch->thePrefetchType = aPrefetchRequest.thePrefetchType;

      theCacheController.processMemoryMessage(aPrefetch);

    }
    #endif


    ++counter;
    if((counter % DUMP_INTERVAL) == 0) {
      cout.flags(ios::dec);
      cout << endl;
      cout << "-------------------------------------------------------------------------------" << endl;
      cout << "Dumping stats at " << counter << " references" << endl;
      theCacheController.dumpAllStats();
      #ifdef ENABLE_PF
      thePrefetcher.dumpStats();
      #endif
    }
    rc = scanf("%d %x", &aMemoryMessageType, &aMemoryAddress); 
  }

  cout.flags(ios::dec);
  cout << "-------------------------------------------------------------------------------" << endl;
  cout << "Dumping final stats at " << counter << " references" << endl;
  theCacheController.dumpAllStats();
  #ifdef ENABLE_PF
  thePrefetcher.dumpStats();
  #endif

  return 0;
}


