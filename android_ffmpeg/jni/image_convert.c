#include "image_convert.h"

#define COEFF  16

const int csY_coeff_16 = (int) (1.164383 * (1 << COEFF));
const int csU_blue_16 = (int) (2.017232 * (1 << COEFF));
const int csU_green_16 = (int) ((-0.391762) * (1 << COEFF));
const int csV_green_16 = (int) ((-0.812968) * (1 << COEFF));
const int csV_red_16 = (int) (1.596027 * (1 << COEFF));

static U8 _color_table[256 * 5];
static const U8 * color_table = &_color_table[256 * 2];

static int Ym_tableEx[256];
static short Um_blue_tableEx[256];
static short Um_green_tableEx[256];
static short Vm_green_tableEx[256];
static short Vm_red_tableEx[256];
static int tableInit = 0;

U8 border_color(int color) {
	if (color > 255)
		return 255;
	else if (color < 0)
		return 0;
	else
		return color;
}

static void yuv420_table_init() {
	if (tableInit > 0)
		return;
	int i;
	for (i = 0; i < 256 * 5; ++i) {
		_color_table[i] = border_color(i - 256 * 2);
	}

	for (i = 0; i < 256; ++i) {
		if (i < 16)
			Ym_tableEx[i] = 0;
		else
			Ym_tableEx[i] = (csY_coeff_16 * (i - 16)) >> (COEFF - 3);

		Um_blue_tableEx[i] = (csU_blue_16 * (i - 128)) >> (COEFF - 3);
		Um_green_tableEx[i] = (csU_green_16 * (i - 128)) >> (COEFF - 3);
		Vm_green_tableEx[i] = (csV_green_16 * (i - 128)) >> (COEFF - 3);
		Vm_red_tableEx[i] = (csV_red_16 * (i - 128)) >> (COEFF - 3);
	}
	tableInit = 1;
}

//NV21: YYYYYYYY VUVU     =>YUV420SP
void NV21RGB(U8* yuv420sp, U8* rgb, int width, int height) {
	yuv420_table_init();
	int j;
	int i;
	U8 r, g, b;
	unsigned int pixel_value;
	int frameSize;
	U8* y_sp;
	U8* uv_sp;
	y_sp = yuv420sp;
	frameSize = width * height;
	uv_sp = yuv420sp + frameSize;

	for (j = 0; j < height; j++) {
		int y = 0, u = 0, v = 0;
		int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;

		uv_sp = yuv420sp + frameSize + (j >> 1) * width;

		for (i = 0; i < width; i += 2) {
			y = y_sp[0];
			v = uv_sp[0];
			u = uv_sp[1];
			ye = Ym_tableEx[y];

			Ue_blue = Um_blue_tableEx[u];
			Ue_green = Um_green_tableEx[u];
			Ve_green = Vm_green_tableEx[v];
			Ve_red = Vm_red_tableEx[v];
			UeVe_green = Ue_green + Ve_green;

			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			pixel_value = (r) | (g << 8) | (b << 16) | (0xff << 24);
			*((unsigned int*) rgb) = pixel_value;

			y = y_sp[1];
			ye = Ym_tableEx[y];

			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			pixel_value = (r) | (g << 8) | (b << 16) | (0xff << 24);
			*((unsigned int*) (rgb + 4)) = pixel_value;
			rgb += 8;
			y_sp += 2;
			uv_sp += 2;
		}
	}

}

//NV12: YYYYYYYY UVUV     =>YUV420SP
void NV12RGB(U8* yuv420sp, U8* rgb, int width, int height) {
	yuv420_table_init();
	int j;
	int i;
	U8 r, g, b;
	unsigned int pixel_value;
	int frameSize;
	U8* y_sp;
	U8* uv_sp;
	y_sp = yuv420sp;
	frameSize = width * height;
	uv_sp = yuv420sp + frameSize;

	for (j = 0; j < height; j++) {
		int y = 0, u = 0, v = 0;
		int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;

		uv_sp = yuv420sp + frameSize + (j >> 1) * width;

		for (i = 0; i < width; i += 2) {
			y = y_sp[0];
			u = uv_sp[0];
			v = uv_sp[1];
			ye = Ym_tableEx[y];

			Ue_blue = Um_blue_tableEx[u];
			Ue_green = Um_green_tableEx[u];
			Ve_green = Vm_green_tableEx[v];
			Ve_red = Vm_red_tableEx[v];
			UeVe_green = Ue_green + Ve_green;

			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			pixel_value = (r) | (g << 8) | (b << 16) | (0xff << 24);
			*((unsigned int*) rgb) = pixel_value;

			y = y_sp[1];
			ye = Ym_tableEx[y];

			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			pixel_value = (r) | (g << 8) | (b << 16) | (0xff << 24);
			*((unsigned int*) (rgb + 4)) = pixel_value;
			rgb += 8;
			y_sp += 2;
			uv_sp += 2;
		}
	}

}

//YV12: YYYYYYYY VV UU    =>YUV420P
void YV12RGB(U8* yuv420sp, U8* rgb, int width, int height) {
	yuv420_table_init();
	int j;
	int i;
	U8 r, g, b;
	int frameSize;
	U8* y_sp;
	U8* u_sp;
	U8* v_sp;
	y_sp = yuv420sp;
	frameSize = width * height;
	v_sp = yuv420sp + frameSize;
	u_sp = v_sp + frameSize / 4;
	unsigned int* rgbColor = (unsigned int*) rgb;

	int y = 0, u = 0, v = 0;
	int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;

	if (tableInit == 0)
		yuv420_table_init();

	for (j = 0; j < height; j += 2) {

		for (i = 0; i < width; i += 2) {
			v = v_sp[0];
			u = u_sp[0];

			//
			y = y_sp[0];
			ye = Ym_tableEx[y];
			Ue_blue = Um_blue_tableEx[u];
			Ue_green = Um_green_tableEx[u];
			Ve_green = Vm_green_tableEx[v];
			Ve_red = Vm_red_tableEx[v];
			UeVe_green = Ue_green + Ve_green;
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[0] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[1];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[width];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[width] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[width + 1];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[width + 1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			rgbColor += 2;
			y_sp += 2;
			u_sp += 1;
			v_sp += 1;
		}
		rgbColor += width;
		y_sp += width;
	}

}

//I420: YYYYYYYY UU VV    =>YUV420P
void I420RGB(U8* yuv420sp, U8* rgb, int width, int height) {
	yuv420_table_init();
	int j;
	int i;
	U8 r, g, b;
	int frameSize;
	U8* y_sp;
	U8* u_sp;
	U8* v_sp;
	y_sp = yuv420sp;
	frameSize = width * height;
	u_sp = yuv420sp + frameSize;
	v_sp = u_sp + frameSize / 4;
	unsigned int* rgbColor = (unsigned int*) rgb;

	int y = 0, u = 0, v = 0;
	int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;

	if (tableInit == 0)
		yuv420_table_init();

	for (j = 0; j < height; j += 2) {

		for (i = 0; i < width; i += 2) {
			v = v_sp[0];
			u = u_sp[0];

			//
			y = y_sp[0];
			ye = Ym_tableEx[y];
			Ue_blue = Um_blue_tableEx[u];
			Ue_green = Um_green_tableEx[u];
			Ve_green = Vm_green_tableEx[v];
			Ve_red = Vm_red_tableEx[v];
			UeVe_green = Ue_green + Ve_green;
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[0] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[1];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[width];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[width] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[width + 1];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[width + 1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			rgbColor += 2;
			y_sp += 2;
			u_sp += 1;
			v_sp += 1;
		}
		rgbColor += width;
		y_sp += width;
	}

}

void FFMPEG_YUV420P2RGB(U8* yuv420sp[], int linesize[], U8* rgb, int width,
		int height) {

	int j;
	int i;
	U8 r, g, b;
	int frameSize;
	U8* y_sp;
	U8* u_sp;
	U8* v_sp;
	frameSize = width * height;

	unsigned int* rgbColor = (unsigned int*) rgb;

	int y = 0, u = 0, v = 0;
	int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;

	yuv420_table_init();

	int halfH = height / 2;
	int halfW = width / 2;
	int wpos = 0;
	for (j = 0; j < halfH; j++) {
		y_sp = yuv420sp[0] + j * 2 * linesize[0];
		u_sp = yuv420sp[1] + j * linesize[1];
		v_sp = yuv420sp[2] + j * linesize[2];
		rgbColor = (unsigned int*) rgb + j * 2 * width;
		for (i = 0; i < halfW; i++) {
			v = v_sp[i];
			u = u_sp[i];

			wpos = i << 1;
			//
			y = y_sp[wpos];
			ye = Ym_tableEx[y];
			Ue_blue = Um_blue_tableEx[u];
			Ue_green = Um_green_tableEx[u];
			Ve_green = Vm_green_tableEx[v];
			Ve_red = Vm_red_tableEx[v];
			UeVe_green = Ue_green + Ve_green;
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[wpos] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[wpos + linesize[0]];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[wpos + width] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			wpos++;
			//
			y = y_sp[wpos];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[wpos] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[wpos + linesize[0]];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[wpos + width] = (r) | (g << 8) | (b << 16) | (0xff << 24);
		}

	}

}

//yyyyuuvv
void YUV420PBuffer2RGB(U8* yuvdata, U8* rgb, int offset[], int stride[],
		int width, int height, int type) {
	yuv420_table_init();

	int j;
	int i;
	U8 r, g, b;

	unsigned int* rgbColor = (unsigned int*) rgb;

	int y = 0, u = 0, v = 0;
	int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;

	U8* u_sp;
	U8* v_sp;
	for (j = 0; j < height; j += 2) {
		U8* y_sp = yuvdata + offset[0] + stride[0] * j;
		if (type == 0) {
			u_sp = yuvdata + offset[1] + stride[1] * (j / 2);
			v_sp = yuvdata + offset[2] + stride[2] * (j / 2);
		} else {
			v_sp = yuvdata + offset[1] + stride[1] * (j / 2);
			u_sp = yuvdata + offset[2] + stride[2] * (j / 2);
		}

		for (i = 0; i < width; i += 2) {
			v = v_sp[0];
			u = u_sp[0];

			//
			y = y_sp[0];
			ye = Ym_tableEx[y];
			Ue_blue = Um_blue_tableEx[u];
			Ue_green = Um_green_tableEx[u];
			Ve_green = Vm_green_tableEx[v];
			Ve_red = Vm_red_tableEx[v];
			UeVe_green = Ue_green + Ve_green;
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[0] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[1];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[stride[0]];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[width] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			//
			y = y_sp[stride[0] + 1];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];
			rgbColor[width + 1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			rgbColor += 2;
			y_sp += 2;
			u_sp += 1;
			v_sp += 1;
		}
		rgbColor += width;
	}

}
//yyyyuvuv
void NV21Buffer2RGB(U8* yuvdata, U8* rgb, int offset[], int stride[], int width,
		int height, int type) {
	yuv420_table_init();
	int j;
	int i;
	U8 r, g, b;
	unsigned int* rgbColor = (unsigned int*) rgb;

	for (j = 0; j < height; j += 2) {
		U8* y_sp = yuvdata + offset[0] + stride[0] * j;
		U8* uv_sp = yuvdata + offset[1] + stride[1] * (j / 2);
		int y = 0, u = 0, v = 0;
		int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;

		for (i = 0; i < width; i += 2) {
			if (type == 0) {
				u = uv_sp[0];
				v = uv_sp[1];
			} else {
				v = uv_sp[0];
				u = uv_sp[1];
			}

			Ue_blue = Um_blue_tableEx[u];
			Ue_green = Um_green_tableEx[u];
			Ve_green = Vm_green_tableEx[v];
			Ve_red = Vm_red_tableEx[v];
			UeVe_green = Ue_green + Ve_green;

			////
			y = y_sp[0];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			rgbColor[0] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			///
			y = y_sp[1];
			ye = Ym_tableEx[y];

			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			rgbColor[1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			////
			y = y_sp[stride[0]];
			ye = Ym_tableEx[y];
			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			rgbColor[width] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			///
			y = y_sp[stride[0] + 1];
			ye = Ym_tableEx[y];

			b = color_table[(ye + Ue_blue) >> 3];
			g = color_table[(ye + UeVe_green) >> 3];
			r = color_table[(ye + Ve_red) >> 3];

			rgbColor[width + 1] = (r) | (g << 8) | (b << 16) | (0xff << 24);

			rgbColor += 2;
			y_sp += 2;
			uv_sp += 2;
		}
		rgbColor += width;
	}
}

//JNIEXPORT void JNICALL Java_com_decoder_xiaomi_ImageConvert_convertYUV420SP2RGB(JNIEnv *env,jobject obj,jbyteArray yuv,jobject bitmap) {
//	void* pixels;
//	U8* pDst;
//	U8* psrc = (U8*)(*env)->GetByteArrayElements(env, yuv, 0);
//
//	AndroidBitmapInfo info;
//	if (AndroidBitmap_getInfo(env, bitmap, &info) < 0 )
//	return; //
//	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
//	return;
//
//	if ( AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0 )
//	return;
//
//	pDst = (U8*)pixels;
//
//	if(tableInit==0)
//	yuv420_table_init();
//
//	YUV420SP2RGB(psrc, pDst, info.width, info.height);
//
////	struct yuv_pack out;
////	out.yuv = pDst;
////	out.pitch = info.width*info.height*4;
////	struct yuv_planes in;
////	in.y = psrc;
////	in.u = in.y+info.width*info.height;
////	in.v = in.u+info.width*info.height/4;
////	in.pitch = info.width*info.height;
////	i420_rgb_neon (&out,
////	                    &in,
////	                    info.width, info.height);
//
//	AndroidBitmap_unlockPixels(env, bitmap);
//	(*env)->ReleaseByteArrayElements(env, yuv, psrc, 0);
//}
//
//JNIEXPORT void JNICALL Java_com_decoder_xiaomi_ImageConvert_convertYUV420Planar2RGB(JNIEnv *env,jobject obj,jbyteArray yuv,jobject bitmap) {
//	void* pixels;
//	U8* pDst;
//	U8* psrc = (U8*)(*env)->GetByteArrayElements(env, yuv, 0);
//
//	AndroidBitmapInfo info;
//	if (AndroidBitmap_getInfo(env, bitmap, &info) < 0 )
//	return; //
//	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
//	return;
//
//	if ( AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0 )
//	return;
//
//	pDst = (U8*)pixels;
//
//	if(tableInit==0)
//	yuv420_table_init();
//
//	YUV420Planar2RGB(psrc, pDst, info.width, info.height);
//
//
//
//	AndroidBitmap_unlockPixels(env, bitmap);
//	(*env)->ReleaseByteArrayElements(env, yuv, psrc, 0);
//}

