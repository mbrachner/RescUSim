import RescUSimCpp
import h5py
from shapely.geometry import LineString, MultiPoint
from mpl_toolkits.basemap import Basemap
import numpy as np

class LineStringI(LineString):
    def discretize(line,dist):
        return MultiPoint([line.interpolate(f) for f in np.arange(0,line.length,dist)]+[line.interpolate(line.length)])

m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=41, urcrnrlat=75.6,area_thresh = 100)
hammerfest = m(23.768302,70.701319);
berlevaag = m(29.090389, 70.854502);
wisting = m(24.232358, 73.491134);
castberg = m(20.347568, 72.494341);
extreme_remote = m(37.000000, 74.500000);

pois = LineStringI([hammerfest,castberg]).discretize(10000).union(
                LineStringI([hammerfest,wisting]).discretize(10000).union(
                LineStringI([berlevaag,extreme_remote]).discretize(10000)));



print "Loading weather...",
h5f = h5py.File('c:\\tmp\\data_idw.h5', 'r')
descr = np.dtype([('wind_xcomp', '<f4'), ('wind_ycomp', '<f4'), ('hs', '<f4'), ('light', 'u1')],align=1)

weatherData = np.array(h5f['weather_data'][:],dtype=descr)
minx, miny, maxx, maxy = h5f['weather_data'].attrs.get("bounds");
resolution = h5f['weather_data'].attrs.get("resolution");
print minx, miny, resolution
weather = RescUSimCpp.Weather(weatherData,float(minx),float(miny),int(resolution))
print "Done"

sim = RescUSimCpp.SimulatorCPU(weather)
sim.addPoi(np.array(pois,dtype=np.float32));

print "Adding RU1...",
ru1 = RescUSimCpp.Helicopter("Heli1").setPos(*hammerfest);
sim.addStationaryRU(ru1);
sim.addStationaryRU(RescUSimCpp.ERV("ERV1").setPos(*castberg))
print "done"
print "Simulating response...",
numIncidents = 30;
sampleSize = 100;
sim.sample(numIncidents*sampleSize);
x = sim.simulateResponseSample();
#print x
x.shape = (sampleSize,numIncidents);
serviceLevel = np.where(x>=21,1.,0.).sum(axis=1);
print x
print serviceLevel
#print x
#print x
#print "Hallo"
#print x
#print "done"
#resCap = sim.getResCap();
#print resCap
#serviceLevel = np.where(resCap>=21,1.,0.).sum(axis=1)/float(weather.getNumScenarios()-1)
#print serviceLevel
