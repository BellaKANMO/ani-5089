#include "NKWindow/NKMain.h"
#include "NKRenderer/Core/NkRendererTypes.h"
#include "NKMath/NkQuat.h"

#include <cmath>
#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

	const double kPi = 3.14159265358979323846;
	const double kHz = 72.0;
	const double kPeriode = 1.0 / kHz;
	const int kImages = 216;
	const uint32 kMax = 64;

	double Lacet(double t) {
		const double amplitude = 90.0, duree = 0.400;
		if (t <= 0.0) return 0.0;
		if (t >= duree) return amplitude;
		return amplitude * 0.5 * (1.0 - cos(kPi * t / duree));
	}

	NkQuatf Pose(double t) {
		return NkQuatf::RotateY(NkAngle((float32)Lacet(t - 1.0)));
	}

	NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dtSeconds) {
		if (dtSeconds <= 0.f) {
			return NkVec3f(0.f, 0.f, 0.f);
		}
		NkQuatf delta = (to * from.Conjugate()).Normalized();
		if (delta.w < 0.f) {
			delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
		}
		const float32 angleRad = delta.Angle().Rad();
		if (angleRad <= 1e-6f) {
			return NkVec3f(0.f, 0.f, 0.f);
		}
		return delta.Axis() * (angleRad / dtSeconds);
	}

	NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse, float32 dtSeconds) {
		const float32 speedRad = vitesse.Len();
		if (speedRad > 1e-6f) {
			const NkVec3f axis = vitesse * (1.f / speedRad);
			const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
			return (delta * orientation).Normalized();
		}
		return orientation;
	}

	double AngleDeg(const NkQuatf &a, const NkQuatf &b) {
		const NkQuatf d = (b * a.Conjugate()).Normalized();
		const double s = std::sqrt((double)d.x * d.x + (double)d.y * d.y + (double)d.z * d.z);
		return 2.0 * std::atan2(s, std::fabs((double)d.w)) * 180.0 / kPi;
	}

	double LacetDeg(NkQuatf q) {
		if (q.w < 0.f) {
			q = NkQuatf(-q.x, -q.y, -q.z, -q.w);
		}
		return 2.0 * std::atan2((double)q.y, (double)q.w) * 180.0 / kPi;
	}

	struct Echantillon {
		double temps = 0.0;
		NkQuatf pose;
		NkVec3f vitesse = NkVec3f(0.f, 0.f, 0.f);
	};

	struct Historique {
		Echantillon e[kMax];
		uint32 tete = kMax - 1;
		uint32 nombre = 0;

		void Ajouter(const Echantillon &s) {
			tete = (tete + 1) % kMax;
			e[tete] = s;
			if (nombre < kMax) ++nombre;
		}

		const Echantillon &Au(double now, double latence) const {
			if (latence <= 0.0 || nombre <= 1) {
				return e[tete];
			}
			const double cible = now - latence;
			uint32 index = tete;
			for (uint32 i = 0; i < nombre; ++i) {
				if (e[index].temps <= cible) {
					return e[index];
				}
				index = (index + kMax - 1) % kMax;
			}
			return e[(tete + kMax - (nombre - 1)) % kMax];
		}
	};

	NkQuatf Predire(const Echantillon &s, double quand, bool borne) {
		double dt = quand - s.temps;
		if (borne) {
			if (dt > 0.1) dt = 0.1;
			if (dt < -0.1) dt = -0.1;
		}
		return Extrapoler(s.pose, s.vitesse, (float32)dt);
	}

	template <typename F>
	void Parcourir(F &&f) {
		Historique h;
		NkQuatf precedente = Pose(0.0);
		for (int i = 0; i <= kImages; ++i) {
			const double t = (double)i * kPeriode;
			const NkQuatf p = Pose(t);
			Echantillon s;
			s.temps = t;
			s.pose = p;
			if (i > 0) {
				s.vitesse = VitesseAngulaire(precedente, p, (float32)kPeriode);
			}
			h.Ajouter(s);
			f(i, t, h);
			precedente = p;
		}
	}

	void Latence(int ms, FILE *csv) {
		const double latence = (double)ms * 0.001;
		double sommeAvec = 0.0, sommeSans = 0.0, maxAvec = 0.0, maxSans = 0.0;
		int n = 0;
		Parcourir([&](int i, double t, const Historique &h) {
			if (i < 72 || i > 136) return;
			const Echantillon &s = h.Au(t, latence);
			const NkQuatf vraie = Pose(t + kPeriode);
			const double avec = AngleDeg(Predire(s, t + kPeriode, true), vraie);
			const double sans = AngleDeg(s.pose, vraie);
			sommeAvec += avec;
			sommeSans += sans;
			if (avec > maxAvec) maxAvec = avec;
			if (sans > maxSans) maxSans = sans;
			++n;
		});
		if (n == 0) return;
		std::printf("latency %d ms : with %.4f deg (max %.4f), without %.4f deg (max %.4f)\n",
					ms, sommeAvec / n, maxAvec, sommeSans / n, maxSans);
		std::fprintf(stderr, "latency %d ms : %d images\n", ms, n);
		if (csv != nullptr) {
			std::fprintf(csv, "%d,%.6f,%.6f\n", ms, sommeAvec / n, sommeSans / n);
		}
	}

	void Borne() {
		const double latence = 1.0;
		double ecart = 1e9;
		int meilleur = -1;
		double quand = 0.0;
		Echantillon choisi;
		Parcourir([&](int i, double t, const Historique &h) {
			const Echantillon &s = h.Au(t, latence);
			const double e = std::fabs(s.temps - 1.2);
			if (e < ecart) {
				ecart = e;
				meilleur = i;
				choisi = s;
				quand = t + kPeriode;
			}
		});
		const double vraie = LacetDeg(Pose(quand));
		for (int borne = 0; borne < 2; ++borne) {
			const double predit = LacetDeg(Predire(choisi, quand, borne == 1));
			std::printf("%s : sample at %.4f s, horizon %.1f ms, predicted %.1f deg, true %.1f deg\n",
						borne == 1 ? "with bound" : "no bound", choisi.temps,
						(quand - choisi.temps) * 1000.0, predit, vraie);
		}
		std::fprintf(stderr, "image %d, instant %.4f s, vitesse %.1f deg/s\n", meilleur,
					 (double)meilleur * kPeriode,
					 (double)choisi.vitesse.Len() * 180.0 / kPi);
	}

} // namespace

int nkmain(const NkEntryState &state) {
	(void)state;

	FILE *csv = std::fopen("courbe.csv", "w");
	if (csv != nullptr) {
		std::fprintf(csv, "latence_ms,avec,sans\n");
	}
	const int latences[5] = { 0, 10, 20, 40, 80 };
	for (int i = 0; i < 5; ++i) {
		Latence(latences[i], csv);
	}
	Borne();
	if (csv != nullptr) {
		std::fclose(csv);
	}
	return 0;
}