#include <unistd.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

#include "ztimer.h"

using namespace std;

void syncsys() {
  for (int i = 0; i < 3; ++i) {
    if (system("sync")) {
      cerr << "Can't clear caches, do you have the file /proc/sys/vm/drop_caches???" << endl;
      exit(1);
    }
  }
}

void clearCache() {
  syncsys();
  if (system("echo 3 > /proc/sys/vm/drop_caches")) {
    cerr << "Can't clear caches, do you have the file /proc/sys/vm/drop_caches???" << endl;
    exit(1);
  }
}

size_t parseInt(const char *arg) {
  stringstream str(arg);
  size_t num = 0;
  if (!(str >> num) || !str.eof()) {
    cerr << "Can't parse : " << arg << endl;
    exit(1);
  }
  return num;
}


size_t FakeCheckSum(const char *p, size_t qty) {
    return std::accumulate(p, p + qty, 0);
}


int main(int argc, char *argv[]) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0] << " <file size in MBs> <bytes to read in MBs> <test directory> " << endl;
    return 1;
  }
  size_t fileSize = parseInt(argv[1]);
  size_t readSize = parseInt(argv[2]);

  string dir = argv[3];

  const string fileName = "largeFile";
  const size_t sizeMult = 1024UL * 1024UL; // MBs

  fileSize *= sizeMult;
  readSize *= sizeMult;

  string fn = dir + "/" + fileName;
  cout << "Creating file: '" << fn << "' size: " << fileSize << " bytes" << endl; 
  ofstream outf(fn.c_str());

  if (!outf) {
    cerr << "Can't open: '" << fn << "' for writing" << endl;
    return 1;
  }

  static char block[1024UL * 1024UL];
  for (size_t i = 0; i < sizeof block; ++i) {
    block[i] = rand() % 255; 
  }

  for (size_t i = 0; i < fileSize / sizeof block; ++i) {
    if (!outf.write(block, sizeof block)) {
      cerr << "Cannot write to '" << fileName << "'" << endl;
    }
  }

  outf.close();

  cout << "file created, let's do some testing!" << endl;

  for (size_t blockSize = 65536; blockSize <= sizeof block; blockSize*=2) {

    size_t readQty = readSize / blockSize;
    cout << "Testing block size: " << blockSize << " bytes, will read " << readQty << " blocks." << endl;
    clearCache();
    sleep(1);
    cout << "cache is cleared" << endl;

    ifstream inf(fn.c_str());

    if (!inf) {
      cerr << "Can't open '" << fn << "'" << endl;
      return 1;
    }

    WallClockTimer wtm;
    wtm.reset();

    size_t ignore = 0;

    size_t maxBlock = fileSize / blockSize;


    for (size_t i = 0; i < readQty; ++i) {
      off_t pos = blockSize * (rand() % maxBlock);

      //cout << pos << " " ;

      if (!inf.seekg(pos, ios_base::beg)) {
        cerr << "Can't change pos to " << pos << " in '" << fn << "'" << endl;
        return 1;
      }
      if (!inf.read(block, blockSize)) {
        cerr << "Can't read from '" << fn << "'" << endl;
        return 1;
      }
      ignore += FakeCheckSum(block, blockSize);
    }
    //cout << endl;

    wtm.split();

    cout << "Wall elapsed: " << wtm.elapsed() / 1e6 << endl;
    cout << "Avg time per block: " << wtm.elapsed() /1e3 / float(readQty) << " ms " << endl;

    inf.close();
    cout << "Ignore: " << ignore << endl; // to prevent from optimizing out
  } 
} 
