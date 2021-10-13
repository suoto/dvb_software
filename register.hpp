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
  void write( uint8_t byte_offset, uint32_t data );
  uint32_t read( uint8_t byte_offset );
  RegisterMap();   // This is the constructor
  ~RegisterMap();  // This is the destructor: declaration
 private:
  int fd;
  void* map;
};

// Member functions definitions including constructor
RegisterMap::RegisterMap( void ) {
  spdlog::info( "Creating register map" );
  // Setup register map
  if ( ( this->fd = open( XDMA_USER, O_RDWR | O_SYNC ) ) == -1 ) FATAL;
  this->map =
      mmap( 0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0 );
  if ( this->map == (void*)-1 ) FATAL;

  spdlog::debug( "Memory mapped at address {:p}", this->map );
};

void RegisterMap::write( uint8_t byte_offset, uint32_t data ) {
  uint32_t* byte_address = (uint32_t*)this->map + byte_offset;
  spdlog::trace( "[W] addr=0x{:X}, data=0x{:X}", byte_offset, data );
  ( *byte_address ) = data;
};

uint32_t RegisterMap::read( uint8_t byte_offset ) {
  uint32_t* byte_address = (uint32_t*)this->map + byte_offset;
  uint32_t data = *( (uint32_t*)byte_address );
  spdlog::trace( "[R] addr=0x{:X}, data=0x{:X}", byte_offset, data );
  return data;
};

RegisterMap::~RegisterMap() {
  spdlog::info( "Unmapping and closing file descriptor" );
  if ( munmap( map, MAP_SIZE ) == -1 ) FATAL;
  close( fd );
};
