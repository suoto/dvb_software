#pragma once

#include <sys/mman.h>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "dvb_types.hpp"
#include "spdlog/spdlog.h"

using std::queue;
using std::string;
using std::thread;
using std::vector;

// typedef std::tuple< char*, ssize_t > frame;
typedef std::pair< char*, ssize_t > frame;

class DvbEncoder {
 public:
  void send_frame( FrameParameters* parms, char* data, ssize_t length );
  void send_from_file( FrameParameters* parms, string filename );
  void join( void );
  void receive_frame( void );
  DvbEncoder();  // This is the constructor
                 // ~DvbEncoder();  // This is the destructor: declaration
 private:
  void send_metadata( FrameParameters* parms );
  void send_data( frame* frame );
  int fd_outdata = -1;
  int fd_metadata = -1;
  // int fd_indata = -1;

  // queue< FrameParameters >* metadata_queue;
  queue< FrameParameters* >* metadata_queue;
  thread* metadata_thread;
  queue< frame* >* data_queue;
  thread* data_thread;

  // thread* recv_thread;
};
