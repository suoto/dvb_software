#pragma once
#include <complex>
#include <vector>

#define GR_M_PI 3.14159265358979323846 /* pi */

enum dvb_code_rate_t {
  C1_4 = 0,
  C1_3,
  C2_5,
  C1_2,
  C3_5,
  C2_3,
  C3_4,
  C4_5,
  C5_6,
  C7_8,
  C8_9,
  C9_10,
  C13_45,
  C9_20,
  C90_180,
  C96_180,
  C11_20,
  C100_180,
  C104_180,
  C26_45,
  C18_30,
  C28_45,
  C23_36,
  C116_180,
  C20_30,
  C124_180,
  C25_36,
  C128_180,
  C13_18,
  C132_180,
  C22_30,
  C135_180,
  C140_180,
  C7_9,
  C154_180,
  C11_45,
  C4_15,
  C14_45,
  C7_15,
  C8_15,
  C32_45,
  C2_9_VLSNR,
  C1_5_MEDIUM,
  C11_45_MEDIUM,
  C1_3_MEDIUM,
  C1_5_VLSNR_SF2,
  C11_45_VLSNR_SF2,
  C1_5_VLSNR,
  C4_15_VLSNR,
  C1_3_VLSNR,
  C_OTHER,
};

enum dvb_framesize_t {
  FECFRAME_SHORT = 0,
  FECFRAME_NORMAL,
  FECFRAME_MEDIUM,
};

enum dvb_constellation_t {
  MOD_QPSK = 0,
  MOD_16QAM,
  MOD_64QAM,
  MOD_256QAM,
  MOD_8PSK,
  MOD_8APSK,
  MOD_16APSK,
  MOD_8_8APSK,
  MOD_32APSK,
  MOD_4_12_16APSK,
  MOD_4_8_4_16APSK,
  MOD_64APSK,
  MOD_8_16_20_20APSK,
  MOD_4_12_20_28APSK,
  MOD_128APSK,
  MOD_256APSK,
  MOD_BPSK,
  MOD_BPSK_SF2,
  MOD_8VSB,
  MOD_OTHER,
};

// typedef dvb_standard_t dvb_standard_t;
typedef dvb_code_rate_t dvb_code_rate_t;
typedef dvb_framesize_t dvb_framesize_t;
typedef dvb_constellation_t dvb_constellation_t;
// typedef dvb_guardinterval_t dvb_guardinterval_t;

typedef std::complex< float > gr_complex;

typedef struct FrameParameters {
  dvb_framesize_t frame_size;
  dvb_constellation_t constellation;
  dvb_code_rate_t code_rate;
  bool pilots;
} FrameParameters;

const std::string format( FrameParameters* parms );
