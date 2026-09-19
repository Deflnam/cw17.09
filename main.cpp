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

double area(double r, std::size_t threads, std::size_t tests)
{
  if (threads == 0)
  {
    throw std::invalid_argument("threads must be > 0");
  }

  const std::size_t base = tests / threads;
  const std::size_t rem = tests % threads;

  std::vector<pthread_t> tids(threads);
  std::vector<Args> args(threads);

  std::size_t created = 0;

  try
  {
    for (std::size_t i = 0; i < threads; ++i)
    {
      args[i].r_ = r;
      args[i].tests_ = base + (i < rem ? 1 : 0);
      args[i].seed_ = 12345 + i;
      args[i].result_ = 0;
      args[i].eptr_ = nullptr;

      const int err = pthread_create(&tids[i], nullptr, threadFunc, &args[i]);
      if (err != 0)
      {
        throw std::runtime_error(std::strerror(err));
      }
      ++created;
    }
  }
  catch (...)
  {
    joinAll(tids, created);
    throw;
  }

  joinAll(tids, created);

  for (std::size_t i = 0; i < threads; ++i)
  {
    if (args[i].eptr_)
    {
      std::rethrow_exception(args[i].eptr_);
    }
  }

  std::size_t pass = 0;
  for (std::size_t i = 0; i < threads; ++i)
  {
    pass += args[i].result_;
  }

  return static_cast<double>(pass) / static_cast<double>(tests) * 4.0 * r * r;
}

int main()
{
  return 0;
}