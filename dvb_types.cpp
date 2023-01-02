#include <string>

#include "dvb_types.hpp"

using std::string;

const std::string format( FrameParameters* parms ) {
  string result = "size=";

  switch ( parms->frame_size ) {
    case FECFRAME_SHORT:
      result.append( "SHORT" );
      break;
    case FECFRAME_NORMAL:
      result.append( "NORMAL" );
      break;
    case FECFRAME_MEDIUM:
      result.append( "MEDIUM" );
      break;
  }

  result.append( ", constellation=" );

  switch ( parms->constellation ) {
    case MOD_QPSK:
      result.append( "QPSK" );
      break;
    case MOD_16QAM:
      result.append( "16QAM" );
      break;
    case MOD_64QAM:
      result.append( "64QAM" );
      break;
    case MOD_256QAM:
      result.append( "256QAM" );
      break;
    case MOD_8PSK:
      result.append( "8PSK" );
      break;
    case MOD_8APSK:
      result.append( "8APSK" );
      break;
    case MOD_16APSK:
      result.append( "16APSK" );
      break;
    case MOD_8_8APSK:
      result.append( "8_8APSK" );
      break;
    case MOD_32APSK:
      result.append( "32APSK" );
      break;
    case MOD_4_12_16APSK:
      result.append( "4_12_16APSK" );
      break;
    case MOD_4_8_4_16APSK:
      result.append( "4_8_4_16APSK" );
      break;
    case MOD_64APSK:
      result.append( "64APSK" );
      break;
    case MOD_8_16_20_20APSK:
      result.append( "8_16_20_20APSK" );
      break;
    case MOD_4_12_20_28APSK:
      result.append( "4_12_20_28APSK" );
      break;
    case MOD_128APSK:
      result.append( "128APSK" );
      break;
    case MOD_256APSK:
      result.append( "256APSK" );
      break;
    case MOD_BPSK:
      result.append( "BPSK" );
      break;
    case MOD_BPSK_SF2:
      result.append( "BPSK_SF2" );
      break;
    case MOD_8VSB:
      result.append( "8VSB" );
      break;
    case MOD_OTHER:
      result.append( "OTHER" );
      break;
  }

  result.append( ", code rate=" );
  switch ( parms->code_rate ) {
    case C1_4:
      result.append( "1/4" );
      break;
    case C1_3:
      result.append( "1/3" );
      break;
    case C2_5:
      result.append( "2/5" );
      break;
    case C1_2:
      result.append( "1/2" );
      break;
    case C3_5:
      result.append( "3/5" );
      break;
    case C2_3:
      result.append( "2/3" );
      break;
    case C3_4:
      result.append( "3/4" );
      break;
    case C4_5:
      result.append( "4/5" );
      break;
    case C5_6:
      result.append( "5/6" );
      break;
    case C7_8:
      result.append( "7/8" );
      break;
    case C8_9:
      result.append( "8/9" );
      break;
    case C9_10:
      result.append( "9/10" );
      break;
    case C13_45:
      result.append( "13/45" );
      break;
    case C9_20:
      result.append( "9/20" );
      break;
    case C90_180:
      result.append( "90/180" );
      break;
    case C96_180:
      result.append( "96/180" );
      break;
    case C11_20:
      result.append( "11/20" );
      break;
    case C100_180:
      result.append( "100/180" );
      break;
    case C104_180:
      result.append( "104/180" );
      break;
    case C26_45:
      result.append( "26/45" );
      break;
    case C18_30:
      result.append( "18/30" );
      break;
    case C28_45:
      result.append( "28/45" );
      break;
    case C23_36:
      result.append( "23/36" );
      break;
    case C116_180:
      result.append( "116/180" );
      break;
    case C20_30:
      result.append( "20/30" );
      break;
    case C124_180:
      result.append( "124/180" );
      break;
    case C25_36:
      result.append( "25/36" );
      break;
    case C128_180:
      result.append( "128/180" );
      break;
    case C13_18:
      result.append( "13/18" );
      break;
    case C132_180:
      result.append( "132/180" );
      break;
    case C22_30:
      result.append( "22/30" );
      break;
    case C135_180:
      result.append( "135/180" );
      break;
    case C140_180:
      result.append( "140/180" );
      break;
    case C7_9:
      result.append( "7/9" );
      break;
    case C154_180:
      result.append( "154/180" );
      break;
    case C11_45:
      result.append( "11/45" );
      break;
    case C4_15:
      result.append( "4/15" );
      break;
    case C14_45:
      result.append( "14/45" );
      break;
    case C7_15:
      result.append( "7/15" );
      break;
    case C8_15:
      result.append( "8/15" );
      break;
    case C32_45:
      result.append( "32/45" );
      break;
    case C2_9_VLSNR:
      result.append( "2/9 VLSNR" );
      break;
    case C1_5_MEDIUM:
      result.append( "1/5 MEDIUM" );
      break;
    case C11_45_MEDIUM:
      result.append( "11/45 MEDIUM" );
      break;
    case C1_3_MEDIUM:
      result.append( "1/3 MEDIUM" );
      break;
    case C1_5_VLSNR_SF2:
      result.append( "1/5 VLSNR_SF2" );
      break;
    case C11_45_VLSNR_SF2:
      result.append( "11/45 VLSNR_SF2" );
      break;
    case C1_5_VLSNR:
      result.append( "1/5 VLSNR" );
      break;
    case C4_15_VLSNR:
      result.append( "4/15 VLSNR" );
      break;
    case C1_3_VLSNR:
      result.append( "1/3 VLSNR" );
      break;
    case C_OTHER:
      result.append( "C_OTHER" );
      break;
  }

  return result;
}
