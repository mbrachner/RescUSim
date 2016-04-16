import numpy as np
import h5py
import glob
from mpl_toolkits.basemap import Basemap
from scipy.spatial import cKDTree

def loadWeather(m):
    h5f = h5py.File('c:\\tmp\\data.h5','r')
    #randlist = np.random.randint(0,h5f['weather_data'].shape[0],2920)
    randlist = np.arange(161686,164606)
    randlist_usort = list(np.sort(np.array(list(set(randlist)))))
    weather_points = h5f['weather_points'][:]
    [coord] = np.dstack(m(weather_points['lon'],weather_points['lat']))
    weather_data = h5f['weather_data'][randlist_usort]
    h5f.close()

    minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
    grid_x, grid_y = np.mgrid[minx:maxx:10000, miny:maxy:10000]
    [vluPoints]=np.dstack(([grid_x.reshape(grid_x.shape[0]*grid_x.shape[1],1)],[grid_y.reshape(grid_y.shape[0]*grid_y.shape[1],1)]))

    tree = cKDTree(coord)

    d, inds = tree.query(vluPoints, k = 10)
    _,indsNN = tree.query(vluPoints, k = 1)
    w = 1.0 / d**2
    su = np.sum(w, axis=1)
    values_idw = np.empty((weather_data.shape[0],grid_x.shape[0]*grid_x.shape[1]),
                           dtype=[('wsp', np.float32),('wdir', np.float32),('hs', np.float32),('light', np.uint32)])
    for d in range(0,values_idw.shape[0]):
        values_idw[d,:]['wsp'] = weather_data[d,:]['wsp'][indsNN] / 10.
        values_idw[d,:]['wdir'] = np.deg2rad(weather_data[d,:]['dir'][indsNN])
        values_idw[d,:]['hs'] = (np.sum(w * (weather_data[d,:]['hs'][inds]), axis=1) / su)/10.
        values_idw[d,:]['light'] = weather_data[d,:]['light'][indsNN]

    values_idw.shape =((values_idw.shape[0],grid_x.shape[0],grid_x.shape[1]))

    return values_idw