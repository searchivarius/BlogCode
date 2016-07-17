#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

const size_t MEM_GIGS  = 16;
const size_t QTY       = 1024; 
const size_t TotalMem  = MEM_GIGS*1024*1024*1024ull; 
const size_t BlockSize = TotalMem/QTY;

int main(int argc, char* argv[]) {
  cout << "Using the total mem of " << TotalMem/1024/1024/1024 << " GB" << endl;
  try {
    vector<char *> alloced_chunks;
    // Get every possible bit of memory
    for (size_t i = 0; i < QTY; ++i) {
      alloced_chunks.push_back(new char[BlockSize]);
      memset(alloced_chunks[i], -2, BlockSize);
    } 
    size_t totalFree = 0;

    // Free every second chunk
    for (size_t i = 0; i < QTY; i+=2) {
      totalFree += BlockSize;
      delete [] alloced_chunks[i];
    }
    // Finally try to allocate the free mem
    cout << "Allocating " << totalFree/1024/1024 << " GB" << endl;
    cout << size_t(new char[totalFree]) << endl;

  } catch (bad_alloc) {
    cerr << "bad_alloc happened!" << endl;
    return -1;
  }
  cout << "All is fine!" << endl;
  return 0;
};
