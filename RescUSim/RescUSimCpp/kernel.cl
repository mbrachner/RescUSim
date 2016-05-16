#pragma OPENCL EXTENSION cl_amd_printf : enable


#define M_PI       3.14159265358979323846264338328
#define M_PI_2     1.57079632679489661923132169164
#define DISTSTEP   10000

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

void kernel simple_add(global const WeatherData* A, unsigned int dimX, unsigned int dimY,
						global const Position* B, global float* C, 
						float minx, float miny,
						float initPosX, float initPosY, float mobTime, float speed ){
	Position dest = B[get_global_id(1)];
	Position start = {initPosX, initPosY};
	unsigned int scenario = get_global_id(0);
	float t = 0;



	float crs = atan2(dest.y - start.y, dest.x - start.x);
	float dist = sqrt(pow(dest.x - start.x, 2) + pow(dest.y - start.y, 2));

		for (int step = 0; step <= (int)dist/DISTSTEP; step++) {
		float distStep = min((float)DISTSTEP, dist - step * DISTSTEP);

		Position act = {
			start.x + step * DISTSTEP * (float)cos(crs),
			start.y + step * DISTSTEP * (float)sin(crs)
		};


		size_t ix = (size_t)((act.x-minx) / DISTSTEP);
		size_t iy = (size_t)((act.y-miny) / DISTSTEP);

		WeatherData weather = A[iy + dimY*ix + dimX*dimY*scenario];

		//float wsp = weather.wspAt(scenario, ix, iy);
		//float wd = weather.wdAt(scenario, ix, iy);
		float wsp = weather.wsp;
		float wd = weather.wd;

		float m = wd - fmod(M_PI_2 - crs, 2 * M_PI);
		float gs = speed*sqrt(1 - pow(wsp, 2) / pow(speed, 2) * pow(sin(m), 2)) - wsp*cos(m);
		t += distStep / (gs * 60);
		
		if (t > 120) {
			t = INFINITY;
			break;
		}
	}

	C[get_global_id(0)*get_global_size(1)+get_global_id(1)] = t;
}