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
#include "NKRenderer/Materials/NkMaterial.h"

#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData& d)
{
    d.appName = "Exo4Mur";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const float kW = 4.f;
static const float kD = 4.f;
static const float kH = 2.5f;
static const float kT = 0.1f;

static void Pave(NkRender3D* r3d, NkMeshHandle mesh, NkVec3f pos, NkVec3f size, NkVec3f tint)
{
    NkDrawCall3D dc;
    dc.mesh = mesh;
    dc.transform = NkMat4f::Translate(pos) * NkMat4f::Scale(size);
    dc.tint = tint;
    dc.aabb = { {pos.x - size.x * 0.5f, pos.y - size.y * 0.5f, pos.z - size.z * 0.5f},
                {pos.x + size.x * 0.5f, pos.y + size.y * 0.5f, pos.z + size.z * 0.5f} };
    r3d->Submit(dc);
}

template <typename MatPtr>
static void PaveMat(NkRender3D* r3d, NkMeshHandle mesh, NkVec3f pos, NkVec3f size, MatPtr mat)
{
    NkDrawCall3D dc;
    dc.mesh = mesh;
    dc.transform = NkMat4f::Translate(pos) * NkMat4f::Scale(size);
    dc.tint = {1.f, 1.f, 1.f};
    dc.material = mat->GetInstHandle();
    dc.aabb = { {pos.x - size.x * 0.5f, pos.y - size.y * 0.5f, pos.z - size.z * 0.5f},
                {pos.x + size.x * 0.5f, pos.y + size.y * 0.5f, pos.z + size.z * 0.5f} };
    r3d->Submit(dc);
}

int nkmain(const NkEntryState& state)
{
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Exo4 - Le premier mur texture";
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
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    auto tex = renderer->GetTextures()->Load("murs/photo.jpeg"); 
    auto mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    mat->SetAlbedoMap(tex);
    mat->SetTriplanarTileSize(1.0f); 

    bool running = true;
    NkEventSystem& events = NkEvents();

    events.AddEventCallback<NkWindowCloseEvent>(
        [&](NkWindowCloseEvent*) { running = false; });

    events.AddEventCallback<NkKeyPressEvent>(
        [&](NkKeyPressEvent* e)
        {
            if (e->GetKey() == NkKey::NK_ESCAPE) running = false;
        });

    while (running && window.IsOpen())
    {
        events.PollEvents();
        if (!renderer->BeginFrame()) continue;

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)devInfo.width / (float32)devInfo.height;
        camData.nearPlane = 0.05f;
        camData.farPlane = 100.f;

        NkCamera3D cam(camData);
        cam.SetPosition({0.f, 1.6f, 1.5f});
        cam.SetTarget({0.f, 1.3f, -2.f});

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sun.castShadow = false;
        sctx.lights.PushBack(sun);

        r3d->BeginScene(sctx);

        float ow = kW + 2.f * kT;
        float od = kD + 2.f * kT;

        {
            NkDrawCall3D dc;
            dc.mesh = meshPlane;
            dc.transform = NkMat4f::Translate({0.f, 0.f, 0.f}) * NkMat4f::Scale({kW, 1.f, kD});
            dc.tint = {0.6f, 0.6f, 0.6f};
            dc.aabb = { {-kW * 0.5f, -0.01f, -kD * 0.5f}, {kW * 0.5f, 0.01f, kD * 0.5f} };
            r3d->Submit(dc);
        }

        //PaveMat(r3d, meshCube, {0.f, kH * 0.5f, -(kD + kT) * 0.5f}, {ow, kH, kT}, mat);
        Pave(r3d, meshCube, {0.f, kH * 0.5f, -(kD + kT) * 0.5f}, {ow, kH, kT}, {0.9f, 0.9f, 0.85f});
        Pave(r3d, meshCube, {0.f, kH * 0.5f,  (kD + kT) * 0.5f}, {ow, kH, kT}, {0.9f, 0.9f, 0.85f});
        Pave(r3d, meshCube, {-(kW + kT) * 0.5f, kH * 0.5f, 0.f}, {kT, kH, od}, {0.85f, 0.9f, 0.9f});
        Pave(r3d, meshCube, { (kW + kT) * 0.5f, kH * 0.5f, 0.f}, {kT, kH, od}, {0.85f, 0.9f, 0.9f});
        Pave(r3d, meshCube, {0.f, kH + kT * 0.5f, 0.f}, {ow, kT, od}, {0.95f, 0.95f, 0.95f});

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}