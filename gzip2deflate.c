// gzip2deflate by Piotr Fusik <fox@scene.pl>
// http://xasm.atari.org

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIN32
#include <fcntl.h>
#endif

static int get_byte(void)
{
	int b = getchar();
	if (b == EOF) {
		fputs("gzip2deflate: unexpected end of stream\n", stderr);
		exit(1);
	}
	return b;
}

static void skip_bytes(int n)
{
	while (--n >= 0)
		get_byte();
}

static void skip_string(void)
{
	while (get_byte() != 0);
}

int main (int argc, char *argv[])
{
	int flg;
	char buf[8 + 4096];
	size_t len;
#ifdef __WIN32
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	if (get_byte() != 0x1f || get_byte() != 0x8b || get_byte() != 8) {
		fputs("gzip2deflate: not a gzip file on stdin\n", stderr);
		return 1;
	}
	flg = get_byte();
	skip_bytes(6);
	if ((flg & 4) != 0) {
		int xlen = get_byte();
		xlen += get_byte() << 8;
		skip_bytes(xlen);
	}
	if ((flg & 8) != 0)
		skip_string();
	if ((flg & 16) != 0)
		skip_string();
	if ((flg & 2) != 0)
		skip_bytes(2);
	/* copy everything except the last 8 bytes */
	len = 0;
	for (;;) {
		len += fread(buf + len, 1, sizeof(buf) - len, stdin);
		if (len != sizeof(buf))
			break;
		fwrite(buf, 1, sizeof(buf) - 8, stdout);
		memcpy(buf, buf + sizeof(buf) - 8, 8);
		len = 8;
	}
	if (ferror(stdin)) {
		fputs("gzip2deflate: read error\n", stderr);
		return 1;
	}
	if (len < 8) {
		fputs("gzip2deflate: unexpected end of stream\n", stderr);
		return 1;
	}
	fwrite(buf, 1, len - 8, stdout);
	return 0;
}
