#pragma once

#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		// 2차원 평면 써클이다 - 삼각형으로 이뤄지지 않아서 색깔을 채울 수는 없다
		template<typename T>
		std::vector<T> makeCircle2DXY_Line(int vertexNumber, float radius)
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
			std::vector<T> circle = makeCircle2DXY_Line<T>(vertexNumber, radius);

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
			std::vector<T> circle = makeCircle2DXY_Line<T>(vertexNumber, radius);	// XY평면의 2D 써클

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

		template<typename T>
		std::vector<T> makeSimpleRect2DXY_Line(const T& leftBottom, const T& rightTop, float z)
		{
			std::vector<T> vertices(4);

			// 반시계 방향으로, xy 평면에다가 만드는데 z는 지정이다

			vertices[0].x = leftBottom.x;	// 좌하
			vertices[0].y = leftBottom.y;
			vertices[0].z = z;

			vertices[1].x = rightTop.x;	// 우하
			vertices[1].y = leftBottom.y;
			vertices[1].z = z;

			vertices[2].x = rightTop.x;	// 우상
			vertices[2].y = rightTop.y;
			vertices[2].z = z;

			vertices[3].x = leftBottom.x;	// 좌상
			vertices[3].y = rightTop.y;
			vertices[3].z = z;

			return vertices;
		}

		template<typename T>
		std::vector<T> makeSimpleCube3D_Line(const T& centerPos, const T& size)
		{
			std::vector<T> vertices;

			const auto halfSize = size / 2.f;

			// 6면을 만들자
			auto xyzLeftBottom = centerPos - halfSize;
			auto xyzRightTop = centerPos + halfSize;

			// xyz 평면에서의 사각형 2개 - maxZ, minZ
			auto xyMZRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzLeftBottom.z);	// M == maxZ
			auto xymZRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzRightTop.z);	// m == minZ

			// yzx 평면에서의 사각형 2개 - 이 평면에서는 z가 x역할, y가 y역할, x가 Z역할을 한다 - 오른손 좌표계에서 생각해라
			glm::vec3 yzxLeftBottom(xyzLeftBottom.z, xyzLeftBottom.y, xyzLeftBottom.x);
			glm::vec3 yzxRightTop(xyzRightTop.z, xyzRightTop.y, xyzRightTop.x);
			auto yzMXRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzLeftBottom.x);
			auto yzmXRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzRightTop.x);

			// zxy 평면에서의 사각형 2개 - 이 평면에서는 x가 x역할, z가 y역할, y가 Z역할을 한다 - 오른손 좌표계에서 생각해라
			glm::vec3 zxyLeftBottom(xyzLeftBottom.x, xyzLeftBottom.z, xyzLeftBottom.y);
			glm::vec3 zxyRightTop(xyzRightTop.x, xyzRightTop.z, xyzRightTop.y);
			auto zxMYRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzLeftBottom.y);
			auto zxmYRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzRightTop.y);

			std::copy(xyMZRect.begin(), xyMZRect.end(), std::back_inserter(vertices));
			std::copy(xymZRect.begin(), xymZRect.end(), std::back_inserter(vertices));
			std::copy(yzMXRect.begin(), yzMXRect.end(), std::back_inserter(vertices));
			std::copy(yzmXRect.begin(), yzmXRect.end(), std::back_inserter(vertices));
			std::copy(zxMYRect.begin(), zxMYRect.end(), std::back_inserter(vertices));
			std::copy(zxmYRect.begin(), zxmYRect.end(), std::back_inserter(vertices));
			vertices.shrink_to_fit();

			return vertices;
		}
	}
}
