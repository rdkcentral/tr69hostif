################################################################################
# Copyright 2024 Comcast Cable Communications Management, LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0
################################################################################

# Initialize Autoconf
AC_INIT([tr69hostif_gtest], [1.0])
LT_INIT
# Initialize Automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
# Check for necessary headers
#AC_CHECK_HEADERS([jsoncpp/json/json.h gtest/gtest.h gmock/gmock.h])
# Check for necessary libraries
#AC_CHECK_LIB([jsoncpp], [main])
#AC_CHECK_LIB([gtest], [main])
#AC_CHECK_LIB([gmock], [main])
# Checks for programs.
AC_PROG_CXX
# Checks for libraries.
AC_CHECK_LIB([stdc++], [main])
# Checks for header files.
AC_HEADER_STDC
# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# Check for GLib 2.0
PKG_CHECK_MODULES([GLIB], [glib-2.0])

# This enables libsoup-3 support
AC_ARG_ENABLE([libsoup3],
[  --enable-libsoup3    Turn on libsoup-3 support],
[case "${enableval}" in
  yes) libsoup3=true ;;
  no)  libsoup3=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-libsoup3]) ;;
esac],[libsoup3=false])
AM_CONDITIONAL([LIBSOUP3_ENABLE], [test x$libsoup3 = xtrue])

AC_ARG_ENABLE([t2api],
	      AS_HELP_STRING([--enable-t2api],[enables telemetry]),
	      [
	        case "${enableval}" in
                 yes) IS_TELEMETRY2_ENABLED=true
                      T2_EVENT_FLAG=" -DT2_EVENT_ENABLED ";;
                 no)  IS_TELEMETRY2_ENABLED=false ;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-t2enable]) ;;
	       esac
	       ],
	   [echo "telemetry is disabled"])
AM_CONDITIONAL([IS_TELEMETRY2_ENABLED], [test x$IS_TELEMETRY2_ENABLED = xtrue])
AC_SUBST(T2_EVENT_FLAG)


# Generate the Makefile
 AC_CONFIG_FILES([
    hostif/parodusClient/gtest/Makefile
    hostif/httpserver/src/gtest/Makefile
    hostif/src/gtest/Makefile
    hostif/profiles/DeviceInfo/gtest/Makefile
    ])

# Generate the configure script
AC_OUTPUT
