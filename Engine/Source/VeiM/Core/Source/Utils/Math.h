#pragma once

#include <glm/glm.hpp>

namespace VeiM
{
	namespace MathUtils
	{
		inline const glm::vec3 ForwardVector(0.0f, 0.0f, -1.0f);
		inline const glm::vec3 UpVector(0.0f, 1.0f, 0.0f);
		inline const glm::vec3 RightVector(1.0f, 0.0f, 0.0f);

		inline const glm::mat4 IdentityMatrix4x4 = glm::mat4(1.0f);
		inline const glm::mat3 IdentityMatrix3x3 = glm::mat3(1.0f);
	}
}