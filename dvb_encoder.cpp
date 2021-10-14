#include <sys/mman.h>
#include <chrono>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "dma_utils.hpp"
#include "dvb_encoder.hpp"
#include "dvb_types.hpp"
#include "spdlog/spdlog.h"

using std::queue;
using std::string;
using std::vector;

#define XDMA_H2C_DATA_DEV "/dev/xdma0_h2c_0"
#define XDMA_H2C_METADATA_DEV "/dev/xdma0_h2c_1"
#define XDMA_C2H_DATA_DEV "/dev/xdma0_c2h_0"

#define FATAL                                                           \
  do {                                                                  \
    fprintf( stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
             __FILE__, errno, strerror( errno ) );                      \
    exit( 1 );                                                          \
  } while ( 0 )

// Member functions definitions including constructor
DvbEncoder::DvbEncoder( void ) {
  SPDLOG_INFO( "Creating data interface" );

  this->fd_outdata = open( XDMA_H2C_DATA_DEV, O_RDWR | O_NONBLOCK );
  this->fd_metadata = open( XDMA_H2C_METADATA_DEV, O_RDWR | O_NONBLOCK );
  this->fd_indata = open( XDMA_C2H_DATA_DEV, O_RDWR | O_NONBLOCK );
  this->metadata_queue = new queue< FrameParameters* >();
  this->metadata_thread = new thread( [&]() {
    SPDLOG_DEBUG( "Starting metadata thread" );
    while ( 1 ) {
      SPDLOG_DEBUG( "Waiting for metadata requests" );
      while ( this->metadata_queue->empty() ) {
        // SPDLOG_DEBUG( "Queue is empty" );
      };
      SPDLOG_DEBUG( "Metadata request received" );
      // FrameParameters parms = this->metadata_queue->back();
      this->send_metadata( this->metadata_queue->front() );
      this->metadata_queue->pop();
      SPDLOG_DEBUG( "Exiting metadata thread" );
    }
  } );

  this->data_queue = new queue< frame* >();
  this->data_thread = new thread( [&]() {
    SPDLOG_DEBUG( "Starting data thread" );
    while ( 1 ) {
      SPDLOG_DEBUG( "Waiting for data requests" );
      while ( this->data_queue->empty() ) {
        // SPDLOG_DEBUG( "Queue is empty" );
      };
      SPDLOG_DEBUG( "Data request received" );
      // FrameParameters parms = this->data_queue->back();
      this->send_data( this->data_queue->front() );
      this->data_queue->pop();
      SPDLOG_DEBUG( "Exiting data thread" );
    }
  } );

  this->metadata_thread->detach();
  this->data_thread->detach();
}

void DvbEncoder::send_frame( FrameParameters* parms, frame* frame ) {
  SPDLOG_INFO(
      "Setting up transfer or {} bytes. Currently there are {} metadata "
      "entries and {} data entries",
      frame->size(), this->metadata_queue->size(), this->data_queue->size() );
  this->metadata_queue->push( parms );
  this->data_queue->push( frame );
}

void DvbEncoder::join( void ) {
  SPDLOG_INFO( "Joining existing threads" );
  // this->metadata_thread->join();
  // this->data_thread->join();
  while ( !this->metadata_queue->empty() ) {
  };
  while ( !this->data_queue->empty() ) {
  };
  SPDLOG_INFO( "Completed" );
};

int get_metadata_value( FrameParameters* parms ) {
  // if ( parms->frame_size == FECFRAME_SHORT ) {
  //   return 0;
  // };

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
      "Unable to translate metadata value for framesize={}, constellation={}, "
      "code rate={}",
      parms->frame_size, parms->constellation, parms->code_rate );
  return -1;
};

void DvbEncoder::send_metadata( FrameParameters* parms ) {
  // std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
  int metadata = get_metadata_value( parms );
  string data = std::to_string( metadata );
  SPDLOG_DEBUG(
      "Sending metadata for framesize={}, constellation={}, code rate={}, "
      "metadata value is {}",
      parms->frame_size, parms->constellation, parms->code_rate, metadata );
  int rc = write_from_buffer( (char*)"metadata", this->fd_metadata,
                              (char*)data.c_str(), 1, 0 );
  SPDLOG_DEBUG( "Sending metadata completed, return code is {}", rc );
  if ( rc != 1 ) {
    SPDLOG_WARN(
        "Something went wrong, expected to have sent 1 byte but driver "
        "confirmed "
        "{}",
        rc );
  };
};

void DvbEncoder::send_data( frame* data ) {
  SPDLOG_DEBUG( "Sending {} bytes of data", data->size() );

  ssize_t rc =
      write_from_buffer( (char*)"data", this->fd_outdata, (char*)&data->at( 0 ),
                         data->size() * sizeof( char ), 0 );

  if ( rc != (ssize_t)data->size() ) {
    SPDLOG_WARN(
        "Something went wrong, expected to have sent {} but driver confirmed "
        "{}",
        data->size(), rc );
  }
  SPDLOG_DEBUG( "Sending data completed, return code is {}", rc );
};
