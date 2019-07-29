# ruby (ルビィ)

> Waah! Wh-Wh-Wh-What is it?

ruby (ルビィ) is a [PlayStation](https://en.wikipedia.org/wiki/PlayStation_(console)) emulator written in C++.

![current_progress.gif](/images/current_progress.gif)

### Progress

- [x] CPU
- [x] RAM
- [x] DMA
- [x] GPU
- [x] CDROM
- [x] Interrupts
- [x] Timers
- [x] Controllers and Memory Cards
- [ ] GTE
- [ ] SPU
- [ ] SIO
- [ ] PIO

### Building

#### Prerequisites

- PlayStation BIOS: SCPH1001 (SHA1: 10155d8d6e6e832d6ea66db9bc098321fb5e8ebf)
- C++17
- OpenGL 4.5
- SDL2

#### Compiling

```
$ mkdir build
$ cd build
$ cmake ..
$ make -j8
```

#### Compiling with GDB support

```
$ mkdir build
$ cd build
$ cmake -DHANA=ON ..
$ make -j8
```

#### GDB support

If compiled with GDB support, pressing the backspace key at any time will stop the emulator until GDB is attached to `localhost:2109`. You will need a [GDB build with support for MIPS little endian](https://images.linux-mips.org/wiki/Toolchains#GDB).

```
$ mipsel-unknown-linux-gnu-gdb
$ target remote localhost:2109
$ (gdb) info registers
          zero       at       v0       v1       a0       a1       a2       a3
 R0   00000000 00000001 000022bc 1c42220a 00000001 00000001 00000000 00000260
            t0       t1       t2       t3       t4       t5       t6       t7
 R8   80080000 04000000 050eccf0 5e42220a 40000000 00000008 00008000 00000000
            s0       s1       s2       s3       s4       s5       s6       s7
 R16  00000027 00000027 00000027 0000000d 0000003c 00000003 00000003 00000003
            t8       t9       k0       k1       gp       sp       s8       ra
 R24  000022bc 000022bb 8005aa18 00000f1c a0010ff0 801ffcd8 801fff00 80059d18
            sr       lo       hi      bad    cause       pc
      00000001 00000027 00000000 8005aa14 00000020 80059e00
           fsr      fir
       <unavl>  <unavl>
```

For a comprehensive list of supported GDB features, see [libHana](https://github.com/Ruenzuo/libHana).

#### Compiling release build

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j8
```

### Running

```
$ ./build/ruby # with SCPH1001.BIN in $PWD
```

### Tests

#### Running

```
$ ./build/ruby --exe TESTNAME.exe
```

#### CPU Tests

[amidog CPU tests](https://psx.amidog.se/doku.php?id=psx:download:cpu#CPU_Test): psxtest_cpu.exe (SHA1: 023aec8c92aaaf4d3b07956e26dd6c77ff397456)

![cpu_tests.png](/images/cpu_tests.png)

#### GPU Tests

[PeterLemon/PSX GPU tests](https://github.com/PeterLemon/PSX/tree/master/GPU):

![gpu_tests_quads](/images/gpu_tests_quads.png)  ![gpu_tests_textured_quads](/images/gpu_tests_textured_quads.png)
![gpu_tests_polys](/images/gpu_tests_polys.png)  ![gpu_tests_textured_polys](/images/gpu_tests_textured_polys.png)
![gpu_tests_lines](/images/gpu_tests_lines.png)

### Acknowledgments

This emulator, like many others, is based on the [Nocash PSX Specifications](http://problemkaputt.de/psx-spx.htm). For ease of access and review purposes, relevant parts of the specification are pasted before the implementation.
