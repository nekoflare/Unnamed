string(JOIN " " SHARED_C_CXX_FLAGS
    -nostdinc                                                          # Do not include standard headers
    -Wall -Wextra -Werror -Wformat -Wunused-function -Wformat-security # Set errors
    -fno-stack-protector -fno-stack-check                              # Prevent certain shit that will break me
    -ffreestanding                                                     # Dont use C/C++ libraries the way it thinks
    -mno-red-zone                                                      # Some x86 shit
    -ffunction-sections -fdata-sections                                # Remove unused shit
    -fno-PIE -fno-PIC                                                  # Kernel is located at 0xffffffff80000000 anyways
    -m64 -march=x86-64                                                 # Target x86_64
    -mcmodel=kernel                                                    # Memory model
    -fno-builtin                                                       # Take a guess
)

if (KERNEL_TUNE_FOR_NATIVE)
    set(SHARED_C_CXX_FLAGS "${SHARED_C_CXX_FLAGS} -march=native -mtune=native")
else()
    set(SHARED_C_CXX_FLAGS "${SHARED_C_CXX_FLAGS} -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 -mno-avx -mno-avx2 -mno-avx512f")
endif()

if(KERNEL_USE_OPTIMIZATIONS)
    set(SHARED_C_CXX_FLAGS "${SHARED_C_CXX_FLAGS} -O3 -funroll-loops -ftree-vectorize -fstrict-aliasing -fno-lto -fvectorize -freciprocal-math -fassociative-math -falign-functions=32 -falign-loops=32 -fno-math-errno -fomit-frame-pointer -ffast-math")
endif()

if(KERNEL_USE_DEBUG)
    set(SHARED_C_CXX_FLAGS "${SHARED_C_CXX_FLAGS} -fno-omit-frame-pointer -Wstrict-aliasing -mno-omit-leaf-frame-pointer -fno-plt")
endif()

if(KERNELUSE_SANITIZERS)
    set(SHARED_C_CXX_FLAGS "${SHARED_C_CXX_FLAGS} -fsanitize=undefined,shift,null,vla-bound,float-divide-by-zero,float-cast-overflow,return,object-size,bool -fno-sanitize-link-runtime -fno-sanitize=function")
endif()