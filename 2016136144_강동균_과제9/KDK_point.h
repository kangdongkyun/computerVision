void gaussianSmoothing(unsigned char * src, unsigned char * dst, int number_of_pixels, int cols, int rows, double sigma);
void gaussianDerivative(unsigned char *src, unsigned char *dst, float *angle, int rows, int cols);
void nonMaximumSuppression(unsigned char* src, unsigned char* dst, float *angle, int rows, int cols);
void doubleThresholding(unsigned char* src, unsigned char* dst, int rows, int cols, float low_ratio, float high_ratio);
void subpixel(unsigned char* src, int number_of_pixels, int rows, int cols);