////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Class: Vector. For Vector calculation, based on work by Hill.
// Author: Arthur (Yikun) Liu
// Email: yikliu@umail.iu.edu
// 
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
#ifndef _Vector3_H
#define _Vector3_H

#include <math.h>


class Vector3 {
	
public:
	
	// vars
	double x, y, z;
	
	// constructors
	Vector3() {}
	
	Vector3( double x1, double y1, double z1 ) {
		
		x = x1; y = y1; z = z1;
		
	}
	
	// vector ops
	void normalize() {
		
		double fTemp = 1 / Length();
		
		x *= fTemp;
		y *= fTemp;
		z *= fTemp;
		
	}

	Vector3 cross( const Vector3 & rhs ) {
		return( Vector3( y * rhs.z - z * rhs.y, 
		               - x * rhs.z - z * rhs.x,
					     x * rhs.y - y * rhs.x ) );


	}

	Vector3 leftMultiplyBy3DMatrix (const float *M){
		return (Vector3(
				M[0]*x + M[1]*y + M[2]*z,
				M[3]*x + M[4]*y + M[5]*z,
				M[6]*x + M[7]*y + M[8]*z
			));
	}

	double dot( const Vector3 & rhs ) const {
		
		// dot product
		return( (x * rhs.x) + (y * rhs.y) + (z * rhs.z) );
		
	}

	
	inline double Length() const {
		
		return( sqrt( (x * x) + (y * y) + (z * z) ) );
		
	}
	
	//  operators
	Vector3 operator + ( const Vector3 & rhs ) const {
		
		return( Vector3( x + rhs.x, y + rhs.y, z + rhs.z ) );
		
	}
	
	Vector3 operator - ( const Vector3 & rhs ) const {
		
		return( Vector3( x - rhs.x, y - rhs.y, z - rhs.z ) );
		
	}
	
	Vector3 operator / ( double k ) const {
		
		return( Vector3( x / k, y / k, z / k ) );
		
	}
	
	Vector3 operator * ( const double k ) const {

		// scalar multiply
		return( Vector3( x * k, y * k, z * k ) );

	}

	Vector3 operator *= ( const double k ) {

		x *= k;
		y *= k;
		z *= k;

		return( *this );

	}

	Vector3 operator += ( const Vector3 & rhs ) {

		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return( *this );

	}
	
	double & operator [] ( int n ) {
		
		// access like an array
		switch( n ) {
			
		case 0: return( x );
		case 1: return( y );
		case 2: return( z );
		default: /* bug out */;
			
		}
	}

	
	
}; // end class


#endif