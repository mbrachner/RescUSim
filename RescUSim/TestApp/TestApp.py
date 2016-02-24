import RescUSimCpp
import numpy as np

f = np.load("C:\\tmp\\w.npz")
wd = f['arr_0']
wsp = f['arr_1']
hs = f['arr_2']

r = RescUSimCpp.Helicopter("Test")
r.setName("Neu")
print r.getName()

sim = RescUSimCpp.Simulator();
sim.addStationaryRU(r);

