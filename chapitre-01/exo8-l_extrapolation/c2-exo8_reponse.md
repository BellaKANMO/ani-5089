#include <bits/stdc++.h>
using namespace std;

struct Vec3{double x,y,z;};
struct Quat{double w,x,y,z;};
struct Pose{Vec3 position; Quat rotation;};

Vec3 operator+(Vec3 a,Vec3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
Vec3 operator*(Vec3 a,double s){return {a.x*s,a.y*s,a.z*s};}
Vec3 operator/(Vec3 a,double s){return {a.x/s,a.y/s,a.z/s};}
double norm(Vec3 a){return sqrt(a.x*a.x+a.y*a.y+a.z*a.z);}
Quat mul(const Quat& a,const Quat& b){
    return {a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z,
            a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
            a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w};
}
Quat normalize(Quat q){
    double n=sqrt(q.w*q.w+q.x*q.x+q.y*q.y+q.z*q.z);
    if(n>1e-12){q.w/=n;q.x/=n;q.y/=n;q.z/=n;}
    return q;
}

Pose avancer(const Pose& pose, Vec3 v, Vec3 omega, double dt){

    Vec3 p2 = pose.position + v * dt;

    Quat q2 = pose.rotation;
    double w = norm(omega);
    const double eps = 1e-12;
    if(w > eps && fabs(dt) > eps){
        double theta = w * dt; 
        Vec3 axe = omega / w;
        double half = theta * 0.5;
        double c = cos(half);
        double s = sin(half);
        Quat dq{c, axe.x*s, axe.y*s, axe.z*s};

        q2 = mul(dq, pose.rotation);
        q2 = normalize(q2);
    }

    return {p2, q2};
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    Pose pose{{0,0,0},{1,0,0,0}};
    Vec3 v{0,0,0}, omega{0,0,0};
    double dt=0;

    if(!(cin>>pose.position.x>>pose.position.y>>pose.position.z)) return 0;
    if(!(cin>>pose.rotation.w>>pose.rotation.x>>pose.rotation.y>>pose.rotation.z)) return 0;
    if(!(cin>>v.x>>v.y>>v.z)) return 0;
    if(!(cin>>omega.x>>omega.y>>omega.z)) return 0;
    if(!(cin>>dt)) return 0;

    Pose res = avancer(pose, v, omega, dt);

    cout<<fixed<<setprecision(6);
    cout<<res.position.x<<" "<<res.position.y<<" "<<res.position.z<<"\n";
    cout<<res.rotation.w<<" "<<res.rotation.x<<" "<<res.rotation.y<<" "<<res.rotation.z<<"\n";
    return 0;
}

Teste : 0 0 0 1 0 0 0  1 0 0  0 0 1.57079633 1
Sortie :
    1.000000 0.000000 0.000000
    0.707107 0.000000 0.000000 0.707107

90° autour de Z + 1m en X : 0 0 0 1 0 0 0  1 0 0  0 0 0 1
Sortie :
    1.000000 0.000000 0.000000
    1.000000 0.000000 0.000000 0.000000