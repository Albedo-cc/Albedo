#pragma once

namespace Albedo
{
   
	class AssetSystem final
	{
        friend class AlbedoAPP;
    public:

	private:
        AssetSystem()                              = delete;
        ~AssetSystem()                             = delete;
        AssetSystem(const AssetSystem&)            = delete;
        AssetSystem(AssetSystem&&)                 = delete;
        AssetSystem& operator=(const AssetSystem&) = delete;
        AssetSystem& operator=(AssetSystem&&)      = delete;
	};

} // namespace Albedo