inflate.obx: inflate.asx
	xasm -d inflate=\$$b700 -d inflate_data=\$$b900 -d inflate_zp=\$$f0 inflate.asx

clean:
	rm -f inflate.obx

.PHONY: clean

.DELETE_ON_ERROR:
