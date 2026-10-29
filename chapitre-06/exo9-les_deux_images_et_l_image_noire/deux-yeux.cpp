#include <cmath>
 
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKImage/Core/NkImage.h"
#include "NKMath/NkColor.h"
 
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
 
#include <cstdio>
#include <cstdlib>
 
using namespace nkentseu;
using namespace nkentseu::renderer;
 
static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)
 
namespace {
 
	const float32 kPiece = 4.00f;	  
	const float32 kPlafond = 2.50f;	  
	const float32 kMur = 0.10f;		
	const float32 kPorteL = 0.90f;
	const float32 kPorteH = 2.00f;	
	const float32 kFenL = 1.20f;	
	const float32 kFenH = 1.00f;	
	const float32 kFenY = 1.40f;	
	const float32 kTableL = 1.20f;	
	const float32 kTableP = 0.80f;	
	const float32 kTableH = 0.75f;
	const float32 kYeux = 1.70f;
 
	const float32 kDemi = kPiece * 0.5f;
 
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
 
		{ 
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
 
		Pave(r3d, meshes, { 0.f, yMur, -bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { 0.f, yMur, bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { -bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);
		Pave(r3d, meshes, { bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);

		Pave(r3d, meshes, { 0.f, kPlafond + kMur * 0.5f, 0.f },
			 { kPiece, kMur, kPiece }, grisMur);
 
		Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
			 { kPorteL, kPorteH, 0.04f }, bois, 0.6f);
 
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
 
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
 
		Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.f, 1.f, 1.f },
			 { 0.85f, 0.80f, 0.30f }, 0.8f);
	}
 
	uint64 EnvU64(const char *nom, uint64 defaut) {
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0') return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}
 
	const float kIpd = 0.063f;		
	const uint32 kLarg = 640, kHaut = 720;
 
	void DessinerOeil(NkRenderer *renderer, NkICommandBuffer *cmd, const NkVec3f &pos,
					  uint64 image) {
		NkRender3D *r3d = renderer->GetRender3D();
 
		renderer->FlushGraphRebuilds();
		r3d->ResetFrame();
		if (auto *mc = renderer->GetMaterialCollection()) {
			mc->Upload();
		}
 
		NkCamera3DData cd;
		cd.position = pos;
		cd.target = { pos.x, pos.y, pos.z - 1.f };
		cd.up = { 0.f, 1.f, 0.f };
		cd.fovY = 70.f;
		cd.aspect = (float32)kLarg / (float32)kHaut;
		cd.nearPlane = 0.05f;
		cd.farPlane = 100.f;
 
		NkSceneContext sctx;
		sctx.camera = NkCamera3D(cd);
		sctx.time = (float32)image / 60.f;
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
 
		if (auto *graph = renderer->GetRenderGraph()) {
			graph->Execute(cmd);
		}
	}
 
	void Comparer(const char *fichierG, const char *fichierD, const char *sortiePng,
				  const char *etiquette) {
		NkImage a, b;
		if (!a.Load(fichierG, 4) || !b.Load(fichierD, 4)) {
			std::printf("[MaSalle] impossible de recharger %s et %s\n", fichierG, fichierD);
			return;
		}
		NkImage diff = NkImage::Create(kLarg, kHaut, 4);
		long long n = 0;
		int ecartMax = 0;
		for (int y = 0; y < (int)kHaut; ++y) {
			for (int x = 0; x < (int)kLarg; ++x) {
				const NkColor p = a.GetPixel(x, y);
				const NkColor q = b.GetPixel(x, y);
				int dr = (int)p.r - (int)q.r; if (dr < 0) dr = -dr;
				int dg = (int)p.g - (int)q.g; if (dg < 0) dg = -dg;
				int db = (int)p.b - (int)q.b; if (db < 0) db = -db;
				int e = dr; if (dg > e) e = dg; if (db > e) e = db;
				if (e > 1) ++n;
				if (e > ecartMax) ecartMax = e;
				int v = 4 * e; if (v > 255) v = 255;
				diff.SetPixel(x, y, NkColor((uint8)v, (uint8)v, (uint8)v, (uint8)255));
			}
		}
		const bool ok = diff.SavePNG(sortiePng);
		char l[160];
		std::snprintf(l, sizeof l, "%s : %lld pixels out of %d, max gap %d\n", etiquette, n,
					  (int)(kLarg * kHaut), ecartMax);
		std::printf("%s(%s : %s)\n", l, sortiePng, ok ? "ok" : "echec");
		std::FILE *f = std::fopen("difference.txt", "a");
		if (f != nullptr) {
			std::fputs(l, f);
			std::fclose(f);
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
 
	NkWindow fenetre(cfg);
	if (!fenetre.IsValid()) {
		std::printf("[MaSalle] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}
 
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
 
	const bool oubliee = EnvU64("MASALLE_OUBLIEE", 0) == 1;
 
	NkOffscreenDesc od;
	od.width = kLarg;
	od.height = kHaut;
	od.hasDepth = true;
	od.hdr = false;
	od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	od.readable = true;
	od.readback = true;
 
	NkRenderer *rOeil[2] = { nullptr, nullptr };
	NkOffscreenTarget *cible[2] = { nullptr, nullptr };
	const char *nomCible[2] = { "MaSalle_OeilGauche", "MaSalle_OeilDroit" };
 
	for (int i = 0; i < 2; ++i) {
		od.name = nomCible[i];
		cible[i] = rEcran->CreateOffscreen(od);
		if (cible[i] == nullptr) {
			std::printf("[MaSalle] cible hors ecran refusee\n");
			return 1;
		}
 
		NkRendererConfig cfgSalle = NkRendererConfig::ForGame(devInit.api, kLarg, kHaut);
		cfgSalle.Disable(NK_SS_RENDER2D);
		cfgSalle.Disable(NK_SS_TEXT);
		cfgSalle.Disable(NK_SS_OVERLAY);
		cfgSalle.Enable(NK_SS_OFFSCREEN);
 
		rOeil[i] = NkRenderer::Create(device, cfgSalle);
		if (rOeil[i] == nullptr || !rOeil[i]->Initialize()) {
			std::printf("[MaSalle] le renderer de l'oeil %d n'a pas pu s'initialiser\n", i);
			return 1;
		}
		rOeil[i]->SetFinalColorTarget(
			rEcran->GetTextures()->GetRHIHandle(cible[i]->GetColorResult()));
		rOeil[i]->SetRenderSizeOverride(kLarg, kHaut);
	}
 
	std::printf("[MaSalle] salle de %.1f m sur %.1f m, plafond a %.2f m\n",
				(double)kPiece, (double)kPiece, (double)kPlafond);
 
	const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
 
	uint64 image = 0;
	bool tourne = true;
	bool captures = false;
 
	while (tourne && fenetre.IsOpen()) {
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev)) {
			if (ev->As<NkWindowCloseEvent>() != nullptr) {
				std::printf("[MaSalle] fermeture demandee\n");
				tourne = false;
			}
		}
		if (!tourne) break;
 
		if (rEcran->BeginFrame()) {
			NkICommandBuffer *cmd = rEcran->GetCmd();
 
			const NkVec3f tete(0.30f, 1.70f, 1.20f);
			const float demi = kIpd * 0.5f;
			const NkVec3f gauche = oubliee ? tete : NkVec3f(tete.x - demi, tete.y, tete.z);
			const NkVec3f droit = oubliee ? tete : NkVec3f(tete.x + demi, tete.y, tete.z);
 
			DessinerOeil(rOeil[0], cmd, gauche, image);
			DessinerOeil(rOeil[1], cmd, droit, image);
 
			if (auto *r2d = rEcran->GetRender2D()) {
				r2d->DrawImage(cible[0]->GetColorResult(),
							   { 0.f, 0.f, (float32)kLarg, (float32)kHaut });
				r2d->DrawImage(cible[1]->GetColorResult(),
							   { (float32)kLarg, 0.f, (float32)kLarg, (float32)kHaut });
			}
 
			rEcran->Present();
			rEcran->EndFrame();
		}
 
		++image;
 
		if (image == 30) {
			const char *nomG = oubliee ? "oeil-gauche-oubliee.png" : "oeil-gauche.png";
			const char *nomD = oubliee ? "oeil-droit-oubliee.png" : "oeil-droit.png";
			const bool okG = cible[0]->Capture(nomG);
			const bool okD = cible[1]->Capture(nomD);
			std::printf("[MaSalle] capture image 30 : %s (%s), %s (%s)\n", nomG,
						okG ? "ok" : "echec", nomD, okD ? "ok" : "echec");
			captures = okG && okD;
		}
 
		if (imageDeSortie != 0 && image >= imageDeSortie) {
			tourne = false;
		}
	}
 
	std::printf("[MaSalle] %llu images\n", (unsigned long long)image);
 
	if (captures) {
		if (oubliee) {
			Comparer("oeil-gauche-oubliee.png", "oeil-droit-oubliee.png",
					 "difference-vue-oubliee.png", "view forgotten");
		} else {
			Comparer("oeil-gauche.png", "oeil-droit.png", "difference.png", "two eyes");
		}
	}
 
	for (int i = 0; i < 2; ++i) NkRenderer::Destroy(rOeil[i]);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return 0;
}
