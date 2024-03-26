# find JEGA libraries
set(JEGA_DIR "/home/anarkhede/tinkercliffs/JEGA")
set(JEGA_INSTALL "/home/anarkhede/tinkercliffs/JEGA/build/release")
find_library(JEGA_LIB REQUIRED NAMES libjega.so HINTS "${JEGA_INSTALL}/lib")
find_library(JEGA_ED_LIB REQUIRED NAMES libeutils.so HINTS "${JEGA_INSTALL}/lib")
find_library(JEGA_FE_LIB REQUIRED NAMES libjega_fe.so HINTS "${JEGA_INSTALL}/lib")
find_library(JEGA_UT_LIB REQUIRED NAMES libutilities.so HINTS "${JEGA_INSTALL}/lib")
#find_library(JEGA_ET_LIB REQUIRED NAMES libethreads.so HINTS "${JEGA_INSTALL}/lib")
find_library(SOGA_LIB REQUIRED NAMES libsoga.so HINTS "${JEGA_INSTALL}/lib")
find_library(MOGA_LIB REQUIRED NAMES libmoga.so HINTS "${JEGA_INSTALL}/lib")
 
add_library(jega SHARED IMPORTED)
add_library(jega_fe SHARED IMPORTED)
add_library(utilities SHARED IMPORTED)
add_library(eutils SHARED IMPORTED)
add_library(soga SHARED IMPORTED)
add_library(moga SHARED IMPORTED)

set_property(TARGET jega APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(jega PROPERTIES 
IMPORTED_LOCATION_NOCONFIG ${JEGA_LIB} 
IMPORTED_SONAME_NOCONFIG "libjega.so"
)

set_property(TARGET jega_fe APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(jega_fe PROPERTIES
IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "moga;soga;eutils"
IMPORTED_LOCATION_NOCONFIG ${JEGA_FE_LIB} 
IMPORTED_SONAME_NOCONFIG "libjega_fe.so"
)

set_property(TARGET soga APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(soga PROPERTIES 
IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "jega;utilities"
IMPORTED_LOCATION_NOCONFIG ${SOGA_LIB}
IMPORTED_SONAME_NOCONFIG "libsoga.so"
)

set_property(TARGET moga APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(moga PROPERTIES 
IMPORTED_LINK_INTERFACE_LIBRARIES_NOCONFIG "jega;utilities"
IMPORTED_LOCATION_NOCONFIG ${MOGA_LIB}
IMPORTED_SONAME_NOCONFIG "libmoga.so"
)

set_property(TARGET eutils APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(eutils PROPERTIES 
IMPORTED_LOCATION_NOCONFIG ${JEGA_ED_LIB} 
IMPORTED_SONAME_NOCONFIG "libeutils.so"
)

set_property(TARGET utilities APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(utilities PROPERTIES 
IMPORTED_LOCATION_NOCONFIG ${JEGA_UT_LIB} 
IMPORTED_SONAME_NOCONFIG "libutilities.so"
)

include_directories("${JEGA_DIR}/include")
include_directories("${JEGA_DIR}/eddy")
