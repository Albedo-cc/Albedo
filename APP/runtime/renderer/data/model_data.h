#pragma once

namespace Albedo{
namespace APP
{
	
	class ModelData
	{
		friend class Renderer;
		static inline uint64_t SID = 0;
	public:
		using VertexType = float;
		struct VBO
		{
			VertexType* data { nullptr };
			size_t		count{ 0 };
		}vertices;

		using IndexType = unsigned int;
		struct IBO
		{
			IndexType*  data { nullptr };
			size_t		count{ 0 };
		}indices;

		enum Primitive { Triangle, Line };
		const Primitive primitive = Triangle;
		const uint64_t  id = SID++;
	};

}} // namespace Albedo::APP

namespace std
{
	template<>
	struct hash<Albedo::APP::ModelData> // Partical Template Specification
	{
	public:
		size_t operator()(const Albedo::APP::ModelData& model) const
		{
			return model.id;
		}
	};
}