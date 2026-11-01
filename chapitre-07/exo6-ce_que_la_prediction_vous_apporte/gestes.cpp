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

	double Lacet(double t, double amplitude, double duree) {
		if (t <= 0.0) return 0.0;
		if (t >= duree) return amplitude;
		return amplitude * 0.5 * (1.0 - cos(kPi * t / duree));
	}

	NkQuatf Pose(double t, double amplitude, double duree) {
		return NkQuatf::RotateY(NkAngle((float32)Lacet(t, amplitude, duree)));
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

	void Geste(const char *nom, double amplitude, double duree, double hz) {
		const double periode = 1.0 / hz;
		double sommeAvec = 0.0;
		double sommeSans = 0.0;
		int n = 0;
		NkQuatf precedente = Pose(0.0, amplitude, duree);

		for (int i = 1; (double)i * periode <= duree + 1e-9; ++i) {
			const double t = (double)i * periode;
			const NkQuatf courante = Pose(t, amplitude, duree);
			const NkVec3f v = VitesseAngulaire(precedente, courante, (float32)periode);
			const NkQuatf predite = Extrapoler(courante, v, (float32)periode);
			const NkQuatf vraie = Pose(t + periode, amplitude, duree);
			sommeAvec += AngleDeg(predite, vraie);
			sommeSans += AngleDeg(courante, vraie);
			++n;
			precedente = courante;
		}

		if (n == 0) return;
		const double avec = sommeAvec / n;
		const double sans = sommeSans / n;
		std::printf("%s : with %.4f deg, without %.4f deg, ratio %.2f\n",
					nom, avec, sans, avec > 0.0 ? sans / avec : 0.0);
		std::fprintf(stderr, "%s : %d poses\n", nom, n);
	}

} // namespace

int nkmain(const NkEntryState &state) {
	(void)state;

	const double hz = 72.0;
	Geste("fast (90 deg in 400 ms)", 90.0, 0.400, hz);
	Geste("slow (30 deg in 1200 ms)", 30.0, 1.200, hz);
	Geste("half-turn (180 deg in 200 ms)", 180.0, 0.200, hz);
	return 0;
}