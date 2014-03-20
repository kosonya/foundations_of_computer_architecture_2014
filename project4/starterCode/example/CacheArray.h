///////////////////////////////////////////////////////////////////////
//
// CacheArray.h
// Copyright (C) 2005 Carnegie Mellon University
//
// Description:
// Cache array header file
//
// Revision history
// 9-9-2005, Added Prefetching Features
//
// 8-21-2005, Created by Eric Chung
//
//

#include <iostream>
#include <cassert>
#include <string>
#include <list>

#ifndef CACHEARRAY
#define CACHEARRAY

typedef unsigned char		uint8_t;
typedef unsigned int 		uint16_t;
typedef unsigned long 		uint32_t;
typedef unsigned long long 	uint64_t;


namespace nCache {

  using std::string;

  typedef uint32_t      	Set;
  typedef uint32_t 		Tag;
  typedef uint32_t		Block;
  typedef uint32_t		MemoryAddress;


  unsigned int log_base2(unsigned int num);

  enum ValidBit {
    VALID = 1,
    INVALID = 0
  };

  enum DirtyBit {
    DIRTY = 1,
    NOTDIRTY = 0
  };

  enum ReplacementPolicy {
    REPLACEMENT_LRU,
    REPLACEMENT_MRU,
    REPLACEMENT_RANDOM
  };

  enum MemoryMessageType {
    // Requests       
    PC		     = 0,
    LOAD	     = 1,
    STORE            = 2,
    EVICTDIRTY,
    PROBE,           		// Requests from lower-levels to higher up
    PROBEREPLY,

    PREFETCH,			// Messages from the Prefetcher
    NONE
  };

  enum PrefetchType {
    L2,                         // Prefetch directly into the L2
    AUX                         // Prefetch into an auxiliary structure
  };

  enum EvictType {
    ByNormal,                   // Indicates normal eviction
    ByPrefetch                  // Indicates an eviction caused by a prefetch
  };

  enum Source {
    NIL = 0,
    CPU = 1,       
    L1D = 2,
    L1I = 3,
    L2U = 4,
    SPF = 5  
  };

  struct MemoryMessage {
    MemoryMessageType 	theMemoryMessageType;
    MemoryAddress 	theMemoryAddress;
    Source		theSource;
    PrefetchType	thePrefetchType;
  };


  class CacheBlock {
    public:

      CacheBlock(void) :
        theTag(0),
	theDirtyBit(NOTDIRTY),
	theValidBit(INVALID),
	theSource(NIL)
      {}

      Tag getTag(void) { return theTag; }
      MemoryAddress getMemoryAddress(void) { return theMemoryAddress; }

      void updateDirtyBit(DirtyBit aDirtyBit) { theDirtyBit = aDirtyBit; }
      void updateValidBit(ValidBit aValidBit) { theValidBit = aValidBit; }

      DirtyBit getDirtyBit() { return theDirtyBit; }
      ValidBit getValidBit() { return theValidBit; }

      void updateTag(Tag aTag) { theTag = aTag; }
      void updateMemoryAddress(MemoryAddress aMemoryAddress) { theMemoryAddress = aMemoryAddress; }

      void updateSource(Source aSource) { theSource = aSource; }
      Source getSource() { return theSource; }

    private: 
      Tag theTag;
      MemoryAddress theMemoryAddress;       // Purely for simulation purposes
      ValidBit theValidBit;
      DirtyBit theDirtyBit;
      Source theSource;
  };


  struct LookupResult {
    bool isHit;
    int  theIndex;
  };

  class CacheSet {
    public:
      CacheSet ( const uint64_t aAssociativity );
      void updateDirtyBit(DirtyBit aDirtyBit, int aBlock); 
      void updateValidBit(ValidBit aValidBit, int aBlock);

      void updateTag ( Tag aTag, MemoryAddress aMemoryAddress, int aBlock );
      LookupResult lookupBlock( const Tag aTag );
      MemoryAddress getMemoryAddress(int aBlock); 
      DirtyBit getDirtyBit(int aBlock);
      ValidBit getValidBit(int aBlock);

      void updateSource(Source aSource, int aBlock);
      Source getSource(int aBlock);

      // LRU functions
      int getTail();
      void moveToHead(int aBlock);
      void moveToTail(int aBlock);

    private:
      CacheBlock *theBlocks;
      int theAssociativity;
      int *theMRUOrder;             // More recent values closer to the head
  };


  class CacheArray {
    public:
      CacheArray( uint64_t		aCacheSize, 
                  uint64_t		aAssociativity,
		  uint16_t		aBlockSize,
		  Source		aSource,
		  string		aCacheName ); 

      void processFrontSideIn(MemoryMessage *aMemoryMessage);
      void processBackSideIn(MemoryMessage *aMemoryMessage);

      MemoryMessage *getBackSideOut();
      MemoryMessage *getFrontSideOut();

      //-------------------------------------------------------
      // Direct manipulators in the cache (use only for special
      // purposes like prefetchers or prefetch buffers)

      bool isHit(MemoryAddress aMemoryAddress);
      void insertEntry(MemoryAddress aMemoryAddress, Source aSource, bool isDirty, bool isPrefetch);



      //--------------------------------------
      // Internal event information exposed

      MemoryAddress getForcedDirtyWritebackAddress();
      bool didForcedDirtyWriteback();

      MemoryAddress getCleanEvictionAddress();
      bool didCleanEviction();

      //---

      bool isBackSideOutReady();
      bool isFrontSideOutReady();

      void resetStats();
      void dumpStats();
      

    private:

      Set getSetIndex(MemoryAddress aMemoryAddress); 
      Tag getTag(MemoryAddress aMemoryAddress);

      void processProbe(MemoryMessage *aProbe);
      void handleVictimBlock(int victimBlock, Set aSetIndex, bool isPrefetch);

      void dbg_PrintMemoryMessage(MemoryMessage *aMemoryMessage);
      void dbg_PrintSource(Source aSource);

      uint64_t theCacheSize; 
      uint64_t theAssociativity;
      uint16_t theBlockSize;
      uint64_t theSetCount;
      Source theSource;

      ReplacementPolicy theReplacementPolicy;

      // Block
      uint16_t theBlockOffsetNumBits;
      Block theBlockOffsetMask;

      // Set
      uint16_t theSetIndexNumBits;
      Set theSetIndexMask;

      // Tag
      uint32_t theTagShift;
      Tag theTagMask;

      string theCacheName;

      CacheSet **theSets;
      std::list < MemoryMessage* > theBackSideOutputQueue;
      std::list < MemoryMessage* > theFrontSideOutputQueue;


      //---------------------------------------------------------------
      // Internal information exposed to the outside world
      // (specifically for prefetching)
      // This is so ugly, die die!

      void resetInternalFlags();

      MemoryAddress forcedDirtyWritebackAddress;
      bool theDidForcedDirtyWriteback;

      MemoryAddress cleanEvictionAddress;
      bool theDidCleanEviction;


    public:

      //---------------------------------------------------------------
      // Statistics counters

      uint64_t theNumReads;
      uint64_t theNumReadMisses;
      uint64_t theNumWrites;
      uint64_t theNumWriteMisses;

      uint64_t theNumCleanEvictions;
      uint64_t theNumDirtyWritebacks;

      uint64_t theNumProbesSent;
      uint64_t theNumProbesReceived;
      uint64_t theNumForcedEvictions;
      uint64_t theNumForcedCleanEvictions;
      uint64_t theNumForcedDirtyEvictions;

      uint64_t theNumDirtyEvictionsReceivedProcessed;
      uint64_t theNumDirtyEvictionsReceivedDropped;


      #ifdef ENABLE_PF
      uint64_t theNumForcedCleanEvictionsByPrefetch;
      uint64_t theNumForcedDirtyEvictionsByPrefetch;
      #endif

  };
}

#endif
