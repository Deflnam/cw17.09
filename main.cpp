#include <iostream>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <random>
#include <stdexcept>
#include <vector>

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

std::size_t calc(double r, std::size_t tests, std::size_t seed)
{
  std::mt19937 engine(seed);
  std::uniform_real_distribution<double> dist(-r, r);

  std::size_t pass = 0;
  for (std::size_t i = 0; i < tests; ++i)
  {
    const double x = dist(engine);
    const double y = dist(engine);
    if (isInside(x, y, r))
    {
      ++pass;
    }
  }
  return pass;
}

void * threadFunc(void * arg)
{
  Args * args = static_cast<Args *>(arg);
  try
  {
    args->result_ = calc(args->r_, args->tests_, args->seed_);
  }
  catch (...)
  {
    args->eptr_ = std::current_exception();
  }
  return nullptr;
}

void joinAll(std::vector<pthread_t> & tids, std::size_t count)
{
  for (std::size_t i = 0; i < count; ++i)
  {
    pthread_join(tids[i], nullptr);
  }
}

int main()
{
  return 0;
}