
#include "dvb_encoder.hpp"

#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include "cdev_sgdma.h"
#include "dma_utils.hpp"
#include "dvb_types.hpp"
#include "spdlog/spdlog.h"
#include "third_party/cppzmq/zmq.hpp"

using std::queue;
using std::string;
using std::vector;

#define XDMA_H2C_DATA_DEV "/dev/xdma0_h2c_0"
#define XDMA_H2C_METADATA_DEV "/dev/xdma0_h2c_1"
#define XDMA_C2H_DATA_DEV "/dev/xdma0_c2h_0"

// #define MAX_FRAME_LENGTH 256 * 1024
#define MAX_FRAME_LENGTH 86760

#define UDP_SEND 0
#define ZMQ_SEND 1
#define ZMQ_URL "tcp://*:5557"

#define MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define MAX( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )

#define MAX_UDP_PAYLOAD_LENGTH 1472

#define FATAL                                                           \
  do {                                                                  \
    fprintf( stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
             __FILE__, errno, strerror( errno ) );                      \
    exit( 1 );                                                          \
  } while ( 0 )

void foo() {
  int fd = open( XDMA_H2C_DATA_DEV, O_RDWR | O_NONBLOCK );
  struct pollfd pfd;
  pfd.events = POLLIN;
  pfd.fd = fd;
  pfd.events = POLLIN;

  int rc = poll( &pfd, 1, 1 );

  if ( rc < 0 ) {
    SPDLOG_ERROR( "poll() failed with rc={}", rc );
    FATAL;
  }

  if ( rc == 0 ) {
    SPDLOG_INFO( "poll() timed out" );
  }

  SPDLOG_INFO( "pfd.revents={}", pfd.revents );

  if ( pfd.revents != POLLIN ) {
    SPDLOG_WARN( "Expected POLLIN, but got {}", pfd.revents );
  }
};

int resolve( const char* hostname, int family, const char* service,
             sockaddr_storage* pAddr ) {
  int result;
  addrinfo* result_list = NULL;
  addrinfo hints = {};
  hints.ai_family = family;
  hints.ai_socktype =
      SOCK_DGRAM;  // without this flag, getaddrinfo will return 3x the number
                   // of addresses (one for each socket type).
  result = getaddrinfo( hostname, service, &hints, &result_list );
  SPDLOG_INFO( "getaddrinfo returned {}", result );
  if ( result == 0 ) {
    // ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
    memcpy( pAddr, result_list->ai_addr, result_list->ai_addrlen );
    freeaddrinfo( result_list );
  }

  return result;
}

// Member functions definitions including constructor
DvbEncoder::DvbEncoder( void ) {
  SPDLOG_TRACE( "Creating data interface" );

  this->fd_indata = open( XDMA_C2H_DATA_DEV, O_RDWR | O_NONBLOCK );
  this->fd_outdata = open( XDMA_H2C_DATA_DEV, O_RDWR | O_NONBLOCK );
  posix_memalign( (void**)&this->indata_buffer, 4096 /*alignment */,
                  MAX_FRAME_LENGTH + 4096 );

  if ( ZMQ_SEND ) {
    SPDLOG_INFO( "Creating ZMQ context" );
    // zmq::context_t context;
    this->sender = zmq::socket_t( context, ZMQ_PUSH );
    this->sender.bind( ZMQ_URL );
  }

  if ( UDP_SEND ) {
    this->sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    memset( &this->dest_addr, 0, sizeof( this->dest_addr ) );
    int result = resolve( "192.168.1.135", AF_INET, "50000", &this->dest_addr );
    if ( result != 0 ) {
      int lasterror = errno;
      SPDLOG_ERROR( "Error resolving address: {}", lasterror );
      exit( 1 );
    }
  }
}

DvbEncoder::~DvbEncoder( void ) {
  SPDLOG_WARN( "Closing FDs" );
  close( this->fd_indata );
  close( this->fd_outdata );
  free( this->indata_buffer );
};

char get_metadata_value( FrameParameters* parms ) {
  if ( parms->frame_size == FECFRAME_SHORT ) {
    if ( parms->constellation == MOD_QPSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x00;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x01;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x02;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x03;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x04;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x05;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x06;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x07;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x08;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x09;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x0A;
      };
    };
    if ( parms->constellation == MOD_8PSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x0B;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x0C;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x0D;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x0E;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x0F;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x10;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x11;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x12;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x13;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x14;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x15;
      };
    };
    if ( parms->constellation == MOD_16APSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x16;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x17;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x18;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x19;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x1A;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x1B;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x1C;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x1D;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x1E;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x1F;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x20;
      };
    };
    if ( parms->constellation == MOD_32APSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x21;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x22;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x23;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x24;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x25;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x26;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x27;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x28;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x29;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x2A;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x2B;
      };
    }
  };
  if ( parms->frame_size == FECFRAME_NORMAL ) {
    if ( parms->constellation == MOD_QPSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x2C;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x2D;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x2E;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x2F;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x30;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x31;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x32;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x33;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x34;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x35;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x36;
      };
    };
    if ( parms->constellation == MOD_8PSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x37;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x38;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x39;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x3A;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x3B;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x3C;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x3D;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x3E;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x3F;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x40;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x41;
      };
    };
    if ( parms->constellation == MOD_16APSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x42;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x43;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x44;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x45;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x46;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x47;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x48;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x49;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x4A;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x4B;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x4C;
      };
    };
    if ( parms->constellation == MOD_32APSK ) {
      if ( parms->code_rate == C1_4 ) {
        return 0x4D;
      };
      if ( parms->code_rate == C1_3 ) {
        return 0x4E;
      };
      if ( parms->code_rate == C2_5 ) {
        return 0x4F;
      };
      if ( parms->code_rate == C1_2 ) {
        return 0x50;
      };
      if ( parms->code_rate == C3_5 ) {
        return 0x51;
      };
      if ( parms->code_rate == C2_3 ) {
        return 0x52;
      };
      if ( parms->code_rate == C3_4 ) {
        return 0x53;
      };
      if ( parms->code_rate == C4_5 ) {
        return 0x54;
      };
      if ( parms->code_rate == C5_6 ) {
        return 0x55;
      };
      if ( parms->code_rate == C8_9 ) {
        return 0x56;
      };
      if ( parms->code_rate == C9_10 ) {
        return 0x57;
      };
    };
  };

  SPDLOG_ERROR(
      "Unable to translate metadata value for framesize={}, "
      "constellation={}, "
      "code rate={}",
      parms->frame_size, parms->constellation, parms->code_rate );
  return -1;
};

void DvbEncoder::send_frame( char metadata, char* data, ssize_t length ) {
  SPDLOG_INFO( "Setting up transfer for {} bytes", length );
  char config_and_data[ length + 4 ];
  config_and_data[ 0 ] = metadata;
  std::memcpy( config_and_data + 4, data, length );
  // // this->metadata_queue->push( parms );
  // this->data_queue->push( new frame{ config_and_data, length + 4 } );
}

ssize_t get_bb_frame_length( FrameParameters* parms ) {
  if ( parms->frame_size == FECFRAME_SHORT ) {
    if ( parms->code_rate == C1_4 ) {
      return 3072 / 8;
    };
    if ( parms->code_rate == C1_3 ) {
      return 5232 / 8;
    };
    if ( parms->code_rate == C2_5 ) {
      return 6312 / 8;
    };
    if ( parms->code_rate == C1_2 ) {
      return 7032 / 8;
    };
    if ( parms->code_rate == C3_5 ) {
      return 9552 / 8;
    };
    if ( parms->code_rate == C2_3 ) {
      return 10632 / 8;
    };
    if ( parms->code_rate == C3_4 ) {
      return 11712 / 8;
    };
    if ( parms->code_rate == C4_5 ) {
      return 12432 / 8;
    };
    if ( parms->code_rate == C5_6 ) {
      return 13152 / 8;
    };
    if ( parms->code_rate == C8_9 ) {
      return 14232 / 8;
    };
  };
  if ( parms->frame_size == FECFRAME_NORMAL ) {
    if ( parms->code_rate == C1_4 ) {
      return 16008 / 8;
    };
    if ( parms->code_rate == C1_3 ) {
      return 21408 / 8;
    };
    if ( parms->code_rate == C2_5 ) {
      return 25728 / 8;
    };
    if ( parms->code_rate == C1_2 ) {
      return 32208 / 8;
    };
    if ( parms->code_rate == C3_5 ) {
      return 38688 / 8;
    };
    if ( parms->code_rate == C2_3 ) {
      return 43040 / 8;
    };
    if ( parms->code_rate == C3_4 ) {
      return 48408 / 8;
    };
    if ( parms->code_rate == C4_5 ) {
      return 51648 / 8;
    };
    if ( parms->code_rate == C5_6 ) {
      return 53840 / 8;
    };
    if ( parms->code_rate == C8_9 ) {
      return 57472 / 8;
    };
    if ( parms->code_rate == C9_10 ) {
      return 58192 / 8;
    };
  };

  SPDLOG_ERROR(
      "Unable to determine baseband frame length for framesize={}, "
      "constellation={}, "
      "code rate={}",
      parms->frame_size, parms->constellation, parms->code_rate );
  return -1;
}

#define COLUMNS 32

string format( char* data, size_t size ) {
  size_t size_adj = MIN( size, 256 );
  if ( size != size_adj )
    SPDLOG_TRACE( "Size {} is too big, adjusting to {}", size, size_adj );
  std::ostringstream s;
  const auto save = s.flags();
  s << "      | ";
  for ( int i = 0; i < COLUMNS; i++ ) {
    if ( i ) {
      if ( i % 8 == 0 ) s << " ";
    };
    s << std::setw( 2 ) << std::setfill( ' ' ) << i << ' ';
  };
  s << "\n" << std::setw( 5 ) << 0 << " | ";
  for ( size_t i = 0; i < size_adj; i++ ) {
    if ( i ) {
      if ( i % COLUMNS == 0 )
        s << "\n"
          << std::setw( 5 ) << std::dec << std::setfill( ' ' ) << i << " | ";
      else {
        if ( i % 8 == 0 ) s << " ";
        s << " ";
      };
    };
    // const auto value = data[ i ];
    // std::cout << value;
    s << std::hex << std::setw( 2 ) << std::setfill( '0' ) << ( *data & 0xFF );
    data++;
  };
  if ( size != size_adj )
    s << "\n(" << std::dec << size - size_adj << " bytes omitted)";
  s.flags( save );
  return s.str();
}
string format( std::vector< char >* data ) {
  string s( data->begin(), data->end() );
  return format( (char*)s.c_str(), s.size() );
}
string format( std::vector< char >* data, size_t size ) {
  string s( data->begin(), data->begin() + size );
  return format( (char*)s.c_str(), s.size() );
}

int DvbEncoder::send_from_file( FrameParameters* parms, string filename ) {
  char metadata = get_metadata_value( parms );
  ssize_t bb_frame_length = get_bb_frame_length( parms ) + 4;
  SPDLOG_INFO( "Metadata value for {}: 0x{:2x} / {:d}. Frame length is {}",
               format( parms ), metadata, metadata, bb_frame_length );

  std::ifstream istream( filename.c_str(), std::ifstream::binary );
  std::vector< char > buffer( bb_frame_length, 0 );

  for ( auto i = 0; i < 4; i++ ) buffer[ i ] = metadata;
  std::streamsize bytes_read = 0;
  int outframes = 0;
  while ( !istream.eof() ) {
    // First pass has metadata inserted as first byte of the buffer
    istream.read( buffer.data() + 4, buffer.size() - 4 );
    bytes_read += istream.gcount();
    int frame_size = istream.gcount() + 4;

    SPDLOG_DEBUG( "[{}] {}: read {} bytes, total {}", outframes, filename,
                  istream.gcount(), bytes_read );

    SPDLOG_TRACE( "Output data;\n{}", format( &buffer, frame_size ) );

    this->send_data( &buffer[ 0 ], frame_size );
    outframes++;
  }

  SPDLOG_INFO( "Read {} bytes from \"{}\"", bytes_read, filename );
  // this->send_frame( metadata, &indata[ 0 ], indata.size() );
  return outframes;

  // SPDLOG_DEBUG( "Waiting for completion" );
  // this->join();
}

void DvbEncoder::receive_frame( void ) {
  SPDLOG_INFO( "Receiving frame" );

  ssize_t length = read_to_buffer( (char*)XDMA_C2H_DATA_DEV, this->fd_indata,
                                   this->indata_buffer, MAX_FRAME_LENGTH, 0 );
  SPDLOG_INFO( "Received {} bytes", length );
  SPDLOG_TRACE( "Data received:\n{}", format( this->indata_buffer, length ) );
  if ( length < 0 ) {
    SPDLOG_ERROR( "Error reading data: {}", length );
    FATAL;
  };

  if ( UDP_SEND ) {
    this->udp_send( this->indata_buffer,
                    MIN( length, MAX_UDP_PAYLOAD_LENGTH ) );
  };

  if ( ZMQ_SEND ) {
    this->zmq_send( this->indata_buffer,
                    MIN( length, MAX_UDP_PAYLOAD_LENGTH ) );
  };
};

void DvbEncoder::zmq_send( char* data, ssize_t length ) {
  SPDLOG_DEBUG( "Sending {} bytes via ZMQ", length );
  zmq::message_t message( data, length );
  this->sender.send( message, zmq::send_flags::none );
}

ssize_t DvbEncoder::udp_send( char* data, ssize_t length ) {
  SPDLOG_DEBUG( "Sending {} bytes via UDP", length );
  // Send the data
  int udp_bytes_sent;
  int udp_bytes_remaining = length;
  while ( udp_bytes_remaining ) {
    udp_bytes_sent =
        sendto( this->sock, data, MIN( length, MAX_UDP_PAYLOAD_LENGTH ), 0,
                (struct sockaddr*)&this->dest_addr, sizeof( this->dest_addr ) );

    if ( udp_bytes_sent < 0 ) {
      SPDLOG_ERROR( "Sending UDP data failed" );
      FATAL;
    }

    udp_bytes_remaining = MAX( udp_bytes_remaining - udp_bytes_sent, 0 );

    // SPDLOG_DEBUG( "UDP bytes sent: {}, remaining {}", udp_bytes_sent,
    //               udp_bytes_remaining );
  }
  return length;
}

void DvbEncoder::send_data( char* frame, ssize_t size ) {
  SPDLOG_DEBUG( "Sending {} bytes of data", size );

  ssize_t pending = size;
  int i = 0;
  while ( pending ) {
    ssize_t rc =
        write_from_buffer( (char*)"data", this->fd_outdata, frame, size, 0 );

    if ( rc < 0 ) {
      SPDLOG_ERROR( "#{}: Error sending data: rc={}", i, rc );
      FATAL;
      break;
    }
    pending -= rc;
    i++;
  }
  SPDLOG_DEBUG( "Sending data completed after {} iterations", i );
};
