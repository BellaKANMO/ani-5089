#include <bits/stdc++.h>
using namespace std;

struct Vec3 {
    double x, y, z;
};

struct Quat {
    double w, x, y, z;
};

Quat conj(Quat q) {
    return {q.w, -q.x, -q.y, -q.z};
}

Quat mul(Quat a, Quat b) {
    return {
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
    };
}

double dot(Quat a, Quat b) {
    return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

Quat normalize(Quat q) {
    double n = sqrt(dot(q, q));
    q.w /= n;
    q.x /= n;
    q.y /= n;
    q.z /= n;
    return q;
}

Vec3 vitesseMoyenne(Quat q1, Quat q2, double dt, bool forcerCourt) {
    q1 = normalize(q1);
    q2 = normalize(q2);

    if (forcerCourt && dot(q1, q2) < 0) {
        q2.w = -q2.w;
        q2.x = -q2.x;
        q2.y = -q2.y;
        q2.z = -q2.z;
    }

    Quat dq = mul(q2, conj(q1));
    dq = normalize(dq);

    double n = sqrt(dq.x * dq.x + dq.y * dq.y + dq.z * dq.z);
    const double eps = 1e-12;

    if (n < eps || fabs(dt) < eps) {
        return {0, 0, 0};
    }

    double angle = 2.0 * atan2(n, dq.w);
    double inv = 1.0 / n;
    Vec3 axe = {dq.x * inv, dq.y * inv, dq.z * inv};

    return {axe.x * angle / dt, axe.y * angle / dt, axe.z * angle / dt};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Quat q1{1, 0, 0, 0};
    Quat q2{1, 0, 0, 0};
    double dt = 0.1;

    if (!(cin >> q1.w >> q1.x >> q1.y >> q1.z)) return 0;
    if (!(cin >> q2.w >> q2.x >> q2.y >> q2.z)) return 0;
    if (!(cin >> dt)) return 0;

    Vec3 wCourt = vitesseMoyenne(q1, q2, dt, true);
    Vec3 wLong = vitesseMoyenne(q1, q2, dt, false);

    cout << fixed << setprecision(6);
    cout << wCourt.x << " " << wCourt.y << " " << wCourt.z << "\n";
    cout << wLong.x << " " << wLong.y << " " << wLong.z << "\n";
    cout << sqrt(wCourt.x * wCourt.x + wCourt.y * wCourt.y + wCourt.z * wCourt.z) << "\n";
    cout << sqrt(wLong.x * wLong.x + wLong.y * wLong.y + wLong.z * wLong.z) << "\n";

    return 0;
}

Teste absude 1 : 
    1 0 0 0
    -0.9961947 0 0 -0.0871557
    0.1
Sortie : 
    0.000000 0.000000 1.745329
    0.000000 0.000000 -61.086524
    1.745329
    61.086524

Teste normal 2 :
    1 0 0 0
    0.9961947 0 0 0.0871557
    0.1
Sortie :
    0.000000 0.000000 1.745329
    0.000000 0.000000 1.745329
    1.745329
    1.745329

