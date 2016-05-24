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
from shapely.geometry import LineString, MultiPoint
from time import sleep;
from matplotlib import animation;


class LineStringI(LineString):
    def discretize(line,dist):
        return MultiPoint([line.interpolate(f) for f in np.arange(0,line.length,dist)]+[line.interpolate(line.length)])

print "Preparing map...",
fig, ax = plt.subplots()

m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=38, urcrnrlat=74.6,area_thresh = 100,ax=ax)
print "done"
#plt.ion();

hammerfest = m(23.768302,70.701319);
berlevaag = m(29.090389, 70.854502);
wisting = m(24.232358, 73.491134);
castberg = m(20.347568, 72.494341);
extreme_remote = m(37.000000, 74.500000);
#print hammerfest;






print "Loading weather...",
weatherData = Weather.loadWeather(m)

minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
a=np.array((minx,miny))
b=np.array((maxx,maxy))

weather = RescUSimCpp.Weather(weatherData,RescUSimCpp.Bounds(minx, maxx, miny, maxy))
print "done"

print "Setting up RUs...",
#sim = RescUSimCpp.SimulatorOpenCL(weather)
sim = RescUSimCpp.SimulatorOpenCL(weather)
#ru1 = RescUSimCpp.Helicopter("Heli1").setPos(357309.0, 131195.0)
rus = list();
rus.append(RescUSimCpp.Helicopter("Heli1").setPos(*hammerfest));
ru2 = RescUSimCpp.Helicopter("Heli2").setPos(255000.0, 554000.0)
rus.append(RescUSimCpp.Helicopter("Heli2").setPos(255000.0, 554000.0));

#ru3 = RescUSimCpp.ERV("ERV1").setPos(726000.0, 450000.0)
#ru4 = RescUSimCpp.ERV("ERV2").setPos(723558.0, 345403.0)
#ru5 = RescUSimCpp.ERV("ERV3").setPos(637900.0, 273100.0)
print "done"

#grid=np.array(np.mgrid[minx:maxx:10000, miny:maxy:10000],dtype=np.float32)
#grid = np.array(np.transpose(grid,axes=[1,2,0]).reshape(grid.shape[1]*grid.shape[2],grid.shape[0]))
#sim.addPoi(grid)

pois = np.array(LineStringI([hammerfest,castberg]).discretize(10000).union(
                LineStringI([hammerfest,wisting]).discretize(10000).union(
                LineStringI([berlevaag,extreme_remote]).discretize(10000))
                ),dtype=np.float32);
print ("Adding {} POIs...".format(pois.shape[0]));
sim.addPoi(pois);
print "done"

numScenarios = 2920;

startpointx, startpointy = (357309.0, 131195.0)

m.fillcontinents(color='lightgray',lake_color='blue',zorder=15);
m.drawcoastlines(zorder=15); 
m.drawparallels(np.arange(-80.,81.,20.),zorder=10);
m.drawmapboundary(fill_color='white',zorder=10)
#m.plot(357309.0, 131195.0,marker='*',markersize=40,zorder=20,color='yellow')
m.plot(255000.0, 554000.0,marker='*',markersize=5,zorder=20,color='yellow')
m.plot(*hammerfest, marker='o',markersize=5,zorder=20,color='yellow')
m.plot(*castberg, marker='o',markersize=5,zorder=20,color='yellow')
m.plot(*wisting, marker='o',markersize=5,zorder=20,color='yellow')
m.plot(*berlevaag, marker='o',markersize=5,zorder=20,color='yellow')
m.plot(*extreme_remote, marker='o',markersize=5,zorder=20,color='yellow')
markersCur = dict();
for r in rus:
    mo, = m.plot(*r.getPosTuple(), marker='o',markersize=20,zorder=20,color='yellow');
    markersCur[r] = mo;
markersOpt = dict();
for r in rus:
    mo, = m.plot(*r.getPosTuple(), marker='*',markersize=20,zorder=20,color='red');
    markersOpt[r] = mo;

cm = plt.cm.get_cmap('RdYlBu')
scp = m.scatter(pois[:,0],pois[:,1], c=np.zeros(pois.shape[0]), marker='o', vmin=0.9, vmax=1., s=50, cmap=cm, zorder=3);
fig.tight_layout();
#plt.pause(0.05);

#plt.draw();
#plt.show()

bestsol = dict();
bestobj = 0

def init():
    for r in rus:
        x,y = r.getPosTuple();
        markersCur[r].set_data([x], [y]);
        markersOpt[r].set_data([x], [y]);
    scp.set_array(np.zeros(pois.shape[0]));
    
    return [markersCur[r] for r in rus]+[markersOpt[r] for r in rus]+list([scp])

def animate(i):
    global bestobj;

    print ("Iteration {}".format(i));
    ru = rus[i%2];
    sim.removeRU(ru);
    oldX,oldY = ru.getPosTuple();
    #rus[idx].setPos(x,y+1000);
    x,y = (b-a)*np.random.random(2)+a;
    ru.setPos(x,y);
    for r in rus:
        markersCur[r].set_data(*r.getPosTuple());

    sim.addRU(ru);
    sim.simulateResponse();
    
    resCap = sim.getResCap();
    serviceLevel = np.where(resCap>=21,1.,0.).sum(axis=1)/float(weather.getNumScenarios())
    
    objective = np.where(serviceLevel>0.99,1.,0.).sum();
    
    if objective > bestobj:
        bestobj = objective;
        scp.set_array(serviceLevel);

        for r in rus:
            markersOpt[r].set_data(*r.getPosTuple());
        
        
    else:
        sim.removeRU(ru);
        oldX, oldY = np.array([oldX,oldY])+np.array([2000,2000])*np.random.random(2)-np.array([1000,1000]);
        ru.setPos(oldX, oldY);
        sim.addRU(ru);
        
    
    
    
    print objective, bestobj;

    

    return [markersCur[r] for r in rus]+[markersOpt[r] for r in rus]+list([scp])

anim = animation.FuncAnimation(fig, animate, init_func=init,
                               interval=0,blit=True)

plt.colorbar(scp)
plt.show();
#for r in rus:
#    print ("Adding {}...".format(r.getName()));
#    sim.addRU(r);
#sim.simulateResponse();
#resCap = sim.getResCap();
#print resCap.shape;

#for i in range(0,400):
#    print ("Iteration {}".format(i));
#    for k,r in enumerate(rus):
#        sim.removeRU(r);
#        x,y = r.getPosTuple();
#        if k==0:
#            r.setPos(x,y+1000);
#            #m.plot(*r.getPosTuple(), marker='*',markersize=20,zorder=20,color='red');
#            markers[k].set_data(*r.getPosTuple());
#            plt.pause(0.000001);
#        print ("Adding {}...".format(r.getName()));
#        start = time.clock()
#        sim.addRU(r);
#        end = time.clock()
#        print "done",
#        print (end-start)
#        sim.simulateResponse();




