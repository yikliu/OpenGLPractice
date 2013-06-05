#ifndef _MY3DVECTOR_H
#define _MY3DVECTOR_H

#include <cmath>
using namespace std;

class my3DVector {	
public:	
	GLdouble x, y, z;	
	my3DVector():x(0.0),y(0.0),z(0.0){}	
	my3DVector( GLdouble x1, GLdouble y1, GLdouble z1 ) {		
		x = x1; y = y1; z = z1;		
	}	
	
	my3DVector cross( const my3DVector & rhs ) {
		return( my3DVector( y * rhs.z - z * rhs.y, 
		               - x * rhs.z - z * rhs.x,
					     x * rhs.y - y * rhs.x ) );
	}

	GLdouble dot( const my3DVector & rhs ) const {			
		return( (x * rhs.x) + (y * rhs.y) + (z * rhs.z) );		
	}

	
	inline GLdouble Length() const {		
		return( sqrt( (x * x) + (y * y) + (z * z) ) );		
	}	
	
	my3DVector operator + ( const my3DVector & rhs ) const {		
		return( my3DVector( x + rhs.x, y + rhs.y, z + rhs.z ) );		
	}
	
	my3DVector operator - ( const my3DVector & rhs ) const {		
		return( my3DVector( x - rhs.x, y - rhs.y, z - rhs.z ) );		
	}
	
	my3DVector operator / ( GLdouble k ) const {		
		return( my3DVector( x / k, y / k, z / k ) );		
	}
	
	my3DVector operator * ( const GLdouble k ) const {		
		return( my3DVector( x * k, y * k, z * k ) );
	}

}; 


#endif