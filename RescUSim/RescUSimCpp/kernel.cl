#pragma OPENCL EXTENSION cl_amd_printf : enable

typedef struct {
	float wsp;
	float wd;
	float hs;
	unsigned int light;
} WeatherData;

typedef struct {
	float x;
	float y;
} Position;

void kernel simple_add(global const WeatherData* A, global const Position* B, global int* C){
	Position p = B[get_global_id(0)];
	printf ("%f ",p.x);
	printf ("%f\n",p.y);

	//C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];
}