#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <string>

using namespace std;

struct URL2DocID {
  string url;
  int    id;
  URL2DocID(const string& url, int id) : url(url), id(id) {}
  bool operator<(const URL2DocID& o) const {
    return url < o.url;
  }
};

typedef URL2DocID* URL2DocIDPtr;
const   int JavaCharSize = 2;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <qty>" << endl;
    return 1;
  }
  int qty = atoi(argv[1]);
  int maxLen = 60;
  int minLen = 40;

  URL2DocIDPtr *arr = new URL2DocIDPtr[qty];
    
  size_t sz = 0;

  // We want to mimic Java consumption
  // In Java a char uses > one byte
  char buf[JavaCharSize*maxLen];

  fill(buf, buf + sizeof buf, 'a');

  cout << "Qty: " << qty << endl;
    
  for (int i = 0; i < qty; ++i) {
    int len = minLen + rand() % (maxLen - minLen);
      
    sz += JavaCharSize*len + 4;
      
    arr[i] = new URL2DocID(string(buf, 0, JavaCharSize*len), i);
  }    
    
  cout << "Before sorting!" << endl;
  sort(arr, arr + qty);
  cout << "Sorting is done, space occupied : " 
                        << sz/1024.0/1024.0/1024.0 << " Gb" << endl;
  for (int i = 0; i < qty; ++i) {
    delete arr[i];
  }

  delete [] arr;

  return 0;
}
