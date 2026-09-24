#include <bits/stdc++.h>
using namespace std;

struct Vec3 { double x,y,z; };
struct Quat { double w,x,y,z; }; // normalisé
struct Pose { Vec3 position; Quat rotation; };

Vec3 operator+(Vec3 a, Vec3 b){ return {a.x+b.x,a.y+b.y,a.z+b.z}; }
Vec3 operator-(Vec3 a, Vec3 b){ return {a.x-b.x,a.y-b.y,a.z-b.z}; }

Vec3 cross(Vec3 a, Vec3 b){
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
Vec3 rotate(const Quat& q, Vec3 p){
    Vec3 qv{q.x,q.y,q.z};
    Vec3 t = cross(qv,p);
    t.x*=2; t.y*=2; t.z*=2;
    Vec3 c = cross(qv,t);
    return {p.x + q.w*t.x + c.x, p.y + q.w*t.y + c.y, p.z + q.w*t.z + c.z};
}

Vec3 appliquer(const Pose& pose, Vec3 p){
    return rotate(pose.rotation, p) + pose.position;
}

Pose Inverser(const Pose& pose){
    Quat qc{pose.rotation.w, -pose.rotation.x, -pose.rotation.y, -pose.rotation.z};
    Vec3 tInv = rotate(qc, {-pose.position.x, -pose.position.y, -pose.position.z});
    return {tInv, qc};
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Pose pose;
    Vec3 p;

    if(!(cin>>pose.position.x>>pose.position.y>>pose.position.z)) return 0;
    if(!(cin>>pose.rotation.w>>pose.rotation.x>>pose.rotation.y>>pose.rotation.z)) return 0;
    if(!(cin>>p.x>>p.y>>p.z)) return 0;

    Pose inv = Inverser(pose);
    
    Vec3 p1 = appliquer(pose, p);
    Vec3 p2 = appliquer(inv, p1);
    Vec3 ecart = p2 - p;
    double norm = sqrt(ecart.x*ecart.x + ecart.y*ecart.y + ecart.z*ecart.z);

    cout<<fixed<<setprecision(6);
    cout<< p2.x << " " << p2.y << " " << p2.z << "\n";
    cout<< ecart.x << " " << ecart.y << " " << ecart.z << "\n";
    cout<< norm << "\n";
    return 0;
}

1 2 3 0.70710678 0 0 0.70710678 1 0 0
p1 = 1.000000 3.000000 3.000000
p2 = 1.000000 0.000000 0.000000
ecart = 0.000000 0.000000 0.000000
norm = 0.000000