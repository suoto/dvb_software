#include <sys/mman.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "dvb_encoder.hpp"
#include "dvb_types.hpp"
#include "register.hpp"
#include "spdlog/spdlog.h"

using std::thread;
using namespace std::chrono_literals;

#define FATAL                                                           \
  do {                                                                  \
    fprintf( stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
             __FILE__, errno, strerror( errno ) );                      \
    exit( 1 );                                                          \
  } while ( 0 )

FrameParameters* infer_parameters( string filename ) {
  FrameParameters* params =
      new FrameParameters{ FECFRAME_SHORT, MOD_QPSK, C1_4 };

  if ( filename.find( "C1_4" ) != string::npos ) {
    params->code_rate = C1_4;
  } else if ( filename.find( "C1_3" ) != string::npos ) {
    params->code_rate = C1_3;
  } else if ( filename.find( "C2_5" ) != string::npos ) {
    params->code_rate = C2_5;
  } else if ( filename.find( "C1_2" ) != string::npos ) {
    params->code_rate = C1_2;
  } else if ( filename.find( "C3_5" ) != string::npos ) {
    params->code_rate = C3_5;
  } else if ( filename.find( "C2_3" ) != string::npos ) {
    params->code_rate = C2_3;
  } else if ( filename.find( "C3_4" ) != string::npos ) {
    params->code_rate = C3_4;
  } else if ( filename.find( "C4_5" ) != string::npos ) {
    params->code_rate = C4_5;
  } else if ( filename.find( "C5_6" ) != string::npos ) {
    params->code_rate = C5_6;
  } else if ( filename.find( "C8_9" ) != string::npos ) {
    params->code_rate = C8_9;
  } else if ( filename.find( "C9_10" ) != string::npos ) {
    params->code_rate = C9_10;
  } else {
    SPDLOG_ERROR( "Unable to find code rate for {}", filename );
    FATAL;
  };

  if ( filename.find( "MOD_QPSK" ) != string::npos ) {
    params->constellation = MOD_QPSK;
  } else if ( filename.find( "MOD_8PSK" ) != string::npos ) {
    params->constellation = MOD_8PSK;
  } else if ( filename.find( "MOD_16APSK" ) != string::npos ) {
    params->constellation = MOD_16APSK;
  } else if ( filename.find( "MOD_32APSK" ) != string::npos ) {
    params->constellation = MOD_32APSK;
  } else {
    SPDLOG_ERROR( "Unable to find constellation for {}", filename );
    FATAL;
  };

  if ( filename.find( "FECFRAME_SHORT" ) != string::npos ) {
    params->frame_size = FECFRAME_SHORT;
  } else if ( filename.find( "FECFRAME_NORMAL" ) != string::npos ) {
    params->frame_size = FECFRAME_NORMAL;
  } else {
    SPDLOG_ERROR( "Unable to find frame size for {}", filename );
    FATAL;
  };

  return params;
};

// enum dvb_code_rate_t {
// enum dvb_constellation_t {
//
dvb_framesize_t get_frame_size( string s ) {
  std::map< string, dvb_framesize_t > map;
  map = { { "NORMAL", FECFRAME_NORMAL },
          { "SHORT", FECFRAME_SHORT },
          { "MEDIUM", FECFRAME_MEDIUM } };
  for ( size_t i = 0; i < s.size(); i++ ) s[ i ] = toupper( s[ i ] );
  return map.at( s );
}

dvb_constellation_t get_constellation( string s ) {
  std::map< string, dvb_constellation_t > map;
  map = { { "QPSK", MOD_QPSK },
          { "16QAM", MOD_16QAM },
          { "64QAM", MOD_64QAM },
          { "256QAM", MOD_256QAM },
          { "8PSK", MOD_8PSK },
          { "8APSK", MOD_8APSK },
          { "16APSK", MOD_16APSK },
          { "8_8APSK", MOD_8_8APSK },
          { "32APSK", MOD_32APSK },
          { "4_12_16APSK", MOD_4_12_16APSK },
          { "4_8_4_16APSK", MOD_4_8_4_16APSK },
          { "64APSK", MOD_64APSK },
          { "8_16_20_20APSK", MOD_8_16_20_20APSK },
          { "4_12_20_28APSK", MOD_4_12_20_28APSK },
          { "128APSK", MOD_128APSK },
          { "256APSK", MOD_256APSK },
          { "BPSK", MOD_BPSK },
          { "BPSK_SF2", MOD_BPSK_SF2 },
          { "8VSB", MOD_8VSB },
          { "OTHER", MOD_OTHER } };

  for ( size_t i = 0; i < s.size(); i++ ) s[ i ] = toupper( s[ i ] );
  return map.at( s );
}

dvb_code_rate_t get_code_rate( string s ) {
  std::map< string, dvb_code_rate_t > map;

  map = { { "1/4", C1_4 },
          { "1/3", C1_3 },
          { "2/5", C2_5 },
          { "1/2", C1_2 },
          { "3/5", C3_5 },
          { "2/3", C2_3 },
          { "3/4", C3_4 },
          { "4/5", C4_5 },
          { "5/6", C5_6 },
          { "7/8", C7_8 },
          { "8/9", C8_9 },
          { "9/10", C9_10 },
          { "13/45", C13_45 },
          { "9/20", C9_20 },
          { "90/180", C90_180 },
          { "96/180", C96_180 },
          { "11/20", C11_20 },
          { "100/180", C100_180 },
          { "104/180", C104_180 },
          { "26/45", C26_45 },
          { "18/30", C18_30 },
          { "28/45", C28_45 },
          { "23/36", C23_36 },
          { "116/180", C116_180 },
          { "20/30", C20_30 },
          { "124/180", C124_180 },
          { "25/36", C25_36 },
          { "128/180", C128_180 },
          { "13/18", C13_18 },
          { "132/180", C132_180 },
          { "22/30", C22_30 },
          { "135/180", C135_180 },
          { "140/180", C140_180 },
          { "7/9", C7_9 },
          { "154/180", C154_180 },
          { "11/45", C11_45 },
          { "4/15", C4_15 },
          { "14/45", C14_45 },
          { "7/15", C7_15 },
          { "8/15", C8_15 },
          { "32/45", C32_45 },
          { "2/9-VLSNR", C2_9_VLSNR },
          { "1/5-MEDIUM", C1_5_MEDIUM },
          { "11/45-MEDIUM", C11_45_MEDIUM },
          { "1/3-MEDIUM", C1_3_MEDIUM },
          { "1/5-VLSNR-SF2", C1_5_VLSNR_SF2 },
          { "11/45-VLSNR-SF2", C11_45_VLSNR_SF2 },
          { "1/5-VLSNR", C1_5_VLSNR },
          { "4/15-VLSNR", C4_15_VLSNR },
          { "1/3-VLSNR", C1_3_VLSNR },
          { "OTHER", C_OTHER } };

  for ( size_t i = 0; i < s.size(); i++ ) s[ i ] = toupper( s[ i ] );
  return map.at( s );
}

int main( int argc, char* argv[] ) {
  if ( argc < 2 ) {
    printf( "Need the filename as argument\n" );
    return 1;
  };

  int verbose = 0;
  FrameParameters* parms =
      new FrameParameters{ FECFRAME_SHORT, MOD_QPSK, C1_4 };
  for ( int i = 0; i < argc; i++ ) {
    string arg = string( argv[ i ] );
    try {
      parms->frame_size = get_frame_size( arg );
    } catch ( std::out_of_range ) {
    };
    try {
      parms->constellation = get_constellation( arg );
    } catch ( std::out_of_range ) {
    };
    try {
      parms->code_rate = get_code_rate( arg );
    } catch ( std::out_of_range ) {
    };
    if ( arg == "-v" ) {
      verbose += 1;
    } else if ( arg == "-vv" ) {
      verbose += 2;
    } else if ( arg == "-vvv" ) {
      verbose += 3;
    };
  };
  SPDLOG_INFO( "Inferred frame parameters: {}", format( parms ) );

  switch ( verbose ) {
    case 0:
      spdlog::set_level( spdlog::level::warn );
      break;
    case 1:
      spdlog::set_level( spdlog::level::info );
      break;
    case 2:
      spdlog::set_level( spdlog::level::debug );
      break;
    case 3:
      spdlog::set_level( spdlog::level::trace );
      break;
    default:
      break;
  }

  string filename = argv[ 1 ];
  // FrameParameters* parms = infer_parameters( filename );

  // TODO: setup a thread for the receiver side and make the encoder receive a
  // callback to be called when data is received
  DvbEncoder* encoder = new DvbEncoder();

  thread* send_thread;
  send_thread = new thread( [ & ]() {
    SPDLOG_DEBUG( "Starting send thread" );
    int outframes = encoder->send_from_file( parms, filename );
    SPDLOG_INFO(
        "Waiting for receive thread to complete, need to receive {} frames",
        outframes );
    SPDLOG_DEBUG( "Exiting send thread. Should received {}", outframes );
  } );

  for ( ;; ) encoder->receive_frame();
  SPDLOG_INFO( "Joining send thread" );
  send_thread->join();
  SPDLOG_INFO( "Done" );

  // encoder->join();

  return 0;
}
