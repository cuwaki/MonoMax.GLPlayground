#pragma once

#include <map>
#include <array>
#include <vector>
#include <functional>
#include <string>
#include <forward_list>
#include <memory>
#include "GECommonIncludes.h"

namespace SMGE
{
	template<typename T, typename SizeType> class CQuadTree;
	template<typename T, typename SizeType> class COcTree;

	template<typename T>
	struct SPoint2D
	{
		union
		{
			T xy_[2];	// xy 평면으로 다룰 때
			T zx_[2];	// zx 평면으로 다룰 때
			T yz_[2];	// yz 평면으로 다룰 때
			T uv_[2];	// uv 평면으로 다룰 때

			T min_[2];	// 좌하단이 시작, 우상단이 끝인 좌표계 
			T max_[2];
		};

		SPoint2D() {}
		SPoint2D(T u, T v)
		{
			uv_[0] = u;
			uv_[1] = v;
		}

		bool operator==(const SPoint2D<T>& r)
		{
			return uv_[0] == r.uv_[0] && uv_[1] == r.uv_[1];
		}
		bool operator!=(const SPoint2D<T>& r)
		{
			return !operator==(r);
		}
	};

	template<typename T>
	struct SRect2D
	{
		SRect2D() : l_(leftBottom_.uv_[0]), b_(leftBottom_.uv_[1]), r_(rightTop_.uv_[0]), t_(rightTop_.uv_[1])
		{
		}
		SRect2D(T l, T b, T r, T t) : SRect2D()
		{
			l_ = l;
			b_ = b;
			r_ = r;
			t_ = t;
		}

		SPoint2D<T> leftBottom_, rightTop_;
		T& l_, &b_, &r_, &t_;

		bool operator==(const SRect2D<T>& r)
		{
			return leftBottom_ == r.leftBottom_ && rightTop_ == r.rightTop_;
		}
		bool operator!=(const SRect2D<T>& r)
		{
			return !operator==(r);
		}

		T Width() const {return rightTop_.uv_[0] - leftBottom_.uv_[0];}
		T Height() const { return rightTop_.uv_[1] - leftBottom_.uv_[1]; }
	};

	template<typename ContainerT, typename SizeType>
	class CQuadTreeNode
	{
		friend CQuadTree;
		friend COcTree;

	public:
		using TChild = CQuadTreeNode<ContainerT, SizeType>;
		using TChildren = std::array<TChild, 4>;	// quadtree 니까 4
		using TContainer = ContainerT;
		using TValue = typename ContainerT::value_type;
		using TValueIterator = typename ContainerT::iterator;

	public:
		CQuadTreeNode()
		{
		}
		CQuadTreeNode(SizeType startW, SizeType startH, SizeType maxW, SizeType maxH) : rect_(startW, startH, maxW, maxH)
		{
		}
		CQuadTreeNode(const ContainerT& data, SizeType startW, SizeType startH, SizeType maxW, SizeType maxH) : CQuadTreeNode(startW, startH, maxW, maxH)
		{
			container_ = data;
		}
		CQuadTreeNode(ContainerT&& data, SizeType startW, SizeType startH, SizeType maxW, SizeType maxH) : CQuadTreeNode(startW, startH, maxW, maxH)
		{
			container_ = std::forward<ContainerT>(data);
		}

		ContainerT& GetContainer() { return container_; }
		const ContainerT& GetContainer() const { return container_; }

		bool HasChildren() const { return children_ != nullptr; }

		TChild& GetChild(size_t i) { AllocChildren(); return (*children_.get())[i]; }
		const TChild& GetChild(size_t i) const { AllocChildren(); return (*children_.get())[i]; }

		auto& GetRect() { return rect_; }
		const auto& GetRect() const { return rect_; }

		bool FindValue(const TValue& findingValue, TValueIterator& outFound) const
		{
			outFound = std::find(container_.begin(), container_.end(), findingValue);
			return outFound != container_.end();
		}
		void RemoveValue(TValueIterator& removeTarget)
		{
			container_.erase(removeTarget);
		}

		bool operator==(const CQuadTreeNode& r)
		{
			return rect_ == r.rect_;
		}
		bool operator!=(const CQuadTreeNode& r)
		{
			return !operator==(r);
		}

		void Clear()
		{
			container_ = decltype(container_){};
			children_.reset();
		}

	protected:
		void AllocChildren()
		{
			if(children_ == nullptr)
				children_ = std::make_unique<TChildren>();
		}

	protected:
		ContainerT container_;
		SRect2D<SizeType> rect_;

		std::unique_ptr<TChildren> children_;
	};

	// 모든 좌표와 좌표계는 GL 오른손 좌표계를 사용한다.
	template<typename ContainerT, typename SizeType>
	class CQuadTree
	{
		friend COcTree;

	public:
		using TSize = SizeType;

		using TContainer = ContainerT;
		using TValue = typename ContainerT::value_type;
		using TValueIterator = typename ContainerT::iterator;

		using TNode = CQuadTreeNode<ContainerT, SizeType>;
		using TNodeList = std::forward_list<TNode*>;

	public:
		CQuadTree() {}
		CQuadTree(const std::string& treeName, SizeType width, SizeType height, SizeType leafNodeWidth, SizeType leafNodeHeight)
		{
			Create(treeName, width, height, leafNodeWidth, leafNodeHeight);
		}

		void Create(const std::string& treeName, SizeType width, SizeType height, SizeType leafNodeWidth, SizeType leafNodeHeight)
		{
			assert(width == height);	// 현재 비균등은 제대로 작동하지 않는 듯 하다, 필요시 나중에 고치자;;

			treeName_ = treeName;

			width_ = width;
			height_ = height;
			leafNodeWidth_ = leafNodeWidth;
			leafNodeHeight_ = leafNodeHeight;

			// GL 사분면에 맞도록 -1 ~ 0 ~ 1 사분면 좌표계로 바꿔서 넣어준다
			// 이 쿼드트리의 중심이 GL 월드좌표 0,0에 위치하게 되는 것이다
			BuildQuadTree(rootNode_, width_/-2, height_/-2, width_, height_);

			// 리프노드들을 시작위치를 가지고 2차원 테이블로 만든다
			const auto vSize = tempUVNodes_.size();
			uvNodeTable_.reserve(vSize);
			for (auto vIT : tempUVNodes_)	// [v] map
			{
				const auto& tempUNodes = vIT.second;	// [u] map
				const auto uSize = tempUNodes.size();
				
				uvNodeTable_.emplace_back();	// [v]  할당
				
				auto& uNodes = *uvNodeTable_.rbegin();
				uNodes.reserve(uSize);
				for (auto uIT : tempUNodes)
				{
					uNodes.emplace_back(uIT.second);	// [u] 할당
				}
			}
			tempUVNodes_.clear();

			// 정보 정리
			treeDepth_ = CalculateDepth(rootNode_);
			halfWidth_ = width_ / 2;
			halfHeight_ = height_ / 2;
			widthGap_ = uvNodeTable_[0][1]->GetRect().l_ - uvNodeTable_[0][0]->GetRect().l_;
			heightGap_ = uvNodeTable_[1][0]->GetRect().b_ - uvNodeTable_[0][0]->GetRect().b_;
		}

		auto HardQuery(const TValue& findingValue) const
		{
			TNodeList ret;
			for (auto& vNodes : uvNodeTable_)
			{
				for (auto& vNode : vNodes)
				{
					TValueIterator found;
					if (vNode->FindValue(findingValue, found))
						ret.emplace_front(vNode);
				}
			}

			return ret;
		}

		TNode* QueryNodeByPoint(SizeType w, SizeType h) const
		{
			//return QueryNodeByPoint(rootNode_, w, h);	// 트리 운행을 통한 쿼리

			// 테이블을 통한 처리
			// vector 인덱스로 쓰기 위하여 -1 ~ 0 ~ +1 좌표계를 0 ~ 2 좌표계로 바꾼다
			h += halfHeight_;	// -5000 ~ 0 ~ 5000 -> 0 ~ 10000
			w += halfWidth_;

			size_t v = h / heightGap_;	// 10000 / 1250
			size_t u = w / widthGap_;

			// 범위 넘어가면 터진다!

			return uvNodeTable_[v][u];
		}
		
		TNodeList QueryNodesByRect(SizeType uMin, SizeType vMin, SizeType uMax, SizeType vMax) const
		{
			assert(vMin <= vMax);	// 이래야 정상 작동함!!
			assert(uMin <= uMax);

			TNodeList ret;

			// vector 인덱스로 쓰기 위하여 -1 ~ 0 ~ +1 좌표계를 0 ~ 2 좌표계로 바꾼다
			vMin += halfHeight_; vMax += halfHeight_;
			uMin += halfWidth_; uMax += halfWidth_;

			size_t sv = vMin / heightGap_, ev = vMax / heightGap_;
			size_t su = uMin / widthGap_, eu = uMax / widthGap_;

			// 점과는 다르게 넘어갈 수 있으므로 클램프 쳐줘야한다
			sv = std::max<size_t>(0, sv); ev = std::min<size_t>(height_ / heightGap_ - 1, ev);
			su = std::max<size_t>(0, su); eu = std::min<size_t>(width_ / widthGap_ - 1, eu);

			for (size_t vv = sv; vv <= ev; ++vv)
				for (size_t uu = su; uu <= eu; ++uu)
					ret.emplace_front(uvNodeTable_[vv][uu]);

			return ret;
		}

		void Clear()
		{
			rootNode_.Clear();
			uvNodeTable_ = decltype(uvNodeTable_){};
			tempUVNodes_ = decltype(tempUVNodes_){};	// 생성시 사용하는 임시이긴한데 혹시 몰라서;;
		}

	protected:
		void BuildQuadTree(TNode& thisNode, SizeType startW, SizeType startH, SizeType width, SizeType height)
		{
			thisNode.TNode::CQuadTreeNode(startW, startH, startW + width, startH + height);

			auto halfW = width / 2, halfH = height / 2;
			if (halfW <= leafNodeWidth_ || halfH <= leafNodeHeight_)
			{	// 종료
				tempUVNodes_[startH][startW] = &thisNode;
				return;
			}

			// 자식들도 중심 기준의 -1 ~ 0 ~ 1 의 좌표를 갖는 사사분면을 구성하게 하기 위하여 시작포인트를 중심으로 옮겨준다
			startW += halfW;
			startH += halfH;

			// 각각 좌하단을 시작 으로 갖고, 우상단을 끝점으로 갖는 사각형이 된다!
			BuildQuadTree(thisNode.GetChild(0), startW,	startH,	halfW, halfH);	// GL 1사분면
			BuildQuadTree(thisNode.GetChild(1), startW - halfW, startH,	halfW, halfH);	// GL 2사분면
			BuildQuadTree(thisNode.GetChild(2), startW - halfW, startH - halfH, halfW, halfH);	// GL 3사분면
			BuildQuadTree(thisNode.GetChild(3), startW, startH - halfH, halfW, halfH);	// GL 4사분면
		}

		TNode* QueryNodeByPoint(TNode& thisNode, SizeType w, SizeType h) const
		{
			TNode* ret = nullptr;

			if (w >= thisNode.rect_.l_ && w < thisNode.rect_.r_ &&
				h >= thisNode.rect_.b_ && h < thisNode.rect_.t_)
			{
				if (IsLeafNode(thisNode))
				{
					ret = &thisNode;
					return ret;
				}
				else
				{
					ret = QueryByPoint(thisNode.GetChild(0), w, h);	if (ret != nullptr)	return ret;
					ret = QueryByPoint(thisNode.GetChild(1), w, h);	if (ret != nullptr)	return ret;
					ret = QueryByPoint(thisNode.GetChild(2), w, h);	if (ret != nullptr)	return ret;
					ret = QueryByPoint(thisNode.GetChild(3), w, h);	if (ret != nullptr)	return ret;
				}
			}

			return ret;
		}

		size_t CalculateDepth(TNode& thisNode)
		{
			if (IsLeafNode(thisNode))
			{
				return 0;
			}
			else
			{
				return 1 + CalculateDepth(thisNode.GetChild(0));
			}
		}

		bool IsLeafNode(TNode& thisNode)
		{
			return thisNode.HasChildren() == false;
		}

	protected:
		std::string treeName_;
		SizeType width_, height_, leafNodeWidth_, leafNodeHeight_, widthGap_, heightGap_, halfWidth_, halfHeight_;
		size_t treeDepth_ = 0;
		
		TNode rootNode_;
		CVector<CVector<TNode*>> uvNodeTable_;	// [v][u]

		// 임시 값
		std::map<SizeType, std::map<SizeType, TNode*>> tempUVNodes_;	// [v][u]
	};

	template<typename ContainerT, typename SizeType>
	class COcTree
	{
	public:
		using TContainer = ContainerT;
		using TValue = typename ContainerT::value_type;
		using TValueIterator = typename ContainerT::iterator;

		using TSubTree = CQuadTree<ContainerT, SizeType>;
		using TNode = typename TSubTree::TNode;

	public:
		COcTree() {}

		void Create(const std::string& treeName, SizeType xWidth, SizeType yWidth, SizeType zWidth, SizeType leafNodeWidth)
		{
			assert(xWidth == zWidth);	// 이 두 값이 같지 않으면 테이블을 통한 판별이 어려워진다 - QueryValuesByCube( 여기의 Pred 함수 구현을 봐라, x 가 같은 것을 가지고 교차 여부 체크를 하기 때문이다
			assert(xWidth == yWidth);	// 현재 비균등은 제대로 작동하지 않는 듯 하다, 필요시 나중에 고치자;;

			treeName_ = treeName;

			xyQTree_.Create("xy", xWidth, yWidth, leafNodeWidth, leafNodeWidth);
			zxQTree_.Create("zx", zWidth, xWidth, leafNodeWidth, leafNodeWidth);
		}

		auto HardQuery(const TValue& findingValue) const
		{
			auto xyNodes = xyQTree_.HardQuery(findingValue);
			auto zxNodes = zxQTree_.HardQuery(findingValue);

			return std::make_pair(xyNodes, zxNodes);
		}

		TNode* QueryNodeByXY(SizeType x, SizeType y)
		{
			return xyQTree_.QueryNodeByPoint(x, y);
		}
		TNode* QueryNodeByZX(SizeType z, SizeType x)
		{
			return zxQTree_.QueryNodeByPoint(z, x);
		}

		auto QueryValuesByCube(SizeType sx, SizeType sy, SizeType sz, SizeType ex, SizeType ey, SizeType ez) const
		{
			assert(sx <= ex);
			assert(sy <= ey);
			assert(sz <= ez);	// 이래야 제대로 작동함

			auto xyNodeList = xyQTree_.QueryNodesByRect(sx, sy, ex, ey);
			auto zxNodeList = zxQTree_.QueryNodesByRect(sz, sx, ez, ex);

			auto intersectNodePred = [](TNode* xyNode, TNode* zxNode)
				{
					return xyNode->rect_.leftBottom_.xy_[0] == zxNode->rect_.leftBottom_.zx_[1];	// x 가 같으면 교집합
				};

			// 이제 x값이 같은 노드에서 교집합인 값들을 찾는다
			CVector<TValue> ret;
			for (auto xyNode : xyNodeList)
			{
				for (auto zxNode : zxNodeList)
				{
					if (intersectNodePred(xyNode, zxNode) == true)
					{
						const TContainer& dataXY = xyNode->GetContainer();
						const TContainer& dataZX = zxNode->GetContainer();
						std::set_intersection(dataXY.begin(), dataXY.end(), dataZX.begin(), dataZX.end(), std::back_inserter(ret));
					}
				}
			}

			// ::set_intersection( 이걸 못쓴다 - stl 에서 내부적으로 Pred 의 l, r 을 뒤집어서도 같은지 체크를 하는데 여기서 어서트 난다! 우리는 지금 이렇게 뒤집지 않은 상태에서만 체크를 해야하기 때문이다!!
			//std::set_intersection(xyNodeList.begin(), xyNodeList.end(), zxNodeList.begin(), zxNodeList.end(), std::back_inserter(intersectNodes),
			//	[](TNode* xyNode, TNode* zxNode)
			//	{
			//		return xyNode->rect_.leftBottom_.xy_[0] == zxNode->rect_.leftBottom_.zx_[1];	// x 가 같으면 교집합
			//	});

			return ret;
		}

		auto QueryValuesByPoint(SizeType x, SizeType y, SizeType z) const
		{
			CVector<TValue> intersect;

			const TNode* xyNode = QueryNodeByXY(x, y);
			const TNode* zxNode = QueryNodeByZX(z, x);

			if (xyNode && zxNode)
			{
				const TContainer& dataXY = xyNode->GetContainer();
				const TContainer& dataZX = zxNode->GetContainer();
				std::set_intersection(dataXY.begin(), dataXY.end(), dataZX.begin(), dataZX.end(), std::back_inserter(intersect));
			}

			return intersect;
		}

		void Clear()
		{
			xyQTree_.Clear();
			zxQTree_.Clear();
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// glm::vec3 를 이용하는 경우의 지원
		template<typename GLMVEC3>
		auto QueryValuesByPoint(const GLMVEC3& point) const
		{
			return QueryValuesByPoint(point.x, point.y, point.z);
		}

		template<typename GLMVEC3>
		auto QueryValuesByCube(const GLMVEC3& lb, const GLMVEC3& rt) const
		{
			return QueryValuesByCube(lb.x, lb.y, lb.z, rt.x, rt.y, rt.z);
		}

		template<typename GLMVEC3>
		bool AddByPoint(const TValue value, const GLMVEC3& point)
		{
			TNode* xyNode = QueryNodeByXY(point.x, point.y);
			TNode* zxNode = QueryNodeByZX(point.z, point.x);

			if (xyNode && zxNode)
			{
				TContainer& dataXY = xyNode->GetContainer();
				dataXY.insert(value);

				TContainer& dataZX = zxNode->GetContainer();
				dataZX.insert(value);
				return true;
			}

			return false;
		}

		template<typename GLMVEC3>
		bool RemoveByPoint(const TValue value, const GLMVEC3& point)
		{
			TNode* xyNode = QueryNodeByXY(point.x, point.y);
			TNode* zxNode = QueryNodeByZX(point.z, point.x);

			if (xyNode && zxNode)
			{
				TContainer& dataXY = xyNode->GetContainer();
				dataXY.erase(value);

				TContainer& dataZX = zxNode->GetContainer();
				dataZX.erase(value);
				return true;
			}

			return false;
		}

	protected:
		std::string treeName_;
		TSubTree xyQTree_, zxQTree_;
	};
}
