#include <sys/mman.h>
#include <fstream>
#include <string>
#include <vector>

#include "dvb_encoder.hpp"
#include "dvb_types.hpp"
#include "register.hpp"
#include "spdlog/spdlog.h"

#define FATAL                                                           \
  do {                                                                  \
    fprintf( stderr, "Error at line %d, file %s (%d) [%s]\n", __LINE__, \
             __FILE__, errno, strerror( errno ) );                      \
    exit( 1 );                                                          \
  } while ( 0 )

FrameParameters* infer_parameters( string filename ) {
  FrameParameters* params = new FrameParameters{FECFRAME_SHORT, MOD_QPSK, C1_4};

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

int main( int argc, char* argv[] ) {
  if ( argc < 2 ) {
    printf( "Need the filename as argument\n" );
    return 1;
  };

  int verbose = 0;
  for ( int i = 0; i < argc; i++ ) {
    string arg = string( argv[ i ] );
    if ( arg == "-v" ) {
      verbose += 1;
    } else if ( arg == "-vv" ) {
      verbose += 2;
    } else if ( arg == "-vvv" ) {
      verbose += 3;
    };
  };

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
  FrameParameters* parms = infer_parameters( filename );
  std::cout << "Inferred frame parameters: " << format( parms ) << std::endl;

  int loop_count = 1;

  if ( argc >= 3 ) {
    loop_count = std::stoi( argv[ 2 ] );
  };

  // Setup register map
  RegisterMap* regs = new RegisterMap();
  regs->updateMappingTable( parms->constellation, parms->frame_size,
                            parms->code_rate );
  uint32_t data = 1;
  data |= ( 1 << 19 );
  data |= ( 1 << 20 );
  regs->write( 0, data );

  std::cout << "Config register: " << std::hex << regs->read( 0 )
            << std::endl;

  DvbEncoder* encoder = new DvbEncoder();

  std::ifstream istream( filename.c_str() );
  string indata( ( std::istreambuf_iterator< char >( istream ) ),
                 ( std::istreambuf_iterator< char >() ) );

  SPDLOG_INFO( "Read {} bytes from \"{}\"", indata.size(), filename );

  for ( int i = 0; i < loop_count; i++ ) {
    // encoder->send_from_file( parms, filename );
    encoder->send_frame( parms, &indata[ 0 ], indata.size() );
    encoder->receive_frame();
  };

  encoder->join();

  return 0;
}
