#pragma once

#include <arpa/inet.h>

#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "dvb_types.hpp"
#include "spdlog/spdlog.h"
#include "third_party/cppzmq/zmq.hpp"

using std::queue;
using std::string;
using std::thread;
using std::vector;

// typedef std::tuple< char*, ssize_t > frame;
// typedef std::pair< char*, ssize_t > frame;

class DvbEncoder {
 public:
  void send_frame( char metadata, char* data, ssize_t length );
  // void send_from_file( char metadata, string filename );
  int send_from_file( FrameParameters* params, string filename );
  // void join( void );
  void receive_frame( void );
  DvbEncoder();   // This is the constructor
  ~DvbEncoder();  // This is the destructor: declaration
 private:
  // int fd_h2c;
  int sock;
  struct sockaddr_storage dest_addr;
  // void send_metadata( FrameParameters* parms );
  void send_data( char* frame, ssize_t size );
  int fd_indata = -1;
  int fd_outdata = -1;
  char* indata_buffer = NULL;

  ssize_t udp_send( char* data, ssize_t length );
  void zmq_send( char* data, ssize_t length );
  zmq::context_t context;
  zmq::socket_t sender;
  // int fd_metadata = -1;
  // int fd_indata = -1;

  // // queue< FrameParameters* >* metadata_queue;
  // // thread* metadata_thread;
  // queue< frame* >* data_queue;
  // thread* data_thread;

  // thread* recv_thread;
};
