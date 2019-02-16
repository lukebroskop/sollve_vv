#include <iostream>
#include <omp.h>

using namespace std;

#define N 1000

class A {

private:
  int h_array[N];
  int size;

public:
  A(const int s) : size(s) {
    for (int i = 0; i < N; i++)
      h_array[i] = 0;
  }

  void modify() {
#pragma omp target      
    {
      for (int i = 0; i < size; ++i)
          h_array[i] += 1;
    }
  }

  int* getArray() {
    return &h_array[0];
  }
};

int main() {

  int sum = 0, errors = 0;

  A *obj = new A(N);

  obj->modify();

  // checking results
  int* h_array = obj->getArray();
  for (int i = 0; i < N; ++i)
    sum += h_array[i];

  errors = N != sum;
  if (errors)
    cout << "test failed with "<< errors << "errors";

  delete obj;

  return errors;
}
