import RescUSimCpp
import numpy as np

f = np.load("C:\\tmp\\w.npz")
wd = f['arr_0']
wsp = f['arr_1']
hs = f['arr_2']

r = RescUSimCpp.Helicopter("Test")
r.setName("Neu")
print r.getName()

print wd.shape
weather = RescUSimCpp.Weather(wd,wsp,hs);
print "{0}, {1}".format(weather.wdAt(5,20,2),wd[5,20,2]);
print "{0}, {1}".format(weather.wspAt(5,20,2),wsp[5,20,2]);
print "{0}, {1}".format(weather.hsAt(5,20,2),hs[5,20,2]);

sim = RescUSimCpp.Simulator(weather);
r.setPos(10,20);
print r.getPos();
sim.addStationaryRU(r);
sim.addStationaryRU(RescUSimCpp.ERV("ERV1"))
sim.addStationaryRU(RescUSimCpp.ERV("ERV2"))



