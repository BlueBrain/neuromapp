# Set variables representing corresponding command-line options for
# various compiler functionality

include(CheckCXXCompilerFlag)

function(first_cxx_flag_of VAR)
    # note that check_cxx_compiler_flag sets its VAR argument as a cache variable
    foreach(flag IN LISTS ARGN)
        # check_cxx_compiler_flag also passes the value of its VAR argument
        # through a regex check (why?!) which will barf on regex sepecial characters
	set(cxx_has_flag_var "cxx_has_flag_${flag}")
	string(REPLACE "-" "_"    cxx_has_flag_var "${cxx_has_flag_var}")
        string(REPLACE "+" "plus" cxx_has_flag_var "${cxx_has_flag_var}")

        check_cxx_compiler_flag(${flag} ${cxx_has_flag_var})
        if(${cxx_has_flag_var})
            set(${VAR} "${flag}" PARENT_SCOPE)
	    unset(has_flag)
            return()
        endif()
    endforeach()
    set(${VAR} PARENT_SCOPE)
endfunction()


# Try modern and gcc-like first, thence xlc etc.

first_cxx_flag_of(CXX_DIALECT_OPT_CXX03    -std=c++03    -qlevel=extended)
first_cxx_flag_of(CXX_DIALECT_OPT_CXX03EXT -std=gnu++03  -qlevel=extended)
first_cxx_flag_of(CXX_DIALECT_OPT_CXX11    -std=c++11    -qlevel=extc1x)
first_cxx_flag_of(CXX_DIALECT_OPT_CXX14EXT -std=gnu++11  -qlevel=extc1x)
first_cxx_flag_of(CXX_DIALECT_OPT_CXX14    -std=c++14    -std=c++1y    -qlevel=extc1x)
first_cxx_flag_of(CXX_DIALECT_OPT_CXX14EXT -std=gnu++14  -std=gnu++1y  -qlevel=extc1x)

# -march=native on x86 is equivalent to -mcpu=native on PPC.
first_cxx_flag_of(COMPILER_OPT_ARCH_NATIVE -march=native -mcpu=native  -qarch=auto)

