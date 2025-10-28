# Unnamed

# To compile this stuff u gotta go do this:
1. Go to `kernel/` folder
2. Create `build` folder.
3. Go to your `build` folder.
4. Invoke cmake with argument `KERNEL_ARCHITECTURE` equal to `x86_64`
   (I do this in case I wanna add other architectures which I doubt)
5. Invoke `make -j$(nproc)` (If not using Ninja)
6. Boom you got kernel compiled and ready (hopefully)
7. Go back to the root of the project
8. Go to `tools/` folder
9. Run `download-required.sh` shell script to get Limine and OVMF binaries.
10. Run `create_iso.sh` to create the ISO (duh)
11. Run `run-kvm-x86_64.sh` and enjoy

The unnamed kernel + whatever else.
