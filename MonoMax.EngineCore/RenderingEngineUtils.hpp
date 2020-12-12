#pragma once

#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		// 2차원 평면 써클이다 - 삼각형으로 이뤄지지 않아서 색깔을 채울 수는 없다
		template<typename T>
		std::vector<T> makeCircle2DXY_Lines(int vertexNumber, float radius)
		{
			std::vector<T> circle;

			// x축 방향 벡터의 롤을 회전시켜서 만들자
			float angle = 0.f, angleAdd = 2.f * 3.141592f / float(vertexNumber);
			glm::mat4 identity(1.f);
			glm::vec4 xDir(radius, 0.f, 0.f, 1.f), zDir(0.f, 0.f, -1.f, 1.f);

			bool isFirst = true;
			T vt;
			do
			{
				glm::mat4 rotation = glm::rotate(identity, angle, glm::vec3(zDir));
				vt = rotation * xDir;
				circle.emplace_back(vt);

				if(isFirst == false)	// GL_LINES 를 위하여
					circle.emplace_back(vt);	// 이번의 끝점이 다음의 시작점이 되므로

				angle += angleAdd;

				isFirst = false;
			} while (--vertexNumber > 0);

			circle.push_back(circle[0]);	// GL_LINES 를 위하여 - 원이니까 마지막 선분은 최초 시작점을 끝점으로 삼아주면 됨

			return circle;
		}

		// XYZ축에 평행한 2D써클 3개를 만든다
		template<typename T>
		std::vector<T> makeSimpleSphere3D_Lines(int vertexNumber, float radius)
		{
			auto circle = makeCircle2DXY_Lines<T>(vertexNumber, radius);	// XY평면의 2D 써클

			float perpAngle = 3.141592f / 2.f;

			glm::mat4 identity(1.f);
			glm::vec3 yDir(0.f, 1.f, 0.f), xDir(1.f, 0.f, 0.f);

			const auto circleVertNumber = circle.size();

			// 2차원 써클을 요로 90도
			glm::mat4 yawPerp = glm::rotate(identity, perpAngle, yDir);
			for (int i = 0; i < circleVertNumber; ++i)
			{
				circle.push_back(yawPerp * glm::vec4(circle[i], 1.f));	// circle[0] ~ circle[i] 까지는 2차원 써클이다
			}

			// 2차원 써클을 피치로 90도
			glm::mat4 pitchPerp = glm::rotate(identity, perpAngle, xDir);
			for (int i = 0; i < circleVertNumber; ++i)
			{
				circle.push_back(pitchPerp * glm::vec4(circle[i], 1.f));	// circle[0] ~ circle[i] 까지는 2차원 써클이다
			}

			return circle;
		}

		// 이게 구가 되긴 하는데 선으로 이뤄진 구 이지 면으로 나눠진 구가 되지는 않는다 - 차후 개선해야함
		template<typename T>
		std::vector<T> makeSphere3D_Line(int vertexNumber, float radius)
		{
			std::vector<T> circle = makeCircle2DXY_Lines<T>(vertexNumber, radius);

			const int latitCount = vertexNumber;

			// 이거 GL_LINES 에 맞게 고쳐야한다

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

		template<typename T>
		std::vector<T> makeSimpleRect2DXY_Lines(const T& leftBottom, const T& rightTop, float z)
		{
			std::vector<T> vertices(8);	// GL_LINES 를 위하여

			// 반시계 방향으로, xy 평면에다가 만드는데 z는 지정이다

			vertices[0].x = leftBottom.x;	// 좌하
			vertices[0].y = leftBottom.y;
			vertices[0].z = z;

			vertices[1].x = rightTop.x;	// 우하
			vertices[1].y = leftBottom.y;
			vertices[1].z = z;

			vertices[2].x = rightTop.x;	// 우하
			vertices[2].y = leftBottom.y;
			vertices[2].z = z;

			vertices[3].x = rightTop.x;	// 우상
			vertices[3].y = rightTop.y;
			vertices[3].z = z;

			vertices[4].x = rightTop.x;	// 우상
			vertices[4].y = rightTop.y;
			vertices[4].z = z;

			vertices[5].x = leftBottom.x;	// 좌상
			vertices[5].y = rightTop.y;
			vertices[5].z = z;

			vertices[6].x = leftBottom.x;	// 좌상
			vertices[6].y = rightTop.y;
			vertices[6].z = z;

			vertices[7] = vertices[0];	// GL_LINES 를 위하여 - 좌하

			return vertices;
		}

		template<typename T>
		std::vector<T> makeSimpleCube3D_Lines(const T& centerPos, const T& size)
		{
			std::vector<T> vertices;

			const auto halfSize = size / 2.f;

			// 6면을 만들자
			auto xyzLeftBottom = centerPos - halfSize;
			auto xyzRightTop = centerPos + halfSize;

			// xy 평면에서의 사각형 2개 - maxZ, minZ
			auto xyMZRect = makeSimpleRect2DXY_Lines(xyzLeftBottom, xyzRightTop, xyzLeftBottom.z);	// M == maxZ
			auto xymZRect = makeSimpleRect2DXY_Lines(xyzLeftBottom, xyzRightTop, xyzRightTop.z);	// m == minZ

			std::copy(xyMZRect.begin(), xyMZRect.end(), std::back_inserter(vertices));
			std::copy(xymZRect.begin(), xymZRect.end(), std::back_inserter(vertices));

			// 이제 위의 사각형 2개를 회전시켜서 육면체를 만들자
			float perpAngle = 3.141592f / 2.f;

			glm::mat4 identity(1.f);
			glm::vec3 yDir(0.f, 1.f, 0.f), xDir(1.f, 0.f, 0.f);

			const auto vertNumber = xyMZRect.size();

			glm::mat4 yawPerp = glm::rotate(identity, perpAngle, yDir);
			for (int i = 0; i < vertNumber; ++i)
				vertices.push_back(yawPerp * glm::vec4(xyMZRect[i], 1.f));
			for (int i = 0; i < vertNumber; ++i)
				vertices.push_back(yawPerp * glm::vec4(xymZRect[i], 1.f));

			glm::mat4 pitchPerp = glm::rotate(identity, perpAngle, xDir);
			for (int i = 0; i < vertNumber; ++i)
				vertices.push_back(pitchPerp * glm::vec4(xyMZRect[i], 1.f));
			for (int i = 0; i < vertNumber; ++i)
				vertices.push_back(pitchPerp * glm::vec4(xymZRect[i], 1.f));

			return vertices;
		}
	}
}
