import numpy as np
import matplotlib.pyplot as plt

w = np.pi # 180°/s
t = np.arange(0.01, 1.001, 0.01)

theta_vrai = w*t
theta_extrap = np.arctan(w*t) # projection de I + W*t sur SO(3)
err_rad = theta_vrai - theta_extrap
err_deg = np.degrees(err_rad)

err_pos_1m = 1000 * 2 * np.sin(err_rad/2)

plt.figure(figsize=(8,5))
plt.plot(t*1000, err_deg, label="Erreur angulaire")
plt.axvline(100, color=\'r\', linestyle=\'--\', label="Borne 100ms")
plt.axhline(1.0, color=\'g\', linestyle=\':\', label="Seuil 1° perceptible")
plt.yscale("log")
plt.xlabel("Durée d\'extrapolation [ms]")
plt.ylabel("Erreur angulaire [deg] - log")
plt.title("Erreur: wt - atan(wt) pour w=180°/s")
plt.legend()
plt.grid(True, which="both")
plt.show()

dt = 0.001
for T in [0.1, 0.2, 0.5]:
    n = int(T/dt)
    theta_step = n * w * dt
    print(f"T={T*1000:.0f}ms vrai={np.degrees(theta_step):.2f}°")

Conclusion : L'extrapolation linéaire est gratuite et parfaite sur 10-30ms, acceptable jusqu'à 80-120ms. La borne à 100ms est le compromis optimal: c'est le dernier point où l'erreur géométrique reste sub-degré et non perceptible. Au delà, il faut ré-ancrer sur une mesure, pas extrapoler.