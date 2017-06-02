/*
 * Checking a very silly table approach.
 * Taken from Daniel's blog and subsequently modified.
 *
 * https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2013/12/23/bitextract.cpp
 *
 * See details in Daniel's blog:
 * http://lemire.me/blog/archives/2013/12/23/even-faster-bitmap-decoding/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vector>
#include <iostream>
#include <cassert>

 class WallClockTimer
  {
  public:
      struct timeval t1, t2;
  public:
      WallClockTimer() : t1(), t2(){ gettimeofday(&t1,0); t2 = t1; }
      void reset() {gettimeofday(&t1,0); t2 = t1;}
      int elapsed() { return (t2.tv_sec * 1000 + t2.tv_usec / 1000) - (t1.tv_sec * 1000 + t1.tv_usec / 1000); }
      int split() { gettimeofday(&t2,0); return elapsed(); }
  };

using namespace std;

    int bitscan0(uint64_t *bitmap, int bitmapsize, int *out) {
      int counter = 0;
      for(int k = 0; k<bitmapsize; ++k) {
        for(int c = 0; c<64; ++c) {
         if (((1l << c) & bitmap[k]) != 0) {
            out[counter++] = k*64+c;
          }
        }
      }
      return counter;
    }  
    
    
    
    
  // inspired by http://www.steike.com/code/bits/debruijn/	
  int bitscan1(uint64_t *bitmap, int bitmapsize, int *out) {
	  int pos = 0;
      for(int k = 0; k < bitmapsize; ++k) {
      	 uint64_t bitset = bitmap[k];
         while (bitset != 0) {
           uint64_t t = bitset & -bitset;
           out[pos++] = k * 64 + _mm_popcnt_u32 (t-1);// __builtin_popcountl (t-1);
           bitset ^= t;
         }
      }
      return pos;
  }
  
  
  int bitscan2(uint64_t *bitmap, int bitmapsize, int *out) {
	  int pos = 0;
      for(int k = 0; k < bitmapsize; ++k) {  
        uint64_t data = bitmap[k];
        while (data != 0) {
          int ntz =  __builtin_ctzl (data);
          out[pos++] = k * 64 + ntz;
          data ^= (1l << ntz);
        }
      }
      return pos;
  }
  
   int bitscan3(uint64_t *bitmap, int bitmapsize, int *out) {
        int pos = 0;
        for (int k = 0; k < bitmapsize; ++k) {
            const uint64_t myword = bitmap[k];
            for(int offset = 0; offset<64;++offset) {
                    if((myword >> offset) == 0) break;
                    offset+=__builtin_ctzl((myword >> offset));
                    out[pos++]=64 * k + offset;
                }
        }
        return pos;
    }
   
   int table[256][8];
   int table_qty[256];
  
   int bitscan4(uint64_t *bitmap, int bitmapsize, int *out) {
        int pos = 0;
        for (int k = 0; k < bitmapsize; ++k) {
            uint64_t myword = bitmap[k];
            int start = 64 * k;
            // myword is unsigned, hence, eventually it becomes zero by shifting >>
            for (; myword ; start +=8, myword >>= 8) {
              int c = myword & 0xff; 
              int qty  = table_qty[c];
              int *p = table[c];
              for (int j = 0; j < qty; ++j) {
                out[pos++]= start + *p++;
              }
            }
        }
        return pos;
    }
   
  
  
  
  int main() {
      for (int i =0;i < 256;++i) {
        int qty = 0;
        for(int j = i,p=0; j; j>>=1,++p) {
          if (j&1) table[i][qty++] = p;
        }
        table_qty[i] = qty;
/*
        cout << "i: " <<i << " # ";
        for (int j = 0; j < table_qty[i]; ++j) cout << table[i][j] << " "; 
        cout << endl;
*/
      }
  
  	  assert(sizeof(uint64_t)==8);
  	  assert(sizeof(int)==4);
  	  WallClockTimer timer;
  	  int N = 100000;
  	  for(int sb = 1; sb<=32;sb*=2) {
  	    int setbitsmax = sb*N;
		vector<uint64_t> bitmap(N);
		for (int k = 0; k < setbitsmax; ++k) {
			int bit = rand() % (N*64);
			bitmap[bit/64] |= (1L<<(bit%64));
		}
		int bitcount = 0;
		for(int k = 0; k <N; ++k) {
			bitcount += __builtin_popcountl(bitmap[k]);
		} 
		vector<int> output(bitcount);
		for(int t = 0; t<5; ++t) {
		  timer.reset();
		  int c0 = 0;
		  for(int t1=0;t1<100;++t1)
		    c0 = bitscan0(&bitmap[0],N,&output[0]);
		  int ti0 = timer.split();
		  timer.reset();
		  int c1 = 0;
		  for(int t1=0;t1<100;++t1)
		    c1 = bitscan1(&bitmap[0],N,&output[0]);
		  assert(c1 == c0);
		  int ti1 = timer.split();
		  timer.reset();
		  int c2 = 0;
		  for(int t1=0;t1<100;++t1)
		    c2 = bitscan2(&bitmap[0],N,&output[0]);
		  int ti2 = timer.split();
		  timer.reset();
		  assert(c1 == c2);
		  int c3 = 0;
		  for(int t1=0;t1<100;++t1)
		    c3 = bitscan3(&bitmap[0],N,&output[0]);
		  int ti3 = timer.split();
		  assert(c1 == c3);
		  int c4 = 0;
		  for(int t1=0;t1<100;++t1)
		    c4 = bitscan4(&bitmap[0],N,&output[0]);
		  int ti4 = timer.split();
		  assert(c1 == c4);

		  if(t>2)
		    cout<<sb<<" " <<bitcount*100.0*0.001 /ti0<<" "<<" " <<bitcount*100.0*0.001 /ti1<<" " <<bitcount*100.0*0.001 /ti2<<" " <<bitcount*100.0*0.001 /ti3 << " " << bitcount*100.0*0.001 / ti4<<endl;
		}
  	  }

  	return 0;
  }
