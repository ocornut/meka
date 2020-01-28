//-----------------------------------------------------------------------------
// MEKA - circular_buffer.c
// A circular buffer with fixed (power of two) capacity - Code
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DEFINITIONS
//-----------------------------------------------------------------------------

#define CBUFF_INTERNAL static //Functions not provided to the external user

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

#include "../system.h"        //Definitions of signed/unsigned integers
#include "circular_buffer.h"

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------


//Return the base 2 logarithm of the given positive number to the base 2, rounded up to the nearest integer
CBUFF_INTERNAL u8 Log2Ceil(const u32 raw_number){

  if(raw_number==0){ //Special case
    return 0;
  }

  u32 n =raw_number-1; //Subtract one to handle the case where we are exactly on a power of two

  //Keep right shifting down until the most significant bit is gone
  //This gives the power of two just above n
  u8 exponent = 0;
  while(n){
    n= n >> 1;
    exponent++;
  }

  return exponent;
  
}

//Return the power of two at or above the given integer
CBUFF_INTERNAL u32 BoundingPowerOfTwo(const u32 n){
  const u8 exponent = Log2Ceil(n);
  const u32 bounding_power = ( 1 << exponent);
  return bounding_power;

}

//Create a circular buffer with heap storage of at least the requested capacity.
//The true capacity will be the closest bounding power of two, which simplifies the internal index arithmetic
CBuff CBuff_CreateCircularBuffer(const u32 requested_capacity){

  //Check for failing cases with blunt asserts
  const u32 CAPACITY_MAX = (1<<31); //2^31
  const u32 CAPACITY_MIN = (1<<2); //2
  assert( requested_capacity <= CAPACITY_MAX);
  assert( requested_capacity >= CAPACITY_MIN);

  //Set the actual capcity to the nearest power of two at or above the requested capacity (simplifies circular buffer) 
  const u32 BoundingCapacity = BoundingPowerOfTwo(requested_capacity);
  
  CBuff cbuff = {};
  cbuff.CAPACITY = BoundingCapacity; 

  //Allocate buffer storage on heap
  cbuff.elements = new CBuff_T [cbuff.CAPACITY];
  
  return cbuff;
}

void CBuff_DeleteCircularBuffer(CBuff* cbuff){

  delete [] cbuff->elements;
}

u32 CBuff_Size(const CBuff* cbuff){
  return cbuff->end - cbuff->begin; //rely on overlow wraparound and unsigned arithmetic to do the right thing
}

u32 CBuff_UnusedCapacity(const CBuff* cbuff){
  return cbuff->CAPACITY - CBuff_Size(cbuff);
}


bool CBuff_Full(const CBuff* cbuff){
  return CBuff_Size(cbuff) >= cbuff->CAPACITY;
}

bool CBuff_Empty(const CBuff* cbuff){
  return CBuff_Size(cbuff) == 0U;
}

//Compute the length of storage block in underlying elements
u32 CBuff_Length(const CBuff_Block* block){
  return block->end - block->begin;
}

//Compute the total storage block span in underlying elements
u32 CBuff_Length(const CBuff_SplitSpan* span){
  return CBuff_Length(&span->block1) + CBuff_Length(&span->block2);
}


//Create a raw block view into the circular buffer's storage region, given the start and end indices of the storage block
CBuff_Block CBuff_MakeBlock(const CBuff* cbuff, const u32 blockstart, const u32 blockend){

  //Double check that the block allocation is valid
  assert(blockstart < cbuff->CAPACITY);
  assert(blockend   <= cbuff->CAPACITY);
  assert(blockstart <= blockend);
  
  CBuff_Block raw_view = {};
  raw_view.begin = cbuff->elements + blockstart;
  raw_view.end   = cbuff->elements + blockend;

  return raw_view;
}

//Convert a pair of buffer storage indices (or buffer indices) into a split span,
//giving two contiguous blocks of storage space
CBUFF_INTERNAL CBuff_SplitSpan CBuff_GetSplitSpan(const CBuff* cbuff, const u32 span_start, const u32 span_end){

  //First mod the indices to lie within [0, cap_end) so they are proper storage indices
  const u32 CAPACITY = cbuff->CAPACITY;
  const u32 start = span_start  % CAPACITY;
  const u32 end = span_end % CAPACITY;

  
  //Let cap_end be the end of the physical storage block
  //The split span will be one of the two forms 
  // (start,     end)     (end, end)   or
  // (start, cap_end)     (0, end)
  //depending on whether the span has wrapped around or not

  //Find data indices  (start, mid1) (mid2, end)
  const u32 cap_end = CAPACITY;
  const u32 mid1 = end < start ? cap_end : end;
  const u32 mid2 = mid1 % CAPACITY;


  const CBuff_Block block1 = CBuff_MakeBlock(cbuff, start, mid1); //{start, mid1}
  const CBuff_Block block2 = CBuff_MakeBlock(cbuff, mid2, end);   //{mid2, end}
  
  const CBuff_SplitSpan split_span = {block1, block2};

  return split_span;

}
  


//Push "count" unitialized onto the end of the buffer, and return a Span into the allocated space
//If there is not enough remaining storage to allocate "count" elements, no elements are added and zero
//length span is returned
CBuff_SplitSpan CBuff_PushBackSpan(CBuff* cbuff, const u32 count){

  const bool can_allocate = count <= CBuff_UnusedCapacity(cbuff);

  CBuff_SplitSpan allocated_span = {}; //Zero length span
  if(can_allocate){

    const u32 prev_end = cbuff->end;
    cbuff->end += count; //only reserving space, elements uninitialised

    allocated_span = CBuff_GetSplitSpan(cbuff, prev_end, cbuff->end);
  }
  
  return allocated_span;
}

//Pop "count" elements from the beginning of the buffer, and return a Span into the freed space. Any popped
//elements are no longer members of the buffer after this call. If there are less than the requested "count"
//elements in the array, no elements are removed and a zero length span is returned
CBuff_SplitSpan CBuff_PopFrontSpan(CBuff* cbuff, const u32 count){

  const bool can_free = count <= CBuff_Size(cbuff);

  CBuff_SplitSpan popped_span = {}; //Zero length span
  if(can_free){

    const u32 prev_begin = cbuff->begin;
    cbuff->begin += count;  //elements are removed from the buffer!
                            //Span points to "undefined" storage (this is not thread safe)

    popped_span = CBuff_GetSplitSpan(cbuff, prev_begin, cbuff->begin);
  }
  
  return popped_span;
}


//-----------------------------------------------------------------------------
