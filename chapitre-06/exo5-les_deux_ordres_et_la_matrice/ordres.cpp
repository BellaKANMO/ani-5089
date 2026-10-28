#include "NKEvent/NkEventDispatcher.h"
#include "NKTime/NkChrono.h"
#include <cmath>

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include "NKMath/NkAngle.h"
#include "NKMath/NkQuat.h"
#include "NKMath/NkMat.h"

#include "NKEvent/NkMouseEvent.h"

#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

	// ── Les dimensions de la salle, en un seul endroit ────────────────────
	// Elles sont ici, nommées, et pas semées dans le code : au chapitre 21 on
	// les lira depuis un fichier, et ce sont ces noms-là qui deviendront des
	// champs.
	const float32 kPiece = 4.00f;	  // côté intérieur du sol
	const float32 kPlafond = 2.50f;	  // hauteur sous plafond
	const float32 kMur = 0.10f;		  // épaisseur d'un mur
	const float32 kPorteL = 0.90f;	  // porte : largeur
	const float32 kPorteH = 2.00f;	  //         hauteur
	const float32 kFenL = 1.20f;	  // fenêtre : largeur
	const float32 kFenH = 1.00f;	  //           hauteur
	const float32 kFenY = 1.40f;	  //           hauteur de son centre
	const float32 kTableL = 1.20f;	  // table : longueur
	const float32 kTableP = 0.80f;	  //         profondeur
	const float32 kTableH = 0.75f;	  //         hauteur du plateau
	const float32 kYeux = 1.70f;	  // hauteur des yeux, debout

	const float32 kDemi = kPiece * 0.5f;

	// Un pavé posé dans la salle. `centre` est le centre géométrique, `taille`
	// les trois dimensions en mètres : on ne manipule jamais autre chose.
	void Pave(NkRender3D *r3d, NkMeshSystem *meshes, const NkVec3f &centre,
			  const NkVec3f &taille, const NkVec3f &teinte, float32 rugosite = 1.f) {
		NkDrawCall3D dc;
		dc.mesh = meshes->GetCube();
		dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
		dc.aabb = { { centre.x - taille.x * 0.5f, centre.y - taille.y * 0.5f,
					  centre.z - taille.z * 0.5f },
					{ centre.x + taille.x * 0.5f, centre.y + taille.y * 0.5f,
					  centre.z + taille.z * 0.5f } };
		dc.tint = teinte;
		dc.metallic = 0.f;
		dc.roughness = rugosite;
		r3d->Submit(dc);
	}

	void SoumettreLaSalle(NkRender3D *r3d, NkMeshSystem *meshes) {
		const NkVec3f grisMur = { 0.72f, 0.70f, 0.66f };
		const NkVec3f grisSol = { 0.38f, 0.36f, 0.34f };
		const NkVec3f bois = { 0.55f, 0.38f, 0.22f };
		const NkVec3f vitre = { 0.75f, 0.85f, 0.92f };

		{ // le sol : le plan est unitaire, une échelle S donne un côté S
			NkDrawCall3D dc;
			dc.mesh = meshes->GetPlane();
			dc.transform = NkMat4f::Scale({ kPiece, 1.f, kPiece });
			dc.aabb = { { -kDemi, 0.f, -kDemi }, { kDemi, 0.f, kDemi } };
			dc.castShadow = false;
			dc.tint = grisSol;
			dc.metallic = 0.f;
			dc.roughness = 1.f;
			r3d->Submit(dc);
		}

		const float32 yMur = kPlafond * 0.5f;
		const float32 bord = kDemi + kMur * 0.5f;

		// Quatre murs. La porte est dans celui du fond, les fenêtres à gauche.
		Pave(r3d, meshes, { 0.f, yMur, -bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { 0.f, yMur, bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { -bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);
		Pave(r3d, meshes, { bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);

		// Le plafond. On l'oublie toujours, et le trou ne se voit qu'en levant
		// les yeux, c'est-à-dire le jour de la démonstration.
		Pave(r3d, meshes, { 0.f, kPlafond + kMur * 0.5f, 0.f },
			 { kPiece, kMur, kPiece }, grisMur);

		// La porte, fermée jusqu'au chapitre 17 : un panneau plaqué contre le
		// mur du fond, décalé pour ne pas se battre avec lui en profondeur.
		Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
			 { kPorteL, kPorteH, 0.04f }, bois, 0.6f);

		// Deux fenêtres sur le mur de gauche, pour qu'il y ait un dehors.
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);

		// La table : un plateau et quatre pieds. Le chapitre 12 y posera un
		// objet, le 21 la déplacera.
		const float32 tx = 0.60f, tz = 0.70f;
		const float32 ep = 0.05f;
		Pave(r3d, meshes, { tx, kTableH - ep * 0.5f, tz },
			 { kTableL, ep, kTableP }, bois, 0.5f);

		const float32 pied = 0.06f;
		const float32 hp = kTableH - ep;
		for (int i = 0; i < 4; ++i) {
			const float32 dx = ((i & 1) ? 1.f : -1.f) * (kTableL * 0.5f - pied);
			const float32 dz = ((i & 2) ? 1.f : -1.f) * (kTableP * 0.5f - pied);
			Pave(r3d, meshes, { tx + dx, hp * 0.5f, tz + dz },
				 { pied, hp, pied }, bois, 0.6f);
		}

		// Le repère d'un mètre du chapitre 9 : un cube d'un mètre de côté,
		// posé au sol, contre lequel on juge l'échelle de tout le reste.
		Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.f, 1.f, 1.f },
			 { 0.85f, 0.80f, 0.30f }, 0.8f);
	}

	uint64 EnvU64(const char *nom, uint64 defaut) {
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0') return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}

	struct Tete {
		float32 yaw = 0.f, pitch = 0.f;
		NkVec3f position = { 0.f, 0.f, 0.f };

		void PoserTete(float32 lacetRad, float32 tangageRad, const NkVec3f &p) {
			yaw = lacetRad;
			pitch = tangageRad;
			position = p;
		}
		NkVec3f Regard() const {
			return { -std::sin(yaw) * std::cos(pitch),
					std::sin(pitch),
					-std::cos(yaw) * std::cos(pitch) };
		}
	};

	bool Epreuve(std::FILE *f, float32 lacetDeg, float32 tangageDeg, const NkVec3f &attendu) {
		const float32 k = 3.14159265f / 180.f;
		Tete t;
		t.PoserTete(lacetDeg * k, tangageDeg * k, { 0.f, 0.f, 0.f });
		const NkVec3f g = t.Regard();
		const float32 e = 1e-4f;   // tolerance : cos(90 deg) n'est pas exactement 0
		const bool ok = std::fabs(g.x - attendu.x) < e && std::fabs(g.y - attendu.y) < e &&
						std::fabs(g.z - attendu.z) < e;
		char ligne[256];
		std::snprintf(ligne, sizeof ligne,
					"yaw %g, pitch %g : expected (%.3f, %.3f, %.3f), got (%.3f, %.3f, %.3f), %s\n",
					(double)lacetDeg, (double)tangageDeg,
					(double)attendu.x, (double)attendu.y, (double)attendu.z,
					(double)(g.x + 0.f), (double)(g.y + 0.f), (double)(g.z + 0.f),
					ok ? "OK" : "FAIL");
		std::printf("%s", ligne);
		if (f != nullptr) std::fputs(ligne, f);
		return ok;
	}

	bool TesterTete() {
		std::FILE *f = std::fopen("test.txt", "w");
		bool ok = true;
		ok &= Epreuve(f, 90.f, 0.f, { -1.f, 0.f, 0.f });    // gauche : rotation de -Z vers -X
		ok &= Epreuve(f, 0.f, 0.f, { 0.f, 0.f, -1.f });     // avant = -Z
		ok &= Epreuve(f, -90.f, 0.f, { 1.f, 0.f, 0.f });    // droite : echoue si le signe est inverse
		ok &= Epreuve(f, 0.f, 90.f, { 0.f, 1.f, 0.f });    // haut = +Y
		if (f != nullptr) std::fclose(f);
		return ok;
	}

	const float kYMain = 0.5f;

	void CalculerOrdres() {
		const float lacetDeg = 45.f, tangageDeg = 30.f;
		const NkQuatf qLacet = NkQuatf::RotateY(NkAngle(lacetDeg));
		const NkQuatf qTangage = NkQuatf::RotateX(NkAngle(tangageDeg));
		const NkVec3f avant(0.f, 0.f, -1.f);

		const NkVec3f g1 = (qLacet * qTangage) * avant;
		const NkVec3f g2 = (qTangage * qLacet) * avant;

		const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) *
								NkMat4f::RotationX(NkAngle(tangageDeg));
		const NkQuatf q = NkQuatf(rotation).Normalized();
		const NkVec3f g3 = q * avant;

		std::FILE *f = std::fopen("regards.txt", "w");
		char l[256];
		const char *noms[3] = { "yaw*pitch", "pitch*yaw", "matrices" };
		const NkVec3f *gs[3] = { &g1, &g2, &g3 };
		for (int i = 0; i < 3; ++i) {
			std::snprintf(l, sizeof l, "%s : (%.4f, %.4f, %.4f)\n", noms[i],
						(double)(gs[i]->x + 0.f), (double)(gs[i]->y + 0.f),
						(double)(gs[i]->z + 0.f));
			std::printf("%s", l);
			if (f != nullptr) std::fputs(l, f);
		}
		if (f != nullptr) std::fclose(f);

		for (int i = 0; i < 3; ++i) {
			std::printf("ecart y (%s) - main : %.4f\n", noms[i],
						(double)(gs[i]->y - kYMain));
		}
	}

} // namespace

int nkmain(const NkEntryState &state) {
	(void)state;

	NkWindowConfig cfg;
	cfg.title = "Ma salle";
	cfg.width = 1280;
	cfg.height = 720;
	cfg.centered = true;
	cfg.vsync = true;

	if (EnvU64("MASALLE_TEST", 0) == 1) {
		return TesterTete() ? 0 : 1;
	}

	if (EnvU64("MASALLE_ORDRES", 0) == 1) {
		CalculerOrdres();
		return 0;
	}

	NkWindow fenetre(cfg);
	if (!fenetre.IsValid()) {
		std::printf("[MaSalle] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}
	
	// fenetre.ShowMouse(false);
	// fenetre.CaptureMouse(true);
	// fenetre.ClipMouseToClient(true);

	NkDeviceInitInfo devInit{};
	devInit.surface = fenetre.GetSurfaceDesc();
	devInit.width = fenetre.GetSize().x;
	devInit.height = fenetre.GetSize().y;

	NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
	if (device == nullptr) {
		std::printf("[MaSalle] aucune API graphique disponible\n");
		return 1;
	}

	NkRendererConfig cfgEcran =
		NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
	NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
	if (rEcran == nullptr || !rEcran->Initialize()) {
		std::printf("[MaSalle] le renderer d'ecran n'a pas pu s'initialiser\n");
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkOffscreenDesc od;
	od.width = devInit.width;
	od.height = devInit.height;
	od.hasDepth = true;
	od.hdr = false;
	od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	od.readable = true;
	od.readback = true; // sans quoi Capture() ne peut rien relire
	od.name = "MaSalle_Cible";

	NkOffscreenTarget *cible = rEcran->CreateOffscreen(od);
	if (cible == nullptr) {
		std::printf("[MaSalle] cible hors ecran refusee\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkRendererConfig cfgSalle =
		NkRendererConfig::ForGame(devInit.api, devInit.width, devInit.height);
	cfgSalle.Disable(NK_SS_RENDER2D);
	cfgSalle.Disable(NK_SS_TEXT);
	cfgSalle.Disable(NK_SS_OVERLAY);
	cfgSalle.Enable(NK_SS_OFFSCREEN);

	// NK_SS_POST_PROCESS porte le tonemap, le bloom, le FXAA et le SSAO.
	// MASALLE_POST=0 le coupe : c'est l'expérience qui dit si un défaut
	// d'image vient de la scène ou d'un effet écran. Voir le journal de bord.
	if (EnvU64("MASALLE_POST", 1) == 0) {
		cfgSalle.Disable(NK_SS_POST_PROCESS);
	}

	NkRenderer *renderer = NkRenderer::Create(device, cfgSalle);
	if (renderer == nullptr || !renderer->Initialize()) {
		std::printf("[MaSalle] le renderer de la salle n'a pas pu s'initialiser\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}
	renderer->SetFinalColorTarget(
		rEcran->GetTextures()->GetRHIHandle(cible->GetColorResult()));

	std::printf("[MaSalle] salle de %.1f m sur %.1f m, plafond a %.2f m\n",
				(double)kPiece, (double)kPiece, (double)kPlafond);

	// Sortie automatique, pour que la construction se vérifie sans main
	// humaine : MASALLE_EXIT=<image> arrête proprement.
	const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
	const uint64 imageDeCapture = EnvU64("MASALLE_SHOT", 0);

	uint64 image = 0;
	bool tourne = true;

	const float32 kSens = 0.002f;  
	int32 prochaineSeconde = 1;

	float32 yaw = 0.f, pitch = 0.f;  
	float32 yawLu = 0.f, yawNonVide = 0.f;
	int32 accVideX = 0, accVideY = 0;
	int32 accNonVideX = 0;
	NkChrono horloge;
	std::FILE *sortie = std::fopen("trois.txt", "w");

	while (tourne && fenetre.IsOpen()) {
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev)) {
			if (ev->As<NkWindowCloseEvent>() != nullptr) {
				std::printf("[MaSalle] fermeture demandee\n");
				tourne = false;
			}
			if (auto *brut = ev->As<NkMouseRawEvent>()) {
				accVideX += brut->GetDeltaX();
				accVideY += brut->GetDeltaY();
				accNonVideX += brut->GetDeltaX();
			}
		}
		if (!tourne) break;

		
		yawLu += (float32)NkInput.MouseRawDeltaX() * kSens;

		const int32 sommeX = accVideX, sommeY = accVideY;
		accVideX = 0;
		accVideY = 0;
		yaw += (float32)sommeX * kSens;
		pitch += (float32)sommeY * kSens;

		yawNonVide += (float32)accNonVideX * kSens;

		const float64 t = horloge.Elapsed().ToMilliseconds() / 1000.0;
		if (t >= (float64)prochaineSeconde) {
			const double k = 180.0 / 3.14159265358979;
			std::printf("t = %.1f s, read = %.1f, emptied = %.1f, not emptied = %.1f\n",
						t, (double)yawLu * k, (double)yaw * k, (double)yawNonVide * k);
			if (sortie != nullptr) {
				std::fprintf(sortie, "t = %.1f s, read = %.1f, emptied = %.1f, not emptied = %.1f\n",
							t, (double)yawLu * k, (double)yaw * k, (double)yawNonVide * k);
				std::fflush(sortie);
			}
			++prochaineSeconde;
		}

		if (rEcran->BeginFrame()) {
			NkICommandBuffer *cmd = rEcran->GetCmd();

			NkRender3D *r3d = renderer->GetRender3D();

			// Ce que BeginFrame ferait si ce renderer possédait l'image. En
			// mode partagé, on le rejoue à la main. Oublier ResetFrame piétine
			// les tampons uniformes d'une image à l'autre.
			renderer->FlushGraphRebuilds();
			r3d->ResetFrame();
			if (auto *mc = renderer->GetMaterialCollection()) {
				mc->Upload();
			}

			NkCamera3DData cd;
			Tete tete;
			tete.PoserTete(yaw, pitch, { 0.30f, kYeux, 1.20f });
			cd.position = tete.position;
			const NkVec3f dir = tete.Regard();
			cd.target = { cd.position.x + dir.x, cd.position.y + dir.y, cd.position.z + dir.z };
			cd.up = { 0.f, 1.f, 0.f };
			cd.fovY = 70.f;
			cd.aspect = (float32)devInit.width / (float32)devInit.height;
			cd.nearPlane = 0.05f;
			cd.farPlane = 100.f;

			NkSceneContext sctx;
			sctx.camera = NkCamera3D(cd);
			sctx.time = (float32)image / 60.f;
			// La part de lumière qui vient du ciel (0 = aucune, 1 = toute).
			r3d->SetIBLStrength(0.35f);

			NkLightDesc soleil;
			soleil.type = NkLightType::NK_DIRECTIONAL;
			soleil.direction = { -0.5f, -1.f, -0.35f };
			soleil.color = { 1.f, 0.96f, 0.88f };
			soleil.intensity = 3.f;
			soleil.castShadow = false;
			sctx.lights.PushBack(soleil);

			r3d->BeginScene(sctx);
			SoumettreLaSalle(r3d, renderer->GetMeshSystem());

			// Le graphe de la salle s'exécute sur le command buffer de l'écran,
			// AVANT son Present : deux passes imbriquées sont interdites.
			if (auto *graph = renderer->GetRenderGraph()) {
				graph->Execute(cmd);
			}

			// La texture, montrée dans la fenêtre. C'est tout ce que fait le
			// renderer d'écran : recopier une image déjà faite.
			if (auto *r2d = rEcran->GetRender2D()) {
				r2d->DrawImage(cible->GetColorResult(),
							   { 0.f, 0.f, (float32)devInit.width,
								 (float32)devInit.height });
			}

			rEcran->Present();
			rEcran->EndFrame();
		}

		++image;

		// La capture, à une image nommée : déterministe, reproductible, et
		// c'est elle qui fait les figures du livre.
		if (imageDeCapture != 0 && image == imageDeCapture) {
			const char *chemin = getenv("MASALLE_SHOT_PATH");
			if (chemin == nullptr || *chemin == '\0') {
				chemin = "masalle.png";
			}
			const bool ok = cible->Capture(chemin);
			std::printf("[MaSalle] capture image %llu -> %s (%s)\n",
						(unsigned long long)image, chemin, ok ? "ok" : "echec");
		}

		if (imageDeSortie != 0 && image >= imageDeSortie) {
			tourne = false;
		}
	}

	if (sortie != nullptr) std::fclose(sortie);

	std::printf("[MaSalle] %llu images\n", (unsigned long long)image);

	// fenetre.ClipMouseToClient(false);
	// fenetre.CaptureMouse(false);
	// fenetre.ShowMouse(true);

	NkRenderer::Destroy(renderer);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return 0;
}
