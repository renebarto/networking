# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
Findlibpcap
-----------

Find the PCAP libraries

IMPORTED targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` target:

``pcap``

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables if found:

``PCAP_INCLUDE_DIRS``
  where to find pcap.h, etc.
``PCAP_LIBRARIES``
  the libraries to link against to use libpcap.
``PCAP_VERSION``
  libpcap version.
``PCAP_FOUND``
  TRUE if found

#]=======================================================================]

# Look for the necessary header
find_path(PCAP_INCLUDE_DIR 
    NAMES pcap.h
    PATHS /usr/include)
mark_as_advanced(PCAP_INCLUDE_DIR)
message(STATUS ${PCAP_INCLUDE_DIR})

# Look for the necessary library
find_library(PCAP_LIBRARY 
    NAMES pcap
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu)
mark_as_advanced(PCAP_LIBRARY)
message(STATUS ${PCAP_LIBRARY})

# Extract version information from the header file
if(PCAP_INCLUDE_DIR)
    file(STRINGS ${PCAP_INCLUDE_DIR}/pcap/pcap.h VERSION_MAJOR
         REGEX "^#define PCAP_VERSION_MAJOR  *[0-9]+"
         LIMIT_COUNT 1)
    file(STRINGS ${PCAP_INCLUDE_DIR}/pcap/pcap.h VERSION_MINOR
         REGEX "^#define PCAP_VERSION_MINOR  *[0-9]+"
         LIMIT_COUNT 1)
    set(PCAP_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
    unset(VERSION_MAJOR)
    unset(VERSION_MINOR)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCAP
    REQUIRED_VARS PCAP_INCLUDE_DIR PCAP_LIBRARY
    VERSION_VAR PCAP_VERSION)

# Create the imported target
if(PCAP_FOUND)
    set(PCAP_INCLUDE_DIRS ${PCAP_INCLUDE_DIR})
    set(PCAP_LIBRARIES ${PCAP_LIBRARY})
endif()
