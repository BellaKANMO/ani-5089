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


using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData& d)
{
    d.appName = "Exo21Cube";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState& state)
{
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exo21 - Un cube et rien d'autre";
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

    NkIDevice* device =
        NkDeviceFactory::CreateAutoDetect(devInfo);

    if (!device || !device->IsValid())
        return 2;

    NkRendererConfig cfg =
        NkRendererConfig::ForGame(
            devInfo.api,
            devInfo.width,
            devInfo.height);

    cfg.subsystems = NK_SS_RENDER3D;

    NkRenderer* renderer =
        NkRenderer::Create(device, cfg);

    if (!renderer || !renderer->Initialize())
        return 3;

    NkRender3D* r3d =
        renderer->GetRender3D();

    NkMeshHandle meshCube =
        renderer->GetMeshSystem()->GetCube();

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

    while (running && window.IsOpen())
    {
        events.PollEvents();

        if (!renderer->BeginFrame())
            continue;

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect =
            (float32)devInfo.width /
            (float32)devInfo.height;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;

        NkCamera3D cam(camData);

        cam.SetPosition({0.f, 1.70f, 1.5f});
        cam.SetTarget({0.f, 1.40f, -2.f});

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc sun;

        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;

        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        NkDrawCall3D cube;

        cube.mesh = meshCube;

        cube.transform =
            NkMat4f::Translate({0.f, 1.70f, -0.5f});

        cube.tint = {1.f, 1.f, 1.f};

        cube.aabb =
        {
            {-0.5f, 1.20f, -1.0f},
            { 0.5f, 2.20f,  0.0f}
        };

        r3d->Submit(cube);

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();

    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);

    window.Close();

    return 0;
}