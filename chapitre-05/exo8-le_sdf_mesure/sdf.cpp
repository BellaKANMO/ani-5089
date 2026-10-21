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

#include "NKFont/NkFont.h"
#include "NKFont/Embedded/NkFontEmbedded.h"
#include "NKImage/Core/NkImage.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData& d)
{
    d.appName = "Exo8Sdf";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const float kW = 4.f;
static const float kD = 4.f;
static const float kH = 2.5f;
static const float kT = 0.1f;
static const float ECHELLE = 8.f;  

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

static float smoothstep(float a, float b, float x)
{
    float t = (x - a) / (b - a);
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;
    return t * t * (3.f - 2.f * t);
}

static float LireBilineaire(const unsigned char* p, int tw, int th, float u, float v)
{
    float x = u * tw - 0.5f, y = v * th - 0.5f;
    int x0 = (int)std::floor(x), y0 = (int)std::floor(y);
    float fx = x - x0, fy = y - y0;
    auto px = [&](int ix, int iy) -> float {
        if (ix < 0) ix = 0; if (ix >= tw) ix = tw - 1;
        if (iy < 0) iy = 0; if (iy >= th) iy = th - 1;
        return (float)p[iy * tw + ix];
    };
    float a = px(x0, y0),     b = px(x0 + 1, y0);
    float c = px(x0, y0 + 1), d = px(x0 + 1, y0 + 1);
    float haut = a + (b - a) * fx;
    float bas  = c + (d - c) * fx;
    return haut + (bas - haut) * fy;
}

static void Melanger(NkImage& img, int x, int y, float c, int tr, int tg, int tb)
{
    if (x < 0 || y < 0 || x >= (int)img.Width() || y >= (int)img.Height()) return;
    auto p = img.GetPixel(x, y);
    int r = (int)(p.r + (tr - p.r) * c + 0.5f);
    int g = (int)(p.g + (tg - p.g) * c + 0.5f);
    int b = (int)(p.b + (tb - p.b) * c + 0.5f);
    img.SetPixel(x, y, math::NkColor((uint8)r, (uint8)g, (uint8)b, 255));
}

static bool Construire(bool sdf, float s, const char* fichier,
                       int& imgW, int& imgH, int& atlasW, int& atlasH)
{
    NkFontAtlas atlas;
    if (sdf) { atlas.sdfMode = true; atlas.sdfSpread = 6; }
    NkFont* police = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, 32.f);
    if (!police) { std::printf("AddToAtlas a echoue\n"); return false; }
    if (!atlas.Build()) { std::printf("atlas.Build a echoue\n"); return false; }

    unsigned char* pixels = nullptr;
    int tw = 0, th = 0;
    atlas.GetTexDataAsAlpha8(&pixels, &tw, &th);
    
    if (!pixels || tw <= 0 || th <= 0) { std::printf("atlas vide\n"); return false; }
    atlasW = atlas.texWidth;
    atlasH = atlas.texHeight;

    const char* mot = "Sortie";
    float largeur = 0.f;
    for (const char* c = mot; *c; ++c) {
        const NkFontGlyph* g = police->FindGlyph((unsigned int)*c);
        if (!g) { std::printf("glyphe absent : %c\n", *c); return false; }
        largeur += g->advanceX;
    }
    imgW = (int)std::ceil(largeur * ECHELLE);
    imgH = (int)std::ceil((police->ascent - police->descent) * ECHELLE);

    NkImage img = NkImage::Create((uint32)imgW, (uint32)imgH, (int32)4, (uint32)0x0B6B3AFFu);
    if (!img.IsValid()) { std::printf("Create a echoue\n"); return false; }

    const float base = police->ascent;
    float curseur = 0.f;
    for (const char* c = mot; *c; ++c) {
        const NkFontGlyph* g = police->FindGlyph((unsigned int)*c);
        float rx0 = (curseur + g->x0) * ECHELLE, rx1 = (curseur + g->x1) * ECHELLE;
        float ry0 = (base + g->y0) * ECHELLE,    ry1 = (base + g->y1) * ECHELLE;

        for (int y = (int)std::floor(ry0); y < (int)std::ceil(ry1); ++y) {
            for (int x = (int)std::floor(rx0); x < (int)std::ceil(rx1); ++x) {
                float fx = ((x + 0.5f) - rx0) / (rx1 - rx0);
                float fy = ((y + 0.5f) - ry0) / (ry1 - ry0);
                if (fx < 0.f || fx > 1.f || fy < 0.f || fy > 1.f) continue;

                float u = g->u0 + fx * (g->u1 - g->u0);
                float v = g->v0 + fy * (g->v1 - g->v0);
                float valeur = LireBilineaire(pixels, tw, th, u, v);

                float alpha;
                if (sdf) alpha = smoothstep(0.5f - s, 0.5f + s, valeur / 255.f);  
                else     alpha = valeur / 255.f;                                

                Melanger(img, x, y, alpha, 255, 255, 255);
            }
        }
        curseur += g->advanceX;
    }
    img.FlipHorizontal();
    if (!img.SavePNG(fichier)) { std::printf("SavePNG a echoue\n"); return false; }
    return true;
}

static bool Taille128(int& w, int& h)
{
    NkFontAtlas atlas;
    if (!NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, 128.f)) return false;
    if (!atlas.Build()) return false;
    w = atlas.texWidth; h = atlas.texHeight;
    return true;
}

int nkmain(const NkEntryState& state)
{
    (void)state;

    float s = 0.02f;
    if (const char* e = std::getenv("SDF_S")) s = (float)std::atof(e);

    int ow = 0, oh = 0, oaw = 0, oah = 0;
    int sw = 0, sh = 0, saw = 0, sah = 0;
    int gw = 0, gh = 0;
    if (!Construire(false, s, "ordinaire.png", ow, oh, oaw, oah)) return 10;
    if (!Construire(true,  s, "sdf.png",       sw, sh, saw, sah)) return 11;
    if (!Taille128(gw, gh)) return 12;

    std::printf("ordinaire 32 : %d x %d\n", oaw, oah);
    std::printf("sdf 32 : %d x %d\n", saw, sah);
    std::printf("ordinaire 128 : %d x %d\n", gw, gh);
    std::printf("image ordinaire : %d x %d | image sdf : %d x %d | s = %.3f\n", ow, oh, sw, sh, s);

    {
        std::ofstream out("atlas.txt");
        out << "ordinaire 32 : " << oaw << " x " << oah << "\n";
        out << "sdf 32 : " << saw << " x " << sah << "\n";
        out << "ordinaire 128 : " << gw << " x " << gh << "\n";
    }

    const char* vs = std::getenv("SIGNE");
    const bool signeSdf = (vs && vs[0] == '1');
    const char* chemin = signeSdf ? "sdf.png" : "ordinaire.png";
    const int imgW = signeSdf ? sw : ow;
    const int imgH = signeSdf ? sh : oh;

    NkWindowConfig winCfg;
    winCfg.title = "Exo8 - Le SDF mesure";
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

    auto tex = renderer->GetTextures()->Load(chemin);
    auto mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    mat->SetAlbedoMap(tex);

    const float panH = 0.20f;
    const float panW = panH * (float)imgW / (float)imgH;

    const char* vc = std::getenv("PANNEAU_CAM");
    const bool pres = (vc && vc[0] == '1');
    const NkVec3f camPos = pres ? NkVec3f{-0.70f, 2.20f, -1.50f}
                                : NkVec3f{-0.70f, 1.70f,  1.00f};
    const NkVec3f cible  = {-0.70f, 2.15f, -1.99f};

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
        cam.SetPosition(camPos);
        cam.SetTarget(cible);

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

        float ovw = kW + 2.f * kT;
        float ovd = kD + 2.f * kT;

        {
            NkDrawCall3D dc;
            dc.mesh = meshPlane;
            dc.transform = NkMat4f::Translate({0.f, 0.f, 0.f}) * NkMat4f::Scale({kW, 1.f, kD});
            dc.tint = {0.6f, 0.6f, 0.6f};
            dc.aabb = { {-kW * 0.5f, -0.01f, -kD * 0.5f}, {kW * 0.5f, 0.01f, kD * 0.5f} };
            r3d->Submit(dc);
        }

        Pave(r3d, meshCube, {0.f, kH * 0.5f, -(kD + kT) * 0.5f}, {ovw, kH, kT}, {0.9f, 0.9f, 0.85f});
        Pave(r3d, meshCube, {0.f, kH * 0.5f,  (kD + kT) * 0.5f}, {ovw, kH, kT}, {0.9f, 0.9f, 0.85f});
        Pave(r3d, meshCube, {-(kW + kT) * 0.5f, kH * 0.5f, 0.f}, {kT, kH, ovd}, {0.85f, 0.9f, 0.9f});
        Pave(r3d, meshCube, { (kW + kT) * 0.5f, kH * 0.5f, 0.f}, {kT, kH, ovd}, {0.85f, 0.9f, 0.9f});
        Pave(r3d, meshCube, {0.f, kH + kT * 0.5f, 0.f}, {ovw, kT, ovd}, {0.95f, 0.95f, 0.95f});

        Pave(r3d, meshCube, {-0.70f, 1.0f, -1.99f}, {0.90f, 2.0f, 0.02f}, {0.45f, 0.28f, 0.15f});
        PaveMat(r3d, meshCube, {-0.70f, 2.15f, -1.99f + 0.003f}, {panW, panH, 0.01f}, mat);

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}