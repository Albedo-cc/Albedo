message(NOTICE "[Version]: 3.21.12")

include(target_put_into_folder)

# Environment Configuration
if(WIN32)
    set(PROTOBUF_PLATFORM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/windows)
else()
    message(FATAL_ERROR "Not supported systems!")
endif()

set(PROTOBUF_COMPILER ${PROTOBUF_PLATFORM_PATH}/protoc.exe
    CACHE INTERNAL "The proper compiler of the Protocol Buffers")
message("[Protobuf Compiler] $CACHE{PROTOBUF_COMPILER}")

add_library(protobuf INTERFACE)
target_sources(protobuf PRIVATE $CACHE{PROTOBUF_COMPILER})
target_link_libraries(protobuf 
    INTERFACE
    debug ${PROTOBUF_PLATFORM_PATH}/libprotobufd.lib)
target_link_libraries(protobuf 
    INTERFACE
    optimized ${PROTOBUF_PLATFORM_PATH}/libprotobuf.lib)
target_include_directories(protobuf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)



message(NOTICE "[Function]: makeProtobufTarget(targetName absSrcPath absGenPath)")
# Interfaces
function(makeProtobufTarget targetName absSrcPath absGenPath)
    if (TARGET ${targetName})
        message(FATAL_ERROR "The target ${targetName} existed in this project!")
    endif()

    # 1. Create directory for new protobuf target.
    if(NOT EXISTS ${absGenPath})
        message(FATAL_ERROR "Generate Path is not existent! - ${absGenPath}")
    endif()

    # 2. Glob and compile each .proto file
    file(GLOB allProtoFiles "${absSrcPath}/*.proto")
    if (NOT allProtoFiles)
        message(FATAL_ERROR "No .proto files exist in ${absSrcPath}")
    endif()

    foreach(protoFile IN LISTS allProtoFiles)
        get_filename_component(fileName ${protoFile} NAME_WE)
        set(ProtoHeader "${absGenPath}/${fileName}.pb.h")
        set(ProtoSource "${absGenPath}/${fileName}.pb.cc")
        add_custom_command(
            OUTPUT ${ProtoHeader}
                   ${ProtoSource}
            COMMAND $CACHE{PROTOBUF_COMPILER}
            ARGS --proto_path=${absSrcPath} 
                 --cpp_out=${absGenPath} 
                 ${protoFile}
            DEPENDS protobuf ${protoFile}
            COMMENT "Generated ${ProtoHeader} and ${ProtoSource}"
        )
        list(APPEND ProtoHeaders ${ProtoHeader})
        list(APPEND ProtoSources ${ProtoSource})
    endforeach()

    # 3. Create new protobuf target
    add_library(${targetName} STATIC ${ProtoHeaders} ${ProtoSources})
    target_link_libraries(${targetName} PUBLIC protobuf)
    target_include_directories(${targetName} INTERFACE ${absGenPath})

    # 4. Archive
    target_sources(${targetName} PRIVATE ${allProtoFiles})
    
    message(NOTICE "<Protobuf> Generated a target - ${targetName}")
endfunction(makeProtobufTarget)