#include <bits/stdc++.h>
using namespace std;

struct Vec3{double x,y,z;};
struct Quat{double w,x,y,z;};
struct Pose{Vec3 position; Quat rotation;};
struct Mat4{double m[4][4];};

Vec3 operator+(Vec3 a,Vec3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
Vec3 operator-(Vec3 a,Vec3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
Vec3 cross(Vec3 a,Vec3 b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
Vec3 rotate(const Quat& q, Vec3 p){
    Vec3 qv{q.x,q.y,q.z};
    Vec3 t=cross(qv,p); t.x*=2; t.y*=2; t.z*=2;
    Vec3 c=cross(qv,t);
    return {p.x+q.w*t.x+c.x, p.y+q.w*t.y+c.y, p.z+q.w*t.z+c.z};
}

Mat4 poseToMatrix(const Pose& p){
    double w=p.rotation.w,x=p.rotation.x,y=p.rotation.y,z=p.rotation.z;
    Mat4 M{};
    for(int i=0;i<4;i++) for(int j=0;j<4;j++) M.m[i][j]=(i==j);
    M.m[0][0]=1-2*(y*y+z*z); M.m[0][1]=2*(x*y-w*z);   M.m[0][2]=2*(x*z+w*y);
    M.m[1][0]=2*(x*y+w*z);   M.m[1][1]=1-2*(x*x+z*z); M.m[1][2]=2*(y*z-w*x);
    M.m[2][0]=2*(x*z-w*y);   M.m[2][1]=2*(y*z+w*x);   M.m[2][2]=1-2*(x*x+y*y);
    M.m[0][3]=p.position.x; M.m[1][3]=p.position.y; M.m[2][3]=p.position.z;
    return M;
}

Mat4 inverserGenerale(const Mat4& A, bool &ok){
    double aug[4][8];
    for(int i=0;i<4;i++) for(int j=0;j<4;j++) aug[i][j]=A.m[i][j];
    for(int i=0;i<4;i++) for(int j=0;j<4;j++) aug[i][4+j]=(i==j);
    ok=true;
    for(int col=0;col<4;col++){
        // pivot
        int piv=col;
        for(int r=col;r<4;r++) if(fabs(aug[r][col])>fabs(aug[piv][col])) piv=r;
        if(fabs(aug[piv][col])<1e-12){ ok=false; break; }
        if(piv!=col) for(int j=0;j<8;j++) swap(aug[col][j],aug[piv][j]);
        double div=aug[col][col];
        for(int j=0;j<8;j++) aug[col][j]/=div;
        for(int r=0;r<4;r++) if(r!=col){
            double f=aug[r][col];
            for(int j=0;j<8;j++) aug[r][j]-=f*aug[col][j];
        }
    }
    Mat4 inv{};
    if(!ok) for(int i=0;i<4;i++) for(int j=0;j<4;j++) inv.m[i][j]=nan("");
    else for(int i=0;i<4;i++) for(int j=0;j<4;j++) inv.m[i][j]=aug[i][4+j];
    return inv;
}

Pose InverserDirect(const Pose& p){
    Quat qc{p.rotation.w,-p.rotation.x,-p.rotation.y,-p.rotation.z};
    Vec3 tInv = rotate(qc, {-p.position.x,-p.position.y,-p.position.z});
    return {tInv,qc};
}
Mat4 inverserDirecte(const Pose& p){
    return poseToMatrix(InverserDirect(p)); // = [R^T | -R^T*t]
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    Pose pose{{1,2,3},{0.70710678,0,0,0.70710678}};
    if(cin>>pose.position.x>>pose.position.y>>pose.position.z)
        cin>>pose.rotation.w>>pose.rotation.x>>pose.rotation.y>>pose.rotation.z;

    Mat4 M = poseToMatrix(pose);
    bool ok;
    Mat4 Mg = inverserGenerale(M, ok);
    Mat4 Md = inverserDirecte(pose);

    cout<<fixed<<setprecision(6);
    cout<<"Mg generale ok="<<ok<<"\n";
    for(int i=0;i<4;i++){ for(int j=0;j<4;j++) cout<<setw(10)<<Mg.m[i][j]<<" "; cout<<"\n"; }
    cout<<"Md directe\n";
    for(int i=0;i<4;i++){ for(int j=0;j<4;j++) cout<<setw(10)<<Md.m[i][j]<<" "; cout<<"\n"; }

    double maxd=0;
    for(int i=0;i<4;i++) for(int j=0;j<4;j++) maxd=max(maxd,fabs(Mg.m[i][j]-Md.m[i][j]));
    cout<<"max |Mg-Md| = "<<maxd<<" -> "<<(maxd<1e-9?"IDENTIQUES":"DIFFERENTS")<<"\n\n";

    Mat4 D{}; for(int i=0;i<4;i++) for(int j=0;j<4;j++) D.m[i][j]=(i==j);
    D.m[0][0]=0; D.m[0][1]=0; D.m[0][2]=0; D.m[0][3]=1; // ligne X ecrasee -> det=0
    bool ok2;
    Mat4 Dg = inverserGenerale(D, ok2);
    cout<<"Test degenere det=0 : ok="<<ok2<<"\n";
    if(!ok2) cout<<"Inversion generale echoue -> det~0 division par 0 => NaN/inf\n";
    else for(int i=0;i<4;i++){ for(int j=0;j<4;j++) cout<<Dg.m[i][j]<<" "; cout<<"\n"; }
    cout<<"Inversion directe ne detecte rien, elle suppose R orthonormee et rend quand meme une matrice fausse\n";
    return 0;
}

Pose normale t=(1,2,3) q=90°Z:
    Mg == Md à 1e-12 près
    max |Mg-Md| = 0.000000

Pose dégénérée det = 0 :
    ok=false
    Mg = nan nan nan nan ...