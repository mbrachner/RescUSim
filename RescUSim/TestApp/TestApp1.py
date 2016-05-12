import sys
sys.path.append('C:\\Users\\mbrachner\\Source\\Repos\\RescUSim\\RescUSim\\x64\\Release\\')
import RescUSimCpp
import Weather
import numpy as np
from mpl_toolkits.basemap import Basemap

print "Loading weather...",
m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=38, urcrnrlat=74.6,area_thresh = 100)
weatherData = Weather.loadWeather(m)

minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
weather = RescUSimCpp.Weather(weatherData,RescUSimCpp.Bounds(minx, maxx, miny, maxy))
print "done"

print "Setting up RUs...",
sim = RescUSimCpp.Simulator(weather)
ru1 = RescUSimCpp.Helicopter("Heli1").setPos(357309.0, 131195.0)

#ru2 = RescUSimCpp.Helicopter("Heli2").setPos(255000.0, 554000.0)
#ru3 = RescUSimCpp.ERV("ERV1").setPos(726000.0, 450000.0)
#ru4 = RescUSimCpp.ERV("ERV2").setPos(723558.0, 345403.0)
#ru5 = RescUSimCpp.ERV("ERV3").setPos(637900.0, 273100.0)
print "done"

print "Adding POIs...",
grid=np.array(np.mgrid[minx:maxx:10000, miny:maxy:10000],dtype=np.float32)
grid = np.array(np.transpose(grid,axes=[1,2,0]).reshape(grid.shape[1]*grid.shape[2],grid.shape[0]))

sim.addPoi(grid)
print "done"

print "Adding RUs...",
sim.addRUOpenCL(ru1);
print "done"