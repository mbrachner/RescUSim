import sys
import RescUSimCpp
import h5py
from shapely.geometry import LineString, MultiPoint
from mpl_toolkits.basemap import Basemap
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import scipy.stats
from shapely.ops import cascaded_union

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
bjornoya = m( 19.170307, 74.418419 );

pois_c = LineStringI([hammerfest,castberg]).union(
                LineStringI([hammerfest,wisting]).union(
                LineStringI([berlevaag,extreme_remote])));

#pois_c.to_wkt()
pois = cascaded_union([LineStringI(l).discretize(10000) for l in pois_c])

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
print "Adding RUs...",
rus = list()
rus.append(sim.addStationaryRU(RescUSimCpp.Helicopter("Heli1").setPos(*hammerfest)));
#rus.append(sim.addStationaryRU(RescUSimCpp.Helicopter("Heli2").setPos(*wisting)));
rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV1").setPos(*castberg)));
rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV2").setPos(*castberg)));
rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV3").setPos(*castberg)));
rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV4").setPos(*castberg)));
rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV5").setPos(*castberg)));
rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV6").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV7").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV8").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV9").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV10").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV11").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV12").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV13").setPos(*castberg)));
#rus.append(sim.addStationaryRU(RescUSimCpp.ERV("ERV14").setPos(*castberg)));
print "done"

sampleSize=10000

def coordToUnit((x,y)):
    xpos = (x-minx)/(maxx-minx)
    ypos = (y-miny)/(maxy-miny)
    return (xpos,ypos)

def unitsquare((x1,y1)):
    xpos = minx+(maxx-minx)*x1
    ypos = miny+(maxy-miny)*y1
    return (xpos,ypos)

def simat((x1,y1),ru):
    xpos = minx+(maxx-minx)*x1
    ypos = miny+(maxy-miny)*y1
    ru.setPos(*unitsquare((x1,y1)))
    n=sampleSize
    sim.sample(sampleSize);
    x = sim.simulateResponseSample();

    n1 = np.where(x>=21,1.,0.).sum(axis=0);
    alpha=0.05;
    z_alpha_2=scipy.stats.norm.ppf(1-alpha/2);
    pd = (n1+0.5*z_alpha_2**2)/(n+z_alpha_2**2);
    sq = z_alpha_2*np.sqrt((pd*(1-pd))/(n+z_alpha_2**2));
    agresti_coull = (pd-sq,pd+sq);
    bt = scipy.stats.binom_test(n1, n, p=1-alpha,alternative='less')

    return (pd,agresti_coull,bt,x)


def simul():
    n=sampleSize

    x = sim.simulateResponseSample();

    n1 = np.where(x>=21,1.,0.).sum(axis=0);
    alpha=0.05;
    z_alpha_2=scipy.stats.norm.ppf(1-alpha/2);
    pd = (n1+0.5*z_alpha_2**2)/(n+z_alpha_2**2);
    sq = z_alpha_2*np.sqrt((pd*(1-pd))/(n+z_alpha_2**2));
    agresti_coull = (pd-sq,pd+sq);
    bt = scipy.stats.binom_test(n1, n, p=1-alpha,alternative='less')

    return (pd,agresti_coull,bt,x)

bestof=list()

sim.sample(sampleSize);
bestVal=0
for ru in rus:
    ra = np.random.random(2);
    print ra
    ru.setPos(*unitsquare(tuple(ra)));
bestVal = 0;
for rep in range(30):
    sd = 15./(rep+1)/31
    print "SD ", sd
    #for ru in rus:
    #ru = rus[np.random.randint(0,len(rus))]
    ru = rus[rep % len(rus)]
    res = list()
    for x in range(10):
        p = (np.random.random(2)-0.5)*sd
        nc = np.array(coordToUnit(ru.getPosTuple()))
        #print nc
        #print nc+p
        npos = np.min(np.dstack((np.max(np.dstack(((nc+p),[0.1,0.1]))[0],axis=1),[0.9,0.9]))[0],axis=1)
        print "NP ",npos
        ru.setPos(*unitsquare((npos[0],npos[1])))
        c = simul()
        res.append((p[0],p[1],c[0],c[1][0],c[1][1],c[2],c[3]))
    print "Finished"
    res=sorted(res,key= lambda t:t[2],reverse=True)[0]
    #print res
    #res = np.array(res)
    mx,my,mv,acl,ach,bt,bres = res
    mxp = minx+(maxx-minx)*mx
    myp = miny+(maxy-miny)*my
    ru.setPos(mxp,myp)
    
    if mv > bestVal:
        bestVal = mv
        ps = [(r,r.getPosTuple()) for r in rus ] 
    print rep,(acl,ach),bt,mv
bestof.append((bestVal,ps,bres))