#include "SphereMesh.h"
#include <glm/gtc/constants.hpp>

namespace VeiM
{

	SphereMesh::SphereMesh(uint32 xSegments, uint32 ySegments)
	{
		for (uint32 y = 0; y <= ySegments; y++)
		{
			for (uint32 x = 0; x <= xSegments; x++)
			{
				float xSegment = (float)x / (float)xSegments;
				float ySegment = (float)y / (float)ySegments;
				float xPos = glm::cos(xSegment * glm::two_pi<float>()) * glm::sin(ySegment * glm::pi<float>());
				float yPos = glm::cos(ySegment * glm::pi<float>());
				float zPos = glm::sin(xSegment * glm::two_pi<float>()) * glm::sin(ySegment * glm::pi<float>());

				Positions.emplace_back(xPos, yPos, zPos);
				UV.emplace_back(xSegment, ySegment);
				Normals.emplace_back(xPos, yPos, zPos);
			}
		}

		bool oddRow = false;
		for (int32 y = 0; y < ySegments; y++)
		{
			for (int32 x = 0; x < xSegments; x++)
			{
				Indices.push_back((y + 1)	* (xSegments + 1) + x);
				Indices.push_back( y		* (xSegments + 1) + x);
				Indices.push_back( y		* (xSegments + 1) + x + 1);
				Indices.push_back((y + 1)	* (xSegments + 1) + x);
				Indices.push_back( y		* (xSegments + 1) + x + 1);
				Indices.push_back((y + 1)	* (xSegments + 1) + x + 1);
			}
		}
		Topology = ETopology::Triangles;
		Finilize();
	}

}

