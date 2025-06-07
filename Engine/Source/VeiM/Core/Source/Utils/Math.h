#pragma once
#include "CoreDefines.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <format>

#define VM_PI (3.1415926535897932f)
#define VM_EPSILON_SMALL glm::epsilon<float>()
#define VM_EPSILON_MEDIUM (1.e-4f)



namespace VeiM
{
	namespace Math
	{
		namespace Utils
		{
			inline const glm::vec3 ForwardVector(0.0f, 0.0f, -1.0f);
			inline const glm::vec3 UpVector(0.0f, 1.0f, 0.0f);
			inline const glm::vec3 RightVector(1.0f, 0.0f, 0.0f);
			inline const glm::vec3 ZeroVector(0.0f, 0.0f, 0.0f);
			inline const glm::vec3 OnesVector(1.0f, 1.0f, 1.0f);

			inline const glm::vec2 ZeroVector2d(0.0f, 0.0f);


			inline const glm::mat4 MatrixIdentity4x4 = glm::mat4(1.0f);
			inline const glm::mat3 MatrixIdentity3x3 = glm::mat3(1.0f);

			inline const glm::quat QuatIdentity = glm::quat();

			static bool IsQuatNormalized(const glm::quat& q, float epsilon = VM_EPSILON_SMALL)
			{
				float length = glm::length(q);
				return glm::abs(length - 1.0f) < epsilon;
			}
			glm::vec3 SafeNormalize(const glm::vec3& vec, float epsilon = VM_EPSILON_SMALL, const glm::vec3& defaultValue = ZeroVector);

			template<typename T>
			bool IsPowerOfTwo(T value)
			{
				return ((value & (value - 1)) == (T)0);
			}
		}

		struct CORE_API Transform
		{
		public:
			glm::quat Rotation;
			glm::vec3 Translation;
			glm::vec3 Scale;

			static const Transform Identity;
		public:
			Transform()
				: Rotation(Utils::QuatIdentity),
				Translation(0.f),
				Scale(Utils::OnesVector)
			{
			}
			Transform(const glm::quat& inRotation, const glm::vec3& inTranslation, const glm::vec3& inScale = Utils::OnesVector)
				: Rotation(inRotation),
				Translation(inTranslation),
				Scale(inScale)
			{
			}
			Transform(const glm::vec3& inRotation, const glm::vec3& inTranslation, const glm::vec3& inScale = Utils::OnesVector)
				: Rotation(glm::radians(inRotation)),
				Translation(inTranslation),
				Scale(inScale)
			{
			}
			explicit Transform(const glm::vec3& inTranslation)
				: Rotation(Utils::QuatIdentity),
				Translation(inTranslation),
				Scale(Utils::OnesVector)
			{
			}
			explicit Transform(const glm::quat& inRotation)
				: Rotation(inRotation),
				Translation(Utils::ZeroVector),
				Scale(Utils::OnesVector)
			{
			}
			explicit Transform(const glm::vec3& inRotation, bool bIsRotation)
				: Rotation(glm::radians(inRotation)),
				Translation(Utils::ZeroVector),
				Scale(Utils::OnesVector)
			{
			}

			explicit Transform(const glm::mat4& inMatrix)
			{
				SetFromMatrix(inMatrix);
			}

			void SetFromMatrix(const glm::mat4& inMatrix)
			{
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(inMatrix, Scale, Rotation, Translation, skew, perspective);
				if (glm::determinant(glm::mat3(inMatrix)) < 0.0f) {
					Scale.x = -Scale.x;
					Rotation = -Rotation;
				}
				glm::normalize(Rotation);
			}


			glm::mat4 ToMatrix() const
			{
				glm::mat4 rotationMatrix = glm::toMat4(Rotation);

				rotationMatrix[0] *= Scale.x;
				rotationMatrix[1] *= Scale.y;
				rotationMatrix[2] *= Scale.z;

				glm::mat4 transform = rotationMatrix;
				transform[3] = glm::vec4(Translation, 1.0f);

				return transform;
			}

			FORCEINLINE Transform operator*(float value) const
			{
				return Transform(Rotation * value, Translation * value, Scale * value);
			}

			FORCEINLINE Transform& operator*=(float value)
			{
				Translation = Translation * value;
				Rotation = Rotation * value;
				Scale = Scale * value;
				return *this;
			}

			FORCEINLINE Transform& operator*(const Transform& other) const
			{
				Transform output;
				Multiply(&output, this, &other);
				return output;
			}

			FORCEINLINE void operator*=(const Transform& other)
			{
				Multiply(this, this, &other);
			}

			FORCEINLINE Transform operator*(const glm::quat& other) const
			{
				Transform output, otherTrans(other, Utils::ZeroVector, Utils::OnesVector);
				Multiply(&output, this, &otherTrans);
				return output;
			}

			FORCEINLINE void operator*=(const glm::quat& other)
			{
				Transform otherTrans(other, Utils::ZeroVector, Utils::OnesVector);
				Multiply(this, this, &otherTrans);
			}


			FORCEINLINE static void Multiply(Transform* outTransform, const Transform* a, const Transform* b)
			{
				if (a->Scale.x < 0 || a->Scale.y < 0 || a->Scale.z < 0 || b->Scale.x < 0 || b->Scale.y < 0 || b->Scale.z < 0)
				{
					glm::mat4 matA = a->ToMatrix();
					glm::mat4 matB = b->ToMatrix();
					glm::mat4 matOut = matA * matB;
					outTransform->SetFromMatrix(matOut);
				}
				else
				{
					outTransform->Rotation = b->Rotation * a->Rotation;
					outTransform->Scale = a->Scale * b->Scale;
					outTransform->Translation = b->Rotation * (b->Scale * a->Translation) + b->Translation;
				}
			}



			FORCEINLINE bool Equals(const Transform& other, float epsilon = VM_EPSILON_MEDIUM) const
			{
				return TranslationEquals(other.Translation, epsilon) && RotationEquals(other.Rotation, epsilon) && ScaleEquals(other.Scale, epsilon);
			}

			FORCEINLINE glm::vec3 InverseTransformTranslation(const glm::vec3& t) const
			{
				return glm::inverse(Rotation) * (t - Translation) * GetSafeScaleReciprocal(Scale);
			}


			Transform GetRelative(const Transform& other) const;
			static void GetRelativeWithMatrix(Transform* outTransform, const Transform* parent, const Transform* relative);

			FORCEINLINE static glm::vec3 GetSafeScaleReciprocal(const glm::vec3& inScale, float epsilon = VM_EPSILON_SMALL)
			{
				glm::vec3 safeScale;
				if (glm::abs(inScale.x) <= epsilon)
					safeScale.x = 0.f;
				else
					safeScale.x = 1 / inScale.x;
				if (glm::abs(inScale.y) <= epsilon)
					safeScale.y = 0.f;
				else
					safeScale.y = 1 / inScale.y;
				if (glm::abs(inScale.z) <= epsilon)
					safeScale.z = 0.f;
				else
					safeScale.z = 1 / inScale.z;
				return safeScale;
			}
			FORCEINLINE static bool HasNegativeScale(const glm::vec3& inScale)
			{
				if (inScale.x < 0 || inScale.y < 0 || inScale.z < 0)
					return true;
				return false;
			}

			

			String ToStringPretty() const
			{
				glm::vec3 eulerRot = glm::eulerAngles(Rotation);

				String output = std::format("Rotation: Pitch {} Yaw {} Roll {}\n", eulerRot.x, eulerRot.y, eulerRot.z);
				output += std::format("Translation: {} {} {}\n", Translation.x, Translation.y, Translation.z);
				output += std::format("Scale: {} {} {}\n", Scale.x, Scale.y, Scale.z);
				return output;
			}

			String ToString() const
			{
				glm::vec3 eulerRot = glm::degrees(glm::eulerAngles(Rotation));
				return std::format("{},{},{}|{},{},{}|{},{},{}", Translation.x, Translation.y, Translation.z, eulerRot.x, eulerRot.y, eulerRot.z, Scale.x, Scale.y, Scale.z);
			}
			void DebugPring() const
			{
				VM_CORE_TRACE(ToStringPretty());
			}

			private:
				FORCEINLINE bool TranslationEquals(const glm::vec3& inTranslation, float epsilon = VM_EPSILON_MEDIUM) const
				{
					return glm::all(glm::epsilonEqual(Translation, inTranslation, epsilon));
				}
				FORCEINLINE bool RotationEquals(const glm::quat& inRotation, float epsilon = VM_EPSILON_MEDIUM) const
				{
					return glm::all(glm::epsilonEqual(Rotation, inRotation, epsilon));
				}
				FORCEINLINE bool ScaleEquals(const glm::vec3& inScale, float epsilon = VM_EPSILON_MEDIUM) const
				{
					return glm::all(glm::epsilonEqual(Scale, inScale, epsilon));
				}


		};
	}

}