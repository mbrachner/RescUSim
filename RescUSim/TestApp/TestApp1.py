import matplotlib
matplotlib.use('Qt4Agg')
import sys
sys.path.append('C:\\Users\\mbrachner\\Source\\Repos\\RescUSim\\RescUSim\\x64\\Release\\')
import RescUSimCpp
import Weather
import numpy as np
import time
from mpl_toolkits.basemap import Basemap
import matplotlib.pyplot as plt

print "Preparing map...",
fig, ax = plt.subplots()
m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=38, urcrnrlat=74.6,area_thresh = 100,ax=ax)
print "done"


print "Loading weather...",
weatherData = Weather.loadWeather(m)

minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
weather = RescUSimCpp.Weather(weatherData,RescUSimCpp.Bounds(minx, maxx, miny, maxy))
print "done"

print "Setting up RUs...",
sim = RescUSimCpp.SimulatorOpenCL(weather)
ru1 = RescUSimCpp.Helicopter("Heli1").setPos(357309.0, 131195.0)
ru2 = RescUSimCpp.Helicopter("Heli2").setPos(255000.0, 554000.0)

#ru3 = RescUSimCpp.ERV("ERV1").setPos(726000.0, 450000.0)
#ru4 = RescUSimCpp.ERV("ERV2").setPos(723558.0, 345403.0)
#ru5 = RescUSimCpp.ERV("ERV3").setPos(637900.0, 273100.0)
print "done"

print "Adding POIs...",
grid=np.array(np.mgrid[minx:maxx:10000, miny:maxy:10000],dtype=np.float32)
grid = np.array(np.transpose(grid,axes=[1,2,0]).reshape(grid.shape[1]*grid.shape[2],grid.shape[0]))

sim.addPoi(grid)
print "done"

start = time.clock()
print "Adding r1..."
sim.addRU(ru1);
end = time.clock()
print "done",
print (end-start)

#start = time.clock()
#print "Initializing OpenCL..."
#sim.initOpenCL()
#end = time.clock()
#print "done",
#print (end-start)

#print "Adding RUs...",
#start = time.clock()
#sim.addRUOpenCL(ru1);
#end = time.clock()
#print "done",
#print (end-start)

#print "Adding RUs...",
#start = time.clock()
#sim.addRUOpenCL(ru2);
#end = time.clock()
#print "done",
#print (end-start)

#sim.simulateResponse();

#resCap = sim.getResCap()
#print resCap.shape
#capMax = np.max(resCap);

#numScenarios = 2920
#scenValid = np.ones ((numScenarios),dtype=np.bool);
#numValid = len(scenValid[scenValid == True]);

#startpointx, startpointy = (357309.0, 131195.0)
#m.fillcontinents(color='lightgray',lake_color='blue',zorder=15);
#m.drawcoastlines(zorder=15); 
#m.drawparallels(np.arange(-80.,81.,20.),zorder=10);
#m.drawmapboundary(fill_color='white',zorder=10)
#m.plot(357309.0, 131195.0,marker='*',markersize=40,zorder=20,color='yellow')
#m.plot(255000.0, 554000.0,marker='*',markersize=40,zorder=20,color='yellow')
#serviceLevel=0.90
#grid=grid.reshape(83,59,2)
#resCap=resCap.reshape(83,59,numScenarios)
#resCap=resCap[:,:,scenValid]
#print resCap.shape
#print resCap[:,:,0]
#slGrid = np.where(resCap>21,1.,0.).sum(axis=2)/float(numValid)
#slGrid = np.ma.masked_where(slGrid < serviceLevel, slGrid)

#cm = ax.pcolormesh(grid[:,:,0],grid[:,:,1],slGrid[:,:], vmin=serviceLevel, vmax=1.,zorder=1)
#cbar = fig.colorbar(cm)
#for l in cbar.ax.yaxis.get_ticklabels():
#    l.set_size(28)

#fig.tight_layout()
#plt.draw()
#plt.show()

#print "Adding RUs...",
#start = time.clock()
#sim.addRU(ru1);
#end = time.clock()
#print "done",
#print (end-start)

#print "Adding RUs...",
#start = time.clock()
#sim.addRUOpenCL(ru2);
#end = time.clock()
#print "done",
#print (end-start)
