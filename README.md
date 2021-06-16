[![Build Status](https://travis-ci.com/pfusik/zlib6502.svg?branch=master)](https://travis-ci.com/github/pfusik/zlib6502)

6502 inflate routine
====================

[DEFLATE](https://en.wikipedia.org/wiki/DEFLATE) is a popular compression format,
used in ZIP, gzip, PNG and many other formats.

In 2000 I wrote a DEFLATE decompression routine (called "inflate")
in the [6502](https://en.wikipedia.org/wiki/6502) assembly language.
In 2007 I optimized it so it is about 30% shorter and 10% faster than before.
In 2017 I fixed bugs causing invalid expansion of some streams.
These were edge cases, unlikely to encounter unless intentionally triggerred.

Compilation
-----------

Use [xasm](https://github.com/pfusik/xasm).

The routine uses three memory areas:

* `inflate` - code and constants (508 bytes)
* `inflate_data` - uninitialized data (765 bytes)
* `inflate_zp` - variables on zero page (10 bytes)

You must select these locations at compile time, for example:

    xasm -d inflate=$b700 -d inflate_data=$b900 -d inflate_zp=$f0 inflate.asx

(escape the dollars if in Unix shell or Makefile).

Usage
-----

The `inflate` routine assumes that the compressed and the uncompressed data
fit in the memory. Before calling `inflate`, set the locations
of the compressed and the uncompressed data in the zero-page variables:

    mwa #compressedData inflate_zp
    mwa #uncompressedData inflate_zp+2
    jsr inflate

As the compressed data is read sequentially and only once, it is possible
to overlap the compressed and uncompressed data. That is, the data being
uncompressed can be stored in place of some compressed data which has been
already read.

It is also possible to get the compressed data from any forward-only stream.
In this case, modify the `getBit` routine to use your `readByte`:

    getBit
        lsr getBit_buffer
        bne getBit_return
        pha
        stx getBit_buffer
        jsr readByte
        ldx getBit_buffer
        ldy #0
        sec
        ror @
        sta getBit_buffer
        pla
    getBit_return
        rts

Compression
-----------

There are several ways to get DEFLATE compressed data.

If you are looking for maximum compression, use [Zopfli](https://github.com/google/zopfli).
For example:

    zopfli --deflate --i1000 INPUT_FILE

will compress to `INPUT_FILE.deflate`.
I have compiled a [Windows exe](http://pfusik.github.io/zlib6502/zopfli.exe.gz) for you.
Increasing the number passed to the `--i` option can improve the compression by a few bytes
at the cost of increased compression time.

Historically, I have used:

* my programs using the [Deflater](https://docs.oracle.com/en/java/javase/13/docs/api/java.base/java/util/zip/Deflater.html) Java class
* my `deflater` program written with the [zlib library](http://www.zlib.net/).
* my `gzip2deflate` program that extracted the DEFLATE stream from a GZ file created with gzip or [7-Zip](http://7-zip.org/)
* my `zip2deflate` program that extracted the DEFLATE stream from a ZIP created with [KZIP](http://advsys.net/ken/utils.htm)

zlib?
-----

This project is named zlib6502, but only supports DEFLATE decompression.
Someday I'm going to include more functions, including compression.

Meanwhile, you may look at [cc65](https://github.com/cc65/cc65) `zlib.h`.
In addition to `inflate`, it supports zlib-compatible `uncompress`, `adler32` and `crc32`.

License
-------

This code is licensed under the standard zlib license.

Copyright (C) 2000-2017 Piotr Fusik

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
