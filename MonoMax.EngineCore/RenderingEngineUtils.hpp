#pragma once

#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		// 2���� ��� ��Ŭ�̴� - �ﰢ������ �̷����� �ʾƼ� ������ ä�� ���� ����
		template<typename T>
		std::vector<T> makeCircle2DXY_Line(int vertexNumber, float radius)
		{
			std::vector<T> circle;
			circle.reserve(vertexNumber);

			// x�� ���� ������ ���� ȸ�����Ѽ� ������
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

		// �̰� ���� �Ǳ� �ϴµ� ������ �̷��� �� ���� ������ ������ ���� ������ �ʴ´� - ���� �����ؾ���
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

				// 2���� ��Ŭ�� y�� ���� ������ �並 ȸ�����Ѽ� ���� ������
				for (int i = 0; i < vertexNumber; ++i)
				{
					auto v = rotation * glm::vec4(circle[i], 1.f);
					//v.z += (20.f * (latitude + 1));
					circle.push_back(v);	// circle[0] ~ circle[i] ������ 2���� ��Ŭ�̴�
				}
			}

			return circle;
		}

		// XYZ�࿡ ������ 2D��Ŭ 3���� �����
		template<typename T>
		std::vector<T> makeSimpleSphere3D_Line(int vertexNumber, float radius)
		{
			std::vector<T> circle = makeCircle2DXY_Line<T>(vertexNumber, radius);	// XY����� 2D ��Ŭ

			float perpAngle = 3.141592f / 2.f;

			glm::mat4 identity(1.f);
			glm::vec4 yDir(0.f, 1.f, 0.f, 1.f), xDir(1.f, 0.f, 0.f, 1.f);

			// 2���� ��Ŭ�� ��� 90��
			glm::mat4 yawPerp = glm::rotate(identity, perpAngle, glm::vec3(yDir));
			for (int i = 0; i < vertexNumber; ++i)
				circle.push_back(yawPerp * glm::vec4(circle[i], 1.f));	// circle[0] ~ circle[i] ������ 2���� ��Ŭ�̴�

			// 2���� ��Ŭ�� ��ġ�� 90��
			glm::mat4 pitchPerp = glm::rotate(identity, perpAngle, glm::vec3(xDir));
			for (int i = 0; i < vertexNumber; ++i)
				circle.push_back(pitchPerp * glm::vec4(circle[i], 1.f));	// circle[0] ~ circle[i] ������ 2���� ��Ŭ�̴�

			return circle;
		}

		template<typename T>
		std::vector<T> makeSimpleRect2DXY_Line(const T& leftBottom, const T& rightTop, float z)
		{
			std::vector<T> vertices(4);

			// �ݽð� ��������, xy ��鿡�ٰ� ����µ� z�� �����̴�

			vertices[0].x = leftBottom.x;	// ����
			vertices[0].y = leftBottom.y;
			vertices[0].z = z;

			vertices[1].x = rightTop.x;	// ����
			vertices[1].y = leftBottom.y;
			vertices[1].z = z;

			vertices[2].x = rightTop.x;	// ���
			vertices[2].y = rightTop.y;
			vertices[2].z = z;

			vertices[3].x = leftBottom.x;	// �»�
			vertices[3].y = rightTop.y;
			vertices[3].z = z;

			return vertices;
		}

		template<typename T>
		std::vector<T> makeSimpleCube3D_Line(const T& centerPos, const T& size)
		{
			std::vector<T> vertices;

			const auto halfSize = size / 2.f;

			// 6���� ������
			auto xyzLeftBottom = centerPos - halfSize;
			auto xyzRightTop = centerPos + halfSize;

			// xyz ��鿡���� �簢�� 2�� - maxZ, minZ
			auto xyMZRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzLeftBottom.z);	// M == maxZ
			auto xymZRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzRightTop.z);	// m == minZ

			// yzx ��鿡���� �簢�� 2�� - �� ��鿡���� z�� x����, y�� y����, x�� Z������ �Ѵ� - ������ ��ǥ�迡�� �����ض�
			glm::vec3 yzxLeftBottom(xyzLeftBottom.z, xyzLeftBottom.y, xyzLeftBottom.x);
			glm::vec3 yzxRightTop(xyzRightTop.z, xyzRightTop.y, xyzRightTop.x);
			auto yzMXRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzLeftBottom.x);
			auto yzmXRect = makeSimpleRect2DXY_Line(xyzLeftBottom, xyzRightTop, xyzRightTop.x);

			// zxy ��鿡���� �簢�� 2�� - �� ��鿡���� x�� x����, z�� y����, y�� Z������ �Ѵ� - ������ ��ǥ�迡�� �����ض�
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
