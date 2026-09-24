#include <bits/stdc++.h>
using namespace std;

struct Vec3 { double x,y,z; };
struct Quat { double w,x,y,z; };
struct Pose { Vec3 position; Quat rotation; };

Vec3 operator+(Vec3 a, Vec3 b){ return {a.x+b.x, a.y+b.y, a.z+b.z}; }

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

Vec3 appliquer_RT(const Pose& pose, Vec3 point){
    return rotate(pose.rotation, point) + pose.position;
}

Vec3 appliquer_TR(const Pose& pose, Vec3 point){
    return rotate(pose.rotation, point + pose.position);
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Pose pose;
    Vec3 point;

    if(!(cin>>pose.position.x>>pose.position.y>>pose.position.z)) return 0;
    if(!(cin>>pose.rotation.w>>pose.rotation.x>>pose.rotation.y>>pose.rotation.z)) return 0;
    if(!(cin>>point.x>>point.y>>point.z)) return 0;

    Vec3 p1 = appliquer_RT(pose, point);
    Vec3 p2 = appliquer_TR(pose, point);

    cout<<fixed<<setprecision(6)<<p1.x<<" "<<p1.y<<" "<<p1.z<<"\n";
    cout<<fixed<<setprecision(6)<<p2.x<<" "<<p2.y<<" "<<p2.z<<"\n";
    return 0;
}

Sorties pour le meme point : 
    RT : R*point + t 
    TR : R*(point + t)


Cas de coincidence : 
    p1 = R*(1,0,0) + (0,0,1) = (0,1,0) + (0,0,1) = (0,1,1)
    p2 = R*((1,0,0)+(0,0,1)) = R*(1,0,1) = (0,1,1)

Entree : 0 0 1 0.70710678 0 0 0.70710678 1 0 0
Sorties : 
    0.000000 1.000000 1.000000
    0.000000 1.000000 1.000000

Il y a coincidence si t = (0,0,0) ou si R est la matrice identitee i.e R=(1,0,0,0)