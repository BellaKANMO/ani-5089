#include <bits/stdc++.h>
using namespace std;

struct Vec3 { double x,y,z; };
struct Quat { double w,x,y,z; };
struct Pose { Vec3 position; Quat rotation; };

Vec3 operator+(Vec3 a, Vec3 b){ return {a.x+b.x,a.y+b.y,a.z+b.z}; }
Vec3 operator-(Vec3 a, Vec3 b){ return {a.x-b.x,a.y-b.y,a.z-b.z}; }

Vec3 cross(Vec3 a, Vec3 b){
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
Vec3 rotate(const Quat& q, Vec3 p){
    Vec3 qv{q.x,q.y,q.z};
    Vec3 t = cross(qv,p); t.x*=2; t.y*=2; t.z*=2;
    Vec3 c = cross(qv,t);
    return {p.x + q.w*t.x + c.x, p.y + q.w*t.y + c.y, p.z + q.w*t.z + c.z};
}
Quat mul(const Quat& a, const Quat& b){
    return {
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
    };
}

Vec3 appliquer(const Pose& pose, Vec3 p){
    return rotate(pose.rotation, p) + pose.position;
}

Pose composer(const Pose& A, const Pose& B){
    Quat q = mul(A.rotation, B.rotation);
    Vec3 t = rotate(A.rotation, B.position) + A.position;
    return {t, q};
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Pose A,B;
    Vec3 p;

    if(!(cin>>A.position.x>>A.position.y>>A.position.z)) return 0;
    if(!(cin>>A.rotation.w>>A.rotation.x>>A.rotation.y>>A.rotation.z)) return 0;
    if(!(cin>>B.position.x>>B.position.y>>B.position.z)) return 0;
    if(!(cin>>B.rotation.w>>B.rotation.x>>B.rotation.y>>B.rotation.z)) return 0;
    if(!(cin>>p.x>>p.y>>p.z)) return 0;

  
    Vec3 p_seq = appliquer(A, appliquer(B, p));
    Pose C = composer(A,B);
    Vec3 p_comp = appliquer(C, p);

    Vec3 ecart = p_comp - p_seq;
    double norm = sqrt(ecart.x*ecart.x + ecart.y*ecart.y + ecart.z*ecart.z);

    cout<<fixed<<setprecision(6);
    cout<<p_comp.x<<" "<<p_comp.y<<" "<<p_comp.z<<"\n";
    cout<<p_seq.x<<" "<<p_seq.y<<" "<<p_seq.z<<"\n";
    cout<<ecart.x<<" "<<ecart.y<<" "<<ecart.z<<"\n";
    cout<<norm<<"\n"; 
    return 0;
}

Teste : 1 0 0 0.70710678 0 0 0.70710678 0 1 0 0.70710678 0 0 0.70710678 1 0 0
Sorties :
    -1.000000 0.000000 0.000000 (composer puis appliquer)
    -1.000000 0.000000 0.000000 (appliquer l'une après l'autre)
    0.000000 0.000000 0.000000 (écart)
    0.000000 (distance)