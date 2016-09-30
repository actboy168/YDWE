library Vector

//*****************************************************************
//*  VECTOR LIBRARY
//*
//*  written by: Anitarf
//*
//*  The library contains a struct named vector, which represents a
//*  point in 3D space. As such, it has three real members, one for
//*  each coordinate: x, y, z. It also has the following methods:
//*
//*        static method create takes real x, real y, real z returns vector
//*  Creates a new vector with the given coordinates.
//*
//*        method getLength takes nothing returns real
//*  Returns the length of the vector it is called on.
//*
//*        static method sum takes vector augend, vector addend returns vector
//*  Returns the sum of two vectors as a new vector.
//*
//*        method add takes vector addend returns nothing
//*  Similar to sum, except that it doesn't create a new vector for the result,
//*  but changes the vector it is called on by adding the "added" to it.
//*
//*        static method difference takes vector minuend, vector subtrahend returns vector
//*  Returns the difference between two vectors as a new vector.
//*
//*        method subtract takes vector subtrahend returns nothing
//*  Similar to difference, except that it doesn't create a new vector for the result,
//*  but changes the vector it is called on by subtracting the "subtrahend" from it.
//*
//*        method scale takes real factor returns nothing
//*  Scales the vector it is called on by the given factor.
//*
//*        method setLength takes real length returns nothing
//*  Sets the length of the vector it is called on to the given value, maintaining it's orientation.
//*
//*        static method dotProduct takes vector a, vector b returns real
//*  Calculates the dot product (also called scalar product) of two vectors.
//*
//*        static method crossProduct takes vector a, vector b returns vector
//*  Calculates the cross product (also called vector product) of two vectors
//*  and returns it as a new vector.
//*
//*        static method tripleProductScalar takes vector a, vector b, vector c returns real
//*  Calculates the triple scalar product of three vectors.
//*
//*        static method tripleProductVector takes vector a, vector b, vector c returns vector
//*  Calculates the triple vector product of three vectors and returns it as a new vector.
//*
//*
//*        static method projectionVector takes vector projected, vector direction returns vector
//*  Calculates the projection of the vector "projected" onto the vector "direction"
//*  and returns it as a new vector.
//*  Returns null if the vector direction has a length of 0.
//*
//*        method projectVector takes vector direction returns nothing
//*  Projects the vector it is called on onto the vector "direction".
//*  Does nothing if the vector "direction" has a length of 0.
//*
//*        static method projectionPlane takes vector projected, vector normal returns vector
//*  Calculates the projection of the vector projected onto a plane defined by
//*  it's normal vector and returns it as a new vector.
//*  Returns null if the vector "normal" has a length of 0.
//*
//*        method projectPlane takes vector normal returns nothing
//*  Projects the vector it is called on onto a plane defined by it's normal vector.
//*  Does nothing if the vector "normal" has a length of 0.
//*
//*        static method getAngle takes vector a, vector b returns real
//*  Returns the angle between two vectors, in radians, returns a value between 0 and pi.
//*  Returns 0.0 if any of the vectors are 0 units long.
//*
//*        method rotate takes vector axis, real angle returns nothing
//*  Rotates the vector it is called on around the axis defined by the vector "axis"
//*  by the given angle, which should be input in radians.
//*  Does nothing if axis is 0 units long.
//*
//*
//*        static method createTerrainPoint takes real x, real y returns vector
//*  Creates a vector to the given terrain coordinate, taking it's z height into account.
//*
//*        method getTerrainPoint takes real x, real y returns nothing
//*  Sets the vector it is called on to the given terrain coordinate, taking it's z height into account.
//*
//*        static method createTerrainNormal takes real x, real y, real sampleRadius returns vector
//*  Creates the normal vector of the terrain at given coordinates. "sampleRadius" defines
//*  how far apart the reference points will be, if they are further apart, the result will
//*  be an impression of smoother terrain; normaly the value should be between 0 and 128.
//*
//*        method getTerrainNormal takes real x, real y, real sampleRadius returns nothing
//*  Sets the vector it is called on to the normal of the terrain at given coordinates.
//*
//*
//*        method isInCylinder takes vector cylinderOrigin, vector cylinderHeight, real cylinderRadius returns boolean
//*  Determines if a point is within a given cylinder. The cylinder's origin vector points
//*  to the center of one of the two paralel circular sides, and the height vector points
//*  from the origin point to the center of the other of the two paralel circular sides.
//*  Returns false if the point is not in the cylinder or if the vector cylinderHeight is 0 units long.
//*
//*        method isInCone takes vector coneOrigin, vector coneHeight, real coneRadius returns boolean
//*  Determines if a point is within a given cone. The cone's origin vector points to the
//*  center of the circular side, and the height vector points from the origin point to
//*  the tip of the cone.
//*  Returns false if the point is not in the cylinder or if the vector coneHeight is 0 units long.
//*
//*        method isInSphere takes vector sphereOrigin, real sphereRadius returns boolean
//*  Determines if a point is within a give sphere. The sphere's origin vector points to the
//*  center of the sphere.
//*  Returns false if the point is not in the sphere.
//*****************************************************************

    struct vector
        real x
        real y
        real z
        
        static method create takes real x, real y, real z returns vector
            local vector v = vector.allocate()
            set v.x=x
            set v.y=y
            set v.z=z
            return v
        endmethod
        
        method getLength takes nothing returns real
          return SquareRoot(.x*.x + .y*.y + .z*.z)
        endmethod
        
        static method sum takes vector augend, vector addend returns vector
            local vector v = vector.allocate()
            set v.x = augend.x+addend.x
            set v.y = augend.y+addend.y
            set v.z = augend.z+addend.z
            return v
        endmethod
        method add takes vector addend returns nothing
            set this.x=this.x+addend.x
            set this.y=this.y+addend.y
            set this.z=this.z+addend.z
        endmethod
        
        static method difference takes vector minuend, vector subtrahend returns vector
            local vector v = vector.allocate()
            set v.x = minuend.x-subtrahend.x
            set v.y = minuend.y-subtrahend.y
            set v.z = minuend.z-subtrahend.z
            return v
        endmethod
        method subtract takes vector subtrahend returns nothing
            set this.x=this.x-subtrahend.x
            set this.y=this.y-subtrahend.y
            set this.z=this.z-subtrahend.z
        endmethod
        
        method scale takes real factor returns nothing
            set this.x=this.x*factor
            set this.y=this.y*factor
            set this.z=this.z*factor
        endmethod
        
        method setLength takes real length returns nothing
            local real l = SquareRoot(.x*.x + .y*.y + .z*.z)
            if l == 0.0 then
                debug call BJDebugMsg("Attempted to set the length of a vector with no length!")
                return
            endif
            set l = length/l
            set this.x = this.x*l
            set this.y = this.y*l
            set this.z = this.z*l
        endmethod
        
        static method dotProduct takes vector a, vector b returns real
            return (a.x*b.x+a.y*b.y+a.z*b.z)
        endmethod
        
        static method crossProduct takes vector a, vector b returns vector
            local vector v = vector.allocate()
            set v.x = a.y*b.z - a.z*b.y
            set v.y = a.z*b.x - a.x*b.z
            set v.z = a.x*b.y - a.y*b.x
            return v
        endmethod

        static method tripleProductScalar takes vector a, vector b, vector c returns real
            return ((a.y*b.z - a.z*b.y)*c.x+(a.z*b.x - a.x*b.z)*c.y+(a.x*b.y - a.y*b.x)*c.z)
        endmethod

        static method tripleProductVector takes vector a, vector b, vector c returns vector
            local vector v = vector.allocate()
            local real n = a.x*c.x+a.y*c.y+a.z*c.z
            local real m = a.x*b.x+a.y*b.y+a.z*b.z
            set v.x = b.x*n-c.x*m
            set v.y = b.y*n-c.y*m
            set v.z = b.z*n-c.z*m
            return v
        endmethod

// ================================================================

        static method projectionVector takes vector projected, vector direction returns vector
            local vector v = vector.allocate()
            local real l = direction.x*direction.x+direction.y*direction.y+direction.z*direction.z
            if l == 0.0 then
                call v.destroy()
                debug call BJDebugMsg("Attempted to project onto a vector with no length!")
                return null
            endif
            set l = (projected.x*direction.x+projected.y*direction.y+projected.z*direction.z) / l
            set v.x = direction.x*l
            set v.y = direction.y*l
            set v.z = direction.z*l
            return v
        endmethod
        method projectVector takes vector direction returns nothing
            local real l = direction.x*direction.x+direction.y*direction.y+direction.z*direction.z
            if l == 0.0 then
                debug call BJDebugMsg("Attempted to project onto a vector with no length!")
                return
            endif
            set l = (this.x*direction.x+this.y*direction.y+this.z*direction.z) / l
            set this.x = direction.x*l
            set this.y = direction.y*l
            set this.z = direction.z*l
        endmethod

        static method projectionPlane takes vector projected, vector normal returns vector
            local vector v = vector.allocate()
            local real l = normal.x*normal.x+normal.y*normal.y+normal.z*normal.z
            if l == 0.0 then
                call v.destroy()
                debug call BJDebugMsg("Attempted to project onto an undefined plane!")
                return null
            endif
            set l = (projected.x*normal.x+projected.y*normal.y+projected.z*normal.z) / l
            set v.x = projected.x - normal.x*l
            set v.y = projected.y - normal.y*l
            set v.z = projected.z - normal.z*l
            return v
        endmethod
        method projectPlane takes vector normal returns nothing
            local real l = normal.x*normal.x+normal.y*normal.y+normal.z*normal.z
            if l == 0.0 then
                debug call BJDebugMsg("Attempted to project onto an undefined plane!")
                return
            endif
            set l = (this.x*normal.x+this.y*normal.y+this.z*normal.z) / l
            set this.x = this.x - normal.x*l
            set this.y = this.y - normal.y*l
            set this.z = this.z - normal.z*l
        endmethod

        static method getAngle takes vector a, vector b returns real
            local real l = SquareRoot(a.x*a.x + a.y*a.y + a.z*a.z)*SquareRoot(b.x*b.x + b.y*b.y + b.z*b.z)
            if l == 0 then
                debug call BJDebugMsg("Attempted to get angle between vectors with no length!")
                return 0.0
            endif
            return Acos((a.x*b.x+a.y*b.y+a.z*b.z)/l) //angle is returned in radians
        endmethod
        
        method rotate takes vector axis, real angle returns nothing //angle is taken in radians
            local real xx
            local real xy
            local real xz
            local real yx
            local real yy
            local real yz
            local real zx
            local real zy
            local real zz
            local real al = axis.x*axis.x+axis.y*axis.y+axis.z*axis.z //axis length^2
            local real f
            local real c = Cos(angle)
            local real s = Sin(angle)
            if al == 0.0 then
                debug call BJDebugMsg("Attempted to project onto a vector with no length!")
                return
            endif
            set f = (this.x*axis.x+this.y*axis.y+this.z*axis.z) / al
            set zx = axis.x*f
            set zy = axis.y*f
            set zz = axis.z*f //axis component of rotated vector
            set xx = this.x-zx
            set xy = this.y-zy
            set xz = this.z-zz //component of vector perpendicular to axis
            set al = SquareRoot(al)
            set yx = (axis.y*xz - axis.z*xy)/al
            set yy = (axis.z*xx - axis.x*xz)/al //y same length as x by using cross product and dividing with axis length
            set yz = (axis.x*xy - axis.y*xx)/al //x,y - coordinate system in which we rotate
            set this.x=xx*c+yx*s+zx
            set this.y=xy*c+yy*s+zy
            set this.z=xz*c+yz*s+zz
        endmethod
        
// ================================================================

        private static location loc = Location(0.0,0.0)

        static method createTerrainPoint takes real x, real y returns vector
            local vector v = vector.allocate()
            call MoveLocation(vector.loc,x,y)
            set v.x=x
            set v.y=y
            set v.z=GetLocationZ(loc)
            return v
        endmethod
        method getTerrainPoint takes real x, real y returns nothing
            call MoveLocation(vector.loc,x,y)
            set this.x=x
            set this.y=y
            set this.z=GetLocationZ(loc)
        endmethod

        static method createTerrainNormal takes real x, real y, real sampleRadius returns vector
            local vector v = vector.allocate()
            local real zx
            local real zy
            call MoveLocation(vector.loc, x-sampleRadius, y)
            set zx=GetLocationZ(vector.loc)
            call MoveLocation(vector.loc, x+sampleRadius, y)
            set zx=zx-GetLocationZ(vector.loc)
            call MoveLocation(vector.loc, x, y-sampleRadius)
            set zy=GetLocationZ(vector.loc)
            call MoveLocation(vector.loc, x, y+sampleRadius)
            set zy=zy-GetLocationZ(vector.loc)
            set sampleRadius=2*sampleRadius
            set v.x = zx*sampleRadius
            set v.y = zy*sampleRadius
            set v.z = sampleRadius*sampleRadius
            return v
        endmethod
        method getTerrainNormal takes real x, real y, real sampleRadius returns nothing
            local real zx
            local real zy
            call MoveLocation(vector.loc, x-sampleRadius, y)
            set zx=GetLocationZ(vector.loc)
            call MoveLocation(vector.loc, x+sampleRadius, y)
            set zx=zx-GetLocationZ(vector.loc)
            call MoveLocation(vector.loc, x, y-sampleRadius)
            set zy=GetLocationZ(vector.loc)
            call MoveLocation(vector.loc, x, y+sampleRadius)
            set zy=zy-GetLocationZ(vector.loc)
            set sampleRadius=2*sampleRadius
            set this.x = zx*sampleRadius
            set this.y = zy*sampleRadius
            set this.z = sampleRadius*sampleRadius
        endmethod

// ================================================================

        method isInCylinder takes vector cylinderOrigin, vector cylinderHeight, real cylinderRadius returns boolean
            local real l

            local real x = this.x-cylinderOrigin.x
            local real y = this.y-cylinderOrigin.y
            local real z = this.z-cylinderOrigin.z
            if x*cylinderHeight.x+y*cylinderHeight.y+z*cylinderHeight.z < 0.0 then //point below cylinder
                return false
            endif
            
            set x = x-cylinderHeight.x
            set y = y-cylinderHeight.y
            set z = z-cylinderHeight.z
            if x*cylinderHeight.x+y*cylinderHeight.y+z*cylinderHeight.z > 0.0 then //point above cylinder
                return false
            endif
            
            set l = cylinderHeight.x*cylinderHeight.x+cylinderHeight.y*cylinderHeight.y+cylinderHeight.z*cylinderHeight.z
            if l == 0.0 then
                debug call BJDebugMsg("Cylinder with no height!")
                return false
            endif
            set l = (x*cylinderHeight.x+y*cylinderHeight.y+z*cylinderHeight.z) / l
            set x = x - cylinderHeight.x*l
            set y = y - cylinderHeight.y*l
            set z = z - cylinderHeight.z*l
            if x*x+y*y+z*z > cylinderRadius*cylinderRadius then //point outside cylinder
                return false
            endif
            
            return true
        endmethod

        method isInCone takes vector coneOrigin, vector coneHeight, real coneRadius returns boolean
            local real l

            local real x = this.x-coneOrigin.x
            local real y = this.y-coneOrigin.y
            local real z = this.z-coneOrigin.z
            if x*coneHeight.x+y*coneHeight.y+z*coneHeight.z < 0.0 then //point below cone
                return false
            endif
            
            set l = coneHeight.x*coneHeight.x+coneHeight.y*coneHeight.y+coneHeight.z*coneHeight.z
            if l == 0.0 then
                debug call BJDebugMsg("cone with no height!")
                return false
            endif
            set l = (x*coneHeight.x+y*coneHeight.y+z*coneHeight.z) / l
            set x = x - coneHeight.x*l
            set y = y - coneHeight.y*l
            set z = z - coneHeight.z*l
            if SquareRoot(x*x+y*y+z*z) > coneRadius*(1.0-l) then //point outside cone
                return false
            endif
            
            return true
        endmethod

        method isInSphere takes vector sphereOrigin, real sphereRadius returns boolean
            if sphereRadius*sphereRadius < ((this.x-sphereOrigin.x)*(this.x-sphereOrigin.x)+(this.y-sphereOrigin.y)*(this.y-sphereOrigin.y)+(this.z-sphereOrigin.z)*(this.z-sphereOrigin.z)) then
                return false
            endif
            return true
        endmethod
    endstruct

endlibrary
library VectorLib requires Vector // For backwards compatibility.
endlibrary