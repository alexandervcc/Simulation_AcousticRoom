//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <gl\gl.h>
#include <gl\glu.h>
#include <ctype.h>
#include <string> 
#include "Classes.h"
#include "MathFunctions.h"
#include "Definitions.h"

#include "UnitView.h"
#define MAXDOUBLE 1000000
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtCtrls"
#pragma resource "*.dfm"
TFormView *FormView;
//---------------------------------------------------------------------------
__fastcall TFormView::TFormView(TComponent* Owner) : TForm(Owner) {
        drawRays=false;
    int i,j;
    Application->OnIdle=IdleLoop;
    FormView->Top=0;//ToolBarHeight;
    FormView->Left=0;
    FormView->Height=FormView->ClientHeight;
    FormView->Width=FormView->ClientWidth;

//default colors
    Red.R      =1;
    Red.G      =0.65;
    Red.B      =0.65;;
    Green.R    =0.65;
    Green.G    =1;
    Green.B    =0.65;
    Blue.R     =0.65;
    Blue.G     =0.65;
    Blue.B     =1;
    Yellow.R   =1;
    Yellow.G   =1;
    Yellow.B   =0.4;
    LightGray.R=0.55;
    LightGray.G=0.55;
    LightGray.B=0.55;
    DarkGray.R =0.4;
    DarkGray.G =0.4;
    DarkGray.B =0.4;
//default values
    a=19*PI/60;
    b=13*PI/60;
    d=10;
    f=0.5;      //approximately 30 degrees (or PI*30/180 rad)
    g=0.2;
    e=40;

    c=0;
    w=ClientWidth;
    h=ClientHeight;
    cuartoCargado=false;
    TestExtra=false;
    distancia=0.0;
    tiempo=0.0;
}
//---------------------------------------------------------------------------
void __fastcall TFormView::FormCreate(TObject *Sender) {
//OpenGL initialize
    hdc=GetDC(Handle);
    SetPixelFormatDescriptor();
    hrc=wglCreateContext(hdc);
    if(hrc==NULL)
        ShowMessage("hrc==NULL");
    if(wglMakeCurrent(hdc,hrc)==false)
        ShowMessage("Could not MakeCurrent");
//OpenGL default parameters
    glClearColor(0.6,0.6,0.6,0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0,1.0);
    glLineWidth(1);
    glPointSize(4);

    a   =19*PI/60;
    b   =13*PI/60;
    d   =10;
    f   =0.5;      //approximately 30 degrees (or PI*30/180 rad)
    g   =0.2;
    e   =40;
    c   =0;
}
//---------------------------------------------------------------------------

void __fastcall TFormView::SetPixelFormatDescriptor() {
//OpenGL initialize
    PIXELFORMATDESCRIPTOR pfd= {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        24,
        0,0,0,0,0,0,
        0,0,
        0,0,0,0,0,
        32,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0,0,
    };
    PixelFormat=ChoosePixelFormat(hdc,&pfd);
    SetPixelFormat(hdc,PixelFormat,&pfd);
}

inline void __fastcall TFormView::glVertexp(point p)
{
//draw point
    glVertex3f(p.x,p.y,p.z);
}
//---------------------------------------------------------------------------
inline void __fastcall TFormView::glColorc(color c)
{
//color RGB
    glColor3f(c.R,c.G,c.B);
}

void __fastcall TFormView::IdleLoop(TObject*,bool& done) {
//draw scene
    DrawScene();
}

//---------------------------------------------------------------------------
void __fastcall TFormView::DrawScene(void) {
    int     i,j;
    double  df, //Distance of Front-plane of view
    db; //Distance of Back-plane of view

    //OpenGL parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    db=sqrt(3*e*g*e*g+2*e*g*(abs(c.x)+abs(c.y)+abs(c.z))+c.x*c.x+c.y*c.y+c.z*c.z);
    if(d>db)
        df=db;
    else
        df=0.99*d;

    gluPerspective(180*f/PI,double(w)/double(h),d-df,d+db);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    o.x=d*cos(b)*cos(a)+c.x;
    o.y=d*cos(b)*sin(a)+c.y;
    o.z=d*sin(b)+c.z;
    Up.x=-sin(a);
    Up.y=cos(a);
    Up.z=0;
    Up=Normal(Up/(c-o));

    gluLookAt(o.x,o.y,o.z,c.x,c.y,c.z,Up.x,Up.y,Up.z);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    //draw room
    if(cuartoCargado){
        int tmpG=0;
        if(TestExtra){
                tmpG=floor(1000*distancia/V_SON);
                if(tmpG>TIMSEC){
                        tmpG=TIMSEC-1;
                }
        }
        
        for (i=0;i<r.NP;i++)
                DrawPlane(r.p[i],tmpG);

        for(int nS=0;nS<r.NS;nS++){
                DrawSource(r.s[nS]);
        }

        //DrawReceptor(r.r[0]);
        for(int nR=0;nR<r.NR;nR++){
                DrawReceptor(r.r[nR]);
        }

    }

    if(TestExtra){
        for (i=0;i<NRAYS;i++){
            for (j=0;j<ry[i].N-1;j++){
                vector vg;
                point pnt;
                double da,ds; //Distancia recorrida en reflexi�n y en la siguiente
                vg=vis_vector[i][j];
                da=vis_modvec[i][j];
                ds=vis_modvec[i][j+1];
                pnt=ry[i].r[j];
                // distancia es el valor recorrido controlado por el timer
                if(distancia>da && distancia <ds )//Dibujar un vector creciente (din�mico)
                    DrawVector(pnt,vg,true,distancia-da);
                else if (distancia > ds){//Dibujar un vector est�tico
                    DrawVector(pnt,vg,false,0);
                }
            }
        }
    }

    //draw grid
    //DrawGrid();
    //draw coordinates
    glDisable(GL_DEPTH_TEST);
    DrawAxis();
    glEnable(GL_DEPTH_TEST);
    //swap OpenGL buffers
    SwapBuffers(hdc);
}

//---------------------------------------------------------------------------
inline void __fastcall TFormView::DrawPlane (plane p, int tmpG) {

    int i;
    int triID=0;
    color col;

    for(int i=0;i<p.NT;i++){
        col=p.Color;
        triID=p.t[i].ID;
        if(TestExtra){
                col.getHeatMapColor(mEA.energia[triID][tmpG]);
        }
        glColorc(col*(1-(p.n*Normal(c-o)*0.2)));
        glBegin(GL_TRIANGLES);
                glVertexp(p.t[i].p0);
                glVertexp(p.t[i].p1);
                glVertexp(p.t[i].p2);
        glEnd();

        glColorc(p.Color*0.5);
        glBegin(GL_LINE_LOOP);
                glVertexp(p.t[i].p0);
                glVertexp(p.t[i].p1);
                glVertexp(p.t[i].p2);
        glEnd();          



    }

//Contour
    glColorc(p.Color*0.5);
  	glBegin(GL_LINE_LOOP);
        for(i=0;i<p.NP;i++)
            glVertexp(p.p[i]);
  	glEnd();
}

inline void __fastcall TFormView::DrawVector(point p, vector v, bool f, double m) {

    glColorc(Yellow);

    if(f){
        v=Normal(v);
        v=v*m;
        double rad=0.05;
        glPushMatrix();
            glTranslatef(v.x+p.x, v.y+p.y, v.z+p.z);
            gluSphere(gluNewQuadric(), rad, 35, 56);
        glPopMatrix();
    }
        
        if(drawRays){
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex3f(p.x,p.y,p.z);
    glVertex3f(p.x+v.x,p.y+v.y,p.z+v.z);
    glEnd();
    }
}

inline void __fastcall TFormView::DrawSource(source f) {
    int i;
    //inner
    glColorc(f.Color);
    glBegin(GL_TRIANGLES);
        for(i=0;i<20;i++){
            glVertexp(f.p + f.IcoFace[i].p0*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p1*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p2*f.VisualRadius);
        }
    glEnd();
    //contour
    glColorc(f.Color*0.5);
    glLineWidth(1);
  	for(i=0;i<20;i++){
        glBegin(GL_LINE_LOOP);
            glVertexp(f.p + f.IcoFace[i].p0*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p1*f.VisualRadius);
            glVertexp(f.p + f.IcoFace[i].p2*f.VisualRadius);
     	glEnd();
    }
}

inline void __fastcall TFormView::DrawReceptor(receptor q) {
    glColorc(q.Color);
    glBegin(GL_QUADS);
        for(int i=0;i<32;i++)
            for(int j=0;j<4;j++)
                glVertexp(q.p+(q.SphereFace[i][j]*q.VisualRadius));
    glEnd();

    glColorc(q.Color*0.5);
    //contour of sphere faces
    for(int i=0;i<32;i++){
        glBegin(GL_LINE_LOOP);
            for(int j=0;j<4;j++)
                glVertexp(q.p+(q.SphereFace[i][j]*q.VisualRadius));
  	    glEnd();
    }
}

inline void __fastcall TFormView::DrawGrid(void) {
    double l;
//inner lines
    glColorc(LightGray);
    glBegin(GL_LINES);
    for(l=g; l<e*g-g/2; l+=g) {
        glVertex3f(l,-e*g,0);
        glVertex3f(l,e*g,0);
        glVertex3f(-e*g,l,0);
        glVertex3f(e*g,l,0);
    }
    for(l=-g; l>-e*g+g/2; l-=g) {
        glVertex3f(l,e*g,0);
        glVertex3f(-e*g,l,0);
        glVertex3f(e*g,l,0);
        glVertex3f(l,-e*g,0);
    }
    glEnd();
//contour lines
    glColorc(DarkGray);
    glBegin(GL_LINE_LOOP);
    glVertex3f(e*g,e*g,0);
    glVertex3f(e*g,-e*g,0);
    glVertex3f(-e*g,-e*g,0);
    glVertex3f(-e*g,e*g,0);
    glEnd();
//axis lines
    glBegin(GL_LINES);
    glVertex3f(-e*g,0,0);
    glVertex3f(e*g,0,0);
    glVertex3f(0,-e*g,0);
    glVertex3f(0,e*g,0);
    glVertex3f(0,0,-e*g);
    glVertex3f(0,0,e*g);
    glEnd();
}
//

inline void __fastcall TFormView::DrawCoordinate(point p,color c) {
//draw coordinates
    glColorc(c);
    glBegin(GL_LINES);
    glVertex3f(p.x,0,0);
    glVertex3f(p.x,p.y,0);
    glVertex3f(0,p.y,0);
    glVertex3f(p.x,p.y,0);
    glVertex3f(p.x,p.y,0);
    glVertex3f(p.x,p.y,p.z);
    glEnd();
}


inline void __fastcall TFormView::DrawAxis(void) {
//x, y and z coloured axis
    glLineWidth(1);
    glColorc(Red);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0.1*e*g,0,0);
    glEnd();
    glColorc(Green);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0.1*e*g,0);
    glEnd();
    glColorc(Blue);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(0,0,0.1*e*g);
    glEnd();
    glLineWidth(1);
}

//---------------------------------------------------------------------------

void __fastcall TFormView::FormMouseMove(TObject *Sender,
        TShiftState Shift, int X, int Y) {
    int dx,dy;
    int k;
    vector n,nplane;
    point i,m,z;
    double v;

    if(Shift.Contains(ssShift))
        v=4;
    else if(Shift.Contains(ssCtrl))
        v=2;
    else
        v=1;

    if(Shift.Contains(ssLeft) ) {
        dx = X-LastMousePos[0];//Negativo mueve a la derecha
        dy = Y-LastMousePos[1];//Negativo mueve a arriba

        //viewer rotation

        if(dx<0)
            a+=v*PI/60;
        else
            a-=v*PI/60;

        if(dy>0) {
            b+=v*PI/60;
        } else {
            b-=v*PI/60;
        }
    }

    LastMousePos[0] = X;
    LastMousePos[1] = Y;
}
//---------------------------------------------------------------------------

void __fastcall TFormView::FormMouseWheelDown(TObject *Sender,
        TShiftState Shift, TPoint &MousePos, bool &Handled) {
    //viewer zoom
    double v;
    if(Shift.Contains(ssShift)) {
        v=4;
    } else if(Shift.Contains(ssCtrl)) {
        v=8;
    } else {
        v=2;
    }
    if(d<100*v*2*g)
        d+=v*2*g*tan(0.5/(v*2))/tan(f/(v*2));
}
//---------------------------------------------------------------------------
void __fastcall TFormView::FormMouseWheelUp(TObject *Sender,
        TShiftState Shift, TPoint &MousePos, bool &Handled) {
//viewer zoom
    double v;
    if(Shift.Contains(ssShift)) {
        v=4;
    } else if(Shift.Contains(ssCtrl)) {
        v=8;
    } else {
        v=2;
    }
    if(d>v*2*g)
        d-=v*2*g*tan(0.5/(v*2))/tan(f/(v*2));   //0.5 is the initial fov
}
//---------------------------------------------------------------------------

void __fastcall TFormView::FormResize(TObject *Sender) {
    //resize OpenGL viewport
    w=ClientWidth;
    h=ClientHeight;
    if(h==0)h=1;
    glViewport(0,0,w,h);
}
//---------------------------------------------------------------------------

void __fastcall TFormView::Timer1Timer(TObject *Sender) {
    double speed=0.3;
    tiempo++;
    distancia=speed*tiempo;
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TFormView::btnCargarCuboClick(TObject *Sender)
{
    r.Clear();

    TIMSEC=1000;

//Inicializo receptor
        int numRec=27;
        int n_div=nTri->Text.ToInt();

        r.NewReceptor(numRec);
        int lim_i=-2;
        int lim_s=2;
        int cont_g=0;

        double paso=(lim_s-lim_i)/(4);
        for(int x=lim_i+paso;x<lim_s;x=x+paso){
            for(int y=lim_i+paso;y<lim_s;y=y+paso){
                for(int z=lim_i+paso;z<lim_s;z=z+paso){
                    r.r[cont_g].p.x=x;
                    r.r[cont_g].p.y=y;
                    r.r[cont_g].p.z=z;
                    r.r[cont_g].createTimeSamples(TIMSEC);

                    cont_g++;
                }
            }
        }
        /*
        r.NewReceptor(1);
        r.r[0].p=-1;
        r.r[0].createTimeSamples(TIMSEC);
        */


//Inicializo fuente
    r.NewSources(1);
    r.s[0].p=1.5;

    //r.s[0].createRays(1000);
    int raysNum=nRayos->Text.ToInt();
    r.s[0].createRays(raysNum);

    NRAYS=r.s[0].NRAYS;

    r.s[0].eF=100;

//Inicializaciones cubo

    r.NewPlanes(6);
//-------------square 1 back
    r.p[0].NewPoints(4);
    r.p[0].p[0].x=-2.0f;
    r.p[0].p[0].y=2.0f;
    r.p[0].p[0].z=2.0f;
    r.p[0].p[1].x=-2.0f;
    r.p[0].p[1].y=-2.0f;
    r.p[0].p[1].z=2.0f;
    r.p[0].p[2].x=-2.0f;
    r.p[0].p[2].y=-2.0f;
    r.p[0].p[2].z=-2.0f;
    r.p[0].p[3].x=-2.0f;
    r.p[0].p[3].y=2.0f;
    r.p[0].p[3].z=-2.0f;
    r.p[0].MoreTriangle(n_div);//PointGenTriangle();

    //-------------square 2 front
    r.p[1].NewPoints(4);
    r.p[1].p[0].x=2.0f;
    r.p[1].p[0].y=2.0f;
    r.p[1].p[0].z=2.0f;
    r.p[1].p[1].x=2.0f;
    r.p[1].p[1].y=2.0f;
    r.p[1].p[1].z=-2.0f;
    r.p[1].p[2].x=2.0f;
    r.p[1].p[2].y=-2.0f;
    r.p[1].p[2].z=-2.0f;
    r.p[1].p[3].x=2.0f;
    r.p[1].p[3].y=-2.0f;
    r.p[1].p[3].z=2.0f;
    r.p[1].MoreTriangle(n_div);//PointGenTriangle();

    //-------------square 3 left
    r.p[2].NewPoints(4);
    r.p[2].p[0].x=-2.0f;
    r.p[2].p[0].y=-2.0f;
    r.p[2].p[0].z=2.0f;
    r.p[2].p[1].x=2.0f;
    r.p[2].p[1].y=-2.0f;
    r.p[2].p[1].z=2.0f;
    r.p[2].p[2].x=2.0f;
    r.p[2].p[2].y=-2.0f;
    r.p[2].p[2].z=-2.0f;
    r.p[2].p[3].x=-2.0f;
    r.p[2].p[3].y=-2.0f;
    r.p[2].p[3].z=-2.0f;
    r.p[2].MoreTriangle(n_div);//PointGenTriangle();

    //-------------square 4 right
    r.p[3].NewPoints(4);
    r.p[3].p[0].x=2.0f;
    r.p[3].p[0].y=2.0f;
    r.p[3].p[0].z=2.0f;
    r.p[3].p[1].x=-2.0f;
    r.p[3].p[1].y=2.0f;
    r.p[3].p[1].z=2.0f;
    r.p[3].p[2].x=-2.0f;
    r.p[3].p[2].y=2.0f;
    r.p[3].p[2].z=-2.0f;
    r.p[3].p[3].x=2.0f;
    r.p[3].p[3].y=2.0f;
    r.p[3].p[3].z=-2.0f;
    r.p[3].MoreTriangle(n_div);//PointGenTriangle();

    //-------------square 5 top
    r.p[4].NewPoints(4);
    r.p[4].p[0].x=-2.0f;
    r.p[4].p[0].y=-2.0f;
    r.p[4].p[0].z=2.0f;
    r.p[4].p[1].x=-2.0f;
    r.p[4].p[1].y=2.0f;
    r.p[4].p[1].z=2.0f;
    r.p[4].p[2].x=2.0f;
    r.p[4].p[2].y=2.0f;
    r.p[4].p[2].z=2.0f;
    r.p[4].p[3].x=2.0f;
    r.p[4].p[3].y=-2.0f;
    r.p[4].p[3].z=2.0f;
    r.p[4].MoreTriangle(n_div);//PointGenTriangle();

    //-------------square 1 bottom
    r.p[5].NewPoints(4);
    r.p[5].p[0].x=-2.0f;
    r.p[5].p[0].y=2.0f;
    r.p[5].p[0].z=-2.0f;
    r.p[5].p[1].x=-2.0f;
    r.p[5].p[1].y=-2.0f;
    r.p[5].p[1].z=-2.0f;
    r.p[5].p[2].x=2.0f;
    r.p[5].p[2].y=-2.0f;
    r.p[5].p[2].z=-2.0f;
    r.p[5].p[3].x=2.0f;
    r.p[5].p[3].y=2.0f;
    r.p[5].p[3].z=-2.0f;
    r.p[5].MoreTriangle(n_div);//PointGenTriangle();

    //Inicializar normales de los planos
    // se calculan las normales con la normal de su primer triangulo
    int nt=0;
    for (int i=0; i<r.NP; i++) {
        r.p[i].n=TriangleNormal(r.p[i].t[0]);
        for (int j=0;j<r.p[i].NT;j++){
            r.p[i].t[j].CalculateProjection();
            r.p[i].t[j].ID=nt;
            r.p[i].t[j].calculateBC();
            nt++;
        }
    }
    NTRI=nt;

    dR= new matDouble[r.NR];
    pR= new matDouble[r.NR];
    aR= new matDouble[r.NR];
    tvR=new matInt[r.NR];



    for(int re=0;re<r.NR;re++){
        dR[re].init(1,NTRI);
        tvR[re].init(1,NTRI);
        aR[re].init(1,NTRI);
        pR[re].init(1,NTRI);
    }


    md.init(NTRI,NTRI);
    mi.init(NTRI,NTRI);
    mp.init(NTRI+1,NTRI+1);
    mpor.init(NTRI,NTRI);


    for(int i=0;i<r.NP;i++){                               //recorre planos
        for(int j=0;j<r.p[i].NT;j++){                      //recprre tri x plano
                int indI=r.p[i].t[j].ID;
                mp.d[indI][NTRI]=0.0;

                for(int k=0;k<r.NP;k++){                   //recprre planos
                        for(int l=0;l<r.p[k].NT;l++){      //recprre tri x plano
                                int indJ=r.p[k].t[l].ID;
                                mp.d[NTRI][indJ]=0.0;
                                if(i!=k){
                                        md.d[indI][indJ]=Module(r.p[i].t[j].bc-r.p[k].t[l].bc);

                                        vector vBC =r.p[k].t[l].bc-r.p[i].t[j].bc;
                                        double beta=(vBC.Angle(r.p[i].n)*PI)/180;
                                        if(beta>(PI/2)){
                                                beta=beta;
                                        }
                                        double cos_beta = cos(beta);

                                        mp.d[indI][indJ]=r.p[k].t[l].solidAngle(r.p[i].t[j].bc)*cos_beta;
                                        mp.d[indI][NTRI]=mp.d[indI][NTRI]+mp.d[indI][indJ];

                                }else{
                                        md.d[indI][indJ]=0.0;
                                }
                                mi.d[indI][indJ]=int(1000*md.d[indI][indJ]/V_SON);
                        }
                }
        }
    }

    for(int i=0;i<NTRI;i++){
        for(int j=0;j<NTRI;j++){
                mpor.d[i][j]=mp.d[i][j]/mp.d[i][NTRI];
        }
    }


    //Distancias/T.Vuelo Source->tri
    for(int re=0;re<r.NR;re++){
	
	for(int i=0;i<r.NP;i++){
        for(int j=0;j<r.p[i].NT;j++ ){
                int indTri=r.p[i].t[j].ID;

                vector dis_vec=r.r[re].p-r.p[i].t[j].bc;
                dR[re].d[0][indTri]= dis_vec.Module();                        //Distancia
                tvR[re].d[0][indTri]=int(1000*dR[re].d[0][indTri]/V_SON) ;        //Tiempo de vuelo

                //Angulo Solido Triangulo->Receptor
                double beta=(dis_vec.Angle(r.p[i].n)*PI)/180;
                double cos_beta = cos(beta);
                aR[re].d[0][indTri]=r.r[re].solidAngle(r.p[i].t[j].bc)*cos_beta;
                }
        }
    }
    for(int re=0;re<r.NR;re++){
    for(int i=0;i<r.NP;i++){
        for(int j=0;j<r.p[i].NT;j++ ){
                int indTri=r.p[i].t[j].ID;
                pR[re].d[0][indTri]=aR[re].d[0][indTri]/mp.d[indTri][NTRI];
        }
    }
    }
    mpor.grabarArchivo('V',NTRI);
    mp.grabarArchivo('A',NTRI+1);
    md.grabarArchivo('D',NTRI);
    mi.grabarArchivo('I',NTRI);


    r.MaxDistance();
    
    cuartoCargado=true;
}





//---------------------------------------------------------------------------

void __fastcall TFormView::Button1Click(TObject *Sender)
{
	
    if(TestExtra==false){

        double eneRay,eneRes;


        eneRay=r.s[0].eF/r.s[0].NRAYS;

        vi=r.s[0].Rays;
        p_org=r.s[0].p;

        ry=NULL;
        ry=r.RayTracing(p_org,vi,NRAYS);

        //Iniciar matriz de energia
        mE.init(NTRI,TIMSEC);
        mEA.init(NTRI,TIMSEC);

        vis_vector = new vector*[NRAYS];
        vis_modvec = new double*[NRAYS];
        vis_timacu = new int*[NRAYS];

        double alfa, delta;
        alfa=0.2;
        delta=0.15;

	for(int Ry=0; Ry<NRAYS; Ry++){

            eneRes=eneRay;

            vis_vector[Ry] = new vector[ry[Ry].N-1];
            vis_modvec[Ry] = new double[ry[Ry].N];
            vis_timacu[Ry] = new int[ry[Ry].N];
            vis_modvec[Ry][0]=0.0;
            vis_timacu[Ry][0]=0;

            for(int Rf=0; Rf<ry[Ry].N-1;Rf++){
                //Guardo el vector con la ruta de la reflexion
                vis_vector[Ry][Rf] = ry[Ry].r[Rf+1]-ry[Ry].r[Rf];
                //Registro la distancia acumulada de las reflexiones anteriores
                vis_modvec[Ry][Rf+1] = vis_modvec[Ry][Rf] + ry[Ry].d[Rf+1];
                vis_timacu[Ry][Rf+1] = int((1000*vis_modvec[Ry][Rf+1]/V_SON));
            }

            for (int i=1; i<ry[Ry].N; i++){
                int idTri,idTime;
                idTime=vis_timacu[Ry][i];
                idTri=ry[Ry].idTriangle[i];



                mE.energia[idTri][idTime]+=(eneRes*(1-alfa)*delta);

                for(int re=0;re<r.NR;re++){
                        r.r[re].receptionRayTracing(ry[Ry].r[i-1],vis_vector[Ry][i-1],vis_timacu[Ry][i],r.maxd,eneRes);
                }

                eneRes=eneRes*(1-alfa)*(1-delta);
            }

        }


    //Transicion de Energia
        for(int t=0;t<TIMSEC;t++){
                for(int to=0;to<NTRI;to++){
                        for(int td=0;td<NTRI;td++){
                                int tv=t+mi.d[to][td];
                                if(tv<TIMSEC){
                                    mE.energia[td][tv]+=mE.energia[to][t]*mpor.d[to][td]*(1-alfa);
                                }
                        }
                }
        }


     for(int re=0;re<r.NR;re++){
        for(int t=0;t<TIMSEC;t++){
                for(int to=0;to<NTRI;to++){
                        int tvRec= t+tvR[re].d[0][to];
                        if(tvRec<TIMSEC){
			        r.r[re].eR[tvRec]+=mE.energia[to][t]*pR[re].d[0][to];
                        }

                }
        }
     }

        //Matriz de Energia de Triangulos
        for(int i=0;i<mEA.ele;i++){
                for(int j=1;j<mEA.tim;j++){
                        mEA.energia[i][j]=mEA.energia[i][j-1]+mE.energia[i][j];
                }
        }
        mEA.normalizar(mEA.maxEne());

        //Crea un archivo con la respuesta en el receptor
        for(int re=0;re<r.NR;re++){
                r.r[re].grabarArchivo();
        }

        distancia=0.0;
        tiempo=0.0;
        Timer1->Enabled=true;
        TestExtra=true;
  }
    else{
        TestExtra=false;
        Timer1->Enabled=false;
        distancia=0.0;
        tiempo=0.0;
    }
}

       
//---------------------------------------------------------------------------




void __fastcall TFormView::tRayosClick(TObject *Sender)
{
    if(drawRays){
        drawRays=false;
    }else{
        drawRays=true;
    }
}
//---------------------------------------------------------------------------

