import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("UpwindGodunov_step_0.dat")

x   = data[:,0]
rho = data[:,1]
P   = data[:,3]
E   = data[:,4]

plt.plot(x,rho)
plt.ylim(0.0,3.0)
plt.xlabel("$t$")
plt.ylabel("$rho$")
plt.savefig(u"densidad")
plt.close()

plt.plot(x,P)
plt.ylim(0.0,3.0)
plt.xlabel("$t$")
plt.ylabel("$P$")
plt.savefig(u"Presion")
plt.close()

plt.plot(x,E)
plt.ylim(0.0,3.0)
plt.xlabel("$t$")
plt.ylabel("$E$")
plt.savefig(u"Energia")
plt.close()


