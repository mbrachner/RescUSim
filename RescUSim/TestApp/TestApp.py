import matplotlib
matplotlib.use('Qt4Agg')
import RescUSimCpp
import numpy as np
import time
from mpl_toolkits.basemap import Basemap
import matplotlib.pyplot as plt

import matplotlib.animation as animation
from time import sleep
import sqlalchemy as sa
import datetime
import pandas as pd
import Weather


def getScenario(hoursFromStart):
    dt = datetime.datetime(2013,1,1,0)+datetime.timedelta(hours=1)*hoursFromStart
    timestamp = (dt - datetime.datetime(1970, 1, 1)).total_seconds()
    
    tw = 24*60


    with disk_engine.connect() as conn:
        sqlstr = """
        SELECT mmsi,sec,lon,lat FROM data WHERE 1 
                               AND sec >= {0}-60*{1} 
                               AND sec <= {0}+60*{1}
        """.format(timestamp,tw/2)
        df = pd.read_sql_query(sqlstr,conn)


    dfl = df[df['sec']<=timestamp]
    dfm = df[df['sec']>=timestamp]

    sdfl = dfl.ix[dfl.groupby('mmsi')['sec'].idxmax()]
    sdfm = dfm.ix[dfm.groupby('mmsi')['sec'].idxmin()]

    res = pd.merge(sdfl,sdfm,on='mmsi',suffixes=['_before','_after'])
    res['timefrac']=(timestamp-res['sec_before'])/(res['sec_after']-res['sec_before'])
    coordsBefore=np.array(m(*np.array(res[['lon_before','lat_before']]).T))
    coordsAfter=np.array(m(*np.array(res[['lon_after','lat_after']]).T))
    res['coordsNowX'],res['coordsNowY'] =coordsBefore+(coordsAfter-coordsBefore)*np.array([res['timefrac']])
    res = res[(res['coordsNowX']>=0) & (res['coordsNowY']>=0)]
    return (np.array(res[['coordsNowX','coordsNowY']]))


disk_engine = sa.create_engine('sqlite:///c:\\tmp\\ais.db')


fig, ax = plt.subplots()
m = Basemap(projection='aeqd',lat_0=72,lon_0=29, resolution='l',   
            llcrnrlon=15, llcrnrlat=69,
            urcrnrlon=38, urcrnrlat=74.6,area_thresh = 100,ax=ax)


#matplotlib.pyplot.ion();


weatherData = Weather.loadWeather(m)


numScenarios = 2920

#f = np.load("C:\\tmp\\w.npz")
#wd = f['arr_0'][:numScenarios]
#wsp = f['arr_1'][:numScenarios]
#hs = f['arr_2'][:numScenarios]


#print wsp[23,4,5],wd[23,4,5],hs[23,4,5]
#print weatherData[23,4,5]

minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)



weather = RescUSimCpp.Weather(weatherData,RescUSimCpp.Bounds(minx, maxx, miny, maxy))
print "Das Wetter: ",
print weather.wspAt(1400,4,5),weather.wdAt(1400,4,5),weather.hsAt(1400,4,5),weather.lightAt(1400,4,5);

sim = RescUSimCpp.Simulator(weather)
sim.addStationaryRU(RescUSimCpp.Helicopter("Heli1").setPos(357309.0, 131195.0))
sim.addStationaryRU(RescUSimCpp.Helicopter("Heli2").setPos(255000.0, 554000.0))

#sim.addStationaryRU(RescUSimCpp.ERV("ERV1").setPos(726000.0, 450000.0))
#sim.addStationaryRU(RescUSimCpp.ERV("ERV2").setPos(723558.0, 345403.0))
#sim.addStationaryRU(RescUSimCpp.ERV("ERV3").setPos(637900.0, 273100.0))
scenValid = np.ones ((numScenarios),dtype=np.bool);
for c in range(0):
    #for n in range(0):
    #    x = float(np.random.randint(minx,maxx))
    #    y = float(np.random.randint(miny,maxy))
    #    sim.addTemporaryRU(RescUSimCpp.ERV("ERV").setPos(x,y),c)
    cs = getScenario(c)
    scenValid[c] = (len(cs) > 0);

    print "Scenario {0}: Adding {1}".format(c, cs.shape)
    for cx,cy in cs:
        sim.addTemporaryRU(RescUSimCpp.ERV("ERV").setPos(cx,cy),c)


numValid = len(scenValid[scenValid == True]);
print ("Valid: %s" % numValid);

minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
grid=np.mgrid[minx:maxx:10000, miny:maxy:10000]
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
#print resCap[np.all(grid[:]==[10000.,40000.],axis=1)][0,234]
capMax = np.max(resCap);

startpointx, startpointy = (357309.0, 131195.0)
#startpointx, startpointy = (360000.0, 130000.0)

#fig, ax = plt.subplots()

#grid_x, grid_y = np.mgrid[0:maxx-minx:10000, 0:maxy-miny:10000]


m.fillcontinents(color='lightgray',lake_color='blue',zorder=15);
m.drawcoastlines(zorder=15); 
m.drawparallels(np.arange(-80.,81.,20.),zorder=10);
#m.drawmeridians(np.arange(-180.,181.,20.),zorder=0); 
m.drawmapboundary(fill_color='white',zorder=10)
#m.plot(startpointx,startpointy,marker='*',markersize=15,zorder=20,color='yellow')
m.plot(357309.0, 131195.0,marker='*',markersize=40,zorder=20,color='yellow')
m.plot(255000.0, 554000.0,marker='*',markersize=40,zorder=20,color='yellow')
#m.plot(726000.0, 450000.0,marker='v',markersize=15,zorder=20,color='yellow')
#m.plot(723558.0, 345403.0,marker='v',markersize=15,zorder=20,color='yellow')
#m.plot(637900.0, 273100.0,marker='v',markersize=15,zorder=20,color='yellow')

grid=grid.reshape(83,59,2)
resCap=resCap.reshape(83,59,numScenarios)
resCap=resCap[:,:,scenValid]
print resCap.shape
#print ("Valid scenarios shape: %s" % resCap.shape);
slGrid = np.where(resCap>21,1.,0.).sum(axis=2)/float(numValid)
serviceLevel = 0.90
#np.save("C:\\tmp\\sl_aa_full_mit",slGrid)
slGrid = np.ma.masked_where(slGrid < serviceLevel, slGrid)

#cm = ax.pcolormesh(grid[:,:,0],grid[:,:,1],resCap[:,:,0], vmin=21, vmax=capMax,zorder=1)
cm = ax.pcolormesh(grid[:,:,0],grid[:,:,1],slGrid[:,:], vmin=serviceLevel, vmax=1.,zorder=1)
cbar = fig.colorbar(cm)
for l in cbar.ax.yaxis.get_ticklabels():
    #l.set_family("Times New Roman")
    l.set_size(28)

def animate(i):
    cm.set_array(resCap[:-1,:-1,i].ravel())  # update the data
    return cm,

#Init only required for blitting to give a clean slate.
def init():
    cm.set_array(np.array([]))
    return cm,

#ani = animation.FuncAnimation(fig, animate, range(0, numScenarios), init_func=init,blit=False, interval=1000)

fig.tight_layout()
plt.draw()
plt.show()
