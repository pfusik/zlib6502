all: inflate.obx gzip2deflate zip2deflate

inflate.obx: inflate.asx
	xasm -d inflate=\$$b700 -d inflate_data=\$$b900 -d inflate_zp=\$$f0 inflate.asx

%: %.c
	gcc -s -O2 -Wall -o $@ $<

2deflate.zip: gzip2deflate zip2deflate
	rm -f $@ && 7z a -mx=9 -tzip $@ gzip2deflate.exe zip2deflate.exe

clean:
	rm -f inflate.obx gzip2deflate zip2deflate 2deflate.zip gzip2deflate.exe zip2deflate.exe

.PHONY: all clean

.DELETE_ON_ERROR:
