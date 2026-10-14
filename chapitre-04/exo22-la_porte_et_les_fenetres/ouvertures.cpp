#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

struct V3 { double x, y, z; };
static V3 operator+(V3 a, V3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
static V3 operator-(V3 a, V3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
static V3 operator*(V3 a, double s) { return {a.x * s, a.y * s, a.z * s}; }
static double dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static V3 cross(V3 a, V3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
static V3 norm(V3 a) { double l = std::sqrt(dot(a, a)); return a * (1.0 / l); }

const double INTERIEUR   = 4.00;   
const double HAUTEUR     = 2.50;  
const double EPAISSEUR   = 0.10;   
const double DEMI        = INTERIEUR / 2.0;

const double CENTRE_MUR     = DEMI + EPAISSEUR / 2.0;          
const double FACE_FOND_Z    = -CENTRE_MUR + EPAISSEUR / 2.0;   
const double FACE_GAUCHE_X  = -CENTRE_MUR + EPAISSEUR / 2.0;     
const double PROFONDEUR_PANNEAU = 0.02; 

const double PORTE_LARGEUR  = 0.90;
const double PORTE_HAUTEUR  = 2.00;
const double PORTE_X        = -0.70;     

const double FENETRE_LARGEUR       = 1.20;   
const double FENETRE_HAUTEUR       = 1.00;
const double FENETRE_CENTRE_HAUTEUR = 1.40;  
const double FENETRE_Z1     = -0.90;
const double FENETRE_Z2     = +0.90;

const double DECALAGE = 0.01;   

const int W = 1280, H = 800;
static std::vector<unsigned short> zbuf(W * H, 65535);
static std::vector<unsigned char> img(W * H * 3, 0);

struct Cam { V3 pos, right, up, fwd; double f; } cam;

static V3 toCam(V3 p) {
    V3 d = p - cam.pos;
    return {dot(d, cam.right), dot(d, cam.up), dot(d, cam.fwd)}; 
}

const double ZNEAR = 0.1, ZFAR = 50.0;
const double PI = 3.14159265358979323846; 

struct CV { V3 c; }; 

static void drawTri(V3 a, V3 b, V3 c, const unsigned char col[3]) {
    std::vector<V3> poly = {a, b, c};
    std::vector<V3> out;
    for (size_t i = 0; i < poly.size(); ++i) {
        V3 p = poly[i], q = poly[(i + 1) % poly.size()];
        bool pin = p.z >= ZNEAR, qin = q.z >= ZNEAR;
        if (pin) out.push_back(p);
        if (pin != qin) {
            double t = (ZNEAR - p.z) / (q.z - p.z);
            out.push_back(p + (q - p) * t);
        }
    }
    if (out.size() < 3) return;
    for (size_t k = 1; k + 1 < out.size(); ++k) {
        V3 t[3] = {out[0], out[k], out[k + 1]};
        float sx[3], sy[3], dz[3];
        for (int i = 0; i < 3; ++i) {
            float iz = 1.0f / (float)t[i].z;
            sx[i] = (float)(W * 0.5 + cam.f * t[i].x / t[i].z);
            sy[i] = (float)(H * 0.5 - cam.f * t[i].y / t[i].z);
            dz[i] = (1.0f / (float)ZNEAR - iz) / (1.0f / (float)ZNEAR - 1.0f / (float)ZFAR);
        }
        float area = (sx[1] - sx[0]) * (sy[2] - sy[0]) - (sx[2] - sx[0]) * (sy[1] - sy[0]);
        if (std::fabs(area) < 1e-12f) continue;
        int x0 = std::max(0, (int)std::floor(std::min({sx[0], sx[1], sx[2]})));
        int x1 = std::min(W - 1, (int)std::ceil(std::max({sx[0], sx[1], sx[2]})));
        int y0 = std::max(0, (int)std::floor(std::min({sy[0], sy[1], sy[2]})));
        int y1 = std::min(H - 1, (int)std::ceil(std::max({sy[0], sy[1], sy[2]})));
        for (int y = y0; y <= y1; ++y)
            for (int x = x0; x <= x1; ++x) {
                float px = x + 0.5f, py = y + 0.5f;
                float w0 = ((sx[1] - px) * (sy[2] - py) - (sx[2] - px) * (sy[1] - py)) / area;
                float w1 = ((sx[2] - px) * (sy[0] - py) - (sx[0] - px) * (sy[2] - py)) / area;
                float w2 = 1.0f - w0 - w1;
                if (w0 < 0 || w1 < 0 || w2 < 0) continue;
                float d = w0 * dz[0] + w1 * dz[1] + w2 * dz[2];
                int dq = (int)(std::min(1.0f, std::max(0.0f, d)) * 65535.0f);
                int idx = y * W + x;
                if (dq < zbuf[idx]) {   
                    zbuf[idx] = (unsigned short)dq;
                    for (int ch = 0; ch < 3; ++ch) img[idx * 3 + ch] = col[ch];
                }
            }
    }
}

static void drawBox(V3 c, V3 s, V3 colf) {
    V3 h = s * 0.5;
    struct Face { V3 n, u, v; };
    Face faces[6] = {
        {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},  {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},
        {{0, 1, 0}, {0, 0, 1}, {1, 0, 0}},  {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}},
        {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},  {{0, 0, -1}, {0, 1, 0}, {1, 0, 0}}};
    V3 light = norm({0.4, 1.0, 0.6});
    for (auto& f : faces) {
        V3 ctr = c + V3{f.n.x * h.x, f.n.y * h.y, f.n.z * h.z};
        V3 ue = V3{f.u.x * h.x, f.u.y * h.y, f.u.z * h.z};
        V3 ve = V3{f.v.x * h.x, f.v.y * h.y, f.v.z * h.z};
        double lum = 0.55 + 0.45 * std::max(0.0, dot(f.n, light));
        unsigned char col[3] = {(unsigned char)(255 * std::min(1.0, colf.x * lum)),
                                (unsigned char)(255 * std::min(1.0, colf.y * lum)),
                                (unsigned char)(255 * std::min(1.0, colf.z * lum))};
        V3 p0 = ctr - ue - ve, p1 = ctr + ue - ve, p2 = ctr + ue + ve, p3 = ctr - ue + ve;
        drawTri(toCam(p0), toCam(p1), toCam(p2), col);
        drawTri(toCam(p0), toCam(p2), toCam(p3), col);
    }
}

int main(int argc, char** argv) {
    double decalage = (argc > 1) ? std::atof(argv[1]) : DECALAGE;

    cam.pos = {1.3, 1.45, 1.85};
    V3 target = {-1.3, 1.25, -1.2};
    cam.fwd = norm(target - cam.pos);
    cam.right = norm(cross(cam.fwd, {0, 1, 0}));
    cam.up = cross(cam.right, cam.fwd);
    cam.f = (W * 0.5) / std::tan(0.5 * 95.0 * PI / 180.0);  

    for (int i = 0; i < W * H; ++i) { img[i * 3] = 30; img[i * 3 + 1] = 30; img[i * 3 + 2] = 40; }

    V3 murCol = {0.93, 0.89, 0.80};

    drawBox({0, -EPAISSEUR / 2, 0}, {4.2, EPAISSEUR, 4.2}, {0.45, 0.40, 0.35});
    drawBox({0, HAUTEUR + EPAISSEUR / 2, 0}, {4.2, EPAISSEUR, 4.2}, {0.97, 0.97, 0.97});

    drawBox({0, HAUTEUR / 2, -CENTRE_MUR}, {4.2, HAUTEUR, EPAISSEUR}, murCol);
    drawBox({0, HAUTEUR / 2, +CENTRE_MUR}, {4.2, HAUTEUR, EPAISSEUR}, murCol);
    drawBox({-CENTRE_MUR, HAUTEUR / 2, 0}, {EPAISSEUR, HAUTEUR, INTERIEUR}, murCol);
    drawBox({+CENTRE_MUR, HAUTEUR / 2, 0}, {EPAISSEUR, HAUTEUR, INTERIEUR}, murCol);

    double porteZ = FACE_FOND_Z + decalage - PROFONDEUR_PANNEAU / 2.0;
    drawBox({PORTE_X, PORTE_HAUTEUR / 2.0, porteZ},
            {PORTE_LARGEUR, PORTE_HAUTEUR, PROFONDEUR_PANNEAU}, {0.45, 0.25, 0.10});

    double fenX = FACE_GAUCHE_X + decalage - PROFONDEUR_PANNEAU / 2.0;
    double zs[2] = {FENETRE_Z1, FENETRE_Z2};
    for (double z : zs)
        drawBox({fenX, FENETRE_CENTRE_HAUTEUR, z},
                {PROFONDEUR_PANNEAU, FENETRE_HAUTEUR, FENETRE_LARGEUR}, {0.45, 0.70, 0.95});

    FILE* f = std::fopen("ouvertures.ppm", "wb");
    std::fprintf(f, "P6\n%d %d\n255\n", W, H);
    std::fwrite(img.data(), 1, img.size(), f);
    std::fclose(f);
    std::printf("decalage = %.4f m -> ouvertures.ppm\n", decalage);
    return 0;
}
