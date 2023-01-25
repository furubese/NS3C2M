check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources    
        #test/c2-module-examples-test-suite.cc
        )
endif()    

build_lib(
    LIBNAME c2-module
    SOURCE_FILES model/c2-module.cc
                 helper/c2-module-helper.cc 
                 helper/dynamic-ip-callback-helper.cc
                 helper/myhoney-server-helper.cc
                 model/dynamic-ip-server.cc
                 model/honey-udp-server.cc
                 model/c2-reaction.cc
    HEADER_FILES model/c2-module.h
                 helper/c2-module-helper.h
                 helper/dynamic-ip-callback-helper.h
                 helper/myhoney-server-helper.h
                 model/dynamic-ip-server.h
                 model/honey-udp-server.h
                 model/c2-reaction.h
                 model/json.hpp
    LIBRARIES_TO_LINK
                      ${libinternet}
                      ${libapplications}
                      ${libnetwork}
    TEST_SOURCES test/c2-module-test-suite.cc
                 ${examples_as_tests_sources}
)
    
