prefix=/home/yupeng/parsec/pkgs/apps/x264/inst/amd64-linux.gcc
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
ARCH=X86_64
SYS=LINUX
CC=/usr/bin/gcc
CFLAGS=-O4 -ffast-math  -O2 -g -funroll-loops -fprefetch-loop-arrays -static-libgcc -Wl,--hash-style=both,--as-needed -DPARSEC_VERSION=3.0-beta-20150206 -Wall -I.  -O2 -g -funroll-loops -fprefetch-loop-arrays -static-libgcc -Wl,--hash-style=both,--as-needed -DPARSEC_VERSION=3.0-beta-20150206 -DHAVE_MALLOC_H -DHAVE_MMX -DARCH_X86_64 -DSYS_LINUX -DHAVE_PTHREAD -s -fomit-frame-pointer
ALTIVECFLAGS=
LDFLAGS=-L/usr/lib64 -L/usr/lib -L/usr/lib64 -L/usr/lib  -lm -lpthread -s
AS=yasm
ASFLAGS=-f elf -m amd64
EXE=
VIS=no
HAVE_GETOPT_LONG=1
DEVNULL=/dev/null
ECHON=echo -n
CONFIGURE_ARGS= '--enable-pthread' '--extra-asflags=' '--extra-cflags= -O2 -g -funroll-loops -fprefetch-loop-arrays -static-libgcc -Wl,--hash-style=both,--as-needed -DPARSEC_VERSION=3.0-beta-20150206' '--extra-ldflags=-L/usr/lib64 -L/usr/lib ' '--prefix=/home/yupeng/parsec/pkgs/apps/x264/inst/amd64-linux.gcc'
