// zip2deflate by Piotr Fusik <fox@scene.pl>
// http://xasm.atari.org

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#endif

static int get_byte(void)
{
	int b = getchar();
	if (b == EOF) {
		fputs("zip2deflate: unexpected end of stream\n", stderr);
		exit(1);
	}
	return b;
}

static void skip_bytes(size_t n)
{
	for (; n > 0; n--)
		get_byte();
}

static size_t get_word(void)
{
	size_t lo = get_byte();
	return lo + (get_byte() << 8);
}

static size_t get_long(void)
{
	size_t lo = get_word();
	return lo + (get_word() << 16);
}

int main (int argc, char *argv[])
{
	size_t compressed_size;
	size_t filename_length;
	size_t extra_field_length;
#ifdef _WIN32
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	if (get_long() != 0x04034b50) {
		fputs("zip2deflate: not a ZIP file on stdin\n", stderr);
		return 1;
	}
	if (get_word() != 20 /* version needed to extract */
	 || (get_word() & 0x41) != 0 /* general purpose bit flag */
	 || get_word() != 8 /* compression method */
	) {
		fputs("zip2deflate: unexpected header (not compressed? encrypted?)\n", stderr);
		return 1;
	}
	skip_bytes(8); /* last mod file time, date, crc-32 */
	compressed_size = get_long();
	skip_bytes(4); /* uncompressed size */
	filename_length = get_word();
	extra_field_length = get_word();
	skip_bytes(filename_length + extra_field_length);
	while (compressed_size > 0) {
		char buf[4096];
		size_t len = compressed_size < 4096 ? compressed_size : 4096;
		len = fread(buf, 1, len, stdin);
		if (len == 0) {
			fputs("zip2deflate: unexpected end of stream\n", stderr);
			return 1;
		}
		fwrite(buf, 1, len, stdout);
		compressed_size -= len;
	}

	if (get_long() != 0x02014b50) {
		fputs("zip2deflate: unexpected header (more than one file?)\n", stderr);
		return 1;
	}
	return 0;
}
