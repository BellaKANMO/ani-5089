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
#include <cmath>
#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData& d)
{
    d.appName = "Exo27Cubes";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const float kSpacing = 1.5f;

static NkVec3f CubePos(uint32 i, uint32 side)
{
    float half = (side - 1) * kSpacing * 0.5f;
    return { (i % side) * kSpacing - half, 0.f, -(float)(i / side) * kSpacing };
}

static NkAABB CubeBox(NkVec3f p)
{
    return { {p.x - 0.5f, p.y - 0.5f, p.z - 0.5f},
             {p.x + 0.5f, p.y + 0.5f, p.z + 0.5f} };
}

int nkmain(const NkEntryState& state)
{
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exo27 - Cent cubes et le premier mur du reel";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) return 1;

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice* device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) return 2;

    NkRendererConfig cfg =
        NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;

    NkRenderer* renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) return 3;

    NkRender3D* r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();

    bool running = true;
    bool grouped = false;
    uint32 count = 100;

    NkEventSystem& events = NkEvents();

    events.AddEventCallback<NkWindowCloseEvent>(
        [&](NkWindowCloseEvent*) { running = false; });

    events.AddEventCallback<NkKeyPressEvent>(
        [&](NkKeyPressEvent* e)
        {
            if (e->GetKey() == NkKey::NK_ESCAPE) running = false;
            if (e->GetKey() == NkKey::NK_G) grouped = !grouped;
            if (e->GetKey() == NkKey::NK_UP) count += 100;
            if (e->GetKey() == NkKey::NK_DOWN && count > 100) count -= 100;
        });

    uint32 frames = 0;
    auto t0 = std::chrono::steady_clock::now();

    while (running && window.IsOpen())
    {
        events.PollEvents();
        if (!renderer->BeginFrame()) continue;

        uint32 side = (uint32)std::ceil(std::sqrt((float)count));
        float depth = side * kSpacing;

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)devInfo.width / (float32)devInfo.height;
        camData.nearPlane = 0.05f;
        camData.farPlane = 5000.f;

        NkCamera3D cam(camData);
        cam.SetPosition({0.f, depth * 0.8f, depth * 0.6f});
        cam.SetTarget({0.f, 0.f, -depth * 0.5f});

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        if (!grouped)
        {
            for (uint32 i = 0; i < count; ++i)
            {
                NkVec3f p = CubePos(i, side);
                NkDrawCall3D dc;
                dc.mesh = meshCube;
                dc.transform = NkMat4f::Translate(p);
                dc.aabb = CubeBox(p);
                r3d->Submit(dc);
            }
        }
        else
        {
            NkDrawCallInstanced dc;
            dc.mesh = meshCube;
            dc.aabb = NkAABB::Empty();
            for (uint32 i = 0; i < count; ++i)
            {
                NkVec3f p = CubePos(i, side);
                dc.transforms.PushBack(NkMat4f::Translate(p));
                dc.tints.PushBack({1.f, 1.f, 1.f});
                dc.aabb.Merge(CubeBox(p));
            }
            r3d->SubmitInstanced(dc);
        }

        renderer->Present();
        renderer->EndFrame();

        if (++frames >= 400)
        {
            auto t1 = std::chrono::steady_clock::now();
            float fps = frames / std::chrono::duration<float>(t1 - t0).count();
            std::printf("mode=%s cubes=%u drawCalls=%u fps=%.1f\n",
                        grouped ? "grouped" : "one-by-one", count,
                        renderer->GetStats().drawCalls, fps);
            frames = 0;
            t0 = t1;
        }
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}