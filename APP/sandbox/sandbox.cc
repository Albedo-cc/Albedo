#include "sandbox.h"

#include <Albedo.Core.Log>
#include <Albedo.Core.Math>
#include <Albedo.Core.Time>
#include <Albedo.Core.Norm>
#include <Albedo.Editor>

#include <sstream>
#include <algorithm>

#include "runtime/renderer/renderer.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Polygon_2.h>

namespace Albedo{
namespace APP
{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel CGALKernel;
	//typedef CGAL::Triangulation_2<CGALKernal>                   Triangulation;
	typedef CGAL::Exact_predicates_tag                               Itag;
	typedef CGAL::Constrained_Delaunay_triangulation_2<CGALKernel, CGAL::Default, Itag> CDT;
	typedef CDT::Point									Point;
	typedef CDT::Edge									Edge;
	typedef CGAL::Polygon_2<CGALKernel>                 Polygon_2;

	static std::vector<ModelData::VertexType>
		vertices{
		0.0, 0.5, 0.5,
		0.5, -0.5,  0.5,
		-0.5, -0.5, 0.5};

	static std::vector<ModelData::IndexType>	indices{1,2,3};
	static Renderer::Model* s_model = nullptr;

	static std::vector<Point> vs
	{
		Point{0,	0.5},
		Point{0.5, -0.5},
		Point{-0.5, -0.5},
		//Point{0,0},
	};

	static std::vector<Point> constraint_points
	{
		Point{-1.0,  1.0}, Point{1.0,  1.0},
		Point{-1.0, -1.0}, Point{1.0, -1.0},
	};

	void PrepareSandbox()
	{
		/*Triangulation tri{};
		tri.insert(vs.begin(), vs.end());*/
		Point p1, p2;
		CGALKernel::Segment_2 seg{ CGALKernel::Point_2{1,0}, CGALKernel::Point_2{2, 0} };
		//Edge e = Edge({ 1,0 }, { 2,0 });
		CDT tri{};
		tri.insert_constraint(constraint_points[0], constraint_points[1]);
		tri.insert_constraint(constraint_points[0], constraint_points[2]);
		tri.insert_constraint(constraint_points[3], constraint_points[1]);
		tri.insert_constraint(constraint_points[3], constraint_points[2]);
		ALBEDO_ASSERT(tri.is_valid());
		tri.insert(vs.begin(), vs.end());

		//std::cout << tri;
		static std::vector<Vector3D> points;

		for(auto& f : tri.finite_face_handles())
		{
			for (int i = 0; i < 3; ++i)
			{
				auto v = f->vertex(2 - i);
				
				Log::Info("Point({}, {}, 0.5)", v->point().x(), v->point().y());
				points.emplace_back(v->point().x(), v->point().y(), 0.5);
			}
		}

		s_model = &Renderer::GetInstance().RegisterModel(
			ModelData
			{
				.vertices
				{
					.data = points.data()->data(),
					.count = points.size(),
				},
				.indices
				{
					.data = nullptr, // Testing
					.count = indices.size(),
				},
				.primitive = ModelData::Primitive::Triangle,
			});
	}

	void Sandbox()
	{
		static StopWatch timer{};
		//Mesh mesh;
		/*if (timer.Split().seconds() >= 1)
		{
			std::swap(vs[0], vs[1]);
			std::swap(vs[0], vs[2]);
			s_model->vbo->WriteAll(vs.data());
			timer.Reset();
		}*/

	}

}} // namespace Albedo::APP