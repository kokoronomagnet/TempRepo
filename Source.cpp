#include <cmath>
#include <cfloat>
#include <limits>
namespace SchreinerA
{
	struct vec3
	{
		double x , y , z;
		vec3 operator+( vec3 const &a ) const
		{
			return{ x + a.x , y + a.y , z + a.z };
		}
		vec3 operator-( vec3 const &a ) const
		{
			return{ x - a.x , y - a.y , z - a.z };
		}
		vec3 operator-() const
		{
			return{ -x , -y , -z };
		}
		vec3 operator*( double const &a ) const
		{
			return{ x * a , y * a , z * a };
		}
		double operator*( vec3 const &a ) const
		{
			return x * a.x + y * a.y + z * a.z;
		}
		vec3 operator/( double const &a ) const
		{
			return{ x / a , y / a , z / a };
		}
		vec3 norm() const
		{
			return this->operator/( mod() );
		}
		double mod2() const
		{
			return x * x + y * y + z * z;
		}
		double mod() const
		{
			return sqrt( mod2() );
		}
		vec3 operator^( vec3 const &b ) const
		{
			return
			{
				y * b.z - b.y * z ,
				b.x * z - x * b.z ,
				x * b.y - b.x * y
			};
		}
		bool isFinite() const
		{
			return isfinite( x ) && isfinite( y ) && isfinite( z ) &&
				!isnan( x ) && !isnan( y ) && !isnan( z )
				;
		}
		bool operator==( vec3 const &a ) const
		{
			return ( *this - a ).mod2() < std::numeric_limits< double >::epsilon();
		}
	};
	struct plane
	{
		vec3 o , n;
		bool contains( vec3 const &p ) const
		{
			return abs( ( p - o ) * n ) < std::numeric_limits< double >::epsilon();
		}
		vec3 project( vec3 const &p ) const
		{
			return p - n * ( n * ( p - o ) );
		}
		bool intersects( vec3 const &a , vec3 const &b ) const
		{
			if( a == o || b == o )
			{
				return true;
			}
			vec3 v = ( a - b ).norm();
			double prj = n * ( a - o );
			double dot = v * n;
			if( abs( dot ) > std::numeric_limits< double >::epsilon() )
			{
				double t = prj / dot;
				return t >= 0 && t <= ( a - b ).mod();
			} else
			{
				return abs( prj ) < std::numeric_limits< double >::epsilon();
			}
		}
		vec3 project( vec3 const &a , vec3 const &b ) const
		{
			vec3 v = ( b - a ).norm();
			double prj = -n * ( a - o );
			double dot = v * n;
			if( abs( dot ) > std::numeric_limits< double >::epsilon() )
			{
				double t = prj / dot;
				return a + v * t;
			} else
			{
				return a;
			}
		}
	};
	struct triangle
	{
		vec3 a , b , c;
		double area() const
		{
			return ( ( b - a ) ^ ( c - a ) ).mod();
		}
		bool contains( vec3 const &p ) const
		{
			return abs(
				triangle{ a , b , p }.area() +
				triangle{ b , c , p }.area() +
				triangle{ c , a , p }.area() -
				area() ) < std::numeric_limits< double >::epsilon();
		}
	};
	struct edge
	{
		vec3 o , e;
		bool intersects( vec3 const &a ) const
		{
			return abs(
				( a - e ).mod() +
				( a - o ).mod() -
				( o - e ).mod() ) < std::numeric_limits< double >::epsilon();
		}
		bool intersects( vec3 const &a , vec3 const &b ) const//no degenerate cases
		{
			if(
				a == o ||
				a == e ||
				b == o ||
				b == e
				)
			{
				return true;
			}
			plane p{ a , ( ( b - a ) ^ ( o - a ) ).norm() };
			if( p.contains( e ) )
			{
				vec3 v0 = ( e - o ).norm();
				vec3 v1 = ( b - a ).norm();
				double det = v0.x * v1.y - v1.x * v0.y;
				vec3 dp = a - o;
				if( abs( det ) > std::numeric_limits< double >::epsilon() )
				{
					double t0 = ( v1.y * dp.x - v1.x * dp.y ) / det;
					double t1 = -( -v0.y * dp.x + v0.x * dp.y ) / det;
					return
						t0 >= 0.0 && t0 <= ( e - o ).mod() &&
						t1 >= 0.0 && t1 <= ( a - b ).mod()
						;
				} else
				{
					det = v0.y * v1.z - v0.z * v1.y;
					double t0 = ( v1.z * dp.y - v1.y * dp.z ) / det;
					double t1 = -( -v0.z * dp.y + v0.y * dp.z ) / det;
					return
						t0 >= 0.0 && t0 <= ( e - o ).mod() &&
						t1 >= 0.0 && t1 <= ( a - b ).mod()
						;
				}
			} else
			{
				return false;
			}
		}
		bool intersects( vec3 const &a , vec3 const &b , vec3 const &c ) const
		{
			if(
				!e.isFinite() ||
				!o.isFinite() ||
				!a.isFinite() ||
				!b.isFinite() ||
				!c.isFinite()
				)
			{
				return false;
			}
			//degenerate cases
			if( o == e )//o->e is point
			{
				if( c == a )//triangle is line a->b
				{
					if( b == a )//a->b is point
					{
						return a == o;
					} else
					{
						return edge{ a , b }.intersects( o );
					}
				} else
				{
					if( b == c )//triangle is line a->b
					{
						return edge{ a , b }.intersects( o );
					} else if( a == b )//triangle is line a->c
					{
						return edge{ a , c }.intersects( o );
					} else
					{
						return triangle{ a , b , c }.contains( o );
					}
				}
			}
			if( c == a )//triangle is line a->b
			{
				if( b == a )//a->b is point
				{
					return intersects( a );
				} else
				{
					return edge{ a , b }.intersects( o , e );
				}
			} else
			{
				if( b == c )//triangle is line a->b
				{
					return edge{ a , b }.intersects( o , e );
				} else if( a == b )//triangle is line a->c
				{
					return edge{ a , c }.intersects( o , e );
				} else
				{
					vec3 dir = ( e - o ).norm();
					plane p = { a , ( ( b - a ) ^ ( c - a ) ).norm() };
					if( p.intersects( o , e ) )
					{
						vec3 proj = p.project( o , e );
						return triangle{ a , b , c }.contains( proj );
					} else
					{
						return false;
					}
				}
			}
		}
	};
	bool intersects( double *a , double *b )
	{
		vec3 *arr0 = ( vec3* )a;
		vec3 *arr1 = ( vec3* )b;
		for( int i = 0; i < 3; i++ )
		{
			if( !arr0[ i ].isFinite() )
			{
				return false;
			}
			if( !arr1[ i ].isFinite() )
			{
				return false;
			}
		}
		for( int i = 0; i < 3; i++ )
		{
			if( edge{ arr0[ i ] , arr0[ ( i + 1 ) % 3 ] }.intersects( arr1[ 0 ] , arr1[ 1 ] , arr1[ 2 ] ) )
			{
				return true;
			}
		}
		for( int i = 0; i < 3; i++ )
		{
			if( edge{ arr1[ i ] , arr1[ ( i + 1 ) % 3 ] }.intersects( arr0[ 0 ] , arr0[ 1 ] , arr0[ 2 ] ) )
			{
				return true;
			}
		}
		return false;
	}
}
#include <iostream>
int main( int argc , char **argv )
{
	using namespace SchreinerA;
	double triangles[] =
	{
		//0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
		//0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , nan( "" ) ,
		//0.0 , 0.0 , 0.0 , std::numeric_limits<double>::infinity() , 0.0 , 0.0 , 0.0 , 0.0 , nan( "" ) ,
		//0.0 , 0.0 , 0.0 , 1.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0 ,
		//0.5 , 0.0 , 0.0 , 0.0 , 0.5 , 0.0 , 1.0 , 1.0 , 0.0 ,
		//1.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0 , 1.0 , 1.0 , 1.0 ,
		//1.0 , 0.0 , 1.0 , 0.0 , 1.0 , 1.0 , 0.0 , 0.0 , -1.0 ,
		//0.0 , 0.0 , 1.0 , 0.0 , 1.0 , -1.0 , 1.0 , 0.0 , -1.0 ,
		0.0 , 0.0 , 0.0 , 1.0 , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 ,
		0.5 , 0.0 , 0.0 , 0.5 , -1.0 , 1.0 , 0.5 , -1.0 , 0.0 ,
	};
	for( int i = 0; i < sizeof( triangles ) / 9 / 8; i++ )
	{
		for( int j = 0; j < sizeof( triangles ) / 9 / 8; j++ )
		{
			std::cout << intersects( triangles + i * 9 , triangles + j * 9 ) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << edge{ { 0.0 , 0.0 , 0.0 } , { 2.0 , 0.0 , 0.0 } }.intersects(
			{ 1.0 , 1.0 , 0.0 } , { 1.0 , 0.01 , 0.0 }
		) << "=0" << std::endl;
	std::cout << edge{ { 0.0 , 0.0 , 0.0 } ,{ 2.0 , 0.0 , 0.0 } }.intersects(
	{ 1.0 , 1.0 , 0.0 } , { 1.0 , 0.0 , 0.0 }
	) << "=1" << std::endl;
	std::cout << edge{ { 0.0 , 0.0 , 0.0 } ,{ 0.0 , 2.0 , 0.0 } }.intersects(
	{ 0.0 , 1.0 , 0.0 } , { 0.0 , 1.0 , 1.0 }
	) << "=1" << std::endl;
	std::cout << edge{ { 0.0 , 0.0 , 0.0 } ,{ 0.0 , 2.0 , 0.0 } }.intersects(
	{ 0.0 , 1.0 , -1.0 } , { 0.0 , 1.0 , 1.0 }
	) << "=1" << std::endl;
	std::cout << edge{ { 0.0 , 0.0 , 0.0 } ,{ 0.0 , 2.0 , 0.0 } }.intersects(
	{ 0.0 , 2.0 , -1.0 } , { 0.0 , 1.0 , 1.0 }
	) << "=1" << std::endl;
	std::cout << edge{ { 0.0 , 0.0 , 0.0 } ,{ 0.0 , 2.0 , 0.0 } }.intersects(
	{ 0.0 , 2.0 , -1.0 } , { 0.0 , 1.0 , -1.0 }
	) << "=0" << std::endl;
	getchar();
	return 0;
}