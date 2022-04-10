# Project Name
TARGET = Sampler

# Sources
CPP_SOURCES = \
Sampler.cpp \
SamplerMain.cpp \
Quantizer.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

test: Sampler.cpp Sampler.h Sampler_gtest.cpp
	g++ -o SamplerTest -g -pthread Sampler_gtest.cpp -lgtest_main -lgtest -lpthread
