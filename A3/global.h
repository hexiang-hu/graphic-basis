#define WIN_WIDTH 		    1024
#define WIN_HEIGHT 		    728
#define IMAGE_WIDTH  		  5.0
#define DIFFUSE_RAY_NUM   5

// numerical convienent macro
#define EPSILON           1e-8

#ifdef __linux__
#elif defined __APPLE__
#else
  #define INFINITY		1e8
#endif
