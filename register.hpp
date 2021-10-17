#pragma once

#include <sys/mman.h>
#include <iostream>
#include <string>

#include "dvb_types.hpp"
#include "spdlog/spdlog.h"

using std::string;

#define XDMA_USER "/dev/xdma0_user"
#define MAP_SIZE ( 32 * 1024UL )
#define MAP_MASK ( MAP_SIZE - 1 )

#define FATAL                                                           \
  do {                                                                  \
    fprintf( stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
             __FILE__, errno, strerror( errno ) );                      \
    exit( 1 );                                                          \
  } while ( 0 )

class RegisterMap {
 public:
  void write( uint16_t byte_offset, uint32_t data );
  // void write( uint16_t byte_offset, uint16_t data );
  // void write( uint16_t byte_offset, uint8_t data );
  uint32_t read( uint16_t byte_offset );
  void updateMappingTable( dvb_constellation_t constellation,
                           dvb_framesize_t framesize, dvb_code_rate_t rate );
  RegisterMap();   // This is the constructor
  ~RegisterMap();  // This is the destructor: declaration
 private:
  int fd;
  void* map;
  void writePolyphaseFilterCoefficients( void );
};
