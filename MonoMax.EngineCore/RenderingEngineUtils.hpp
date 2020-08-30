#pragma once

#include <vector>

namespace SMGE
{
	namespace nsRE
	{
		// 2���� ��� ��Ŭ�̴� - �ﰢ������ �̷����� �ʾƼ� ������ ä�� ���� ����
		template<typename T>
		std::vector<T> makeCircle2D_Line(int vertexNumber, float radius)
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
			std::vector<T> circle = makeCircle2D_Line<T>(vertexNumber, radius);

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
			std::vector<T> circle = makeCircle2D_Line<T>(vertexNumber, radius);	// XY����� 2D ��Ŭ

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
	}
}
