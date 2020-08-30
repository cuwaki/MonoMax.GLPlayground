#pragma once

#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		// 2차원 평면 써클이다 - 삼각형으로 이뤄지지 않아서 색깔을 채울 수는 없다
		template<typename T>
		std::vector<T> makeCircle2D_Line(int vertexNumber, float radius)
		{
			std::vector<T> circle;
			circle.reserve(vertexNumber);

			// x축 방향 벡터의 롤을 회전시켜서 만들자
			float angle = 0.f, angleAdd = 2.f * 3.141592f / float(vertexNumber);
			glm::mat4 identity(1.f);
			glm::vec4 xDir(radius, 0.f, 0.f, 1.f), zDir(0.f, 0.f, -1.f, 1.f);

			T vt;
			do
			{
				glm::mat4 rotation = glm::rotate(identity, angle, glm::vec3(zDir));
				vt = rotation * xDir;
				circle.emplace_back(vt);

				angle += angleAdd;
			} while (--vertexNumber > 0);

			return circle;
		}

		// 이게 구가 되긴 하는데 선으로 이뤄진 구 이지 면으로 나눠진 구가 되지는 않는다 - 차후 개선해야함
		template<typename T>
		std::vector<T> makeSphere3D_Line(int vertexNumber, float radius)
		{
			std::vector<T> circle = makeCircle2D_Line<T>(vertexNumber, radius);

			const int latitCount = vertexNumber;

			float angle = 0.f, angleAdd = 2.f * 3.141592f / float(latitCount);
			glm::mat4 identity(1.f);
			glm::vec4 yDir(0.f, 1.f, 0.f, 1.f);
			for (int latitude = 0; latitude < latitCount; ++latitude)
			{
				angle = angleAdd * (latitude + 1);

				glm::mat4 rotation = glm::rotate(identity, angle, glm::vec3(yDir));

				// 2차원 써클을 y축 방향 벡터의 요를 회전시켜서 구를 만들자
				for (int i = 0; i < vertexNumber; ++i)
				{
					auto v = rotation * glm::vec4(circle[i], 1.f);
					//v.z += (20.f * (latitude + 1));
					circle.push_back(v);	// circle[0] ~ circle[i] 까지는 2차원 써클이다
				}
			}

			return circle;
		}

		// XYZ축에 평행한 2D써클 3개를 만든다
		template<typename T>
		std::vector<T> makeSimpleSphere3D_Line(int vertexNumber, float radius)
		{
			std::vector<T> circle = makeCircle2D_Line<T>(vertexNumber, radius);	// XY평면의 2D 써클

			float perpAngle = 3.141592f / 2.f;

			glm::mat4 identity(1.f);
			glm::vec4 yDir(0.f, 1.f, 0.f, 1.f), xDir(1.f, 0.f, 0.f, 1.f);

			// 2차원 써클을 요로 90도
			glm::mat4 yawPerp = glm::rotate(identity, perpAngle, glm::vec3(yDir));
			for (int i = 0; i < vertexNumber; ++i)
				circle.push_back(yawPerp * glm::vec4(circle[i], 1.f));	// circle[0] ~ circle[i] 까지는 2차원 써클이다

			// 2차원 써클을 피치로 90도
			glm::mat4 pitchPerp = glm::rotate(identity, perpAngle, glm::vec3(xDir));
			for (int i = 0; i < vertexNumber; ++i)
				circle.push_back(pitchPerp * glm::vec4(circle[i], 1.f));	// circle[0] ~ circle[i] 까지는 2차원 써클이다

			return circle;
		}
	}
}
