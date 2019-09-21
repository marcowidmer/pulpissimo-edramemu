COMMONDIR = ../common

PULP_APP = test
PULP_APP_FC_SRCS = $(wildcard *.c) $(wildcard $(COMMONDIR)/*.c)
PULP_CFLAGS = -I $(COMMONDIR) -O3 -g
PULP_LDFLAGS = -lm

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
