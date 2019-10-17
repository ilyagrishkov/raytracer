#ifndef __FRUSTUM__
#define __FRUSTUM__

#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <camera.hpp>

using namespace std;
using namespace Eigen;

namespace Tucano
{
	template< typename Scalar, int Dim >
	ostream& operator<<( ostream& out, const Hyperplane< Scalar, Dim >& plane )
	{
		out << "normal:" << plane.normal() << endl << ", offset: " << plane.offset();
		
		return out;
	}
	
	template< typename Scalar, int Dim >
	ostream& operator<<( ostream& out, const AlignedBox< Scalar, Dim >& box )
	{
		out << "min:" << box.min() << endl << ", max: " << box.max() << endl << ", size: " << box.sizes();
		
		return out;
	}
	
	/** Camera's frustum. Provide culling of axis-aligned boxes. The camera frustum planes will be in model
	 * coordinates. To get the planes in world coordinates, pass a view-projection matrix instead of a
	 * model-view-projection */
	class Frustum
	{
		using Plane = Hyperplane< float, 3 >;
		using Box = AlignedBox< float, 3 >;
	
	public:
		Frustum( const Camera& camera )
		: Frustum( camera.getProjectionMatrix() * camera.getViewMatrix().matrix() )
		{}
		
		/** @param mvp is the model-view-projection matrix. */
		Frustum( const Matrix4f& mvp )
		: m_viewProj( mvp )
		{
			for( int i = 0; i < 6; ++i )
			{
				m_planes.push_back( new Plane() );
			}
			
			extractPlanes( m_viewProj, true );
		}
		
		~Frustum()
		{
			for( Plane* plane : m_planes )
			{
				delete plane;
			}
		}
		
		void update( const Camera& camera )
		{
			m_viewProj = camera.getProjectionMatrix() * camera.getViewMatrix().matrix();
			update( m_viewProj );
		}
		
		/** Updates the frustrum after changing camera.
		 *	@param mvp is the model-view-projection matrix. */
		void update( const Matrix4f& mvp )
		{
			extractPlanes( mvp, true );
		}
		
		/** Performs optimized view frustum culling as explained in paper Optimized View Frustum Culling Algorithms for
		 * Bounding Boxes. Available in: @link http://www.cse.chalmers.se/~uffe/vfc_bbox.pdf. Differently from the algorithm
		 * there, we don't distinguish intersection and outside cases.
		 * @returns true if the box is outside the frustum, false otherwise. */
		bool isCullable( const Box& box ) const;
		
		friend ostream& operator<<( ostream& out, const Frustum& f )
		{
			cout << "Frustum planes:" << endl << endl;
			for( int i = 0; i < 6; ++i )
			{
				switch( i )
				{
					case 0: cout << "Left: "; break;
					case 1: cout << "Right: "; break;
					case 2: cout << "Top: "; break;
					case 3: cout << "Bottom: "; break;
					case 4: cout << "Near: "; break;
					case 5: cout << "Far: "; break;
				}
				cout << *f.m_planes[ i ] << endl << endl;
			}
			cout << "End of frustum planes." << endl;
			
			return out;
		}
		
		const Matrix4f& viewProj() const { return m_viewProj; }
		
	private:
		/** Algorithm for extraction of the 6 frustum planes from the model-view-projection matrix as explained in paper
		 * Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix. Available in
		 * @link gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf .
		 * @param normalize indicates that the final plane equation should be normalized. */
		void extractPlanes( const Matrix4f& mvp, const bool& normalize )
		{
			// It seems that the d plane coefficient has wrong sign in the original algorithm, so all d coefficients are being
			// multiplied by -1 in this implementation.
			
			// Left clipping plane
			Vector4f& leftCoeffs = m_planes[ 0 ]->coeffs();
			leftCoeffs[ 0 ] = -( mvp( 3, 0 ) + mvp( 0, 0 ) );
			leftCoeffs[ 1 ] = -( mvp( 3, 1 ) + mvp( 0, 1 ) );
			leftCoeffs[ 2 ] = -( mvp( 3, 2 ) + mvp( 0, 2 ) );
			leftCoeffs[ 3 ] = -( mvp( 3, 3 ) + mvp( 0, 3 ) );
			
			// Right clipping plane
			Vector4f& rightCoeffs = m_planes[ 1 ]->coeffs();
			rightCoeffs[ 0 ] = -( mvp( 3, 0 ) - mvp( 0, 0 ) );
			rightCoeffs[ 1 ] = -( mvp( 3, 1 ) - mvp( 0, 1 ) );
			rightCoeffs[ 2 ] = -( mvp( 3, 2 ) - mvp( 0, 2 ) );
			rightCoeffs[ 3 ] = -( mvp( 3, 3 ) - mvp( 0, 3 ) );
			
			// Top clipping plane
			Vector4f& topCoeffs = m_planes[ 2 ]->coeffs();
			topCoeffs[ 0 ] = -( mvp( 3, 0 ) - mvp( 1, 0 ) );
			topCoeffs[ 1 ] = -( mvp( 3, 1 ) - mvp( 1, 1 ) );
			topCoeffs[ 2 ] = -( mvp( 3, 2 ) - mvp( 1, 2 ) );
			topCoeffs[ 3 ] = -( mvp( 3, 3 ) - mvp( 1, 3 ) );
			
			// Bottom clipping plane
			Vector4f& botCoeffs = m_planes[ 3 ]->coeffs();
			botCoeffs[ 0 ] = -( mvp( 3, 0 ) + mvp( 1, 0 ) );
			botCoeffs[ 1 ] = -( mvp( 3, 1 ) + mvp( 1, 1 ) );
			botCoeffs[ 2 ] = -( mvp( 3, 2 ) + mvp( 1, 2 ) );
			botCoeffs[ 3 ] = -( mvp( 3, 3 ) + mvp( 1, 3 ) );
			
			// Near clipping plane
			Vector4f& nearCoeffs = m_planes[ 4 ]->coeffs();
			nearCoeffs[ 0 ] = -( mvp( 3, 0 ) + mvp( 2, 0 ) );
			nearCoeffs[ 1 ] = -( mvp( 3, 1 ) + mvp( 2, 1 ) );
			nearCoeffs[ 2 ] = -( mvp( 3, 2 ) + mvp( 2, 2 ) );
			nearCoeffs[ 3 ] = -( mvp( 3, 3 ) + mvp( 2, 3 ) );
			
			// Far clipping plane
			Vector4f& farCoeffs = m_planes[ 5 ]->coeffs();
			farCoeffs[ 0 ] = -( mvp( 3, 0 ) - mvp( 2, 0 ) );
			farCoeffs[ 1 ] = -( mvp( 3, 1 ) - mvp( 2, 1 ) );
			farCoeffs[ 2 ] = -( mvp( 3, 2 ) - mvp( 2, 2 ) );
			farCoeffs[ 3 ] = -( mvp( 3, 3 ) - mvp( 2, 3 ) );
			
			if ( normalize )
			{
				for( Plane* plane : m_planes )
				{
					plane->normalize();
				}
			}
		}
		
		/** Frustum planes. */
		vector< Plane* > m_planes;
		
		/** The view projection matrix that generated the planes. Just for fast reference. */
		Matrix4f m_viewProj;
	};
	
	inline bool Frustum::isCullable( const Box& box ) const
	{
		//cout << "==== Starting culling ====" << endl << "Box: " << box << endl;
		
		Vector3f boxSizes = box.sizes();
		Vector3f boxMin = box.min();
		Vector3f boxMax = box.max();
		
		// Performs frustum plane dilatation, finds n only, since p is necessary just for detecting intersections points,
		// and verifies if n is inside or outside the frustum.
		for( int i = 0; i < 6; ++i )
		{
			Plane* plane = m_planes[ i ];
			Vector3f normal = plane->normal();
			float dilatation = abs( boxSizes.dot( normal ) );
			float offset = plane->offset() - dilatation;
			Plane dilatatedPlane( normal, offset );
			
			//cout << endl << "Dilatation:" << dilatation << endl << "Dilatated plane:" << dilatatedPlane << endl;
			
			Vector3f n;
			
			n[ 0 ] = ( normal[ 0 ] < 0 ) ? boxMax[ 0 ] : boxMin[ 0 ];
			n[ 1 ] = ( normal[ 1 ] < 0 ) ? boxMax[ 1 ] : boxMin[ 1 ];
			n[ 2 ] = ( normal[ 2 ] < 0 ) ? boxMax[ 2 ] : boxMin[ 2 ];
			
			float signedDist = dilatatedPlane.signedDistance( n );
			//cout << "n point: " << n << endl << "signedDist:" << signedDist << endl;
			
			if( signedDist > 0 )
			{
				/*string planeName;
				switch( i )
				{
					case 0: planeName = "left"; break;
					case 1: planeName = "right"; break;
					case 2: planeName = "top"; break;
					case 3: planeName = "bot"; break;
					case 4: planeName = "near"; break;
					case 5: planeName = "far"; break;
				}
				cout << "Outside " << planeName << " plane." << endl;*/
				return true;
			}
		}
		return false;
	}
}

#endif
