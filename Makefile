CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs)

# CPPFLAGS=-g -std=c++17 -Wall -Wextra -Werror -Wno-long-long -Wno-variadic-macros -fexceptions
# CPPFLAGS += -std=c++11
CPPFLAGS += -I third_party/spdlog/include
# CPPFLAGS += -I /usr/src/linux-hwe-5.11-headers-5.11.0-37/include/
# CPPFLAGS += -I /home/souto/dev/dma_ip_drivers/XDMA/linux-kernel/include/
CPPFLAGS += -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
# CPPFLAGS += -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
# CPPFLAGS += -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO
# CPPFLAGS += -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN
    # -DNDEBUG
    #  Just so we don't get warnings about unused variables


SRCS=$(wildcard *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: dvb_sw_test

dvb_sw_test: $(OBJS)
	$(CXX) $(LDFLAGS) -o dvb_sw_test $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

SUPPORTED_CONFIGS := FECFRAME_SHORT_MOD_QPSK_C8_9 \
										 FECFRAME_SHORT_MOD_QPSK_C5_6 \
										 FECFRAME_SHORT_MOD_QPSK_C4_5 \
										 FECFRAME_SHORT_MOD_QPSK_C3_5 \
										 FECFRAME_SHORT_MOD_QPSK_C3_4 \
										 FECFRAME_SHORT_MOD_QPSK_C2_5 \
										 FECFRAME_SHORT_MOD_QPSK_C2_3 \
										 FECFRAME_SHORT_MOD_QPSK_C1_4 \
										 FECFRAME_SHORT_MOD_QPSK_C1_3 \
										 FECFRAME_SHORT_MOD_QPSK_C1_2 \
										 FECFRAME_SHORT_MOD_8PSK_C8_9 \
										 FECFRAME_SHORT_MOD_8PSK_C5_6 \
										 FECFRAME_SHORT_MOD_8PSK_C3_5 \
										 FECFRAME_SHORT_MOD_8PSK_C3_4 \
										 FECFRAME_SHORT_MOD_8PSK_C2_3 \
										 FECFRAME_SHORT_MOD_32APSK_C8_9 \
										 FECFRAME_SHORT_MOD_32APSK_C5_6 \
										 FECFRAME_SHORT_MOD_32APSK_C4_5 \
										 FECFRAME_SHORT_MOD_32APSK_C3_4 \
										 FECFRAME_SHORT_MOD_16APSK_C8_9 \
										 FECFRAME_SHORT_MOD_16APSK_C5_6 \
										 FECFRAME_SHORT_MOD_16APSK_C4_5 \
										 FECFRAME_SHORT_MOD_16APSK_C3_4 \
										 FECFRAME_SHORT_MOD_16APSK_C2_3 \
										 FECFRAME_NORMAL_MOD_QPSK_C9_10 \
										 FECFRAME_NORMAL_MOD_QPSK_C8_9 \
										 FECFRAME_NORMAL_MOD_QPSK_C5_6 \
										 FECFRAME_NORMAL_MOD_QPSK_C4_5 \
										 FECFRAME_NORMAL_MOD_QPSK_C3_5 \
										 FECFRAME_NORMAL_MOD_QPSK_C3_4 \
										 FECFRAME_NORMAL_MOD_QPSK_C2_5 \
										 FECFRAME_NORMAL_MOD_QPSK_C2_3 \
										 FECFRAME_NORMAL_MOD_QPSK_C1_4 \
										 FECFRAME_NORMAL_MOD_QPSK_C1_3 \
										 FECFRAME_NORMAL_MOD_QPSK_C1_2 \
										 FECFRAME_NORMAL_MOD_8PSK_C9_10 \
										 FECFRAME_NORMAL_MOD_8PSK_C8_9 \
										 FECFRAME_NORMAL_MOD_8PSK_C5_6 \
										 FECFRAME_NORMAL_MOD_8PSK_C3_5 \
										 FECFRAME_NORMAL_MOD_8PSK_C3_4 \
										 FECFRAME_NORMAL_MOD_8PSK_C2_3 \
										 FECFRAME_NORMAL_MOD_32APSK_C9_10 \
										 FECFRAME_NORMAL_MOD_32APSK_C8_9 \
										 FECFRAME_NORMAL_MOD_32APSK_C5_6 \
										 FECFRAME_NORMAL_MOD_32APSK_C4_5 \
										 FECFRAME_NORMAL_MOD_32APSK_C3_4 \
										 FECFRAME_NORMAL_MOD_16APSK_C9_10 \
										 FECFRAME_NORMAL_MOD_16APSK_C8_9 \
										 FECFRAME_NORMAL_MOD_16APSK_C5_6 \
										 FECFRAME_NORMAL_MOD_16APSK_C4_5 \
										 FECFRAME_NORMAL_MOD_16APSK_C3_4 \
										 FECFRAME_NORMAL_MOD_16APSK_C2_3

DATASET_PATH := $(PWD)/../dataset

dataset: $(SUPPORTED_CONFIGS)

data:
	mkdir -p $(DATASET_PATH)

FECFRAME_%: $(DATASET_PATH)
	cp ~/phase4ground/dvb_fpga/gnuradio_data/$@/bb_header_output_packed.bin $(DATASET_PATH)/$@_input.bin
	cp ~/phase4ground/dvb_fpga/gnuradio_data/$@/modulated_pilots_off_fixed_point.bin $(DATASET_PATH)/$@_output.bin

TESTS := test_FECFRAME_SHORT_MOD_QPSK_C8_9 \
				 test_FECFRAME_SHORT_MOD_QPSK_C5_6 \
				 test_FECFRAME_SHORT_MOD_QPSK_C4_5 \
				 test_FECFRAME_SHORT_MOD_QPSK_C3_5 \
				 test_FECFRAME_SHORT_MOD_QPSK_C3_4 \
				 test_FECFRAME_SHORT_MOD_QPSK_C2_5 \
				 test_FECFRAME_SHORT_MOD_QPSK_C2_3 \
				 test_FECFRAME_SHORT_MOD_QPSK_C1_4 \
				 test_FECFRAME_SHORT_MOD_QPSK_C1_3 \
				 test_FECFRAME_SHORT_MOD_QPSK_C1_2 \
				 test_FECFRAME_SHORT_MOD_8PSK_C8_9 \
				 test_FECFRAME_SHORT_MOD_8PSK_C5_6 \
				 test_FECFRAME_SHORT_MOD_8PSK_C3_5 \
				 test_FECFRAME_SHORT_MOD_8PSK_C3_4 \
				 test_FECFRAME_SHORT_MOD_8PSK_C2_3 \
				 test_FECFRAME_SHORT_MOD_32APSK_C8_9 \
				 test_FECFRAME_SHORT_MOD_32APSK_C5_6 \
				 test_FECFRAME_SHORT_MOD_32APSK_C4_5 \
				 test_FECFRAME_SHORT_MOD_32APSK_C3_4 \
				 test_FECFRAME_SHORT_MOD_16APSK_C8_9 \
				 test_FECFRAME_SHORT_MOD_16APSK_C5_6 \
				 test_FECFRAME_SHORT_MOD_16APSK_C4_5 \
				 test_FECFRAME_SHORT_MOD_16APSK_C3_4 \
				 test_FECFRAME_SHORT_MOD_16APSK_C2_3

				 # test_FECFRAME_NORMAL_MOD_QPSK_C9_10 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C8_9 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C5_6 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C4_5 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C3_5 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C3_4 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C2_5 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C2_3 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C1_4 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C1_3 \
				 # test_FECFRAME_NORMAL_MOD_QPSK_C1_2 \
				 # test_FECFRAME_NORMAL_MOD_8PSK_C9_10 \
				 # test_FECFRAME_NORMAL_MOD_8PSK_C8_9 \
				 # test_FECFRAME_NORMAL_MOD_8PSK_C5_6 \
				 # test_FECFRAME_NORMAL_MOD_8PSK_C3_5 \
				 # test_FECFRAME_NORMAL_MOD_8PSK_C3_4 \
				 # test_FECFRAME_NORMAL_MOD_8PSK_C2_3 \
				 # test_FECFRAME_NORMAL_MOD_32APSK_C9_10 \
				 # test_FECFRAME_NORMAL_MOD_32APSK_C8_9 \
				 # test_FECFRAME_NORMAL_MOD_32APSK_C5_6 \
				 # test_FECFRAME_NORMAL_MOD_32APSK_C4_5 \
				 # test_FECFRAME_NORMAL_MOD_32APSK_C3_4 \
				 # test_FECFRAME_NORMAL_MOD_16APSK_C9_10 \
				 # test_FECFRAME_NORMAL_MOD_16APSK_C8_9 \
				 # test_FECFRAME_NORMAL_MOD_16APSK_C5_6 \
				 # test_FECFRAME_NORMAL_MOD_16APSK_C4_5 \
				 # test_FECFRAME_NORMAL_MOD_16APSK_C3_4 \
				 # test_FECFRAME_NORMAL_MOD_16APSK_C2_3

test_all: dvb_sw_test $(TESTS)

test_qpsk: \
	dvb_sw_test \
	test_FECFRAME_SHORT_MOD_QPSK_C8_9 \
	test_FECFRAME_SHORT_MOD_QPSK_C5_6 \
	test_FECFRAME_SHORT_MOD_QPSK_C4_5 \
	test_FECFRAME_SHORT_MOD_QPSK_C3_5 \
	test_FECFRAME_SHORT_MOD_QPSK_C3_4 \
	test_FECFRAME_SHORT_MOD_QPSK_C2_5 \
	test_FECFRAME_SHORT_MOD_QPSK_C2_3 \
	test_FECFRAME_SHORT_MOD_QPSK_C1_4 \
	test_FECFRAME_SHORT_MOD_QPSK_C1_3 \
	test_FECFRAME_SHORT_MOD_QPSK_C1_2

test_FECFRAME_%:
	./dvb_sw_test ~/data/FECFRAME_$*_input.bin 1
	~/compare.py output.bin  ~/data/FECFRAME_$*_output.bin > /dev/null

include .depend
