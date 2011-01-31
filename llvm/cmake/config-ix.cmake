include(CheckIncludeFile)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(TestBigEndian)

if( UNIX AND NOT BEOS )
  # Used by check_symbol_exists:
  set(CMAKE_REQUIRED_LIBRARIES m)
endif()

# Helper macros and functions
macro(add_cxx_include result files)
  set(${result} "")
  foreach (file_name ${files})
     set(${result} "${${result}}#include<${file_name}>\n")
  endforeach()
endmacro(add_cxx_include files result)

function(check_type_exists type files variable)
  add_cxx_include(includes "${files}")
  CHECK_CXX_SOURCE_COMPILES("
    ${includes} ${type} typeVar;
    int main() {
        return 0;
    }
    " ${variable})
endfunction()

# include checks
check_include_file(argz.h HAVE_ARGZ_H)
check_include_file(assert.h HAVE_ASSERT_H)
check_include_file(ctype.h HAVE_CTYPE_H)
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(dl.h HAVE_DL_H)
check_include_file(dld.h HAVE_DLD_H)
check_include_file(dlfcn.h HAVE_DLFCN_H)
check_include_file(errno.h HAVE_ERRNO_H)
check_include_file(execinfo.h HAVE_EXECINFO_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(link.h HAVE_LINK_H)
check_include_file(malloc.h HAVE_MALLOC_H)
check_include_file(malloc/malloc.h HAVE_MALLOC_MALLOC_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(ndir.h HAVE_NDIR_H)
if( NOT LLVM_ON_WIN32 )
  check_include_file(pthread.h HAVE_PTHREAD_H)
endif()
check_include_file(setjmp.h HAVE_SETJMP_H)
check_include_file(signal.h HAVE_SIGNAL_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdio.h HAVE_STDIO_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(sys/dir.h HAVE_SYS_DIR_H)
check_include_file(sys/dl.h HAVE_SYS_DL_H)
check_include_file(sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_file(sys/mman.h HAVE_SYS_MMAN_H)
check_include_file(sys/ndir.h HAVE_SYS_NDIR_H)
check_include_file(sys/param.h HAVE_SYS_PARAM_H)
check_include_file(sys/resource.h HAVE_SYS_RESOURCE_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file(termios.h HAVE_TERMIOS_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(utime.h HAVE_UTIME_H)
check_include_file(valgrind/valgrind.h HAVE_VALGRIND_VALGRIND_H)
check_include_file(windows.h HAVE_WINDOWS_H)
check_include_file(fenv.h HAVE_FENV_H)
check_include_file(mach/mach.h HAVE_MACH_MACH_H)
check_include_file(mach-o/dyld.h HAVE_MACH_O_DYLD_H)

# library checks
if( NOT LLVM_ON_WIN32 )
  check_library_exists(pthread pthread_create "" HAVE_LIBPTHREAD)
  check_library_exists(pthread pthread_getspecific "" HAVE_PTHREAD_GETSPECIFIC)
  check_library_exists(pthread pthread_rwlock_init "" HAVE_PTHREAD_RWLOCK_INIT)
  check_library_exists(dl dlopen "" HAVE_LIBDL)
endif()

# function checks
check_symbol_exists(getpagesize unistd.h HAVE_GETPAGESIZE)
check_symbol_exists(getrusage sys/resource.h HAVE_GETRUSAGE)
check_symbol_exists(setrlimit sys/resource.h HAVE_SETRLIMIT)
check_function_exists(isatty HAVE_ISATTY)
check_symbol_exists(index strings.h HAVE_INDEX)
check_symbol_exists(isinf cmath HAVE_ISINF_IN_CMATH)
check_symbol_exists(isinf math.h HAVE_ISINF_IN_MATH_H)
check_symbol_exists(finite ieeefp.h HAVE_FINITE_IN_IEEEFP_H)
check_symbol_exists(isnan cmath HAVE_ISNAN_IN_CMATH)
check_symbol_exists(isnan math.h HAVE_ISNAN_IN_MATH_H)
check_symbol_exists(ceilf math.h HAVE_CEILF)
check_symbol_exists(floorf math.h HAVE_FLOORF)
check_symbol_exists(fmodf math.h HAVE_FMODF)
if( HAVE_SETJMP_H )
  check_symbol_exists(longjmp setjmp.h HAVE_LONGJMP)
  check_symbol_exists(setjmp setjmp.h HAVE_SETJMP)
  check_symbol_exists(siglongjmp setjmp.h HAVE_SIGLONGJMP)
  check_symbol_exists(sigsetjmp setjmp.h HAVE_SIGSETJMP)
endif()
check_symbol_exists(nearbyintf math.h HAVE_NEARBYINTF)
check_symbol_exists(mallinfo malloc.h HAVE_MALLINFO)
check_symbol_exists(malloc_zone_statistics malloc/malloc.h
                    HAVE_MALLOC_ZONE_STATISTICS)
check_symbol_exists(mkdtemp "stdlib.h;unistd.h" HAVE_MKDTEMP)
check_symbol_exists(mkstemp "stdlib.h;unistd.h" HAVE_MKSTEMP)
check_symbol_exists(mktemp "stdlib.h;unistd.h" HAVE_MKTEMP)
check_symbol_exists(closedir "sys/types.h;dirent.h" HAVE_CLOSEDIR)
check_symbol_exists(opendir "sys/types.h;dirent.h" HAVE_OPENDIR)
check_symbol_exists(readdir "sys/types.h;dirent.h" HAVE_READDIR)
check_symbol_exists(getcwd unistd.h HAVE_GETCWD)
check_symbol_exists(gettimeofday sys/time.h HAVE_GETTIMEOFDAY)
check_symbol_exists(getrlimit "sys/types.h;sys/time.h;sys/resource.h" HAVE_GETRLIMIT)
check_symbol_exists(rindex strings.h HAVE_RINDEX)
check_symbol_exists(strchr string.h HAVE_STRCHR)
check_symbol_exists(strcmp string.h HAVE_STRCMP)
check_symbol_exists(strdup string.h HAVE_STRDUP)
check_symbol_exists(strrchr string.h HAVE_STRRCHR)
if( NOT LLVM_ON_WIN32 )
  check_symbol_exists(pthread_mutex_lock pthread.h HAVE_PTHREAD_MUTEX_LOCK)
endif()
check_symbol_exists(sbrk unistd.h HAVE_SBRK)
check_symbol_exists(srand48 stdlib.h HAVE_RAND48_SRAND48)
if( HAVE_RAND48_SRAND48 )
  check_symbol_exists(lrand48 stdlib.h HAVE_RAND48_LRAND48)
  if( HAVE_RAND48_LRAND48 )
    check_symbol_exists(drand48 stdlib.h HAVE_RAND48_DRAND48)
    if( HAVE_RAND48_DRAND48 )
      set(HAVE_RAND48 1 CACHE INTERNAL "are srand48/lrand48/drand48 available?")
    endif()
  endif()
endif()
check_symbol_exists(strtoll stdlib.h HAVE_STRTOLL)
check_symbol_exists(strtoq stdlib.h HAVE_STRTOQ)
check_symbol_exists(strerror string.h HAVE_STRERROR)
check_symbol_exists(strerror_r string.h HAVE_STRERROR_R)
check_symbol_exists(strerror_s string.h HAVE_STRERROR_S)
check_symbol_exists(memcpy string.h HAVE_MEMCPY)
check_symbol_exists(memmove string.h HAVE_MEMMOVE)
check_symbol_exists(setenv stdlib.h HAVE_SETENV)
if ( LLVM_ON_WIN32 )
  check_symbol_exists(_chsize_s io.h HAVE__CHSIZE_S)
endif()
if( HAVE_ARGZ_H )
  check_symbol_exists(argz_append argz.h HAVE_ARGZ_APPEND)
  check_symbol_exists(argz_create_sep argz.h HAVE_ARGZ_CREATE_SEP)
  check_symbol_exists(argz_insert argz.h HAVE_ARGZ_INSERT)
  check_symbol_exists(argz_next argz.h HAVE_ARGZ_NEXT)
  check_symbol_exists(argz_stringify argz.h HAVE_ARGZ_STRINGIFY)
endif()
if( HAVE_DLFCN_H )
  if( HAVE_LIBDL )
    list(APPEND CMAKE_REQUIRED_LIBRARIES dl)
  endif()
  check_symbol_exists(dlerror dlfcn.h HAVE_DLERROR)
  check_symbol_exists(dlopen dlfcn.h HAVE_DLOPEN)
  if( HAVE_LIBDL )
    list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES dl)
  endif()
endif()

check_symbol_exists(__GLIBC__ stdio.h LLVM_USING_GLIBC)
if( LLVM_USING_GLIBC )
  add_llvm_definitions( -D_GNU_SOURCE )
endif()

# Type checks
check_type_exists(std::bidirectional_iterator<int,int> "iterator;iostream" HAVE_BI_ITERATOR)
check_type_exists(std::iterator<int,int,int> iterator HAVE_STD_ITERATOR)
check_type_exists(std::forward_iterator<int,int> iterator HAVE_FWD_ITERATOR)

set(headers "")
if (HAVE_SYS_TYPES_H)
  set(headers ${headers} "sys/types.h")
endif()

if (HAVE_INTTYPES_H)
  set(headers ${headers} "inttypes.h")
endif()

if (HAVE_STDINT_H)
  set(headers ${headers} "stdint.h")
endif()

check_type_exists(int64_t "${headers}" HAVE_INT64_T)
check_type_exists(uint64_t "${headers}" HAVE_UINT64_T)
check_type_exists(u_int64_t "${headers}" HAVE_U_INT64_T)
check_type_exists(error_t errno.h HAVE_ERROR_T)

# available programs checks
function(llvm_find_program name)
  string(TOUPPER ${name} NAME)
  find_program(LLVM_PATH_${NAME} ${name})
  mark_as_advanced(LLVM_PATH_${NAME})
  if(LLVM_PATH_${NAME})
    set(HAVE_${NAME} 1 CACHE INTERNAL "Is ${name} available ?")
    mark_as_advanced(HAVE_${NAME})
  else(LLVM_PATH_${NAME})
    set(HAVE_${NAME} "" CACHE INTERNAL "Is ${name} available ?")
  endif(LLVM_PATH_${NAME})
endfunction()

llvm_find_program(gv)
llvm_find_program(circo)
llvm_find_program(twopi)
llvm_find_program(neato)
llvm_find_program(fdp)
llvm_find_program(dot)
llvm_find_program(dotty)

if( LLVM_ENABLE_FFI )
  find_path(FFI_INCLUDE_PATH ffi.h PATHS ${FFI_INCLUDE_DIR})
  if( FFI_INCLUDE_PATH )
    set(FFI_HEADER ffi.h CACHE INTERNAL "")
    set(HAVE_FFI_H 1 CACHE INTERNAL "")
  else()
    find_path(FFI_INCLUDE_PATH ffi/ffi.h PATHS ${FFI_INCLUDE_DIR})
    if( FFI_INCLUDE_PATH )
      set(FFI_HEADER ffi/ffi.h CACHE INTERNAL "")
      set(HAVE_FFI_FFI_H 1 CACHE INTERNAL "")
    endif()
  endif()

  if( NOT FFI_HEADER )
    message(FATAL_ERROR "libffi includes are not found.")
  endif()

  find_library(FFI_LIBRARY_PATH ffi PATHS ${FFI_LIBRARY_DIR})
  if( NOT FFI_LIBRARY_PATH )
    message(FATAL_ERROR "libffi is not found.")
  endif()

  list(APPEND CMAKE_REQUIRED_LIBRARIES ${FFI_LIBRARY_PATH})
  list(APPEND CMAKE_REQUIRED_INCLUDES ${FFI_INCLUDE_PATH})
  check_symbol_exists(ffi_call ${FFI_HEADER} HAVE_FFI_CALL)
  list(REMOVE_ITEM CMAKE_REQUIRED_INCLUDES ${FFI_INCLUDE_PATH})
  list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES ${FFI_LIBRARY_PATH})
endif( LLVM_ENABLE_FFI )

# Define LLVM_MULTITHREADED if gcc atomic builtins exists.
include(CheckAtomic)

if( LLVM_ENABLE_PIC )
  set(ENABLE_PIC 1)
else()
  set(ENABLE_PIC 0)
endif()

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-Wno-variadic-macros" SUPPORTS_NO_VARIADIC_MACROS_FLAG)

include(GetTargetTriple)
get_target_triple(LLVM_HOSTTRIPLE)

# FIXME: We don't distinguish the target and the host. :(
set(TARGET_TRIPLE "${LLVM_HOSTTRIPLE}")

# Determine the native architecture.
string(TOLOWER "${LLVM_TARGET_ARCH}" LLVM_NATIVE_ARCH)
if( LLVM_NATIVE_ARCH STREQUAL "host" )
  string(REGEX MATCH "^[^-]*" LLVM_NATIVE_ARCH ${LLVM_HOSTTRIPLE})
endif ()

if (LLVM_NATIVE_ARCH MATCHES "i[2-6]86")
  set(LLVM_NATIVE_ARCH X86)
elseif (LLVM_NATIVE_ARCH STREQUAL "x86")
  set(LLVM_NATIVE_ARCH X86)
elseif (LLVM_NATIVE_ARCH STREQUAL "amd64")
  set(LLVM_NATIVE_ARCH X86)
elseif (LLVM_NATIVE_ARCH STREQUAL "x86_64")
  set(LLVM_NATIVE_ARCH X86)
elseif (LLVM_NATIVE_ARCH MATCHES "sparc")
  set(LLVM_NATIVE_ARCH Sparc)
elseif (LLVM_NATIVE_ARCH MATCHES "powerpc")
  set(LLVM_NATIVE_ARCH PowerPC)
elseif (LLVM_NATIVE_ARCH MATCHES "alpha")
  set(LLVM_NATIVE_ARCH Alpha)
elseif (LLVM_NATIVE_ARCH MATCHES "arm")
  set(LLVM_NATIVE_ARCH ARM)
elseif (LLVM_NATIVE_ARCH MATCHES "mips")
  set(LLVM_NATIVE_ARCH Mips)
elseif (LLVM_NATIVE_ARCH MATCHES "xcore")
  set(LLVM_NATIVE_ARCH XCore)
elseif (LLVM_NATIVE_ARCH MATCHES "msp430")
  set(LLVM_NATIVE_ARCH MSP430)
else ()
  message(STATUS
    "Unknown architecture ${LLVM_NATIVE_ARCH}; lli will not JIT code")
  set(LLVM_NATIVE_ARCH)
endif ()

if (LLVM_NATIVE_ARCH)
  list(FIND LLVM_TARGETS_TO_BUILD ${LLVM_NATIVE_ARCH} NATIVE_ARCH_IDX)
  if (NATIVE_ARCH_IDX EQUAL -1)
    message(STATUS
      "Native target ${LLVM_NATIVE_ARCH} is not selected; lli will not JIT code")
    set(LLVM_NATIVE_ARCH)
  else ()
    message(STATUS "Native target architecture is ${LLVM_NATIVE_ARCH}")
    set(LLVM_NATIVE_TARGET LLVMInitialize${LLVM_NATIVE_ARCH}Target)
    set(LLVM_NATIVE_TARGETINFO LLVMInitialize${LLVM_NATIVE_ARCH}TargetInfo)
    set(LLVM_NATIVE_ASMPRINTER LLVMInitialize${LLVM_NATIVE_ARCH}AsmPrinter)
  endif ()
endif()

if( MINGW )
  set(HAVE_LIBIMAGEHLP 1)
  set(HAVE_LIBPSAPI 1)
  # TODO: Check existence of libraries.
  #   include(CheckLibraryExists)
  #   CHECK_LIBRARY_EXISTS(imagehlp ??? . HAVE_LIBIMAGEHLP)
endif( MINGW )

if( MSVC )
  set(error_t int)
  set(mode_t "unsigned short")
  set(LTDL_SHLIBPATH_VAR "PATH")
  set(LTDL_SYSSEARCHPATH "")
  set(LTDL_DLOPEN_DEPLIBS 1)
  set(SHLIBEXT ".lib")
  set(LTDL_OBJDIR "_libs")
  set(HAVE_STRTOLL 1)
  set(strtoll "_strtoi64")
  set(strtoull "_strtoui64")
  set(stricmp "_stricmp")
  set(strdup "_strdup")
else( MSVC )
  set(LTDL_SHLIBPATH_VAR "LD_LIBRARY_PATH")
  set(LTDL_SYSSEARCHPATH "") # TODO
  set(LTDL_DLOPEN_DEPLIBS 0)  # TODO
endif( MSVC )

# FIXME: Signal handler return type, currently hardcoded to 'void'
set(RETSIGTYPE void)

if( LLVM_ENABLE_THREADS )
  if( HAVE_PTHREAD_H OR WIN32 )
    set(ENABLE_THREADS 1)
  endif()
endif()

if( ENABLE_THREADS )
  message(STATUS "Threads enabled.")
else( ENABLE_THREADS )
  message(STATUS "Threads disabled.")
endif()

set(LLVM_PREFIX ${CMAKE_INSTALL_PREFIX})

configure_file(
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Config/config.h.cmake
  ${LLVM_BINARY_DIR}/include/llvm/Config/config.h
  )

configure_file(
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Config/llvm-config.h.cmake
  ${LLVM_BINARY_DIR}/include/llvm/Config/llvm-config.h
  )

configure_file(
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Support/DataTypes.h.cmake
  ${LLVM_BINARY_DIR}/include/llvm/Support/DataTypes.h
  )

