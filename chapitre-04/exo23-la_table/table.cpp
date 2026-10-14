
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

const double INTERIEUR = 4.00, HAUTEUR = 2.50, EPAISSEUR = 0.10;
const double CENTRE_MUR = INTERIEUR / 2.0 + EPAISSEUR / 2.0;   

const double TABLE_LONGUEUR = 1.20;   
const double TABLE_PROFONDEUR = 0.80;
const double TABLE_X = 0.60;         
const double TABLE_Z = 0.70;
const double FACE_SUP_Y = 0.75;      
const double PLATEAU_EPAISSEUR = 0.05;
const double PIED_SECTION = 0.06; 

const double PLATEAU_Y = FACE_SUP_Y - PLATEAU_EPAISSEUR / 2.0;      

const double FACE_INF_Y = FACE_SUP_Y - PLATEAU_EPAISSEUR;          
const double PIED_HAUTEUR = FACE_INF_Y;                             
const double PIED_Y = PIED_HAUTEUR / 2.0;                          

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

int main() {
    cam.pos = {-1.3, 1.35, 1.8};
    V3 target = {0.6, 0.45, 0.6};
    cam.fwd = norm(target - cam.pos);
    cam.right = norm(cross(cam.fwd, {0, 1, 0}));
    cam.up = cross(cam.right, cam.fwd);
    cam.f = (W * 0.5) / std::tan(0.5 * 80.0 * PI / 180.0);

    for (int i = 0; i < W * H; ++i) { img[i * 3] = 30; img[i * 3 + 1] = 30; img[i * 3 + 2] = 40; }

    V3 murCol = {0.93, 0.89, 0.80};
    drawBox({0, -EPAISSEUR / 2, 0}, {4.2, EPAISSEUR, 4.2}, {0.45, 0.40, 0.35});
    drawBox({0, HAUTEUR + EPAISSEUR / 2, 0}, {4.2, EPAISSEUR, 4.2}, {0.97, 0.97, 0.97});
    drawBox({0, HAUTEUR / 2, -CENTRE_MUR}, {4.2, HAUTEUR, EPAISSEUR}, murCol);
    drawBox({0, HAUTEUR / 2, +CENTRE_MUR}, {4.2, HAUTEUR, EPAISSEUR}, murCol);
    drawBox({-CENTRE_MUR, HAUTEUR / 2, 0}, {EPAISSEUR, HAUTEUR, INTERIEUR}, murCol);
    drawBox({+CENTRE_MUR, HAUTEUR / 2, 0}, {EPAISSEUR, HAUTEUR, INTERIEUR}, murCol);

    drawBox({TABLE_X, PLATEAU_Y, TABLE_Z},
            {TABLE_LONGUEUR, PLATEAU_EPAISSEUR, TABLE_PROFONDEUR}, {0.80, 0.55, 0.30});

    double dx = TABLE_LONGUEUR / 2.0 - PIED_SECTION / 2.0;  
    double dz = TABLE_PROFONDEUR / 2.0 - PIED_SECTION / 2.0;
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
            drawBox({TABLE_X + sx * dx, PIED_Y, TABLE_Z + sz * dz},
                    {PIED_SECTION, PIED_HAUTEUR, PIED_SECTION}, {0.55, 0.33, 0.15});

    FILE* f = std::fopen("table.ppm", "wb");
    std::fprintf(f, "P6\n%d %d\n255\n", W, H);
    std::fwrite(img.data(), 1, img.size(), f);
    std::fclose(f);
    std::printf("plateau y=%.3f, pieds h=%.2f y=%.2f\n", PLATEAU_Y, PIED_HAUTEUR, PIED_Y);
    return 0;
}
