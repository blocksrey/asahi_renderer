#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define wx 600
#define wy 400

typedef struct { int x, y; } v2;
typedef struct { int x, y, z; } v3;
typedef struct { int x, y, z, w; } v4;

static int srt(unsigned n) {
	register unsigned f, p, r;

	f = 0;
	p = 1 << 30;
	r = n;

	while (p > r) p >>= 2;

	while (p) {
		if (r >= f + p) {
			r = r - f - p;
			f += (p << 1);
		}
		f >>= 1;
		p >>= 2;
	}

	return (int)f;
}

static int dot3(v3 *a, v3 *b) { return a->x*b->x + a->y*b->y + a->z*b->z; }
static int square3(v3 *v) { return v->x*v->x + v->y*v->y + v->z*v->z; }
static int norm3(v3 *v) { return srt(v->x*v->x + v->y*v->y + v->z*v->z); }

static void unit3(v3 *v) {
	int n = norm3(v);
	v->x = (v->x << 8)/n;
	v->y = (v->y << 8)/n;
	v->z = (v->z << 8)/n;
}

//static int sdfblock(int px, int py, int pz, int sx, int sy, int sz) {}

int main(int argc, char **argv) {
	static char sin[] = {0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 51, 54, 57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 
 83, 85, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116, 117, 118, 120, 121, 122, 
 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 125, 125, 124, 123, 
 122, 122, 121, 120, 118, 117, 116, 115, 113, 112, 111, 109, 107, 106, 104, 102, 100, 98, 96, 94, 92, 90, 88, 
 85, 83, 81, 78, 76, 73, 71, 68, 65, 63, 60, 57, 54, 51, 49, 46, 43, 40, 37, 34, 31, 28, 25, 22, 19, 16, 12, 9, 
 6, 3, 0, -3, -6, -9, -12, -16, -19, -22, -25, -28, -31, -34, -37, -40, -43, -46, -49, -51, -54, -57, -60, -63, 
 -65, -68, -71, -73, -76, -78, -81, -83, -85, -88, -90, -92, -94, -96, -98, -100, -102, -104, -106, -107, -109, 
 -111, -112, -113, -115, -116, -117, -118, -120, -121, -122, -122, -123, -124, -125, -125, -126, -126, -126, 
 -127, -127, -127, -127, -127, -127, -127, -126, -126, -126, -125, -125, -124, -123, -122, -122, -121, -120, 
 -118, -117, -116, -115, -113, -112, -111, -109, -107, -106, -104, -102, -100, -98, -96, -94, -92, -90, -88, 
 -85, -83, -81, -78, -76, -73, -71, -68, -65, -63, -60, -57, -54, -51, -49, -46, -43, -40, -37, -34, -31, -28, 
 -25, -22, -19, -16, -12, -9, -6, -3};

	int cos[sizeof sin];
	for (int i = sizeof sin; i--;) cos[i] = sin[(i + 65)&255];

	enum { cpx_, cpy_, cpz_, cry_, ldx_, ldy_, ldz_ };
	int arg[7];
	for (int i = argc; --i;) arg[i - 1] = (int)atoi(argv[i]);

	v3 cp = { arg[cpx_], arg[cpy_], arg[cpz_] };
	v3 ld = { arg[ldx_], arg[ldy_], arg[ldz_] };
	int cry = arg[cry_];

	for (int f = 2; --f;) {

	int ryc = cos[cry&255];
	int rys = sin[cry&255];

	printf("P6\n%i %i\n255\n", wx, wy);

	for (int i = wx*wy; i--;) {
		int ix = i%wx;
		int iy = (int)(i/wx);

		v2 ow = {wx - (ix<<1), (iy<<1) - wy};
		v3 cd = {ow.x*ryc/wy - rys, (ow.y<<7)/wy, ow.x*rys/wy + ryc};
		unit3(&cd);

		v3 ir = {0, 0, 0};

		if (cd.y > 0) {
			int o = 0xffff/cd.y;

			int hx = cp.x + o*cd.x;
			int hz = cp.z + o*cd.z;

			ir.x = 188;
			ir.y = 0;
			ir.z = 45;

			int sky = cos[(cos[(hz>>11)&255] + (hx>>8))>>1&255] + cos[hz/500&255]/4 + 30;
			if (sky < 0) {
				ir.x = sky;
				ir.y = sky;
				ir.z = sky;
			}
			else if (dot3(&cd, &ld) < 32000) {
				ir.x = 128 - 128*cd.y/255;
				ir.y = 179 - 179*cd.y/255;
				ir.z = 255 - 76*cd.y/255;
			}
		}
		else if (cd.y < 0) {
			int o = cp.y/-cd.y;

			int hx = cp.x + o*cd.x;
			int hz = cp.z + o*cd.z;

			ir.x = 77;
			ir.y = 40;
			ir.z = 0;

			if (!(((hx>>13)%7)*((hz>>13)%9))) {
				ir.x = 100;
				ir.y = 100;
				ir.z = 110;
			}
			else {
				ir.x = 60;
				ir.y = sin[hx/20&255]/2 + 55;
				ir.z = 0;

				if (ir.y < 0) {
					ir.y = 60;
					ir.z = 120;
				}
			}
		}

		putchar(ir.x);
		putchar(ir.y);
		putchar(ir.z);
	}

	}

	return 0;
}
