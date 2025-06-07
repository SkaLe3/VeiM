#include "Math.h"

namespace VeiM
{
	namespace Math
	{

		const Transform Transform::Identity = Transform();

		Transform Transform::GetRelative(const Transform& other) const
		{
			Transform result;

			if (HasNegativeScale(Scale) || HasNegativeScale(other.Scale))
			{
				GetRelativeWithMatrix(&result, this, &other);
			}
			else
			{
				glm::vec3 recipScale = GetSafeScaleReciprocal(other.Scale, VM_EPSILON_SMALL);
				result.Scale = Scale * recipScale;
				if (!Utils::IsQuatNormalized(other.Rotation))
				{
					return Transform::Identity;
				}
				glm::quat inverseRot = glm::inverse(other.Rotation);
				result.Rotation = inverseRot * Rotation;
				result.Translation = (inverseRot * (Translation - other.Translation)) * recipScale;
			}
			return result;
		}

		void Transform::GetRelativeWithMatrix(Transform* outTransform, const Transform* base, const Transform* relative)
		{
			glm::mat4 am = base->ToMatrix();
			glm::mat4 bm = relative->ToMatrix();
			outTransform->SetFromMatrix(am * glm::inverse(bm));
		}

		glm::vec3 Utils::SafeNormalize(const glm::vec3& vec, float epsilon /*= VM_EPSILON_SMALL*/, const glm::vec3& defaultValue /*= ZeroVector*/)
		{
			if (glm::length2(vec) < epsilon)
			{
				return defaultValue;
			}

			return glm::normalize(vec);
		}

	}
}

