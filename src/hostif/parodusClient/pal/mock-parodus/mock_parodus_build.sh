sed -i '/getCurrentTime/,/^ *}/d' ../../../src/hostIf_utils.cpp
autoreconf --install && \
./configure --prefix=/usr/local && make && make install
