//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2013-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fÃ¼ggvÃ©nyt kivÃ©ve, a lefoglalt adat korrekt felszabadÃ­tÃ¡sa nÃ©lkÃ¼l
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : BÁLINT FERENC
// Neptun : EGUGHN
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

struct Vector {
    float x, y, z;
    
    Vector( ) {
        x = y = z = 0;
    }
    Vector(float x0, float y0, float z0 = 0) {
        x = x0; y = y0; z = z0;
    }
    Vector operator*(float a) {
        return Vector(x * a, y * a, z * a);
    }
    Vector operator+(const Vector& v) {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v) {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    float operator*(const Vector& v) {
        return (x * v.x + y * v.y + z * v.z);
    }
    Vector operator%(const Vector& v) {
        return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float Length() {
        return sqrt(x * x + y * y + z * z);
    }
    Vector Normal() {
        x /= Length();
        y /= Length();
        z /= Length();
        
        return *this;
    }
};

struct Color {
    float r, g, b;
    
    Color( ) {
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) {
        r = r0; g = g0; b = b0;
    }
    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }
    Color operator*(const Color& c) {
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator+(const Color& c) {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

float degreeToRadian(int degree) {
    return ((float)degree) / (2 * 3.14);
}

class Triangle {
public:
    Vector point1, point2, point3, normal1, normal2, normal3;
    
    Triangle() {

    }

    Triangle(Vector p1, Vector p2, Vector p3) {
        point1 = p1;
        point2 = p2;
        point3 = p3;
        
        normal1 = (point2 - point1) % (point3 - point1);
        normal1.Normal();
        
        normal2 = normal3 = normal1;
    }
    
    Triangle(Vector p1, Vector p2, Vector p3, Vector n1, Vector n2, Vector n3) {
        point1 = p1;
        point2 = p2;
        point3 = p3;
        
        normal1 = n1;
        normal2 = n2;
        normal3 = n3;
    }
    
    void drawTriangle() {
        glNormal3f(normal1.x, normal1.y, normal1.z);
        glVertex3f(point1.x, point1.y, point1.z);
        
        glNormal3f(normal2.x, normal2.y, normal2.z);
        glVertex3f(point2.x, point2.y, point2.z);
        
        glNormal3f(normal3.x, normal3.y, normal3.z);
        glVertex3f(point3.x, point3.y, point3.z);
    }
    
    void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
        glTexCoord2f(x1, y1);
        glNormal3f(normal1.x, normal1.y, normal1.z);
        glVertex3f(point1.x, point1.y, point1.z);
        
        glTexCoord2f(x2, y2);
        glNormal3f(normal2.x, normal2.y, normal2.z);
        glVertex3f(point2.x, point2.y, point2.z);
        
        glTexCoord2f(x3, y3);
        glNormal3f(normal3.x, normal3.y, normal3.z);
        glVertex3f(point3.x, point3.y, point3.z);
    }
};

class Ground {
    Vector centerPoint;
    int radius;
    
public:
    Ground() {
        
    }
    
    Ground(int x, int y, int z, int r = 500) {
        centerPoint = Vector(x, y, z);
        radius = r;
    }
    
    void drawGround() {
        Vector leftUpperCorner = Vector(centerPoint.x - radius, centerPoint.y, centerPoint.z + radius);
        Vector leftDownerCorner = Vector(centerPoint.x - radius, centerPoint.y, centerPoint.z - radius);
        Vector rightUpperCorner = Vector(centerPoint.x + radius, centerPoint.y, centerPoint.z + radius);
        Vector rightDownerCorner = Vector(centerPoint.x + radius, centerPoint.y, centerPoint.z - radius);
        
        Vector groundNormal = (rightUpperCorner - rightDownerCorner) % (leftUpperCorner - leftDownerCorner);
        
        glTexCoord2f(0, 1);
        glNormal3f(groundNormal.x, groundNormal.y, groundNormal.z);
        glVertex3f(leftUpperCorner.x, leftUpperCorner.y, leftUpperCorner.z);
        
        glTexCoord2f(1, 1);
        glNormal3f(groundNormal.x, groundNormal.y, groundNormal.z);
        glVertex3f(rightUpperCorner.x, rightUpperCorner.y, rightUpperCorner.z);
        
        glTexCoord2f(1, 0);
        glNormal3f(groundNormal.x, groundNormal.y, groundNormal.z);
        glVertex3f(rightDownerCorner.x, rightDownerCorner.y, rightDownerCorner.z);
        
        glTexCoord2f(0, 1);
        glNormal3f(groundNormal.x, groundNormal.y, groundNormal.z);
        glVertex3f(leftUpperCorner.x, leftUpperCorner.y, leftUpperCorner.z);
        
        glTexCoord2f(1, 0);
        glNormal3f(groundNormal.x, groundNormal.y, groundNormal.z);
        glVertex3f(rightDownerCorner.x, rightDownerCorner.y, rightDownerCorner.z);
        
        glTexCoord2f(0, 0);
        glNormal3f(groundNormal.x, groundNormal.y, groundNormal.z);
        glVertex3f(leftDownerCorner.x, leftDownerCorner.y, leftDownerCorner.z);
    }
};

class Ellipsoid {
    float xRadius, yRadius, zRadius;
    Triangle triangleMesh[72*60*2];
    int triangleCounter;
    
public:
    Ellipsoid() {
        
    }
    
    Ellipsoid(float xR, float yR, float zR) {
        xRadius = xR;
        yRadius = yR;
        zRadius = zR;
        triangleCounter = 0;
        
        Vector p1, p2, p3, p4, n1, n2, n3, n4;
        
        int beta, lambda;
        
        for (beta = -90; beta < 90; beta += 3) {
            for (lambda = -180; lambda < 180; lambda += 5) {
                float betaInRadian = degreeToRadian(beta);
                float lambdaInRadian = degreeToRadian(lambda);
                float betaInRadianPlusOne = degreeToRadian(beta + 1);
                float lambdaInRadianPlusOne = degreeToRadian(lambda + 1);
                
                p1.x = xRadius * cosf(betaInRadian) * cosf(lambdaInRadian);
                p1.y = yRadius * cosf(betaInRadian) * sinf(lambdaInRadian);
                p1.z = zRadius * sinf(betaInRadian);
                n1 = p1;
                
                p2.x = xRadius * cosf(betaInRadianPlusOne) * cosf(lambdaInRadian);
                p2.y = yRadius * cosf(betaInRadianPlusOne) * sinf(lambdaInRadian);
                p2.z = zRadius * sinf(betaInRadianPlusOne);
                n2 = p2;
                
                p3.x = xRadius * cosf(betaInRadian) * cosf(lambdaInRadianPlusOne);
                p3.y = yRadius * cosf(betaInRadian) * sinf(lambdaInRadianPlusOne);
                p3.z = zRadius * sinf(betaInRadian);
                n3 = p3;
                
                p4.x = xRadius * cosf(betaInRadianPlusOne) * cosf(lambdaInRadianPlusOne);
                p4.y = yRadius * cosf(betaInRadianPlusOne) * sinf(lambdaInRadianPlusOne);
                p4.z = zRadius * sinf(betaInRadianPlusOne);
                n4 = p4;
                
                triangleMesh[triangleCounter] = Triangle(p1, p2, p3, n1, n2, n3);
                triangleCounter++;
                triangleMesh[triangleCounter] = Triangle(p2, p3, p4, n2, n3, n4);
                triangleCounter++;
            }
        }
    }
    
    void drawEllipsoid() {
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < triangleCounter; i++) {
            triangleMesh[i].drawTriangle();
        }
        glEnd();
    }
};

class Frog {
    Vector centerPosition;
    int rotationAngle;
    Ellipsoid body, leftArm, leftLeg, rightArm, rightLeg, leftEye, rightEye;
    
public:
    Frog() {
        
    }
    
    Frog(float x, float y, float z, int rotation) {
        centerPosition.x = x;
        centerPosition.y = y;
        centerPosition.z = z;
        rotationAngle = rotation;
        body = Ellipsoid(50, 20, 30);
        leftArm = rightArm = Ellipsoid(14, 6, 10);
        leftLeg = rightLeg = Ellipsoid(12, 5, 9);
        leftEye = rightEye = Ellipsoid(5, 5, 5);
    }
    
    void drawFrog() {
        float ambientGreen[] = {0.14, 0.44, 0.14, 1};
        float diffuseGreen[] = {0.08, 0.76, 0.1 ,1};
        float specularGreen[] = {0.76, 1, 0.66 ,1};
        float ambientWhite[] = {0.4, 0.4, 0.4, 1};
        float diffuseWhite[] = {0.9, 0.9, 0.9 ,1};
        float specularWhite[] = {1, 1, 1 ,1};
        float shininess = 100;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambientGreen);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseGreen);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularGreen);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        
        glTranslatef(centerPosition.x, centerPosition.y, centerPosition.z);
        glRotatef(rotationAngle, 0, 1, 0);
        
        glPushMatrix();
        
        body.drawEllipsoid();
        
        glPushMatrix();
        glTranslatef(35, -14, 15);
        leftArm.drawEllipsoid();
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(35, -14, -15);
        rightArm.drawEllipsoid();
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(-34, -15, 15);
        leftLeg.drawEllipsoid();
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(-34, -15, -15);
        rightLeg.drawEllipsoid();
        glPopMatrix();
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambientWhite);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseWhite);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularWhite);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        
        glPushMatrix();
        glTranslatef(36, 14, 13);
        leftEye.drawEllipsoid();
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(36, 14, -13);
        leftEye.drawEllipsoid();
        glPopMatrix();
        
        glPopMatrix();
    }
};

class CatmullRomCurve {
public:
    Vector controlPoints[7];
    int controlPointNumber;
    float pointParameters[7];
    
    CatmullRomCurve() {
        controlPointNumber = 0;
        for (int i = 0; i < 12; i++) {
            controlPoints[i] = (Vector(0, 0, 0));
        }
    }
    
    void addPoint(Vector newPoint) {
        controlPoints[controlPointNumber] = newPoint;
        controlPoints[controlPointNumber + 1] = newPoint;
        
        if (controlPointNumber == 0) {
            pointParameters[controlPointNumber] = 0;
            pointParameters[controlPointNumber + 1] = 1;
            controlPointNumber += 2;
        } else {
            pointParameters[controlPointNumber] = pointParameters[controlPointNumber - 1] + 1;
            pointParameters[controlPointNumber + 1] = pointParameters[controlPointNumber] + 1;
            controlPointNumber++;
        }
    }
    
    Vector calculateCatmullRom(float t, int i, int derive = 0) {
        Vector c1, c2, c3, c4;
        c1 = controlPoints[i];
        c2 = controlPoints[i + 1];
        c3 = controlPoints[i + 2];
        c4 = controlPoints[i + 3];
        
        Vector point_1 = (((c2 - c1) * (1 / (pointParameters[i + 1] - pointParameters[i]))) + ((c3 - c2) * (1 / (pointParameters[i + 2] - pointParameters[i + 1])))) * (1 - (-0.5)) * 0.5;
        Vector point_2 = (((c3 - c2) * (1 / (pointParameters[i + 2] - pointParameters[i + 1]))) + ((c4 - c3) * (1 / (pointParameters[i + 3] - pointParameters[i + 2])))) * (1 - (-0.5)) * 0.5;
        
        Vector a = c2;
        Vector b = point_1;
        
        Vector c = ((c3 - c2) * 3) * (1 / pow(pointParameters[i + 2] - pointParameters[i + 1], 2)) - (point_2 + point_1 * 2) * (1 / (pointParameters[i + 2] - pointParameters[i + 1]));
        Vector d = (point_2 + point_1) * (1 / pow(pointParameters[i + 2] - pointParameters[i + 1], 2)) - ((c3 - c2) * 2) * (1 / pow(pointParameters[i + 2] - pointParameters[i + 1], 3));
        
        
        Vector nextPoint;
        if (derive == 0) {
            nextPoint =  d*pow(t - pointParameters[i + 1], 3) + c*pow(t - pointParameters[i + 1], 2) + b*(t - pointParameters[i + 1]) + a;
        } else {
            nextPoint =  b + (c * 2 + d * 3 * (t - pointParameters[i + 1])) * (t - pointParameters[i + 1]);
        }
        
        return nextPoint;
    }
    
};

class Cone {
    float radius;
    float height;
    Triangle triangleMesh[360];
    int triangleCounter;
    
public:
    Cone() {
        
    }
    
    Cone(float r, float h) {
        radius = r;
        height = h;
        triangleCounter = 0;
        
        Vector p1, p2, p3, n1, n2, n3;
        
        for (int i = 0; i < 360; i++) {
            float degree = degreeToRadian(i);
            float degreePlus = degreeToRadian(i + 1);
            
            p1.x = radius * sin(degree),
            p1.y = 0;
            p1.z = radius * cos(degree);
            
            p2.x = 0.0;
            p2.y = height;
            p2.z = 0.0;
            
            p3.x = radius * sin(degreePlus);
            p3.y = 0;
            p3.z = radius * cos(degreePlus);
            
            n1.x = sinf(degree);
            n1.y = sqrt(2.0) / 2.0;
            n1.z = cosf(degree);
            n1.Normal();
            
            n2.x = sinf(degreeToRadian(i + 0.5));
            n2.y = sqrt(2.0) / 2.0;
            n2.z = cosf(degreeToRadian(i + 0.5));
            n2.Normal();
            
            n3.x = sinf(degreePlus);
            n3.y = sqrt(2.0) / 2.0;
            n3.z = cosf(degreePlus);
            n3.Normal();
            
            triangleMesh[triangleCounter] = Triangle(p1, p2, p3, n1, n2, n3);
            triangleCounter++;
        }
    }
    
    void drawCone() {
        float ambientRed[] = {0.55, 0.08, 0.08, 1};
        float diffuseRed[] = {0.86, 0.27, 0.27 ,1};
        float specularRed[] = {0.98, 0.51, 0.51 ,1};
        float shininessRed = 20;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambientRed);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseRed);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularRed);
        glMaterialf(GL_FRONT, GL_SHININESS, shininessRed);
        
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < triangleCounter; i++) {
            triangleMesh[i].drawTriangle();
        }
        
        glEnd();
    }
};

class Cylinder {
    float radius;
    Triangle triangleMesh[360 * 2];
    int triangleCounter;
public:
    Cylinder() {
        
    }
    
    Cylinder(float r, int height) {
        float radius = r;
        
        Vector p1, p2, p3, p4, n1, n2, n3, n4;
        triangleCounter = 0;
        
        for(int i = 0; i < 360; i++) {
            float degree = degreeToRadian(i);
            float degreePlus = degreeToRadian(i + 1);
            p1.x = radius * cos(degree);
            p1.y = 0;
            p1.z = radius * sin(degree);
            
            n1 = p1;
            n1.y = 0;
            n1.Normal();
            
            p2.x = radius * cos(degreePlus);
            p2.y = radius * sin(degreePlus);
            p2.z = 0;
            
            n2 = p2;
            n2.y = 0;
            n2.Normal();
            
            p3.x = radius * cos(degree);
            p3.y = height;
            p3.z = radius * sin(degree);
            
            n3 = p3;
            n3.y = 0;
            n3.Normal();
            
            p4.x = radius * cos(degreePlus);
            p4.y = height;
            p4.z = radius * sin(degreePlus);
            
            n4 = p4;
            n4.y = 0;
            n4.Normal();
            
            triangleMesh[triangleCounter] = Triangle(p2, p4, p3, n2, n4, n3);
            triangleCounter++;
            triangleMesh[triangleCounter] = Triangle(p1, p2, p3, n1, n2, n3);
            triangleCounter++;
        }
    }
    
    void drawCylinder(){
        float ambientRed[] = {0.55, 0.08, 0.08, 1};
        float diffuseRed[] = {0.86, 0.27, 0.27 ,1};
        float specularRed[] = {0.98, 0.51, 0.51 ,1};
        float shininessRed = 20;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambientRed);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseRed);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularRed);
        glMaterialf(GL_FRONT, GL_SHININESS, shininessRed);
        
        glBegin(GL_TRIANGLES);
        for(int i = 0; i < triangleCounter; i++){
            triangleMesh[i].drawTriangle();
        }
        glEnd();
    }
};

Color storkTexture[32*32];

class Stork {
public:
    CatmullRomCurve spine;
    CatmullRomCurve body;
    
    Ellipsoid leftEye, rightEye;
    
    Ellipsoid head;
    
    Vector headPoint;
    
    Cone beak;
    
    Cylinder leftLegTop, leftLegBottom, rightLegTop, rightLegBottom;
    
    Triangle triangleMesh[50*360*2];
    int triangleCounter;
    
    void initStork() {
        triangleCounter = 0;
        leftEye = rightEye = Ellipsoid(5, 5, 5);
        head = Ellipsoid(15, 15, 25);
        beak = Cone(10.0, 150.0);
        leftLegTop = leftLegBottom = rightLegTop = rightLegBottom = Cylinder(5, 100);
        
        for (int i = 0; i < spine.controlPointNumber - 2; i++) {
            float stepUnit = 0.1;
            
            for (float t = spine.pointParameters[i + 1]; t < spine.pointParameters[i + 2]; t += stepUnit) {
                Vector spineActualPoint = spine.calculateCatmullRom(t, i);
                Vector bodyActualPoint = body.calculateCatmullRom(t, i);
                Vector spineNextPoint = spine.calculateCatmullRom(t + stepUnit, i);
                Vector bodyNextPoint = body.calculateCatmullRom(t + stepUnit, i);
                Vector spineNextNextPoint = spine.calculateCatmullRom(t + stepUnit * 2, i);
                Vector bodyNextNextPoint = body.calculateCatmullRom(t + stepUnit * 2, i);
                Vector spinePrevPoint = spine.calculateCatmullRom(t - stepUnit, i);
                Vector bodyPrevPoint = body.calculateCatmullRom(t - stepUnit, i);
                
                Vector TActual = spine.calculateCatmullRom(t, i, 1);
                Vector TNext = spine.calculateCatmullRom(t + stepUnit, i, 1);
                Vector TNextNext = spine.calculateCatmullRom(t + stepUnit * 2, i, 1);
                Vector TPrev = spine.calculateCatmullRom(t - stepUnit, i, 1);
                TActual.Normal();
                TNext.Normal();
                TNextNext.Normal();
                TPrev.Normal();
                
                Vector BActual =  Vector(1, 0, 0) % TActual;
                Vector BNext = Vector(1, 0, 0) % TNext;
                Vector BNextNext = Vector(1, 0, 0) % TNextNext;
                Vector BPrev = Vector(1, 0, 0) % TPrev;
                BActual.Normal();
                BNext.Normal();
                BNextNext.Normal();
                BPrev.Normal();
                
                Vector NActual = BActual % TActual;
                Vector NNext = BNext % TNext;
                Vector NNextNext = BNextNext % TNextNext;
                Vector NPrev = BPrev % TPrev;
                NActual.Normal();
                NNext.Normal();
                NNextNext.Normal();
                NPrev.Normal();
                
                float radiusActualPoint = (bodyActualPoint - spineActualPoint).Length();
                float radiusNextPoint = (bodyNextPoint - spineNextPoint).Length();
                float radiusNextNextPoint = (bodyNextNextPoint - spineNextNextPoint).Length();
                float radiusPrevPoint = (bodyPrevPoint - spinePrevPoint).Length();
                
                for (int j = 0; j < 360; j++) {
                    Vector p1 = spineActualPoint + NActual * sinf(j) * radiusActualPoint + BActual * cosf(j) * radiusActualPoint;
                    Vector p2 = spineNextPoint + NNext * sinf(j) * radiusNextPoint + BNext * cosf(j) * radiusNextPoint;
                    Vector p3 = spineActualPoint + NActual * sinf(j + 1) * radiusActualPoint + BActual * cosf(j + 1) * radiusActualPoint;
                    Vector p4 = spineNextPoint + NNext * sinf(j + 1) * radiusNextPoint + BNext * cosf(j + 1) * radiusNextPoint;
                    
                    Vector p1n = p1;
                    Vector p2n = p2;
                    Vector p3n = p3;
                    Vector p4n = p4;
                    
                    p1n.Normal();
                    p2n.Normal();
                    p3n.Normal();
                    p4n.Normal();
                    
                    Vector p1n1Helper = (spinePrevPoint + NPrev * sinf(j) * radiusPrevPoint + BPrev * cosf(j) * radiusPrevPoint) % p1n;
                    Vector p1n2Helper = (spineActualPoint + NActual * sinf(j - 1) * radiusActualPoint + BActual * cosf(j - 1) * radiusActualPoint) % p1n;
                    Vector p1n3Helper = p2 % p1n;
                    Vector p1n4Helper = p3 % p1n;
                    
                    Vector n1 = Vector((p1n1Helper.x + p1n2Helper.x + p1n3Helper.x + p1n4Helper.x) / 4,
                                       (p1n1Helper.y + p1n2Helper.y + p1n3Helper.y + p1n4Helper.y) / 4,
                                       (p1n1Helper.z + p1n2Helper.z + p1n3Helper.z + p1n4Helper.z) / 4);
                    
                    Vector p2n1Helper = p1 % p2n;
                    Vector p2n2Helper = (spineNextPoint + NNext * sinf(j - 1) * radiusNextPoint + BNext * cosf(j - 1) * radiusNextPoint) % p2n;
                    Vector p2n3Helper = (spineNextNextPoint + NNextNext * sinf(j) * radiusNextNextPoint + BNextNext * cosf(j) * radiusNextNextPoint) % p2n;
                    Vector p2n4Helper = p4 % p2n;
                    
                    Vector n2 = Vector((p2n1Helper.x + p2n2Helper.x + p2n3Helper.x + p2n4Helper.x) / 4,
                                       (p2n1Helper.y + p2n2Helper.y + p2n3Helper.y + p2n4Helper.y) / 4,
                                       (p2n1Helper.z + p2n2Helper.z + p2n3Helper.z + p2n4Helper.z) / 4);
                    
                    Vector p3n1Helper = (spinePrevPoint + NPrev * sinf(j + 1) * radiusPrevPoint + BPrev * cosf(j + 1) * radiusPrevPoint) % p3n;
                    Vector p3n2Helper = p1 % p3n;
                    Vector p3n3Helper = p4 % p3n;
                    Vector p3n4Helper = (spineActualPoint + NActual * sinf(j + 2) * radiusActualPoint + BActual * cosf(j + 2) * radiusActualPoint) % p3n;
                    
                    Vector n3 = Vector((p3n1Helper.x + p3n2Helper.x + p3n3Helper.x + p3n4Helper.x) / 4,
                                       (p3n1Helper.y + p3n2Helper.y + p3n3Helper.y + p3n4Helper.y) / 4,
                                       (p3n1Helper.z + p3n2Helper.z + p3n3Helper.z + p3n4Helper.z) / 4);
                    
                    Vector p4n1Helper = p3 % p4n;
                    Vector p4n2Helper = p2 % p4n;
                    Vector p4n3Helper = (spineNextNextPoint + NNextNext * sinf(j + 1) * radiusNextNextPoint + BNextNext * cosf(j + 1) * radiusNextNextPoint) % p4n;
                    Vector p4n4Helper = (spineNextPoint + NNext * sinf(j + 2) * radiusNextPoint + BNext * cosf(j + 2) * radiusNextPoint) % p4n;
                    
                    Vector n4 = Vector((p4n1Helper.x + p4n2Helper.x + p4n3Helper.x + p4n4Helper.x) / 4,
                                       (p4n1Helper.y + p4n2Helper.y + p4n3Helper.y + p4n4Helper.y) / 4,
                                       (p4n1Helper.z + p4n2Helper.z + p4n3Helper.z + p4n4Helper.z) / 4);
                    
                    n1.Normal();
                    n2.Normal();
                    n3.Normal();
                    n4.Normal();
                    
                    triangleMesh[triangleCounter] = Triangle(p2, p1, p3);
                    triangleCounter++;
                    triangleMesh[triangleCounter] = Triangle(p3, p4, p2);
                    triangleCounter++;
                }
                
                headPoint = spineActualPoint;
            }
        }
    }
    
    void drawStork() {
        float ambientBlack[] = {0, 0, 0, 1};
        float diffuseBlack[] = {0.12, 0.12, 0.12 ,1};
        float specularBlack[] = {0.9, 0.9, 0.9 ,1};
        float shininessBlack = 50;
        float ambientWhite[] = {0.82, 0.82, 0.82, 1};
        float diffuseWhite[] = {0.94, 0.94, 0.94 ,1};
        float specularWhite[] = {1, 1, 1 ,1};
        float shininessWhite = 100;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambientWhite);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseWhite);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularWhite);
        glMaterialf(GL_FRONT, GL_SHININESS, shininessWhite);
        
        glEnable(GL_TEXTURE_2D);
        unsigned int textureId;
        
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_FLOAT, storkTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
        glBegin(GL_TRIANGLES);
        float x = 0, y = 0;
        int i;
        for (i = 0; i < triangleCounter; i++) {
            if ((i % 2) == 0) {
                triangleMesh[i].drawTriangle(x / 360, (y + 1) / 43, x / 360, y / 43, (x + 1) / 360, y / 43);
            } else {
                triangleMesh[i].drawTriangle((x + 1) / 360, y / 43, (x + 1) / 360, (y + 1) / 43, x / 360, (y + 1) / 43);
                x++;
            }
            
            if ((i % 720) == 0) {
                x = 0;
                y++;
            }
        }
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        
        glPushMatrix();
        glTranslatef(headPoint.x, headPoint.y, headPoint.z + 10);
        glRotatef(45, 1, 0, 0);
        head.drawEllipsoid();
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambientBlack);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseBlack);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specularBlack);
        glMaterialf(GL_FRONT, GL_SHININESS, shininessBlack);
        
        glPushMatrix();
        glTranslatef(11, 0, 10);
        leftEye.drawEllipsoid();
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(-11, 0, 10);
        leftEye.drawEllipsoid();
        glPopMatrix();
        
        glPushMatrix();
        glRotatef(90, 1, 0, 0);
        beak.drawCone();
        glPopMatrix();
        
        glPopMatrix();
        
        glPushMatrix();
        glRotatef(30, 1, 0, 0);
        glTranslatef(-30, 10, 90);
        
        leftLegTop.drawCylinder();
        
        glPushMatrix();
        glRotatef(-60, 1, 0, 0);
        glTranslatef(0, -90, 5);
        leftLegBottom.drawCylinder();
        glPopMatrix();
        
        glPopMatrix();
        
        glPushMatrix();
        glRotatef(30, 1, 0, 0);
        glTranslatef(30, 10, 90);
        rightLegTop.drawCylinder();
        
        glPushMatrix();
        glRotatef(-60, 1, 0, 0);
        glTranslatef(0, -90, 5);
        leftLegBottom.drawCylinder();
        glPopMatrix();
        
        glPopMatrix();
    }
};

void lightsOn() {
    float positionLight0[] = {-400, 400, -400, 0};
    float ambientLight0[] = {0.7, 0.69, 0.68, 1};
    float diffuseLight0[] = {0.84, 0.82, 0.80, 1};
    float specularLight0[] = {1, 1, 1, 1};
    float shininessLight0 = 10;

    float positionLight1[] = {0, 100, 0, 1};
    float ambientLight1[] = {0.2, 0.11, 0.1, 1};
    float diffuseLight1[] = {0.3, 0.25, 0.2, 1};
    float specularLight1[] = {1, 1, 1, 1};
    float shininessLight1 = 5;
    
    glEnable(GL_LIGHTING);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
    glLightfv(GL_LIGHT0, GL_POSITION, positionLight0);
    glLightf(GL_LIGHT0, GL_SHININESS, shininessLight0);
    
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_POSITION, positionLight1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
    glLightf(GL_LIGHT1, GL_SHININESS, shininessLight1);
    glEnable(GL_LIGHT1);
}

const int screenWidth = 600;
const int screenHeight = 600;
const int frogCounter = 5;

Ground ground;

Color groundTexture[8*8];

Frog frogArray[frogCounter];

Stork stork;

void onInitialization( ) {
    
	glViewport(0, 0, screenWidth, screenHeight);
    
    ground = Ground(0, 0, 0, 600);
    
    stork.spine.addPoint(Vector(0, 0, 0));
    stork.spine.addPoint(Vector(0, 75, 75));
    stork.spine.addPoint(Vector(0, 120, 150));
    stork.spine.addPoint(Vector(0, 190, 225));
    stork.spine.addPoint(Vector(0, 275, 250));
    
    stork.body.addPoint(Vector(0, 0, 0));
    stork.body.addPoint(Vector(0, 125, 75));
    stork.body.addPoint(Vector(0, 177, 150));
    stork.body.addPoint(Vector(0, 215, 225));
    stork.body.addPoint(Vector(0, 275, 260));
    
    stork.initStork();
    
    for (int i = 0; i < frogCounter; i++) {
        frogArray[i] = Frog(rand() % 1000 - 500, 20, rand() % 1000 - 500, rand() % 360 + 1);
    }
    
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            if (j % 2) {
                if (i % 2) {
                    groundTexture[j * 8 + i] = Color(0.27, 0.4, 0.1);
                } else {
                    groundTexture[j * 8 + i] = Color(0.47, 0.63, 0.16);
                }
            } else {
                if (i % 2) {
                    groundTexture[j * 8 + i] = Color(0.47, 0.63, 0.16);
                    
                } else {
                    groundTexture[j * 8 + i] = Color(0.27, 0.4, 0.1);
                }
            }
        }
    }
    
    for (int j = 0; j < 32; j++) {
        for (int i = 0; i < 32; i++) {
            if (j < 1)
                storkTexture[j * 32 + i] = Color(0, 0, 0);
            else
                storkTexture[j * 32 + i] = Color(1, 1, 1);
        }
    }
    
}

void onDisplay( ) {
    glClearColor(0.68, 0.84, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    glShadeModel(GL_SMOOTH);
    
    glViewport(0, 0, 600, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60, 1, 10, 2000);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(800, 400, 800, 0, 0, 0, 0, 1, 0);
    
    lightsOn();
    
    glEnable(GL_TEXTURE_2D);
    unsigned int textureId;
    
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_FLOAT, groundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    ground.drawGround();
    glEnd();
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    
    glPushMatrix();
    glTranslatef(-300, 110, -200);
    stork.drawStork();
    glPopMatrix();
    
    for (int i = 0; i < frogCounter; i++) {
        glPushMatrix();
        frogArray[i].drawFrog();
        glPopMatrix();
    }
    
    glutSwapBuffers();
    
}

void onKeyboard(unsigned char key, int x, int y) {
    glutPostRedisplay( );
}

void onKeyboardUp(unsigned char key, int x, int y) {
    
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        
		glutPostRedisplay( );
}

void onMouseMotion(int x, int y) {
    
}

void onIdle( ) {
    long time = glutGet(GLUT_ELAPSED_TIME);
    
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer
    
    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon
    
    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    
    onInitialization();					// Az altalad irt inicializalast lefuttatjuk
    
    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
    
    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}
