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
		auto& vm = APP::Camera::GetInstance().GetViewMatrix();
		auto& pm = APP::Camera::GetInstance().GetProjectMatrix();
		static bool init = false;
		if (!init)
		{
			init = true;
			Editor::RegisterUIEvent(new UIEvent
			("Test Camera", [&vm, &pm]()->void
				{
					ImGui::Begin("Camera");
					{
						ImGui::Text("%u", GRI::GetFPS());

						ImGui::Text(
							"%.1f, %.1f, %.1f, %.1f\n"
							"%.1f, %.1f, %.1f, %.1f\n"
							"%.1f, %.1f, %.1f, %.1f\n"
							"%.1f, %.1f, %.1f, %.1f\n"
						, vm(0,0), vm(0,1), vm(0,2), vm(0,3)
						, vm(1,0), vm(1,1), vm(1,2), vm(1,3)
						, vm(2,0), vm(2,1), vm(2,2), vm(2,3)
						, vm(3,0), vm(3,1), vm(3,2), vm(3,3)
						);
						ImGui::Text(
							"%.1f, %.1f, %.1f, %.1f\n"
							"%.1f, %.1f, %.1f, %.1f\n"
							"%.1f, %.1f, %.1f, %.1f\n"
							"%.1f, %.1f, %.1f, %.1f\n"
						, pm(0,0), pm(0,1), pm(0,2), pm(0,3)
						, pm(1,0), pm(1,1), pm(1,2), pm(1,3)
						, pm(2,0), pm(2,1), pm(2,2), pm(2,3)
						, pm(3,0), pm(3,1), pm(3,2), pm(3,3)
						);
					}
					ImGui::End();
				}));
		}
		
	}

}} // namespace Albedo::APP