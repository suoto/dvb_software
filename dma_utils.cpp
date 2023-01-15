/*
 * This file is part of the Xilinx DMA IP Core driver tools for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under BSD-style license (found in the
 * LICENSE file in the root directory of this source tree)
 */

#include "dma_utils.hpp"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "spdlog/spdlog.h"

#define FATAL                                                           \
  do {                                                                  \
    fprintf( stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
             __FILE__, errno, strerror( errno ) );                      \
    exit( 1 );                                                          \
  } while ( 0 )

ssize_t read_to_buffer( char* fname, int fd, char* buffer, uint64_t size,
                        uint64_t base ) {
  ssize_t rc;
  uint64_t count = 0;
  char* buf = buffer;
  off_t offset = base;
  int loop = 0;

  while ( count < size ) {
    uint64_t bytes = size - count;
    SPDLOG_TRACE( "[{}] Iteration #{}: read so far: {}", fname, loop, count );

    if ( bytes > RW_MAX_SIZE ) {
      bytes = RW_MAX_SIZE;
    }

    if ( offset ) {
      SPDLOG_TRACE( "[{}] Iteration #{}: seek to 0x{:x}", fname, loop, offset );
      rc = lseek( fd, offset, SEEK_SET );
      if ( rc != offset ) {
        SPDLOG_ERROR( "[{}] seek off 0x{:x} != 0x{:x}", fname, rc, offset );
        perror( "seek file" );
        FATAL;
        return -EIO;
      }
    }

    /* read data from file into memory buffer */
    rc = read( fd, buf, bytes );
    SPDLOG_TRACE( "[{}] read {} bytes @ {}", fname, bytes, offset );
    if ( rc < 0 ) {
      SPDLOG_ERROR( "[{}] read {} @ 0x{:x} failed with {}.", fname, bytes,
                    offset, rc );
      perror( "read file" );
      FATAL;
      return -EIO;
    }

    count += rc;
    if ( (uint64_t)rc != bytes ) {
      SPDLOG_DEBUG( "[{}] read underflow 0x{:x}/{} @ 0x{:x}", fname, rc, bytes,
                    offset );
      break;
    }

    buf += bytes;
    offset += bytes;
    loop++;
  }

  if ( count != size && loop )
    SPDLOG_ERROR( "[{}] read underflow {}/{}.", fname, count, size );
  return count;
}

ssize_t write_from_buffer( char* fname, int fd, char* buffer, uint64_t size,
                           uint64_t base ) {
  ssize_t rc;
  uint64_t count = 0;
  char* buf = buffer;
  off_t offset = base;
  int loop = 0;

  while ( count < size ) {
    uint64_t bytes = size - count;

    if ( bytes > RW_MAX_SIZE ) bytes = RW_MAX_SIZE;

    if ( offset ) {
      rc = lseek( fd, offset, SEEK_SET );
      if ( rc != offset ) {
        SPDLOG_ERROR( "[{}] seek off 0x{:x} != 0x{:x}.", fname, rc, offset );
        perror( "seek file" );
        FATAL;
        return -EIO;
      }
    }

    /* write data to file from memory buffer */
    rc = write( fd, buf, bytes );
    if ( rc < 0 ) {
      SPDLOG_ERROR( "[{}] write 0x{:x} @ 0x{:x} failed {}", fname, bytes,
                    offset, rc );
      perror( "write file" );
      FATAL;
      return -EIO;
    }

    count += rc;
    if ( (uint64_t)rc != bytes ) {
      SPDLOG_ERROR( "[{}] write underflow 0x{:x}/0x{:x} @ 0x{:x}.", fname, rc,
                    bytes, offset );
      break;
    }
    buf += bytes;
    offset += bytes;

    loop++;
  }

  if ( count != size && loop )
    SPDLOG_ERROR( "[{}] write underflow 0x{:x}/0x{:x}.", fname, count, size );

  return count;
}
