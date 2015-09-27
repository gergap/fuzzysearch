Approximate string matching in a tags file
==========================================

Algorithm: When the tagfile is loaded for each tagname a lettermask is computed.
This is a bitmask where for each letter "abc..." a bit is set if it occurs in
the string.
This is used for prefiltering strings.

Step 1: Filtering
For a search string 's' the lettermask is computed. Then the taglist is filtered
using the precomputed lettermasks which are stored with each tag. This scales 
with O(n), but is very fast, because it actually only compares uint32 values
using bitwise AND operator. Iterating over an array of int is very fast due to
lots of cache hits.

Step 2: The tags containing the requested letters get score using a special
string metric for approximate string matching. See comments in metric.c.
The operations scales with O(n*n'), where n and n' are the length of the
compared strings.
The implementation takes core of being fast by avoiding memory allocations
and string copies.

Step 3: The computed score is added to a short list of result with highest scores.
This list is sorted by score and limited to 10 elements currently.
This implementation is stupid at the moment, but OK for such short lists.

Memory: This implementation only requires the memory of all tagnames + 
1 uint32_t per tag + one list of struct listel (2 ints) which are limited to 10 elements.
So this is very resource saving and fast.


