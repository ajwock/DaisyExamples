# Project Name
TARGET = Quantizer

# Sources
CPP_SOURCES = \
QuantizerMain.cpp \
OctaveQuantizer.cpp \
OQManager.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

OctaveQuantizerTest: OctaveQuantizer.cpp OctaveQuantizer.h OctaveQuantizer_gtest.cpp
	g++ -o OctaveQuantizerTest -g -pthread OctaveQuantizer_gtest.cpp -lgtest_main -lgtest -lpthread

OQManagerTest: OctaveQuantizer.cpp OctaveQuantizer.h OQManager.cpp OQManager.h OQManager_gtest.cpp
	g++ -o OQManagerTest -g -pthread OQManager_gtest.cpp OctaveQuantizer.cpp -lgtest_main -lgtest -lpthread -I${DAISYSP_DIR}/Source/

main_test: main_test.cpp OctaveQuantizer.cpp OctaveQuantizer.h
	g++ -o main_test -g main_test.cpp OctaveQuantizer.cpp OctaveQuantizer.h
