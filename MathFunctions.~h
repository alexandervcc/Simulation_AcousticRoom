//---------------------------------------------------------------------------
#ifndef MathFunctionsH
#define MathFunctionsH
//---------------------------------------------------------------------------
#include <math.h>
//---------------------------------------------------------------------------
inline double Round(double x) {
    double y;
    y=floor(x+0.5);
    return y;
}
//---------------------------------------------------------------------------
inline double Module(vector v) { //JFLN: Returns the vector's module
    double m;
    m=sqrt(v*v);
    return m;
}
//---------------------------------------------------------------------------
inline vector Normal(vector v1) { //JFLN: Returns the vector's unitary vector
    double m;
    vector v2;
    m=Module(v1);
    if(m==0)
        v2=0;
    else
        v2=v1/m;
    return v2;
}
//---------------------------------------------------------------------------
vector PointToVector(point p) {
    vector v;
    v.x=p.x;
    v.y=p.y;
    v.z=p.z;
    return v;
}
//---------------------------------------------------------------------------
point VectorToPoint(vector v) {
    point p;
    p.x=v.x;
    p.y=v.y;
    p.z=v.z;
    return p;
}
//---------------------------------------------------------------------------
point Rotation(point p,vector b0,vector b1,vector b2,point p0,double a) {
    point p1;
    vector b3,b4,b5;

    b3 = Normal(b0*cos(a)+b1*sin(a));
    b4 = Normal(b0*sin(-a)+b1*cos(a));
    b5 = Normal(b2);
    //translate
    p0 = p0+PointToVector(p*-1);
    //inverse rotate
    p1.x = b0.x * p0.x  +  b0.y * p0.y  +  b0.z * p0.z;
    p1.y = b1.x * p0.x  +  b1.y * p0.y  +  b1.z * p0.z;
    p1.z = b2.x * p0.x  +  b2.y * p0.y  +  b2.z * p0.z;
    //rotate
    p0.x = b3.x * p1.x  +  b4.x * p1.y  +  b5.x * p1.z;
    p0.y = b3.y * p1.x  +  b4.y * p1.y  +  b5.y * p1.z;
    p0.z = b3.z * p1.x  +  b4.z * p1.y  +  b5.z * p1.z;
    //translate
    p0=p0+PointToVector(p);

    return p0;
}
//---------------------------------------------------------------------------
vector Rotation(vector b0,vector b1,vector b2,vector v0,double a) {
    vector v1,b3,b4,b5;

    b3=Normal(b0*cos(a)+b1*sin(a));
    b4=Normal(b0*sin(-a)+b1*cos(a));
    b5=Normal(b2);
    //inverse rotate
    v1.x=b0.x*v0.x+b0.y*v0.y+b0.z*v0.z;
    v1.y=b1.x*v0.x+b1.y*v0.y+b1.z*v0.z;
    v1.z=b2.x*v0.x+b2.y*v0.y+b2.z*v0.z;
    //rotate
    v0.x=b3.x*v1.x+b4.x*v1.y+b5.x*v1.z;
    v0.y=b3.y*v1.x+b4.y*v1.y+b5.y*v1.z;
    v0.z=b3.z*v1.x+b4.z*v1.y+b5.z*v1.z;

    return v0;
}
//---------------------------------------------------------------------------
inline point IntersectionPoint(vector n,point p,vector u,point o) {
    double d,m;
    point i;
    m=n*u;
    if(m==0)
        return o;
    d=(n*(p-o))/m;
    if(d>0) {
        i=o+(u*d);
        return i;
    } else
        return o;
}
//---------------------------------------------------------------------------
inline double IntersectionDistance(vector n,point p,vector u,point o) {
    /*JFLN:
            vector n is the normal vector of the plane
            point p is one of the vertex of the plane
            vector u is the ray
            point o is the initial position of the ray
    */
    double d,m;
    m=n*u;
    //JFLN: Has to have an error tolerance
    if(m==0)
        return -1;
    d=(n*(p-o))/m;
    return d;
}
//---------------------------------------------------------------------------
inline double TriangleArea(triangle t) {
    double a;
    a=0.5*Module((t.p1-t.p0)/(t.p2-t.p0));
    return a;
}
//---------------------------------------------------------------------------
inline vector TriangleNormal(triangle t) {
    vector n;
    n=Normal((t.p1-t.p0)/(t.p2-t.p0));
    return n;
}
//---------------------------------------------------------------------------
inline bool Inner(point p,triangle t) {
    double a1,a2,x,y,z,x0,y0,z0,x1,y1,z1,x2,y2,z2;

    x=p.x;
    y=p.y;
    z=p.z;

    x0=t.p0.x;
    y0=t.p0.y;
    z0=t.p0.z;
    x1=t.p1.x;
    y1=t.p1.y;
    z1=t.p1.z;
    x2=t.p2.x;
    y2=t.p2.y;
    z2=t.p2.z;

    if(t.Projection==yz) {                              //Projeção yz
        a1=-t.a0*(-y0*z+y2*z+y*z0-y2*z0-y*z2+y0*z2);
        a2=-t.a0*(y0*z-y1*z-y*z0+y1*z0+y*z1-y0*z1);
    }
    if(t.Projection==xz) {                              //Projeção xz
        a1=-t.a0*(-x0*z+x2*z+x*z0-x2*z0-x*z2+x0*z2);
        a2=-t.a0*(x0*z-x1*z-x*z0+x1*z0+x*z1-x0*z1);
    }
    if(t.Projection==xy) {                              //Projeção xy
        a1=-t.a0*(-x2*y0+x*y0+x0*y2-x*y2-x0*y+x2*y);
        a2=t.a0*(-x1*y0+x*y0+x0*y1-x*y1-x0*y+x1*y);
    }

    if((a1+a2<=1)&&(a1>=0)&&(a2>=0))
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
inline bool Inner2(point p,triangle t) {

    double minX,maxX,minY,maxY,minZ,maxZ;

        minX=t.p0.x;

        if(minX>t.p1.x)
            minX=t.p1.x;
        
        if(minX>t.p2.x)
            minX=t.p2.x;

        minY=t.p0.y;

        if(minY>t.p1.y)
            minY=t.p1.y;
        
        if(minY>t.p2.y)
            minY=t.p2.y;

        minZ=t.p0.z;

        if(minZ>t.p1.z)
            minZ=t.p1.z;
        
        if(minZ>t.p2.z)
            minZ=t.p2.z;

        maxX=t.p0.x;

        if(maxX<t.p1.x)
            maxX=t.p1.x;
        
        if(maxX<t.p2.x)
            maxX=t.p2.x;

        maxY=t.p0.y;

        if(maxY<t.p1.y)
            maxY=t.p1.y;
        
        if(maxY<t.p2.y)
            maxY=t.p2.y;

        maxZ=t.p0.z;

        if(maxZ<t.p1.z)
            maxZ=t.p1.z;
        
        if(maxZ<t.p2.z)
            maxZ=t.p2.z;

        if((maxX>=p.x && minX<=p.x) && (maxY>=p.y && minY<=p.y) && (maxZ>=p.z && minZ<=p.z))
            return true;

        return false;

}
//---------------------------------------------------------------------------
double LeastSquares(double *p,int i,int j) {
    double yx,x,y,x2,m,n;
    int t;

    yx=0;
    x=0;
    y=0;
    x2=0;
    n=double(j-i);
    for(t=i; t<j; t++) {
        yx+=0.001*double(t)*p[t];
        x+=0.001*double(t);
        y+=p[t];
        x2+=0.001*double(t)*0.001*double(t);
    }
    m=(n*x2-x*x);
    if(m==0) m=-60*1.0e3;//JFLN: This is for an anechoic chamber, then the minimun value has to be the unit used in RAIOS.
    else m=(n*yx-x*y)/m;
    return m;
}
//---------------------------------------------------------------------------
#endif

