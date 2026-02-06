sed -i '/getCurrentTime/,/^ *}/d' ../../../src/hostIf_utils.cpp

# Detect architecture for multi-platform support (x86_64 and Apple Silicon/aarch64)
ARCH=$(uname -m)
case $ARCH in
    x86_64)
        ARCH_LIB_PATH="/usr/lib/x86_64-linux-gnu"
        ;;
    aarch64)
        ARCH_LIB_PATH="/usr/lib/aarch64-linux-gnu"
        ;;
    *)
        echo "Warning: Unsupported architecture $ARCH, defaulting to x86_64 paths"
        ARCH_LIB_PATH="/usr/lib/x86_64-linux-gnu"
        ;;
esac

# Update Makefile.am with correct architecture-specific glib path
sed -i "s|/usr/lib/x86_64-linux-gnu/glib-2.0/include|$ARCH_LIB_PATH/glib-2.0/include|g" Makefile.am

autoreconf --install && \
./configure --prefix=/usr/local && make && make install
