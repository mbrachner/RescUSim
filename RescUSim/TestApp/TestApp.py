import matplotlib
matplotlib.use('Qt4Agg')
import RescUSimCpp
import numpy as np
import time
from mpl_toolkits.basemap import Basemap
import matplotlib.pyplot as plt

import matplotlib.animation as animation
from time import sleep

#matplotlib.pyplot.ion();

f = np.load("C:\\tmp\\w.npz")
wd = f['arr_0']
wsp = f['arr_1']
hs = f['arr_2']



print wd.shape
#weather = RescUSimCpp.Weather(wd,wsp,hs);
#print "{0}, {1}".format(weather.wdAt(5,20,2),wd[5,20,2]);
#print "{0}, {1}".format(weather.wspAt(5,20,2),wsp[5,20,2]);
#print "{0}, {1}".format(weather.hsAt(5,20,2),hs[5,20,2]);

sim = RescUSimCpp.Simulator(RescUSimCpp.Weather(wd,wsp,hs))
sim.addStationaryRU(RescUSimCpp.Helicopter("Heli1").setPos(357309.0, 131195.0).setMaxCapacity(15))
sim.addStationaryRU(RescUSimCpp.Helicopter("Heli2").setPos(255000.0, 554000.0).setMaxCapacity(15))

sim.addStationaryRU(RescUSimCpp.ERV("ERV1").setPos(726000.0, 450000.0))
sim.addStationaryRU(RescUSimCpp.ERV("ERV2").setPos(723558.0, 345403.0))
sim.addStationaryRU(RescUSimCpp.ERV("ERV3").setPos(637900.0, 273100.0))


minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
grid=np.mgrid[0:maxx-minx:10000, 0:maxy-miny:10000]
grid = np.array(np.transpose(grid,axes=[1,2,0]).reshape(grid.shape[1]*grid.shape[2],grid.shape[0]))
#grid = np.random.rand(10,2)*100;
print grid.shape
print grid
sim.addPoi(grid)

start = time.clock()
sum = sim.simulate()
end = time.clock()
print (end-start)
resCap = sim.getResCap()
print resCap.shape
#print resCap[:,0]
#print resCap[:,1]
print resCap[np.all(grid[:]==[10000.,40000.],axis=1)][0,234]
capMax = np.max(resCap);

minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
startpointx, startpointy = (357309.0, 131195.0)
#startpointx, startpointy = (360000.0, 130000.0)

#fig, ax = plt.subplots()
fig, ax = plt.subplots()
grid_x, grid_y = np.mgrid[0:maxx-minx:10000, 0:maxy-miny:10000]
m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=38, urcrnrlat=74.6,area_thresh = 100,ax=ax)

m.fillcontinents(color='lightgray',lake_color='blue',zorder=15);
m.drawcoastlines(zorder=15); 
m.drawparallels(np.arange(-80.,81.,20.),zorder=10);
#m.drawmeridians(np.arange(-180.,181.,20.),zorder=0); 
m.drawmapboundary(fill_color='white',zorder=10)
#m.plot(startpointx,startpointy,marker='*',markersize=15,zorder=20,color='yellow')
m.plot(357309.0, 131195.0,marker='*',markersize=15,zorder=20,color='yellow')
m.plot(255000.0, 554000.0,marker='*',markersize=15,zorder=20,color='yellow')
m.plot(726000.0, 450000.0,marker='v',markersize=15,zorder=20,color='yellow')
m.plot(723558.0, 345403.0,marker='v',markersize=15,zorder=20,color='yellow')
m.plot(637900.0, 273100.0,marker='v',markersize=15,zorder=20,color='yellow')

grid=grid.reshape(83,59,2)
resCap=resCap.reshape(83,59,2920)
slGrid = np.where(resCap>21,1.,0.).sum(axis=2)/2920.
#cm = ax.pcolormesh(grid[:,:,0],grid[:,:,1],resCap[:,:,0], vmin=21, vmax=capMax,zorder=1)
cm = ax.pcolormesh(grid[:,:,0],grid[:,:,1],slGrid[:,:], vmin=0.95, vmax=1.,zorder=1)
cbar = fig.colorbar(cm)

#def animate(i):
#    cm.set_array(resCap[:-1,:-1,i].ravel())  # update the data
#    return cm,

##Init only required for blitting to give a clean slate.
#def init():
#    cm.set_array(np.array([]))
#    return cm,

#ani = animation.FuncAnimation(fig, animate, range(0, 2920), init_func=init,blit=False)

fig.tight_layout()
plt.draw()
plt.show()
