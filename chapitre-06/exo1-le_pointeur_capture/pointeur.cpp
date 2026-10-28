// pointeur.cpp : ShowMouse, CaptureMouse, ClipMouseToClient
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include <cstdio>

using namespace nkentseu;

static void ConfigureAppData(NkAppData &d) { d.appName = "Pointeur"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
	(void)state;

	NkWindowConfig cfg;
	cfg.title = "Pointeur";
	cfg.width = 1280;
	cfg.height = 720;
	cfg.centered = true;

	NkWindow fenetre(cfg);
	if (!fenetre.IsValid()) {
		std::printf("[pointeur] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}

	fenetre.ShowMouse(false);
	fenetre.CaptureMouse(true);
	fenetre.ClipMouseToClient(true);

	bool tourne = true;
	while (tourne && fenetre.IsOpen()) {
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev)) {
			if (ev->As<NkWindowCloseEvent>() != nullptr) {
				std::printf("[pointeur] fermeture demandee\n");
				tourne = false;
			}
		}
	}

	fenetre.ClipMouseToClient(false);
	fenetre.CaptureMouse(false);
	fenetre.ShowMouse(true);
	return 0;
}