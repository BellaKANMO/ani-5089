#include "NKPlatform/NkPlatformDetect.h"

#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"

#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"

#include "NKRHI/Core/NkDeviceFactory.h"

#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <chrono>
#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData& d)
{
    d.appName = "Exo26Interfaces";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const NkGraphicsApi API_CHOISIE = NkGraphicsApi::NK_GFX_API_SOFTWARE;

static const int IMAGES_CHAUFFE = 20;
static const int IMAGES_MESUREES = 400;

static void Poser(NkRender3D* r3d, NkMeshHandle mesh,
                  float cx, float cy, float cz,
                  float sx, float sy, float sz,
                  float r, float g, float b)
{
    NkDrawCall3D d;
    d.mesh = mesh;
    d.transform = NkMat4f::Translate({cx, cy, cz}) * NkMat4f::Scale({sx, sy, sz});
    d.tint = {r, g, b};
    d.aabb =
    {
        {cx - sx / 2.f, cy - sy / 2.f, cz - sz / 2.f},
        {cx + sx / 2.f, cy + sy / 2.f, cz + sz / 2.f}
    };
    r3d->Submit(d);
}

static void SoumettrePiece(NkRender3D* r3d, NkMeshHandle cube)
{
    Poser(r3d, cube, 0.f, -0.05f, 0.f, 4.2f, 0.1f, 4.2f, 0.45f, 0.40f, 0.35f);
    Poser(r3d, cube, 0.f, 1.25f, -2.05f, 4.2f, 2.5f, 0.1f, 0.93f, 0.89f, 0.80f);
    Poser(r3d, cube, 0.f, 1.25f, 2.05f, 4.2f, 2.5f, 0.1f, 0.93f, 0.89f, 0.80f);
    Poser(r3d, cube, -2.05f, 1.25f, 0.f, 0.1f, 2.5f, 4.f, 0.93f, 0.89f, 0.80f);
    Poser(r3d, cube, 2.05f, 1.25f, 0.f, 0.1f, 2.5f, 4.f, 0.93f, 0.89f, 0.80f);
    Poser(r3d, cube, -0.70f, 1.00f, -2.00f, 0.90f, 2.00f, 0.02f, 0.45f, 0.25f, 0.10f);
    Poser(r3d, cube, -2.00f, 1.40f, -0.90f, 0.02f, 1.00f, 1.20f, 0.45f, 0.70f, 0.95f);
    Poser(r3d, cube, -2.00f, 1.40f, 0.90f, 0.02f, 1.00f, 1.20f, 0.45f, 0.70f, 0.95f);
    Poser(r3d, cube, 0.60f, 0.725f, 0.70f, 1.20f, 0.05f, 0.80f, 0.80f, 0.55f, 0.30f);
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
            Poser(r3d, cube, 0.60f + sx * 0.57f, 0.35f, 0.70f + sz * 0.37f,
                  0.06f, 0.70f, 0.06f, 0.55f, 0.33f, 0.15f);
}

int nkmain(const NkEntryState& state)
{
    (void)state;

    std::printf("Interface : %s\n", NkGraphicsApiName(API_CHOISIE));

    NkWindowConfig winCfg;
    winCfg.title = "Exo26 - Deux interfaces graphiques";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);

    if (!window.IsValid())
        return 1;

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;
    devInfo.api = API_CHOISIE;

    NkIDevice* device = NkDeviceFactory::CreateForApi(API_CHOISIE, devInfo);

    if (!device || !device->IsValid())
    {
        std::printf("refused : le peripherique n'a pas pu etre cree pour %s\n",
                    NkGraphicsApiName(API_CHOISIE));
        return 2;
    }

    NkRendererConfig cfg =
        NkRendererConfig::ForGame(API_CHOISIE, devInfo.width, devInfo.height);

    cfg.api = API_CHOISIE;
    cfg.subsystems = NK_SS_RENDER3D;

    NkRenderer* renderer = NkRenderer::Create(device, cfg);

    if (!renderer || !renderer->Initialize())
    {
        std::printf("refused : le moteur de rendu ne s'initialise pas pour %s\n",
                    NkGraphicsApiName(API_CHOISIE));
        return 3;
    }

    NkRender3D* r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();

    bool running = true;

    NkEventSystem& events = NkEvents();

    events.AddEventCallback<NkWindowCloseEvent>(
        [&](NkWindowCloseEvent* e)
        {
            (void)e;
            running = false;
        });

    events.AddEventCallback<NkKeyPressEvent>(
        [&](NkKeyPressEvent* e)
        {
            if (e->GetKey() == NkKey::NK_ESCAPE)
                running = false;
        });

    int images = 0;
    bool mesure_faite = false;
    std::chrono::steady_clock::time_point debut;

    while (running && window.IsOpen())
    {
        events.PollEvents();

        if (!renderer->BeginFrame())
            continue;

        if (images == IMAGES_CHAUFFE)
            debut = std::chrono::steady_clock::now();

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)devInfo.width / (float32)devInfo.height;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;

        NkCamera3D cam(camData);
        cam.SetPosition({1.75f, 1.75f, 1.85f});
        cam.SetTarget({-0.5f, 0.85f, -0.5f});

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);
        SoumettrePiece(r3d, meshCube);

        renderer->Present();
        renderer->EndFrame();

        ++images;

        if (!mesure_faite && images == IMAGES_CHAUFFE + IMAGES_MESUREES)
        {
            auto fin = std::chrono::steady_clock::now();
            double ms = std::chrono::duration<double, std::milli>(fin - debut).count() / IMAGES_MESUREES;
            std::printf("%s : %.3f ms par image (moyenne sur %d images)\n",
                        NkGraphicsApiName(API_CHOISIE), ms, IMAGES_MESUREES);
            mesure_faite = true;
        }
    }

    device->WaitIdle();

    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);

    window.Close();

    return 0;
}
