import RescUSimCpp
import h5py
from shapely.geometry import LineString, MultiPoint
from mpl_toolkits.basemap import Basemap
import numpy as np

m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=41, urcrnrlat=75.6,area_thresh = 100)
hammerfest = m(23.768302,70.701319);
berlevaag = m(29.090389, 70.854502);
wisting = m(24.232358, 73.491134);
castberg = m(20.347568, 72.494341);
extreme_remote = m(37.000000, 74.500000);

print "Loading weather..."
h5f = h5py.File('c:\\tmp\\data_idw.h5', 'r')
descr = np.dtype([('wind_xcomp', '<f4'), ('wind_ycomp', '<f4'), ('hs', '<f4'), ('light', 'u1')],align=1)

weatherData = np.array(h5f['weather_data'][:],dtype=descr)
minx, miny, maxx, maxy = h5f['weather_data'].attrs.get("bounds");
resolution = h5f['weather_data'].attrs.get("resolution");
print minx, miny, resolution
weather = RescUSimCpp.Weather(weatherData,float(minx),float(miny),int(resolution))
print "Done"

print "Initializing weather..."
sim = RescUSimCpp.SimulatorCPU(weather)
print "Und jetyt"

print "Adding Helicopter"
sim.addStationaryRU(RescUSimCpp.Helicopter("Heli1").setPos(*hammerfest));
print "Done"

print "Adding ERV"
sim.addStationaryRU(RescUSimCpp.ERV("ERV1").setPos(*castberg));
print "Done"


