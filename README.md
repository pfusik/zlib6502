6502 inflate routine
====================

[DEFLATE](http://en.wikipedia.org/wiki/DEFLATE) is a popular compression format,
used in ZIP, gzip, PNG and many other formats.

In 2000 I wrote a DEFLATE decompression routine (called "inflate")
in the [6502](http://en.wikipedia.org/wiki/6502) assembly language.
In 2007 I optimized it so it is about 30% shorter and 10% faster than before.

Compilation
-----------

Use [xasm](https://github.com/pfusik/xasm).

The routine uses three memory areas:

* `inflate` - code and initialized data (508 bytes)
* `inflate_data` - uninitialized data (764 bytes)
* `inflate_zp` - variables on zero page

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
Originally I wrote a command-line utility called `deflater`
which used the standard [zlib library](http://www.zlib.net/).
However, better compression can be obtained with [7-Zip](http://7-zip.org/).
It supports the gzip format which is a thin layer on top of DEFLATE.
My program `gzip2deflate` extracts the DEFLATE stream from a gzip file.
It reads gzip on its standard input and writes DEFLATE to its standard output.
I recommend using it this way:

    7z a -tgzip -mx=9 -so dummy INPUT_FILE | gzip2deflate >OUTPUT_FILE.dfl

If you don't have 7-Zip, use:

    gzip -c -9 INPUT_FILE | gzip2deflate >OUTPUT_FILE.dfl

If you are looking for maximum compression, [KZIP](http://advsys.net/ken/utils.htm)
is also worth a try. It creates ZIP files, so I wrote `zip2deflate` to extract
DEFLATE data from a ZIP.

Windows binaries of `gzip2deflate` and `zip2deflate` are
[available for download](http://pfusik.github.io/zlib6502/2deflate.zip).
For other platforms, you will need to compile these programs yourself.

zlib?
-----

This project is named zlib6502, but only supports DEFLATE decompression.
Someday I'm going to include more functions, including compression.

Meanwhile, you may look at [cc65](https://github.com/cc65/cc65) `zlib.h`.
This is my old code, which includes an old version of `inflate`
plus zlib-compatible `uncompress`, `adler32` and `crc32`.

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
