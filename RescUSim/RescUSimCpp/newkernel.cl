#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define DISTSTEP   10000

typedef struct {
	double scenario;
	double posx;
	double posy;
} SampleStructOpenCL;

typedef struct {
	double2 pos;
	double speed;
	double mobilizationTime;
	uint type;
	uint maxCapacity;
} RescueUnitStruct;

typedef struct {
	float2 wind;
	float hs;
	uint light;
} WeatherStructOpenCL;

typedef struct {
	ushort ruIndex;
	double time;
} RUTime;


size_t getWeatherIndex(uint2 dim, uint3 i) {
	return dim.y* i.x + i.y + dim.x*dim.y*i.z;
}

double8 getBilinearWeights(double3 pos, double2 boundsmin, uint resolution) {
	// http://paulbourke.net/miscellaneous/interpolation/

	double3 p;
	p.xy = (pos.xy - boundsmin)  / resolution;
	p.z = pos.z;

	
	uint3 i = convert_uint3(p);

	// Calculate the weights for each pixel
	double3 f = p-convert_double3(i);
	double3 f1 = 1.0f - f;

	double8 bw;
	bw.s0 = f1.x * f1.y * f1.z ;
	bw.s1 = f.x  * f1.y * f1.z ;
	bw.s2 = f1.x * f.y  * f1.z ;
	bw.s3 = f1.x * f1.y * f.z  ;
	bw.s4 = f.x  * f1.y * f.z  ;
	bw.s5 = f1.x * f.y  * f.z  ;
	bw.s6 = f.x  * f.y  * f1.z ;
	bw.s7 = f.x  * f.y  * f.z  ;

	return bw;
}

double double2Sum(double2 v) {
	return v.x+v.y;
}

double2 windAt(__global const WeatherStructOpenCL* weatherData, double3 pos, uint2 dim, double2 boundsmin, uint resolution) {
	uint3 i;
	i.xy = convert_uint2((pos.xy-boundsmin)/resolution);
	i.z = convert_uint(pos.z);

	//printf ("Positions: %f %f %f\n", pos.x, pos.y, pos.z);
	//printf ("Indexes: %d %d %d\n", i.x, i.y, i.z);
	double8 bw = getBilinearWeights(pos, boundsmin, resolution);
	double8 vx, vy;
	vx.s0 = weatherData[getWeatherIndex(dim,i+(uint3)(0,0,0))].wind.x;	
	vx.s1 = weatherData[getWeatherIndex(dim,i+(uint3)(1,0,0))].wind.x;
	vx.s2 = weatherData[getWeatherIndex(dim,i+(uint3)(0,1,0))].wind.x;
	vx.s3 = weatherData[getWeatherIndex(dim,i+(uint3)(0,0,1))].wind.x;
	vx.s4 = weatherData[getWeatherIndex(dim,i+(uint3)(1,0,1))].wind.x;
	vx.s5 = weatherData[getWeatherIndex(dim,i+(uint3)(0,1,1))].wind.x;
	vx.s6 = weatherData[getWeatherIndex(dim,i+(uint3)(1,1,0))].wind.x;
	vx.s7 = weatherData[getWeatherIndex(dim,i+(uint3)(1,1,1))].wind.x;

	vy.s0 = weatherData[getWeatherIndex(dim,i+(uint3)(0,0,0))].wind.y;
	vy.s1 = weatherData[getWeatherIndex(dim,i+(uint3)(1,0,0))].wind.y;
	vy.s2 = weatherData[getWeatherIndex(dim,i+(uint3)(0,1,0))].wind.y;
	vy.s3 = weatherData[getWeatherIndex(dim,i+(uint3)(0,0,1))].wind.y;
	vy.s4 = weatherData[getWeatherIndex(dim,i+(uint3)(1,0,1))].wind.y;
	vy.s5 = weatherData[getWeatherIndex(dim,i+(uint3)(0,1,1))].wind.y;
	vy.s6 = weatherData[getWeatherIndex(dim,i+(uint3)(1,1,0))].wind.y;
	vy.s7 = weatherData[getWeatherIndex(dim,i+(uint3)(1,1,1))].wind.y;

	

	double2 wind;

	double8 windx = vx*bw;
	double4 windxr1 = windx.lo+windx.hi;
	double2 windxr2 = windxr1.lo+windxr1.hi;
	wind.x = windxr2.lo+windxr2.hi;

	double8 windy = vy*bw;
	double4 windyr1 = windy.lo+windy.hi;
	double2 windyr2 = windyr1.lo+windyr1.hi;
	wind.y = windyr2.lo + windyr2.hi;

	double2 windPol = { sqrt(double2Sum(pow(wind, 2))),
						atan2(wind.y, wind.x) };

	return windPol;
}

double hsAt(__global const WeatherStructOpenCL* weatherData, double3 pos, uint2 dim, double2 boundsmin, uint resolution) {
	uint3 i;
	i.xy = convert_uint2((pos.xy-boundsmin)/resolution);
	i.z = convert_uint(pos.z);

	//printf ("Positions: %f %f %f\n", pos.x, pos.y, pos.z);
	//printf ("Indexes: %d %d %d\n", i.x, i.y, i.z);
	double8 bw = getBilinearWeights(pos, boundsmin, resolution);
	double8 v;
	v.s0 = weatherData[getWeatherIndex(dim,i+(uint3)(0,0,0))].hs;	
	v.s1 = weatherData[getWeatherIndex(dim,i+(uint3)(1,0,0))].hs;
	v.s2 = weatherData[getWeatherIndex(dim,i+(uint3)(0,1,0))].hs;
	v.s3 = weatherData[getWeatherIndex(dim,i+(uint3)(0,0,1))].hs;
	v.s4 = weatherData[getWeatherIndex(dim,i+(uint3)(1,0,1))].hs;
	v.s5 = weatherData[getWeatherIndex(dim,i+(uint3)(0,1,1))].hs;
	v.s6 = weatherData[getWeatherIndex(dim,i+(uint3)(1,1,0))].hs;
	v.s7 = weatherData[getWeatherIndex(dim,i+(uint3)(1,1,1))].hs;


	double8 hsr0 = v*bw;
	double4 hsr1 = hsr0.lo+hsr0.hi;
	double2 hsr2 = hsr1.lo+hsr1.hi;
	double hs = hsr2.lo+hsr2.hi;

	return hs;
}

void kernel simulateSample(__global const double3* samples, __global const RescueUnitStruct* rus,
		ushort numRUs, __global const WeatherStructOpenCL* weather, uint2 dim, double2 boundmin, uint resolution){
	//if (get_global_id(0) >= 10000) {
	//	return;
	//}
	//WeatherStructOpenCL weatherI =  weather[getWeatherIndex(dim, (uint3){20, 4, 10})];
	//printf ("weather 10.20.4: %f %f %f %d \n", weatherI.wind.x, weatherI.wind.y, weatherI.hs, weatherI.light);
	
	double3 sample = samples[get_global_id(0)];
	//printf ("DestPos: %f %f\n", sample.x, sample.y);

	//printf ("BW: %v8f", getBilinearWeights(sample,boundmin,resolution));
	//double8 bw = getBilinearWeights(sample,boundmin,resolution);
	//printf ("BW: %f %f %f %f %f %f %f %f", bw.s0, bw.s1,bw.s2,bw.s3,bw.s4,bw.s5,bw.s6,bw.s7);
	//double2 bw = windAt(weather,sample,dim,boundmin,resolution);
	//printf ("OpenCL WindAt: %f %f \n", bw.x, bw.y);
	//double2 hs = hsAt(weather,sample,dim,boundmin,resolution);
	//printf ("OpenCL HsAt: %f \n", hs );

	RUTime rut[2];
	

	for (ushort ruIndex = 0; ruIndex < numRUs; ruIndex++) {
		double t = 0;
		double2 start = rus[ruIndex].pos ;
		double3 pos;
		pos.xy = start;
		pos.z = sample.z;
		//double crs = atan2(sample.y - pos.y, sample.x - pos.x);
		double2 diff = sample.xy - pos.xy;
		double crs = atan2(sample.y-pos.y, sample.x-pos.x);
		
		double dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
		//printf ("Dist: %f\n", dist);
		double2 sccrs;
		//sccrs.x = cos(crs);
		//sccrs.y = sin(crs);
		{
			double sccrsx;
			sccrs.y=sincos(crs,&sccrsx);
			sccrs.x = sccrsx;
		}

		//printf ("CRS: %f Cos: %f Sin: %f\n", crs, sccrs.x, sccrs.y);
		if (rus[ruIndex].type == 1) {
			//printf ("%s","Using helicopter mode\n");
			for (int step = 0; step <= (int)(dist/DISTSTEP); step++) {
				double distStep = min((double)DISTSTEP, dist - step * DISTSTEP);
				
			
				double2 wind = windAt(weather,pos,dim,boundmin,resolution);
				//printf ("WindAt %.9f,%.9f,%.9f ",pos.x, pos.y, pos.z);

				//printf ("Pos: %f %f Diststep: %f T: %f Wind: %.9f, %.9f ", pos.x, pos.y, distStep, t, wind.s0, wind.s1);
				double m = wind.s1 - fmod(M_PI_2 - crs, 2 * M_PI);
				double gs = rus[ruIndex].speed*sqrt(1 - pow(wind.s0, 2) / pow(rus[ruIndex].speed, 2) * pow(sin(m), 2)) - wind.s0*cos(m);
				pos.xy += distStep * sccrs;
				t += distStep / (gs * 60);
				pos.z = sample.z+t/180;
				//printf ("T: %.9f\n", t);

				if (t > 120) {
					t = 120;
					break;
				}
			}
			//printf ("T: %.9f\n", t);
		} else if (rus[ruIndex].type == 2) {
			//printf ("%s","Using ERV mode\n");
			for (int step = 0; step <= (int)(dist/DISTSTEP); step++) {
				double distStep = min((double)DISTSTEP, dist - step * DISTSTEP);
				
			
				double hs = hsAt(weather,pos,dim,boundmin,resolution);
				if (hs > 10) {
					t = 120;
					break;
				}
				//printf ("WindAt %.9f,%.9f,%.9f ",pos.x, pos.y, pos.z);

				//printf ("Pos: %f %f Diststep: %f T: %f Wind: %.9f, %.9f ", pos.x, pos.y, distStep, t, wind.s0, wind.s1);
				double coeff = (hs <= 3) ? (9.311444 - rus[ruIndex].speed) / 3 :
					(9.054222 - rus[ruIndex].speed) / 5;
				double gs = rus[ruIndex].speed + coeff*hs;
				pos.xy += distStep * sccrs;
				t += distStep / (gs * 60);
				pos.z = sample.z+t/180;
				//printf ("T: %.9f\n", t);

				if (t > 120) {
					t = 120;
					break;
				}
			}
			//printf ("T: %.9f\n", t);
		}
	}
}