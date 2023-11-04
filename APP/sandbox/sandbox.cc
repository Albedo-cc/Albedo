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

namespace Albedo{
namespace APP
{
	
	void Sandbox()
	{
		Mesh mesh;

		// Add 4 vertices
		Mesh::Vertex_index v0 = mesh.add_vertex(Point_3(0,0,0));
		Mesh::Vertex_index v1 = mesh.add_vertex(Point_3(1,0,0));
		Mesh::Vertex_index v2 = mesh.add_vertex(Point_3(0,1,0));
		Mesh::Vertex_index v3 = mesh.add_vertex(Point_3(0,0,1));

		// Add 4 faces
		mesh.add_face(v0,v1,v2);
		mesh.add_face(v0,v1,v3);
		mesh.add_face(v0,v2,v3);
		mesh.add_face(v1,v2,v3);

		// Draw the mesh
		std::string s{ mesh };
		s << mesh;
		Log::Info("{}", s.str());
	}

}} // namespace Albedo::APP