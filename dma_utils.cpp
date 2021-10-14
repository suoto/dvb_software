/*
 * This file is part of the Xilinx DMA IP Core driver tools for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under BSD-style license (found in the
 * LICENSE file in the root directory of this source tree)
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "dma_utils.hpp"

int verbose = 0;

ssize_t read_to_buffer( char* fname, int fd, char* buffer, uint64_t size,
                        uint64_t base ) {
  ssize_t rc;
  uint64_t count = 0;
  char* buf = buffer;
  off_t offset = base;
  int loop = 0;

  while ( count < size ) {
    uint64_t bytes = size - count;

    printf( "count=%lu, bytes=%lu", count, bytes );

    if ( bytes > RW_MAX_SIZE ) {
      bytes = RW_MAX_SIZE;
      printf( "; adjusted bytes to %lu", bytes );
    }

    printf( "\n" );

    if ( offset ) {
      printf( "Seeking to %lu", offset );
      rc = lseek( fd, offset, SEEK_SET );
      if ( rc != offset ) {
        fprintf( stderr, "%s, seek off 0x%lx != 0x%lx.\n", fname, rc, offset );
        perror( "seek file" );
        return -EIO;
      }
    }

    /* read data from file into memory buffer */
    rc = read( fd, buf, bytes );
    if ( rc < 0 ) {
      fprintf( stderr, "%s, read 0x%lx @ 0x%lx failed %ld.\n", fname, bytes,
               offset, rc );
      perror( "read file" );
      return -EIO;
    }

    count += rc;
    if ( (uint64_t)rc != bytes ) {
      fprintf( stderr, "%s, read underflow 0x%lx/0x%lx @ 0x%lx.\n", fname, rc,
               bytes, offset );
      break;
    }

    buf += bytes;
    offset += bytes;
    loop++;
  }

  if ( count != size && loop )
    fprintf( stderr, "%s, read underflow 0x%lx/0x%lx.\n", fname, count, size );
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
        fprintf( stderr, "%s, seek off 0x%lx != 0x%lx.\n", fname, rc, offset );
        perror( "seek file" );
        return -EIO;
      }
    }

    /* write data to file from memory buffer */
    rc = write( fd, buf, bytes );
    if ( rc < 0 ) {
      fprintf( stderr, "%s, write 0x%lx @ 0x%lx failed %ld.\n", fname, bytes,
               offset, rc );
      perror( "write file" );
      return -EIO;
    }

    count += rc;
    if ( (uint64_t)rc != bytes ) {
      fprintf( stderr, "%s, write underflow 0x%lx/0x%lx @ 0x%lx.\n", fname, rc,
               bytes, offset );
      break;
    }
    buf += bytes;
    offset += bytes;

    loop++;
  }

  if ( count != size && loop )
    fprintf( stderr, "%s, write underflow 0x%lx/0x%lx.\n", fname, count, size );

  return count;
}
