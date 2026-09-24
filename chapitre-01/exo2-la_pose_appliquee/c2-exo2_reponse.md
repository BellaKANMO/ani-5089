#include <bits/stdc++.h>
using namespace std;

struct Vec3 {
    double x, y, z;
    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
};

struct Quat {
    double w, x, y, z;
};

struct Pose {
    Vec3 position;
    Quat rotation;
};

Vec3 cross(const Vec3& a, const Vec3& b){
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

// rotation d'un vecteur par un quaternion unitaire: q * p * q^-1
Vec3 rotate(const Quat& q, const Vec3& p){
    Vec3 qv{q.x, q.y, q.z};
    Vec3 t = cross(qv, p);
    t.x *= 2.0; t.y *= 2.0; t.z *= 2.0;
    Vec3 c = cross(qv, t);
    // p\' = p + w*t + c
    return {p.x + q.w * t.x + c.x,
            p.y + q.w * t.y + c.y,
            p.z + q.w * t.z + c.z};
}

Vec3 appliquer(const Pose& pose, const Vec3& point){
    Vec3 r = rotate(pose.rotation, point);
    return r + pose.position;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Pose pose;
    Vec3 point;
    
    if(!(cin >> pose.position.x >> pose.position.y >> pose.position.z)) return 0;
    if(!(cin >> pose.rotation.w >> pose.rotation.x >> pose.rotation.y >> pose.rotation.z)) return 0;
    if(!(cin >> point.x >> point.y >> point.z)) return 0;

    Vec3 res = appliquer(pose, point);

    cout << fixed << setprecision(6) << res.x << " " << res.y << " " << res.z << "\n";
    return 0;
}