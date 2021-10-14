CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs)

# CPPFLAGS=-g -std=c++17 -Wall -Wextra -Werror -Wno-long-long -Wno-variadic-macros -fexceptions
CPPFLAGS += -I third_party/spdlog/include
CPPFLAGS += -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
    # -DNDEBUG
    #  Just so we don't get warnings about unused variables


SRCS=$(wildcard *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: dvb_sw_test

dvb_sw_test: depend $(OBJS)
	$(CXX) $(LDFLAGS) -o dvb_sw_test $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
