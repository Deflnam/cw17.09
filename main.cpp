#include <iostream>

#include <pthread.h>

struct Args
{
  double r_;
  std::size_t tests_;
  std::size_t seed_;
  std::size_t result_;
  std::exception_ptr eptr_;
};

bool isInside(double x, double y, double r)
{
  return (x * x + y * y) <= (r * r);
}

int main()
{
  return 0;
}