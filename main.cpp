#include <sys/mman.h>
#include <functional>  // std::ref
#include <future>      // std::promise, std::future
#include <iostream>    // std::cout
#include <thread>      // std::thread
#include "register.hpp"
#include "spdlog/spdlog.h"

#define FATAL                                                           \
  do {                                                                  \
    fprintf( stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
             __FILE__, errno, strerror( errno ) );                      \
    exit( 1 );                                                          \
  } while ( 0 )

int main() {
  // Setup register map
  RegisterMap *regs = new RegisterMap();
  regs->read(0);
  return 0;
}

