#include <bits/stdc++.h>
using namespace std;

struct Vec3{double x,y,z;};
struct Quat{double w,x,y,z;}; 
struct Pose{Vec3 position; Quat rotation;};

Vec3 operator+(Vec3 a,Vec3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
Vec3 operator-(Vec3 a,Vec3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
Vec3 cross(Vec3 a,Vec3 b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
Vec3 rotate(const Quat& q, Vec3 p){
    Vec3 qv{q.x,q.y,q.z};
    Vec3 t=cross(qv,p); t.x*=2; t.y*=2; t.z*=2;
    Vec3 c=cross(qv,t);
    return {p.x+q.w*t.x+c.x, p.y+q.w*t.y+c.y, p.z+q.w*t.z+c.z};
}

Quat mul(const Quat& a,const Quat& b){
    return {a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z,
            a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
            a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w};
}

Vec3 appliquer(const Pose& pose, Vec3 p){ return rotate(pose.rotation,p)+pose.position; }
Pose composer(const Pose& A,const Pose& B){
    return {rotate(A.rotation,B.position)+A.position, mul(A.rotation,B.rotation)};
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    double L1=1.0, L2=1.0;
    Quat qEpaule{1,0,0,0}, qCoude{1,0,0,0}, qMain{1,0,0,0};

    if(cin>>L1>>L2){
        cin>>qEpaule.w>>qEpaule.x>>qEpaule.y>>qEpaule.z;
        cin>>qCoude.w>>qCoude.x>>qCoude.y>>qCoude.z;
        cin>>qMain.w>>qMain.x>>qMain.y>>qMain.z;
        if(cin.fail()){ 
            qEpaule={1,0,0,0}; qCoude={1,0,0,0}; qMain={1,0,0,0};
            cin.clear();
        }
    }

    Pose poseEpaule{{0,0,0}, qEpaule};
    Pose poseCoudeLocal{{L1,0,0}, qCoude};
    Pose poseMainLocal{{L2,0,0}, qMain};

    Pose poseCoudeMonde = composer(poseEpaule, poseCoudeLocal);
    Pose poseMainMonde = composer(poseCoudeMonde, poseMainLocal);

    cout<<fixed<<setprecision(6);
    cout<<"Epaule identite / pose donnee :\n";
    cout<<poseCoudeMonde.position.x<<" "<<poseCoudeMonde.position.y<<" "<<poseCoudeMonde.position.z<<" // coude monde\n";
    cout<<poseMainMonde.position.x<<" "<<poseMainMonde.position.y<<" "<<poseMainMonde.position.z<<" // main monde\n";

    Quat q90{0.70710678,0,0,0.70710678};
    Quat qEpaule2 = mul(q90, qEpaule);
    Pose poseEpaule2{{0,0,0}, qEpaule2};
    Pose poseCoudeMonde2 = composer(poseEpaule2, poseCoudeLocal);
    Pose poseMainMonde2 = composer(poseCoudeMonde2, poseMainLocal);

    cout<<"Epaule tournee 90deg Z :\n";
    cout<<poseCoudeMonde2.position.x<<" "<<poseCoudeMonde2.position.y<<" "<<poseCoudeMonde2.position.z<<" // coude monde\n";
    cout<<poseMainMonde2.position.x<<" "<<poseMainMonde2.position.y<<" "<<poseMainMonde2.position.z<<" // main monde\n";
    return 0;
}

Teste (L1 = L2 = 1 et toutes les rotations à l'identité ) :
    1.000000 0.000000 0.000000 (coude monde = R_epaule*(1,0,0) )
    2.000000 0.000000 0.000000 (main monde = R_epaule*(1,0,0 + R_coude*(1,0,0)) )

Après Epaule *= 90° Z :
    0.000000 1.000000 0.000000 (coude)
    0.000000 2.000000 0.000000 (main)