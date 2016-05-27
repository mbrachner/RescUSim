import sys
sys.path.append('C:\\Users\\mbrachner\\Source\\Repos\\RescUSim\\RescUSim\\x64\\Release\\')
import RescUSimCpp
import h5py
import numpy as np
from mpl_toolkits.basemap import Basemap

m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=38, urcrnrlat=74.6,area_thresh = 100)
h5f = h5py.File('c:\\tmp\\data.h5','r')
weather_points = h5f['weather_points'][:]
[coord] = np.dstack(m(weather_points['lon'],weather_points['lat']));
print coord;
w = RescUSimCpp.PointWeather(coord);
print w.wdAtP(0.5,1,2);