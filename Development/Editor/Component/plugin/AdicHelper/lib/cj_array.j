// Edit By TigerCN
define {
    
    list(s,b1) =  { s[0] = b1 }
    list(s,b1,b2) =  { s[0] = b1; s[1] = b2}
    list(s,b1,b2,b3) =  { s[0] = b1; s[1] = b2; s[2] = b3; }
    list(s,b1,b2,b3,b4) =  { s[0] = b1; s[1] = b2; s[2] = b3; s[3] = b4 }
    list(s,b1,b2,b3,b4,b5) =  { s[0] = b1; s[1] = b2; s[2] = b3; s[3] = b4; s[4] = b5 }
    list(s,b1,b2,b3,b4,b5,b6) =  { s[0] = b1; s[1] = b2; s[2] = b3; s[3] = b4; s[4] = b5; s[5] = b6 }
    list(s,b1,b2,b3,b4,b5,b6,b7) =  { s[0] = b1; s[1] = b2; s[2] = b3; s[3] = b4; s[4] = b5; s[5] = b6; s[6] = b7 }
    list(s,b1,b2,b3,b4,b5,b6,b7,b8) =  { s[0] = b1; s[1] = b2; s[2] = b3; s[3] = b4; s[4] = b5; s[5] = b6; s[6] = b7; s[7]                                        = b8 }
    list(s,b1,b2,b3,b4,b5,b6,b7,b8,b9) =  { s[0] = b1; s[1] = b2; s[2] = b3; s[3] = b4; s[4] = b5; s[5] = b6; s[6] = b7; s                                          [7] = b8; s[8] = b9 }
    list(s,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10) =  { s[0] = b1; s[1] = b2; s[2] = b3; s[3] = b4; s[4] = b5; s[5] = b6; s[6] =                                                  b7; s[7] = b8; s[8] = b9; s[9] = b10 }
    
    MemberList = {m11, m12, m13, m21, m22, m23, m31, m32, m33}
    ShareList = {m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44}
    ElementList(a) = {a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]}
    RevoltList(a) = {a[0][0],a[0][1],a[0][2],a[1][0],a[1][1],a[1][2],a[2][0],a[2][1],a[2][2]}
    Range(s,m11,m12,m21,m22) = {s[0][0] = m11; s[0][1] = m12; 
                                s[1][0] = m21; s[1][1] = m22; }
    Range(s,MemberList) = {s[0][0] = m11; s[0][1] = m12; s[0][2] = m13;
                           s[1][0] = m21; s[1][1] = m22; s[1][2] = m23;
                           s[2][0] = m31; s[2][1] = m32; s[2][2] = m33;}
    Reshape(a,b) = { b[0][0] = a[0]; b[0][1] = a[1]; b[1][0] = a[2]; b[1][1] = a[3] }
    SubEduct(a) = {a[0][0] * a[1][1] - a[1][0] * a[0][1]}
    AgencyList = {real x, real y, real z}
    ArgumentList = {real m11, real m12, real m13, \
                    real m21, real m22, real m23, \
                    real m31, real m32, real m33  }
    ParameterList = {real m11, real m12, real m13, real m14,\
                     real m21, real m22, real m23, real m24,\
                     real m31, real m32, real m33, real m34,\
                     real m41, real m42, real m43, real m44 }
    SufferMatrix3 = Matrix3.create(1,0,0, \
                                   0,1,0, \
                                   0,0,1 );

    EmptyMatrix3 = Matrix3.create(0,0,0, \
                                  0,0,0, \
                                  0,0,0 );

    SufferMatrix4 = Matrix4.NewSolid(1,0,0,0,\
                                   0,1,0,0,\
                                   0,0,1,0,\
                                   0,0,0,1);

    EmptyMatrix4 = Matrix4.NewSolid(0,0,0,0,\
                                  0,0,0,0,\
                                  0,0,0,0,\
                                  0,0,0,0 );

    InitialParent(state) =  { i = 0; while(i != 3) {
                                state;
                            }}

    InitialChild(state) =   { i = 0; while(i != 2) {
                                state;
                            }}
    PerkData(a) = {a.x=x;a.y=y;a.z=z}
    PerkData(a,0) = {a.x=0;a.y=0;a.z=0}
    PercolateData(a,b) = {a.x=b.x;a.y=b.y;a.z=b.z}
    ScanData(a) = {a.m11 = m11; a.m12 = m12; a.m13 = m13;
                   a.m21 = m21; a.m22 = m22; a.m23 = m23;
                   a.m31 = m31; a.m32 = m32; a.m33 = m33;}
    SkimData(a) = {a.m11 = m11; a.m12 = m12; a.m13 = m13; a.m14 = m14;
                   a.m21 = m21; a.m22 = m22; a.m23 = m23; a.m24 = m24;
                   a.m31 = m31; a.m32 = m32; a.m33 = m33; a.m34 = m34;
                   a.m41 = m41; a.m42 = m42; a.m43 = m43; a.m44 = m44}
    SkimData(a,0) = {a.m11 = 0; a.m12 = 0; a.m13 = 0; a.m14 = 0;
                     a.m21 = 0; a.m22 = 0; a.m23 = 0; a.m24 = 0;
                     a.m31 = 0; a.m32 = 0; a.m33 = 0; a.m34 = 0;
                     a.m41 = 0; a.m42 = 0; a.m43 = 0; a.m44 = 0}
    ScanData(a,b) = {a.m11 = b.m11; a.m12 = b.m12; a.m13 = b.m13; a.m14 = b.m14;
                     a.m21 = b.m21; a.m22 = b.m22; a.m23 = b.m23; a.m24 = b.m24;
                     a.m31 = b.m31; a.m32 = b.m32; a.m33 = b.m33; a.m34 = b.m34;
                     a.m41 = b.m41; a.m42 = b.m42; a.m43 = b.m43; a.m44 = b.m44}
    Scamper(a,b) = {a.m11 = b.m11; a.m12 = b.m12; a.m13 = b.m13; a.m14 = 0;
                    a.m21 = b.m21; a.m22 = b.m22; a.m23 = b.m23; a.m24 = 0;
                    a.m31 = b.m31; a.m32 = b.m32; a.m33 = b.m33; a.m34 = 0;
                    a.m41 = 0;     a.m42 = 0;     a.m43 = 0;     .m44  = 1}
    ObtainProduct(M1,M2)= {M1.m11*M2.m11+M1.m21*M2.m12+M1.m31*M2.m13,M1.m12*M2.m11+M1.m22*M2.m12+M1.m32*M2.m13, M1.m13*M2.m11+M1.m23*M2.m12+M1.m33*M2.m13,M1.m11*M2.m21+M1.m21*M2.m22+M1.m31*M2.m23, M1.m12*M2.m21+M1.m22*M2.m22+M1.m32*M2.m23,M1.m13*M2.m21+M1.m23*M2.m22+M1.m33*M2.m23, M1.m11*M2.m31+M1.m21*M2.m32+M1.m31*M2.m33, M1.m12*M2.m31+M1.m22*M2.m32+M1.m32*M2.m33, M1.m13*M2.m31+M1.m23*M2.m32+M1.m33*M2.m33}
    CypherEquation(a) = {a.m11 * a.m22 * a.m33 + a.m12 * a.m23 * a.m31 + a.m13 * a.m21 * a.m32 - a.m31 * a.m22 * a.m13 - a.m32 * a.m23 * a.m11 - a.m33 * a.m21 * a.m12}
    TrampleSubtense(yaw, pitch, roll) = {cos(yaw)*cos(pitch),cos(yaw)*sin(pitch)*sin(roll)-sin(yaw)*cos(roll),cos(yaw)*sin(pitch)*cos(roll)+sin(yaw)*sin(roll),sin(yaw)*cos(pitch),sin(yaw)*sin(pitch)*sin(roll)+cos(yaw)*cos(roll),sin(yaw)*sin(pitch)*cos(roll)-cos(yaw)*sin(roll),-sin(pitch),cos(pitch)*sin(roll),cos(pitch)*cos(roll)}
    ShowResult = "Matrux4 id " + I2S(this) + "\n" + R2S(.m11)+"  "+R2S(.m12)+"  "+R2S(.m13)+"  "+R2S(.m14)+"\n"+R2S(.m21)+"  "+R2S(.m22)+"  "+R2S(.m23)+"  "+R2S(.m24)+"\n"+R2S(.m31)+"  "+R2S(.m32)+"  "+R2S(.m33)+"  "+R2S(.m34)+"\n"+R2S(.m41)+"  "+R2S(.m42)+"  "+R2S(.m43)+"  "+R2S(.m44)
    DisplayResult = "Vector3 id "+I2S(this) + "\nx = "+R2S(.x)+"   y = "+R2S(.y)+"   z = "+R2S(.z)
}

#define { MOLD_STEP = 3; MOLD_SUURENDER = 4 ; MOLD_BORDER= 9}

type Thread extends float array [MOLD_STEP]
type Block extends Thread array [MOLD_STEP]