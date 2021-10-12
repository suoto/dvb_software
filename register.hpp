#pragma once

#include <sys/mman.h>
#include <iostream>
#include <string>

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
  void write( off_t offset, uint32_t data );
  uint32_t read( off_t offset );
  RegisterMap();  // This is the constructor
 private:
  int fd;
  void* map;
};

// Member functions definitions including constructor
RegisterMap::RegisterMap( void ) {
  spdlog::trace( "Creating register map" );
  // Setup register map
  if ( ( this->fd = open( XDMA_USER, O_RDWR | O_SYNC ) ) == -1 ) FATAL;
  this->map =
      mmap( 0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0 );
  if ( this->map == (void*)-1 ) FATAL;

  spdlog::debug( "Memory mapped at address {:p}", this->map );
};

void RegisterMap::write( off_t offset, uint32_t data ) {
  off_t* address = (off_t*)this->map + offset;
  spdlog::trace( "[W] addr=0x{:X}, data=0x{:X}", offset, data );
  ( *address ) = data;
};
uint32_t RegisterMap::read( off_t offset ) {
  off_t* address = (off_t*)this->map + offset;
  uint32_t data = *( address );
  spdlog::trace( "[R] addr=0x{:X}, data=0x{:X}", offset, data );
  return data;
};
