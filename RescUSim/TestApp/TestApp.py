import RescUSimCpp
import numpy as np
import time

f = np.load("C:\\tmp\\w.npz")
wd = f['arr_0']
wsp = f['arr_1']
hs = f['arr_2']



print wd.shape
#weather = RescUSimCpp.Weather(wd,wsp,hs);
#print "{0}, {1}".format(weather.wdAt(5,20,2),wd[5,20,2]);
#print "{0}, {1}".format(weather.wspAt(5,20,2),wsp[5,20,2]);
#print "{0}, {1}".format(weather.hsAt(5,20,2),hs[5,20,2]);

sim = RescUSimCpp.Simulator(RescUSimCpp.Weather(wd,wsp,hs));
r=RescUSimCpp.Helicopter("Heli1")
r.setPos(357309.0, 131195.0)
r.setSpeed(72.2222222)
sim.addStationaryRU(r)
#sim.addStationaryRU(RescUSimCpp.Helicopter("Heli2"));
#sim.addStationaryRU(RescUSimCpp.Helicopter("Heli3"));
#sim.addStationaryRU(RescUSimCpp.ERV("ERV1"))
#sim.addStationaryRU(RescUSimCpp.ERV("ERV2"))

minx, maxx, miny, maxy = (27686.0,848650.0,56061.0,645608.0)
grid=np.mgrid[0:maxx-minx:10000, 0:maxy-miny:10000]
grid = np.transpose(grid,axes=[1,2,0]).reshape(grid.shape[1]*grid.shape[2],grid.shape[0])
#grid = np.random.rand(10,2)*100;
print grid
sim.addPoi(grid)

start = time.clock()
sum = sim.simulate()
end = time.clock()
print (end-start)
print sum

