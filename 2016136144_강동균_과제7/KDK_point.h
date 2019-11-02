void gaussianSmoothing(unsigned char * src, unsigned char * dst, int number_of_pixels, int cols, int rows, double sigma);
void sobelEdgeDetector(unsigned char * src, unsigned char * dst, int number_of_pixels, int cols, int rows);
int xGradient(unsigned char * src, int rows, int i);
int yGradient(unsigned char * src, int rows, int i);
