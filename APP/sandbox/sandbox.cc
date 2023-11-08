#include "sandbox.h"

#include <Albedo.Core.Log>
#include <Albedo.Editor>

#include <sstream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> Mesh;

#include "runtime/camera/camera.h"

namespace Albedo{
namespace APP
{
	
	void Sandbox()
	{
		Mesh mesh;

		static bool init = false;
		if (!init)
		{
			init = true;
		}
		
	}

}} // namespace Albedo::APP