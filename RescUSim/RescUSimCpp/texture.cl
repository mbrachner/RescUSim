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


double double2Sum(double2 v) {
	return v.x+v.y;
}

void kernel simulateSample(__global const double3* samples, __global const RescueUnitStruct* rus,
		ushort numRUs, read_only image3d_t weather, sampler_t smp, uint2 dim, double2 boundmin, uint resolution){
	float4 boundmin4;
	boundmin4.xy = convert_float2(boundmin.xy-0.5);
	boundmin4.z = -0.5;
	boundmin4.w = 0;
	//float4 test = read_imagef(weather,smp, (float4)(0+.5,0+.5,0+0.5,0));
	//printf ("Texture %f %f\n", test.s0, test.s1);
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
		double4 pos;
		pos.xy = start;
		pos.z = sample.z;
		pos.w = 0;
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
				
			
				double2 windCart = convert_double2(read_imagef(weather,smp, (convert_float4(pos)-boundmin4)).xy);
				double2 wind = { sqrt(double2Sum(pow(windCart, 2))),
						atan2(wind.y, wind.x) };
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
				
			
				double hs = convert_double(read_imagef(weather,smp, (convert_float4(pos)-boundmin4)).z);
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