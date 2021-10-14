#pragma once

#include <sys/mman.h>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "dvb_types.hpp"
#include "spdlog/spdlog.h"

using std::queue;
using std::string;
using std::thread;
using std::vector;

typedef vector< uint8_t > frame;

class DvbEncoder {
 public:
  void send_frame( FrameParameters* parms, frame* frame );
  void join( void );
  DvbEncoder();   // This is the constructor
  // ~DvbEncoder();  // This is the destructor: declaration
 private:
  void send_metadata( FrameParameters* parms );
  void send_data( frame *frame );
  int fd_outdata = -1;
  int fd_metadata = -1;
  int fd_indata = -1;

  // queue< FrameParameters >* metadata_queue;
  queue< FrameParameters* >* metadata_queue;
  thread* metadata_thread;
  queue< frame* >* data_queue;
  thread* data_thread;
};
