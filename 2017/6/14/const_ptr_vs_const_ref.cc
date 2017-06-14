#include <iostream>

using namespace std;

int main() {
  int x=3;
  int  const &rx  = x;
  int* const px   = &x;
  //rx = 33; // Compile error, can't assign through a constant ref.
  //px++;    // Compile error, because the pointer value is constant!
  *px =33; // This is fine, because only the pointer value is constant!
  cout << *px << endl;
  return 0;
};
