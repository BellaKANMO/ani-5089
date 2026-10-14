#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

struct V3 { double x, y, z; };
static V3 operator+(V3 a, V3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
static V3 operator-(V3 a, V3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
static V3 operator*(V3 a, double s) { return {a.x * s, a.y * s, a.z * s}; }
static double dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static V3 cross(V3 a, V3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
static V3 norm(V3 a) { return a * (1.0 / std::sqrt(dot(a, a))); }

struct NkVec3f { float x, y, z; };

struct NkDirectionalLight {
    NkVec3f direction;
    NkVec3f color;
    float intensity;
    bool castShadow;
};

enum NkSubsystem : unsigned {
    NK_SS_RENDER3D = 1u << 0,
    NK_SS_SHADOW = 1u << 1
};

struct NkRendererConfig {
    unsigned subsystems;
};

const double PI = 3.14159265358979323846;
const int W = 1280, H = 800;
const double ZNEAR = 0.1, ZFAR = 50.0;

struct Objet {
    V3 c, s, albedo;
    bool projetteOmbre;
};

struct Camera { V3 pos, right, up, fwd; double f; } cam;

static std::vector<unsigned short> zbuf;
static std::vector<float> vue_z;
static std::vector<signed char> normale;
static std::vector<V3> albedo;

static const V3 NORMALES[6] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
static const V3 AXES_U[6] = {{0, 1, 0}, {0, 0, 1}, {0, 0, 1}, {1, 0, 0}, {1, 0, 0}, {0, 1, 0}};
static const V3 AXES_V[6] = {{0, 0, 1}, {0, 1, 0}, {1, 0, 0}, {0, 0, 1}, {0, 1, 0}, {1, 0, 0}};

static V3 versCamera(V3 p) {
    V3 d = p - cam.pos;
    return {dot(d, cam.right), dot(d, cam.up), dot(d, cam.fwd)};
}

static void tracerTriangle(V3 a, V3 b, V3 c, int face, V3 couleur) {
    std::vector<V3> poly = {a, b, c}, out;
    for (size_t i = 0; i < poly.size(); ++i) {
        V3 p = poly[i], q = poly[(i + 1) % poly.size()];
        bool pin = p.z >= ZNEAR, qin = q.z >= ZNEAR;
        if (pin) out.push_back(p);
        if (pin != qin) out.push_back(p + (q - p) * ((ZNEAR - p.z) / (q.z - p.z)));
    }
    if (out.size() < 3) return;
    for (size_t k = 1; k + 1 < out.size(); ++k) {
        V3 t[3] = {out[0], out[k], out[k + 1]};
        float sx[3], sy[3], dz[3], iz[3];
        for (int i = 0; i < 3; ++i) {
            iz[i] = 1.0f / (float)t[i].z;
            sx[i] = (float)(W * 0.5 + cam.f * t[i].x / t[i].z);
            sy[i] = (float)(H * 0.5 - cam.f * t[i].y / t[i].z);
            dz[i] = (1.0f / (float)ZNEAR - iz[i]) / (1.0f / (float)ZNEAR - 1.0f / (float)ZFAR);
        }
        float aire = (sx[1] - sx[0]) * (sy[2] - sy[0]) - (sx[2] - sx[0]) * (sy[1] - sy[0]);
        if (std::fabs(aire) < 1e-12f) continue;
        int x0 = std::max(0, (int)std::floor(std::min({sx[0], sx[1], sx[2]})));
        int x1 = std::min(W - 1, (int)std::ceil(std::max({sx[0], sx[1], sx[2]})));
        int y0 = std::max(0, (int)std::floor(std::min({sy[0], sy[1], sy[2]})));
        int y1 = std::min(H - 1, (int)std::ceil(std::max({sy[0], sy[1], sy[2]})));
        for (int y = y0; y <= y1; ++y)
            for (int x = x0; x <= x1; ++x) {
                float px = x + 0.5f, py = y + 0.5f;
                float w0 = ((sx[1] - px) * (sy[2] - py) - (sx[2] - px) * (sy[1] - py)) / aire;
                float w1 = ((sx[2] - px) * (sy[0] - py) - (sx[0] - px) * (sy[2] - py)) / aire;
                float w2 = 1.0f - w0 - w1;
                if (w0 < 0 || w1 < 0 || w2 < 0) continue;
                float d = w0 * dz[0] + w1 * dz[1] + w2 * dz[2];
                int dq = (int)(std::min(1.0f, std::max(0.0f, d)) * 65535.0f);
                int idx = y * W + x;
                if (dq < zbuf[idx]) {
                    zbuf[idx] = (unsigned short)dq;
                    vue_z[idx] = 1.0f / (w0 * iz[0] + w1 * iz[1] + w2 * iz[2]);
                    normale[idx] = (signed char)face;
                    albedo[idx] = couleur;
                }
            }
    }
}

static void tracerObjet(const Objet& o) {
    V3 h = o.s * 0.5;
    for (int f = 0; f < 6; ++f) {
        V3 n = NORMALES[f];
        V3 ctr = o.c + V3{n.x * h.x, n.y * h.y, n.z * h.z};
        V3 u = V3{AXES_U[f].x * h.x, AXES_U[f].y * h.y, AXES_U[f].z * h.z};
        V3 v = V3{AXES_V[f].x * h.x, AXES_V[f].y * h.y, AXES_V[f].z * h.z};
        V3 p0 = versCamera(ctr - u - v), p1 = versCamera(ctr + u - v);
        V3 p2 = versCamera(ctr + u + v), p3 = versCamera(ctr - u + v);
        tracerTriangle(p0, p1, p2, f, o.albedo);
        tracerTriangle(p0, p2, p3, f, o.albedo);
    }
}

static bool rayonToucheBoite(V3 origine, V3 dir, const Objet& o) {
    double tmin = 0.0, tmax = 1e9;
    double oo[3] = {origine.x, origine.y, origine.z}, dd[3] = {dir.x, dir.y, dir.z};
    double lo[3] = {o.c.x - o.s.x / 2, o.c.y - o.s.y / 2, o.c.z - o.s.z / 2};
    double hi[3] = {o.c.x + o.s.x / 2, o.c.y + o.s.y / 2, o.c.z + o.s.z / 2};
    for (int i = 0; i < 3; ++i) {
        if (std::fabs(dd[i]) < 1e-12) {
            if (oo[i] < lo[i] || oo[i] > hi[i]) return false;
        } else {
            double t1 = (lo[i] - oo[i]) / dd[i], t2 = (hi[i] - oo[i]) / dd[i];
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }
    return true;
}

static std::vector<Objet> construirePiece() {
    const double EP = 0.10, HAUT = 2.50, CM = 2.05;
    const double PAN = 0.02, DEC = 0.01;
    const double TX = 0.60, TZ = 0.70, FACE_SUP = 0.75, PLAT = 0.05, SECT = 0.06;
    V3 mur = {0.93, 0.89, 0.80};
    std::vector<Objet> s;
    s.push_back({{0, -EP / 2, 0}, {4.2, EP, 4.2}, {0.45, 0.40, 0.35}, false});
    s.push_back({{0, HAUT + EP / 2, 0}, {4.2, EP, 4.2}, {0.97, 0.97, 0.97}, false});
    s.push_back({{0, HAUT / 2, -CM}, {4.2, HAUT, EP}, mur, false});
    s.push_back({{0, HAUT / 2, CM}, {4.2, HAUT, EP}, mur, false});
    s.push_back({{-CM, HAUT / 2, 0}, {EP, HAUT, 4.0}, mur, false});
    s.push_back({{CM, HAUT / 2, 0}, {EP, HAUT, 4.0}, mur, false});
    s.push_back({{-0.70, 1.00, -2.00 + DEC - PAN / 2}, {0.90, 2.00, PAN}, {0.45, 0.25, 0.10}, true});
    for (double z : {-0.90, 0.90})
        s.push_back({{-2.00 + DEC - PAN / 2, 1.40, z}, {PAN, 1.00, 1.20}, {0.45, 0.70, 0.95}, false});
    s.push_back({{TX, FACE_SUP - PLAT / 2, TZ}, {1.20, PLAT, 0.80}, {0.80, 0.55, 0.30}, true});
    double dx = 0.60 - SECT / 2, dz = 0.40 - SECT / 2, hp = FACE_SUP - PLAT;
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
            s.push_back({{TX + sx * dx, hp / 2, TZ + sz * dz}, {SECT, hp, SECT}, {0.55, 0.33, 0.15}, true});
    s.push_back({{0.45, FACE_SUP + 0.04 / 2, 0.60}, {0.30, 0.04, 0.22}, {0.20, 0.35, 0.75}, true});
    s.push_back({{0.90, FACE_SUP + 0.12 / 2, 0.85}, {0.10, 0.12, 0.10}, {0.80, 0.15, 0.15}, true});
    return s;
}

static void eclairer(const std::vector<Objet>& scene, const NkDirectionalLight& soleil,
                     const NkRendererConfig& config, const char* nom) {
    V3 versSoleil = norm(V3{-soleil.direction.x, -soleil.direction.y, -soleil.direction.z});
    V3 couleur = {soleil.color.x, soleil.color.y, soleil.color.z};
    bool ombres = (config.subsystems & NK_SS_RENDER3D) && (config.subsystems & NK_SS_SHADOW) && soleil.castShadow;
    const double AMBIANT = 0.22, GAIN = 0.22;
    std::vector<unsigned char> sortie(W * H * 3, 0);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            int idx = y * W + x;
            unsigned char* px = &sortie[idx * 3];
            if (normale[idx] < 0) { px[0] = 30; px[1] = 30; px[2] = 40; continue; }
            V3 n = NORMALES[(int)normale[idx]];
            double z = vue_z[idx];
            V3 pos = cam.pos + cam.right * ((x + 0.5 - W * 0.5) / cam.f * z) +
                     cam.up * (-(y + 0.5 - H * 0.5) / cam.f * z) + cam.fwd * z;
            double ndotl = std::max(0.0, dot(n, versSoleil));
            double visible = 1.0;
            if (ombres && ndotl > 0.0) {
                V3 origine = pos + n * 0.002;
                for (const Objet& o : scene)
                    if (o.projetteOmbre && rayonToucheBoite(origine, versSoleil, o)) { visible = 0.0; break; }
            }
            double directe = soleil.intensity * GAIN * ndotl * visible;
            double canaux[3] = {albedo[idx].x * (AMBIANT + couleur.x * directe),
                                albedo[idx].y * (AMBIANT + couleur.y * directe),
                                albedo[idx].z * (AMBIANT + couleur.z * directe)};
            for (int c = 0; c < 3; ++c) px[c] = (unsigned char)(255.0 * std::min(1.0, canaux[c]));
        }
    FILE* f = std::fopen(nom, "wb");
    std::fprintf(f, "P6\n%d %d\n255\n", W, H);
    std::fwrite(sortie.data(), 1, sortie.size(), f);
    std::fclose(f);
}

static void rendre(const std::vector<Objet>& scene, const NkDirectionalLight& soleil,
                   const NkRendererConfig& config, const char* nom) {
    cam.pos = {1.75, 1.75, 1.85};
    V3 cible = {-0.5, 0.85, -0.5};
    cam.fwd = norm(cible - cam.pos);
    cam.right = norm(cross(cam.fwd, {0, 1, 0}));
    cam.up = cross(cam.right, cam.fwd);
    cam.f = (W * 0.5) / std::tan(0.5 * 92.0 * PI / 180.0);
    zbuf.assign(W * H, 65535);
    vue_z.assign(W * H, 0.0f);
    normale.assign(W * H, -1);
    albedo.assign(W * H, V3{0, 0, 0});
    for (const Objet& o : scene) tracerObjet(o);
    eclairer(scene, soleil, config, nom);
}

NkDirectionalLight soleilDepart() {
    NkDirectionalLight soleil;
    soleil.direction = {-0.4f, -1.0f, -0.3f};
    soleil.color = {1.0f, 0.95f, 0.85f};
    soleil.intensity = 3.0f;
    soleil.castShadow = true;
    return soleil;
}

const NkVec3f DIRECTION_ESSAYEE = {-1.0f, -0.35f, -0.6f};
const float INTENSITE_ESSAYEE = 0.8f;
const bool OMBRES_ESSAYEES = false;

int main() {
    NkRendererConfig config;
    config.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
    std::vector<Objet> scene = construirePiece();

    rendre(scene, soleilDepart(), config, "depart.ppm");

    NkDirectionalLight direction = soleilDepart();
    direction.direction = DIRECTION_ESSAYEE;
    rendre(scene, direction, config, "direction.ppm");

    NkDirectionalLight intensite = soleilDepart();
    intensite.intensity = INTENSITE_ESSAYEE;
    rendre(scene, intensite, config, "intensite.ppm");

    NkDirectionalLight ombres = soleilDepart();
    ombres.castShadow = OMBRES_ESSAYEES;
    rendre(scene, ombres, config, "ombres.ppm");
    return 0;
}
