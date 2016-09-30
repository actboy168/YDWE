//*========================
//* Import *
//*=========
  #include "cj_types.j"   
  #include "cj_array.j"
  #include "cj_branch.j"
  #include "cj_function.j"
  #include "cj_pointer.j"
  #include "cj_math.j"
//*========================

//*===============================
//* Pattern *
//*==========
  module Substrate {

      static thistype Vain, Basic;
  
  }
//*===============================

//*======================================================
//* Magnificent Delimit * 
//*======================
  //! textmacro repository takes sort, modus, attend, run
      static $sort$ $modus$($attend$) {
          $sort$ this = $sort$.create();
          $run$;
          return this;
      }
  //! endtextmacro
//*======================================================

library Phalanx initializer Main requires optional Indicator {

    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    //           - Start Content Of The Major Structure Named Matrix3 -             //
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    
    struct Matrix3 {

        //-------------------------//
        //  Struct Variable Member //
        //-------------------------//
        
        implement Substrate;
        
        Block block, subdue;
        Pointer pointer;
        
        float accompany [MOLD_BORDER], proxy [MOLD_SUURENDER], MemberList;
        
        //-------------------------//
        //   Struct Method Member  //
        //-------------------------//
        
        static Matrix3 create(ArgumentList) {
            Matrix3 front = Matrix3.allocate(); 
            int i;
            front.block = Block.create()
            InitialParent(front.block[i ++] = Thread.create());
            InitialChild(front.subdue[i ++] = Thread.create());
            Range(front.block, MemberList);
            ScanData(front);
            return front;
        }
        
        void Traversal() {
            int i = 0, j;
            while(i < MOLD_STEP) {
                j = 0; while(j < MOLD_STEP) {
                    /*debug*/ printf("block[" + I2S(i) + "][" + I2S(j) + "] = " + R2S(.block[i][j ++]));
                } i ++;
            }
        }
        
        Matrix3 Reset(ArgumentList) {
            Range(.block, MemberList);
            ScanData(this);
            return this;
        }
        
        void Transpose() {
            int i = 0, j;
            float index;
            while(i < MOLD_STEP) {
                j = 0; while(j < i) {
                    index = .block[i][j];   
                    .block[i][j] = .block[j][i];   
                    .block[j][i] = index; 
                    j ++;
                } i ++;
            }
        }
        
        //! textmacro retroflexion takes view, jumbo
        Matrix3 ReviseRotation$view$(float cape) {
            return .Reset($jumbo$);
        }
        //! endtextmacro
        
        //! runtextmacro retroflexion("X", "1, 0, 0, 0, cos(cape), -sin(cape), 0, sin(cape), cos(cape)")
        //! runtextmacro retroflexion("Y", "cos(cape), 0, sin(cape), 0, 1, 0, -sin(cape), 0, cos(cape)")
        //! runtextmacro retroflexion("Z", "cos(cape), -sin(cape), 0, sin(cape), cos(cape), 0, 0, 0, 1")
        
        Matrix3 WhirlDiverge(float yaw, float pitch, float roll) {
            return .Reset(TrampleSubtense(yaw, pitch, roll));
        }
        
        void BlurMultiply(Matrix3 Napea, Matrix3 Napeb) {
            int i = 0, j, k;
            while(i < MOLD_STEP) {
                j = 0; while(j < MOLD_STEP) {
                    .block[i][j] = Napea.block[i][0] * Napeb.block[0][j];
                    k = 0; while(k < MOLD_STEP) {
                        .block[i][j] += Napea.block[i][k] * Napeb.block[k][j]; 
                        k ++;
                    } j ++;
                } i ++;
            }
        }

        Matrix3 VerticalMultiply(Matrix3 Napea, Matrix3 Napeb) {  
            return .Reset(ObtainProduct(Napea, Napeb));
        }
        
        Matrix3 DiagonalScaling(float x, float y, float z) {
            return .Reset(x, 0, 0, 0, y, 0, 0, 0, z)
        }
        
        //! textmacro operation takes rule, mark
        void Figure$rule$(float value, Matrix3 Nape) {
            int i = 0, j;
            while(i < MOLD_STEP) {
                j = 0; while(j < MOLD_STEP) {
                    .block[i][j] = Nape.block[i][j] $mark$ value;
                    j ++;
                } i ++;
            }
        }
        //! endtextmacro
        
        //! runtextmacro operation("Multiply", "*")
        //! runtextmacro operation("Division", "/")
        
        void BlurSum(Matrix3 Napea, Matrix3 Napeb) {
            int i = 0, j;
            while(i < MOLD_STEP) {
                j = 0; while(j < MOLD_STEP) {
                    .block[i][j] = Napea.block[i][j] + Napeb.block[i][j];
                    j ++;
                } i ++;
            } 
        }

        float GetDeterminant() {
            return CypherEquation(this);
        }

        float GetExorbitancy() {
            return SubEduct(.subdue);
        }

        void SurplusChild(int m, int n) {
            int i = 0, j, k = 0, row = 0, col;
            while (i < 3) {
                if (i != m) { 
                    col = 0;
                    j = 0; 
                    while(j < MOLD_STEP) {
                        if (j != n) {
                            .subdue[row][col] = .block[i][j];
                            .proxy[k] = .subdue[row][col]; 
                            col ++; k ++;
                        } j++;
                    } 
                }
                row ++; i ++;
            }
        }

        void InverseSlay() {
            int i = 0, j, k = 0;
            float value = .GetDeterminant();
            if (value != 0.) {
                while(i < 3) {
                    j = 0; while(j < 3) {
                        .SurplusChild(i, j); 
                        .Reshape(.proxy, subdue);
                        .accompany[k] = Pow(-1, k) * .GetExorbitancy()
                        j ++; k ++;
                    } i ++;
                }
                .Reset(ElementList(.accompany));
                .FigureMultiply((1 / value), this);
                .Reset(RevoltList(.block));
            }
            else {
                debug printf("There is no inverse matrix!");
            }
        }

        bool operator < (Matrix3 other) {
            return .GetDeterminant() < other.GetDeterminant();
        }
        
    }

    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    //                                - The End -                                   //
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#

    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    //                            - Matrix3 Extends -                               //
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    
    struct Matrix4 {

        //-------------------------//
        //  Struct Variable Member //
        //-------------------------//
        
        implement Substrate;
        
        float ShareList;
        
        //-------------------------//
        //   Struct Method Member  //
        //-------------------------//
        
        //! runtextmacro repository("Matrix4", "Create", "", "SkimData(this, 0)")
        //! runtextmacro repository("Matrix4", "NewSolid", "ParameterList", "SkimData(this)")
        //! runtextmacro repository("Matrix4", "NewReplace", "Matrix4 that", "ScanData(this, that)")
        //! runtextmacro repository("Matrix4", "VariationSeed", "Matrix4 that", "Scampe(this, that)")
        
        Matrix4 SetValue(ParameterList) {
            SkimData(this);
            return this;
        }
        
        string ToString() {
            return ShowResult;
        }
    
    }
    
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    //                                - The End -                                   //
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    //                            - Vector3 Construct -                             //
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    
    struct Vector3 {
    
        //-------------------------//
        //  Struct Variable Member //
        //-------------------------//
        
        static Vector3 Origin, VolumeX, VolumeY, VolumeZ;
        
        float x, y, z;
        
        //-------------------------//
        //   Struct Method Member  //
        //-------------------------//
        
        //! runtextmacro repository("Vector3", "Create", "", "PerkData(this, 0)")
        //! runtextmacro repository("Vector3", "NewEqual", "AgencyList", "PerkData(this)")
        //! runtextmacro repository("Vector3", "TurnLeach", "Vector3 that", "PercolateData(this, that)")
   
        Vector3 SetValue(AgencyList) {
            PerkData(this);
            return this;
        }
        
        float GetLength() {
            return sqr(.x * .x + .y * .y + .z * .z);
        }

        string ToString() {
            return DisplayResult;
        }
 
    }
    
    //----------------------------//
    //   Related Function Member  //
    //----------------------------//
    
    //! textmacro arithmetic takes name, count, mark, vecx, vecy, vecz
        Vector3 VecSuffix$name$(Vector3 Export, Vector3 V1, $count$) {
        Export.x = V1.x $mark$ $vecx$;
        Export.y = V1.y $mark$ $vecy$;
        Export.z = V1.z $mark$ $vecz$;
        return Export;
    }
    //! endtextmacro
        
    //! runtextmacro arithmetic("Add", "Vector3 V2", "+", "V2.x", "V2.y", "V2.z")
    //! runtextmacro arithmetic("Subtract", "Vector3 V2", "-", "V2.x", "V2.y", "V2.z")
    //! runtextmacro arithmetic("Scale", "float r", "*", "r", "r", "r")
    //! runtextmacro arithmetic("Division", "float r", "/", "r", "r", "r")
        
    Vector3 VecNormalize(Vector3 Export, Vector3 V) {
        float len = V.GetLength();
        Export.x = V.x / len;
        Export.y = V.y / len;
        Export.z = V.z / len;
        return Export;
    }
    
    float VecDot(Vector3 V1, Vector3 V2) {
        return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;
    }
    
    Vector3 VecCross(Vector3 Export, Vector3 V1, Vector3 V2) {
        float y = V1.z * V2.x - V1.x * V2.z, \
              z = V1.x * V2.y - V1.y * V2.x;
        Export.x = V1.y * V2.z - V1.z * V2.y;
        Export.y = y;
        Export.z = z;
        return Export;
    }
    
    Vector3 VecTransformWithMatrix3(Vector3 Export, Vector3 V, Matrix3 M) {
        float y = V.x * M.m12 + V.y * M.m22 + V.z * M.m32, \
              z = V.x * M.m13 + V.y * M.m23 + V.z * M.m33;
        Export.x = V.x * M.m11 + V.y * M.m21 + V.z * M.m31;
        Export.y = y;
        Export.z = z;
        return Export;
    }
    
    Vector3 VecTransformWithMatrix4(Vector3 Export, Vector3 V, Matrix4 M) {
        float y = V.x * M.m12 + V.y * M.m22 + V.z * M.m32 + M.m42, \
              z = V.x * M.m13 + V.y * M.m23 + V.z * M.m33 + M.m43, \
              t = V.x * M.m14 + V.y * M.m24 + V.z * M.m34 + M.m44;
        Export.x = (V.x * M.m11 + V.y * M.m21 + V.z * M.m31 + M.m41) / t;
        Export.y = y / t;
        Export.z = z / t;
        return Export;
    }
    
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    //                                - The End -                                   //
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    //                                - Initializer -                               //
    //=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#
    
    void Transport(int i) {
        Matrix3 Nape = Trend_Guide[i];
        Nape.Traversal();
    }
    
    private void Main() {
        Matrix3.Vain = EmptyMatrix3;
        Matrix3.Basic = SufferMatrix3;  
        Matrix4.Vain = EmptyMatrix4;
        Matrix4.Basic = SufferMatrix4; 
    }
    
}