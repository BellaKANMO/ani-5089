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
static V3 norm(V3 a) { double l = std::sqrt(dot(a, a)); return a * (1.0 / l); }

const double PI = 3.14159265358979323846;
const int W = 1280, H = 800;
const double ZNEAR = 0.1, ZFAR = 50.0;
static std::vector<unsigned short> zbuf(W * H, 65535);
static std::vector<unsigned char> img(W * H * 3, 0);

struct Cam { V3 pos, right, up, fwd; double f; } cam;

static V3 toCam(V3 p) {
    V3 d = p - cam.pos;
    return {dot(d, cam.right), dot(d, cam.up), dot(d, cam.fwd)};
}

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

struct Face { V3 n, u, v; };
static const Face FACES[6] = {
    {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},  {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},
    {{0, 1, 0}, {0, 0, 1}, {1, 0, 0}},  {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}},
    {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},  {{0, 0, -1}, {0, 1, 0}, {1, 0, 0}}};

static void drawQuad(V3 p0, V3 p1, V3 p2, V3 p3, V3 n, V3 colf) {
    V3 light = norm({0.4, 1.0, 0.6});
    double lum = 0.55 + 0.45 * std::max(0.0, dot(n, light));
    unsigned char col[3] = {(unsigned char)(255 * std::min(1.0, colf.x * lum)),
                            (unsigned char)(255 * std::min(1.0, colf.y * lum)),
                            (unsigned char)(255 * std::min(1.0, colf.z * lum))};
    drawTri(toCam(p0), toCam(p1), toCam(p2), col);
    drawTri(toCam(p0), toCam(p2), toCam(p3), col);
}

static void initCamera() {
    cam.pos = {1.75, 1.75, 1.85};
    V3 target = {-0.5, 0.85, -0.5};
    cam.fwd = norm(target - cam.pos);
    cam.right = norm(cross(cam.fwd, {0, 1, 0}));
    cam.up = cross(cam.right, cam.fwd);
    cam.f = (W * 0.5) / std::tan(0.5 * 92.0 * PI / 180.0);
    for (int i = 0; i < W * H; ++i) { img[i * 3] = 30; img[i * 3 + 1] = 30; img[i * 3 + 2] = 40; }
}

static void ecrireImage(const char* nom) {
    FILE* f = std::fopen(nom, "wb");
    std::fprintf(f, "P6\n%d %d\n255\n", W, H);
    std::fwrite(img.data(), 1, img.size(), f);
    std::fclose(f);
}


struct NkVec3f { float x, y, z; };

struct NkMat4f {
    float m[4][4];

    static NkMat4f Identite() {
        NkMat4f r = {};
        for (int i = 0; i < 4; ++i) r.m[i][i] = 1.0f;
        return r;
    }

    NkMat4f operator*(const NkMat4f& b) const {
        NkMat4f r = {};
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4; ++k) r.m[i][j] += m[i][k] * b.m[k][j];
        return r;
    }

    NkVec3f Appliquer(NkVec3f p) const {
        return {m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3],
                m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3],
                m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3]};
    }
};

NkMat4f Translation(NkVec3f t) {
    NkMat4f r = NkMat4f::Identite();
    r.m[0][3] = t.x;
    r.m[1][3] = t.y;
    r.m[2][3] = t.z;
    return r;
}

NkMat4f Echelle(NkVec3f s) {
    NkMat4f r = NkMat4f::Identite();
    r.m[0][0] = s.x;
    r.m[1][1] = s.y;
    r.m[2][2] = s.z;
    return r;
}

const float HAUTEUR_TABLE = 0.75f;

NkMat4f PoserAuSol(NkVec3f taille, float x, float z) {
    return Translation({x, taille.y / 2.0f, z}) * Echelle(taille);
}

NkMat4f PoserSurTable(NkVec3f taille, float x, float z) {
    return Translation({x, HAUTEUR_TABLE + taille.y / 2.0f, z}) * Echelle(taille);
}

const float INTERIEUR = 4.00f, HAUTEUR = 2.50f, EPAISSEUR = 0.10f;
const float CENTRE_MUR = INTERIEUR / 2.0f + EPAISSEUR / 2.0f;
const float FACE_FOND_Z = -2.00f, FACE_GAUCHE_X = -2.00f;
const float EPAISSEUR_PANNEAU = 0.02f, DECALAGE = 0.01f;
const float PORTE_LARGEUR = 0.90f, PORTE_HAUTEUR = 2.00f, PORTE_X = -0.70f;
const float FENETRE_LARGEUR = 1.20f, FENETRE_HAUTEUR = 1.00f, FENETRE_Y = 1.40f;
const float TABLE_LONGUEUR = 1.20f, TABLE_PROFONDEUR = 0.80f, TABLE_X = 0.60f, TABLE_Z = 0.70f;
const float PLATEAU_EPAISSEUR = 0.05f, PIED_SECTION = 0.06f;
const float PLATEAU_Y = HAUTEUR_TABLE - PLATEAU_EPAISSEUR / 2.0f;
const float PIED_HAUTEUR = HAUTEUR_TABLE - PLATEAU_EPAISSEUR;

static void dessiner(const NkMat4f& M, V3 colf) {
    for (const Face& f : FACES) {
        NkVec3f c = {(float)f.n.x * 0.5f, (float)f.n.y * 0.5f, (float)f.n.z * 0.5f};
        NkVec3f u = {(float)f.u.x * 0.5f, (float)f.u.y * 0.5f, (float)f.u.z * 0.5f};
        NkVec3f v = {(float)f.v.x * 0.5f, (float)f.v.y * 0.5f, (float)f.v.z * 0.5f};
        NkVec3f p[4];
        float su[4] = {-1, 1, 1, -1}, sv[4] = {-1, -1, 1, 1};
        V3 q[4];
        for (int i = 0; i < 4; ++i) {
            p[i] = M.Appliquer({c.x + su[i] * u.x + sv[i] * v.x,
                                c.y + su[i] * u.y + sv[i] * v.y,
                                c.z + su[i] * u.z + sv[i] * v.z});
            q[i] = {p[i].x, p[i].y, p[i].z};
        }
        drawQuad(q[0], q[1], q[2], q[3], f.n, colf);
    }
}

static bool proche(float a, float b) { return std::fabs(a - b) < 1e-5f; }

static bool verifier() {
    NkVec3f t = {0.2f, 0.5f, 0.8f};
    NkMat4f s = PoserAuSol(t, 1.0f, 2.0f);
    NkVec3f cs = s.Appliquer({0, 0, 0}), bs = s.Appliquer({0, -0.5f, 0}), ps = s.Appliquer({0.5f, 0.5f, 0.5f});
    bool ok = proche(cs.x, 1.0f) && proche(cs.y, 0.25f) && proche(cs.z, 2.0f) && proche(bs.y, 0.0f) &&
              proche(ps.x, 1.1f) && proche(ps.y, 0.5f) && proche(ps.z, 2.4f);
    NkVec3f u = {0.2f, 0.1f, 0.3f};
    NkMat4f tb = PoserSurTable(u, 0.6f, 0.7f);
    NkVec3f ct = tb.Appliquer({0, 0, 0}), bt = tb.Appliquer({0, -0.5f, 0}), pt = tb.Appliquer({0.5f, 0.5f, 0.5f});
    ok = ok && proche(ct.y, 0.80f) && proche(bt.y, 0.75f) && proche(pt.x, 0.7f) && proche(pt.y, 0.85f) && proche(pt.z, 0.85f);
    return ok;
}

void construirePiece() {
    V3 murCol = {0.93, 0.89, 0.80};
    V3 porteCol = {0.45, 0.25, 0.10};
    V3 fenetreCol = {0.45, 0.70, 0.95};
    V3 boisCol = {0.80, 0.55, 0.30};
    V3 pieds = {0.55, 0.33, 0.15};
    dessiner(Translation({0, -EPAISSEUR / 2, 0}) * Echelle({4.2f, EPAISSEUR, 4.2f}), {0.45, 0.40, 0.35});
    dessiner(Translation({0, HAUTEUR + EPAISSEUR / 2, 0}) * Echelle({4.2f, EPAISSEUR, 4.2f}), {0.97, 0.97, 0.97});
    dessiner(PoserAuSol({4.2f, HAUTEUR, EPAISSEUR}, 0, -CENTRE_MUR), murCol);
    dessiner(PoserAuSol({4.2f, HAUTEUR, EPAISSEUR}, 0, +CENTRE_MUR), murCol);
    dessiner(PoserAuSol({EPAISSEUR, HAUTEUR, INTERIEUR}, -CENTRE_MUR, 0), murCol);
    dessiner(PoserAuSol({EPAISSEUR, HAUTEUR, INTERIEUR}, +CENTRE_MUR, 0), murCol);
    dessiner(PoserAuSol({PORTE_LARGEUR, PORTE_HAUTEUR, EPAISSEUR_PANNEAU}, PORTE_X,
                        FACE_FOND_Z + DECALAGE - EPAISSEUR_PANNEAU / 2), porteCol);
    for (float z : {-0.90f, 0.90f})
        dessiner(Translation({FACE_GAUCHE_X + DECALAGE - EPAISSEUR_PANNEAU / 2, FENETRE_Y, z}) *
                 Echelle({EPAISSEUR_PANNEAU, FENETRE_HAUTEUR, FENETRE_LARGEUR}), fenetreCol);
    dessiner(Translation({TABLE_X, PLATEAU_Y, TABLE_Z}) *
             Echelle({TABLE_LONGUEUR, PLATEAU_EPAISSEUR, TABLE_PROFONDEUR}), boisCol);
    float dx = TABLE_LONGUEUR / 2 - PIED_SECTION / 2, dz = TABLE_PROFONDEUR / 2 - PIED_SECTION / 2;
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
            dessiner(PoserAuSol({PIED_SECTION, PIED_HAUTEUR, PIED_SECTION},
                                TABLE_X + sx * dx, TABLE_Z + sz * dz), pieds);
    dessiner(PoserSurTable({0.30f, 0.04f, 0.22f}, 0.45f, 0.60f), {0.20, 0.35, 0.75});
    dessiner(PoserSurTable({0.10f, 0.12f, 0.10f}, 0.90f, 0.85f), {0.80, 0.15, 0.15});
}

int main() {
    if (!verifier()) {
        std::printf("verification des fonctions : ECHEC\n");
        return 1;
    }
    std::printf("verification des fonctions : OK\n");
    initCamera();
    construirePiece();
    ecrireImage("poser.ppm");
    return 0;
}
