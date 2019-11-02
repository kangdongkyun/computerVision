#define PI   3.14159265358979323846
#define CLIP(val, low, high) {if(val<low) val=low; if(val>high) val=high;}
#define CLAMP(val, low, high) ((val<low) ? low : ((val>high) ? high : val))
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#define PBM 4
#define PGM 5
#define PPM 6

void write_pnm(unsigned char *ptr, char filein[], int rows, int cols, int magic_number);
unsigned char *read_pnm(char *filename, int *rows, int *cols, int *type);
