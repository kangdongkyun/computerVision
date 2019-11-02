void auto_contrast_stretch(unsigned char *src, unsigned char *dst, int number_of_pixels, float low_percent, float high_percent);
void arith_mult(unsigned char *source, int number_of_pixels, float scale);
void arith_mult2(unsigned char *source, unsigned char *dst, int number_of_pixels, float scale);
void arith_add(unsigned char *source, int number_of_pixels, int val);
void histogram_equalize(unsigned char *src, unsigned char *dst, int number_of_pixels);
void logical_XOR(unsigned char *source, int number_of_pixels);
void thresh(unsigned char *source, int number_of_pixels, int thr);
void histoimg(unsigned char *src, int number_of_pixels, unsigned char *dst);
void arith_add_frames(unsigned char *src1, unsigned char *src2, int number_of_pixels);