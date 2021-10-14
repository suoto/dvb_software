#include <sys/mman.h>
#include <complex>
#include <functional>  // std::ref
#include <future>      // std::promise, std::future
#include <iostream>    // std::cout
#include <thread>      // std::thread
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

int main() {
  // Setup register map
  spdlog::set_level( spdlog::level::trace );

  DvbEncoder* encoder = new DvbEncoder();
  // uint8_t data[] = {0, 1, 2, 3};
  frame* frame = new ::frame{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15};
  FrameParameters* parms = new FrameParameters{C1_4, FECFRAME_SHORT, MOD_QPSK};

  // = (new FrameParameters){};
  // FrameParameters* parms = new FrameParameters(C1_4, FECFRAME_NORMAL,
  // MOD_QPSK);

  // dvb_code_rate_t code_rate;
  // dvb_framesize_t frame_size;
  // dvb_constellation_t constellation;
  // RegisterMap* regs = new RegisterMap();
  // regs->updateMappingTable( MOD_32APSK, FECFRAME_NORMAL, C9_10 );
  // regs->updateMappingTable( MOD_QPSK, FECFRAME_SHORT, C1_4 );
  // regs->read( 1 );
  // regs->read( 0 );
  // regs->write( 1, 0x12345678 );

  // SPDLOG_INFO( "Modulation table:" );
  // for ( size_t i = 0; i < table.size(); i++ ) {
  //   SPDLOG_INFO( "modulation[{}] = {}, {}", i, table[ i ].real(),
  //                 table[ i ].imag() );
  // }

  // delete regs;

  encoder->send_frame( parms, frame );
  // encoder->send_frame( parms, frame );
  // encoder->send_frame( parms, frame );
  encoder->join();

  return 0;
}
