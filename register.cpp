#include <sys/mman.h>
#include <iostream>
#include <string>

#include "register.hpp"
#include "spdlog/spdlog.h"

using std::string;

// Member functions definitions including constructor
RegisterMap::RegisterMap( void ) {
  SPDLOG_INFO( "Creating register map" );
  // Setup register map
  if ( ( this->fd = open( XDMA_USER, O_RDWR | O_SYNC ) ) == -1 ) FATAL;
  this->map =
      mmap( 0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0 );
  if ( this->map == (void*)-1 ) FATAL;

  SPDLOG_DEBUG( "Memory mapped at address {:p}", this->map );
  this->writePolyphaseFilterCoefficients();
};

void RegisterMap::write( uint8_t byte_offset, uint32_t data ) {
  uint32_t* byte_address = (uint32_t*)this->map + byte_offset;
  spdlog::trace( "[W] addr=0x{:04X}, data=0x{:08X}", byte_offset, data );
  ( *byte_address ) = data;
};

uint32_t RegisterMap::read( uint8_t byte_offset ) {
  uint32_t* byte_address = (uint32_t*)this->map + byte_offset;
  uint32_t data = *( (uint32_t*)byte_address );
  spdlog::trace( "[R] addr=0x{:04X}, data=0x{:08X}", byte_offset, data );
  return data;
};

RegisterMap::~RegisterMap() {
  SPDLOG_INFO( "Unmapping and closing file descriptor" );
  if ( munmap( map, MAP_SIZE ) == -1 ) FATAL;
  close( fd );
};

std::vector< gr_complex > getModulationTable( dvb_constellation_t constellation,
                                              dvb_framesize_t framesize,
                                              dvb_code_rate_t rate ) {
  double r0, r1, r2, r3;
  double m = 1.0;
  r1 = m;

  switch ( constellation ) {
    case MOD_QPSK:
      return {gr_complex( ( r1 * cos( GR_M_PI / 4.0 ) ),
                          ( r1 * sin( GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 7 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 7 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 3 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 5 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 5 * GR_M_PI / 4.0 ) ) )};
      break;
    case MOD_8PSK:
      return {gr_complex( ( r1 * cos( GR_M_PI / 4.0 ) ),
                          ( r1 * sin( GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 0.0 ) ), ( r1 * sin( 0.0 ) ) ),
              gr_complex( ( r1 * cos( 4 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 4 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 5 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 5 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 2 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 2 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 7 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 7 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 3 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 6 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 6 * GR_M_PI / 4.0 ) ) )};
      break;

    case MOD_16APSK:
      r2 = m;
      if ( framesize == FECFRAME_NORMAL ) {
        switch ( rate ) {
          case C2_3:
            r1 = r2 / 3.15;
            break;
          case C3_4:
            r1 = r2 / 2.85;
            break;
          case C4_5:
            r1 = r2 / 2.75;
            break;
          case C5_6:
            r1 = r2 / 2.70;
            break;
          case C8_9:
            r1 = r2 / 2.60;
            break;
          case C9_10:
            r1 = r2 / 2.57;
            break;
          case C26_45:
          case C3_5:
            r1 = r2 / 3.70;
            break;
          case C28_45:
            r1 = r2 / 3.50;
            break;
          case C23_36:
          case C25_36:
            r1 = r2 / 3.10;
            break;
          case C13_18:
            r1 = r2 / 2.85;
            break;
          case C140_180:
            r1 = r2 / 3.60;
            break;
          case C154_180:
            r1 = r2 / 3.20;
            break;
          default:
            r1 = 0;
            break;
        }
      } else {
        switch ( rate ) {
          case C2_3:
            r1 = r2 / 3.15;
            break;
          case C3_4:
            r1 = r2 / 2.85;
            break;
          case C4_5:
            r1 = r2 / 2.75;
            break;
          case C5_6:
            r1 = r2 / 2.70;
            break;
          case C8_9:
            r1 = r2 / 2.60;
            break;
          case C7_15:
            r1 = r2 / 3.32;
            break;
          case C8_15:
            r1 = r2 / 3.50;
            break;
          case C26_45:
          case C3_5:
            r1 = r2 / 3.70;
            break;
          case C32_45:
            r1 = r2 / 2.85;
            break;
          default:
            r1 = 0;
            break;
        }
      }
      r0 = sqrt( 4.0 / ( ( r1 * r1 ) + 3.0 * ( r2 * r2 ) ) );
      r1 *= r0;
      r2 *= r0;
      return {gr_complex( ( r2 * cos( GR_M_PI / 4.0 ) ),
                          ( r2 * sin( GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( -GR_M_PI / 4.0 ) ),
                          ( r2 * sin( -GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( 3 * GR_M_PI / 4.0 ) ),
                          ( r2 * sin( 3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( -3 * GR_M_PI / 4.0 ) ),
                          ( r2 * sin( -3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( GR_M_PI / 12.0 ) ),
                          ( r2 * sin( GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( -GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( 11 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( 11 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( -11 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -11 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( 5 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( 5 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( -5 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -5 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( 7 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( 7 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( -7 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -7 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r1 * cos( GR_M_PI / 4.0 ) ),
                          ( r1 * sin( GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( -GR_M_PI / 4.0 ) ),
                          ( r1 * sin( -GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( 3 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r1 * cos( -3 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( -3 * GR_M_PI / 4.0 ) ) )};
      break;

    case MOD_32APSK:
      r3 = m;
      switch ( rate ) {
        case C3_4:
          r1 = r3 / 5.27;
          r2 = r1 * 2.84;
          break;
        case C4_5:
          r1 = r3 / 4.87;
          r2 = r1 * 2.72;
          break;
        case C5_6:
          r1 = r3 / 4.64;
          r2 = r1 * 2.64;
          break;
        case C8_9:
          r1 = r3 / 4.33;
          r2 = r1 * 2.54;
          break;
        case C9_10:
          r1 = r3 / 4.30;
          r2 = r1 * 2.53;
          break;
        default:
          r1 = 0;
          r2 = 0;
          break;
      }
      r0 =
          sqrt( 8.0 / ( ( r1 * r1 ) + 3.0 * ( r2 * r2 ) + 4.0 * ( r3 * r3 ) ) );
      r1 *= r0;
      r2 *= r0;
      r3 *= r0;
      return {gr_complex( ( r2 * cos( GR_M_PI / 4.0 ) ),
                          ( r2 * sin( GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( 5 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( 5 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( -GR_M_PI / 4.0 ) ),
                          ( r2 * sin( -GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( -5 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -5 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( 3 * GR_M_PI / 4.0 ) ),
                          ( r2 * sin( 3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( 7 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( 7 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r2 * cos( -3 * GR_M_PI / 4.0 ) ),
                          ( r2 * sin( -3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( -7 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -7 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r3 * cos( GR_M_PI / 8.0 ) ),
                          ( r3 * sin( GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r3 * cos( 3 * GR_M_PI / 8.0 ) ),
                          ( r3 * sin( 3 * GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r3 * cos( -GR_M_PI / 4.0 ) ),
                          ( r3 * sin( -GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r3 * cos( -GR_M_PI / 2.0 ) ),
                          ( r3 * sin( -GR_M_PI / 2.0 ) ) ),
              gr_complex( ( r3 * cos( 3 * GR_M_PI / 4.0 ) ),
                          ( r3 * sin( 3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r3 * cos( GR_M_PI / 2.0 ) ),
                          ( r3 * sin( GR_M_PI / 2.0 ) ) ),
              gr_complex( ( r3 * cos( -7 * GR_M_PI / 8.0 ) ),
                          ( r3 * sin( -7 * GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r3 * cos( -5 * GR_M_PI / 8.0 ) ),
                          ( r3 * sin( -5 * GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r2 * cos( GR_M_PI / 12.0 ) ),
                          ( r2 * sin( GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r1 * cos( GR_M_PI / 4.0 ) ),
                          ( r1 * sin( GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( -GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r1 * cos( -GR_M_PI / 4.0 ) ),
                          ( r1 * sin( -GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( 11 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( 11 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r1 * cos( 3 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( 3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r2 * cos( -11 * GR_M_PI / 12.0 ) ),
                          ( r2 * sin( -11 * GR_M_PI / 12.0 ) ) ),
              gr_complex( ( r1 * cos( -3 * GR_M_PI / 4.0 ) ),
                          ( r1 * sin( -3 * GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r3 * cos( 0.0 ) ), ( r3 * sin( 0.0 ) ) ),
              gr_complex( ( r3 * cos( GR_M_PI / 4.0 ) ),
                          ( r3 * sin( GR_M_PI / 4.0 ) ) ),
              gr_complex( ( r3 * cos( -GR_M_PI / 8.0 ) ),
                          ( r3 * sin( -GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r3 * cos( -3 * GR_M_PI / 8.0 ) ),
                          ( r3 * sin( -3 * GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r3 * cos( 7 * GR_M_PI / 8.0 ) ),
                          ( r3 * sin( 7 * GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r3 * cos( 5 * GR_M_PI / 8.0 ) ),
                          ( r3 * sin( 5 * GR_M_PI / 8.0 ) ) ),
              gr_complex( ( r3 * cos( GR_M_PI ) ), ( r3 * sin( GR_M_PI ) ) ),
              gr_complex( ( r3 * cos( -3 * GR_M_PI / 4.0 ) ),
                          ( r3 * sin( -3 * GR_M_PI / 4.0 ) ) )};
      break;

    default:
      break;
  };

  spdlog::error( "Unsupported constellation {}", constellation );
  return {};
};

void RegisterMap::updateMappingTable( dvb_constellation_t constellation,
                                      dvb_framesize_t framesize,
                                      dvb_code_rate_t rate ) {
  std::vector< gr_complex > table =
      getModulationTable( constellation, framesize, rate );

  SPDLOG_INFO( "Modulation table:" );
  for ( size_t i = 0; i < table.size(); i++ ) {
    SPDLOG_INFO( "modulation[{}] = {}, {}", i, table[ i ].real(),
                  table[ i ].imag() );
  }

  uint32_t offset = 0;

  switch ( constellation ) {
    case MOD_QPSK:
      break;
    case MOD_8PSK:
      offset += 4;
      break;
    case MOD_16APSK:
      offset += 12;
      break;
    case MOD_32APSK:
      offset += 28;
      break;
    default:
      break;
  };

  SPDLOG_INFO( "Writing modulation table to offset 0x{:X}", offset );

  offset += 0xC;

  for ( size_t i = 0; i < table.size(); i++ ) {
    uint16_t real = ( uint16_t )( ( 1 << 15 ) * table[ i ].real() );
    uint16_t imag = ( uint16_t )( ( 1 << 15 ) * table[ i ].imag() );
    this->write( offset + i, real | ( imag << 16 ) );
  }
};

void RegisterMap::writePolyphaseFilterCoefficients( void ) {
  // def write_polyphase_filter_coefficients(self):
  //     self._logger.info("Updating polyphase filter coefficients")
  const float coeffs[] = {
      -0.000728216778953, 0.00181682675611,  -0.00029152361094,
      -0.00169956660829,  0.00198084092699,  0.000321642903145,
      -0.00423926254734,  0.00304758665152,  0.00683168089017,
      -0.00947270914912,  -0.00942199118435, 0.0211963132024,
      0.0116332434118,    -0.0461302995682,  -0.0131214763969,
      0.156597167253,     0.263359487057,    0.156597167253,
      -0.0131214763969,   -0.0461302995682,  0.0116332434118,
      0.0211963132024,    -0.00942199118435, -0.00947270914912,
      0.00683168089017,   0.00304758665152,  -0.00423926254734,
      0.000321642903145,  0.00198084092699,  -0.00169956660829,
      -0.00029152361094,  0.00181682675611,  -0.000728216778953,
  };

  uint32_t offset = 0x3CC;
  for ( size_t i = 0; i < 33; i++ ) {
    uint16_t fixed = ( uint16_t )( ( 1 << 15 ) * coeffs[ i ] );
    this->write( offset + i, fixed | ( fixed << 16 ) );
  }
}
