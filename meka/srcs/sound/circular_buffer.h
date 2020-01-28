//-----------------------------------------------------------------------------
// MEKA - circular_buffer.c
// A circular buffer with fixed (power of two) capacity - Header
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DEFINITIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// INCLUDES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------


typedef s16   CBuff_T; //The data type stored in the circular buffer.
                       //Needs to match the use case (in this case the sound sample bit depth.

//A circular buffer with heap storage of a power of two elements
struct CBuff {

  //Use unsigned integers to take advantage of automatic overflow wraparound on increment
  //So long as capacity is a power of two, wraparound will keep modular arithmetic in sync.
  u32   begin;          //The storage index of the first element in the buffer
  u32   end;            //The storage index of the end (one past the last element) of te buffer

  u32   CAPACITY;       //The max number of elements in the buff (must be a power of two)
  
  CBuff_T* elements;    //The raw storage elements of the buffer (on heap, require allocation
  
};




//A guaranteed contiguous segment of CBuff storage, to allow external callers contiguous access to a buffer segment
//Provides a raw pointer view into the underlying buffer storage region
struct CBuff_Block{
  CBuff_T* begin;       //A raw pointer to the beginning of the storage block
  CBuff_T* end;         //A raw pointer to the end (one past last element) of the block
};


//A general span of a circular buffer's storage array, representing an ordered set
//of data, which may wrap around back to the beginning of storage. To deal with any
//wraparound, a second contiguous block is provided, which may be empty.
//In an empty (zero element) span, both blocks can be empty
struct CBuff_SplitSpan{

  // No wraparound case
  //
  // |     ################        | cap_end
  //           block1                              //block2 is empty
  //
  //
  // Wraparound case
  //
  //       end      begin
  //        v        v 
  // |#######        ##############| cap_end
  //   block2            block1
  
  CBuff_Block block1; //A contiguous block at the start of the span
  CBuff_Block block2; //A second contiguous data block (possibly empty) logically at the end of the span,
                      //but which can physically be located before the start in memory

};


//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

//Create a circular buffer with heap storage of at least the requested capacity.
//The true capacity will be the closest bounding power of two, which simplifies the internal index arithmetic
CBuff CBuff_CreateCircularBuffer(const u32 requested_capacity);

//Free the Circular buffers heap storage
void CBuff_DeleteCircularBuffer(CBuff* cbuff);

//Return the number of elements in the buffer
u32 CBuff_Size(const CBuff* cbuff);

//Return the remaining free space in the buffer's underlying storage
u32 CBuff_UnusedCapacity(const CBuff* cbuff);

//Returns true if buffer size  capacity
bool CBuff_Full(const CBuff* cbuff);

//Returns true if no more elements remain in the buffer
bool CBuff_Empty(const CBuff* cbuff);

//Compute the length of a raw storage block in underlying elements
u32 CBuff_Length(const CBuff_Block* block);


//Create a raw block view into the circular buffer's storage region,
//given the start and end indices of the storage block
CBuff_Block CBuff_MakeBlock(const CBuff* cbuff, const u32 blockstart, const u32 blockend);  

//Push "count" unitialized elements onto the end of the buffer, and return a split span into the allocated space
//If there is not enough remaining storage to allocate "count" elements, no elements are added and zero
//length span is returned
CBuff_SplitSpan CBuff_PushBackSpan(CBuff* cbuff, const u32 count);

//Pop "count" elements from the beginning of the buffer, and return a split span into the freed space. Any popped
//elements are no longer members of the buffer after this call. If there are less than the requested "count"
//elements in the array, no elements are removed and a zero length span is returned
CBuff_SplitSpan CBuff_PopFrontSpan(CBuff* cbuff, const u32 count);


//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
