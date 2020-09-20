#pragma once

#include <map>
#include <array>
#include <vector>
#include <functional>
#include <string>

namespace SMGE
{
	template<typename T, typename SizeType> class CQuadTree;
	template<typename T, typename SizeType> class COcTree;

	template<typename T>
	struct SPoint2D
	{
		friend CQuadTree;
		friend COcTree;

		union
		{
			T xy_[2];	// xy ������� �ٷ� ��
			T zx_[2];	// zx ������� �ٷ� ��
			T yz_[2];	// yz ������� �ٷ� ��
			T uv_[2];	// uv ������� �ٷ� ��

			T lb_[2];	// ���ϴ��� ����, ������ ���� ��ǥ�� 
			T rt_[2];
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
	struct SRect
	{
		friend CQuadTree;
		friend COcTree;

		SRect() : l_(leftBottom_.uv_[0]), b_(leftBottom_.uv_[1]), r_(rightTop_.uv_[0]), t_(rightTop_.uv_[1])
		{
		}
		SRect(T l, T b, T r, T t) : SRect()
		{
			l_ = l;
			b_ = b;
			r_ = r;
			t_ = t;
		}

		SPoint2D<T> leftBottom_, rightTop_;
		T& l_, &b_, &r_, &t_;

		bool operator==(const SRect<T>& r)
		{
			return leftBottom_ == r.leftBottom_ && rightTop_ == r.rightTop_;
		}
		bool operator!=(const SRect<T>& r)
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
		using TSize = SizeType;
		using TChild = CQuadTreeNode<ContainerT, SizeType>;
		using TChildren = std::array<TChild, 4>;
		using TContainer = ContainerT;
		using TValue = typename ContainerT::value_type;

	public:
		CQuadTreeNode()
		{
		}
		~CQuadTreeNode()
		{
			if(children_) delete children_;	// c2460 ���� ������ �̷��� �Ѵ�
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

		TChild& GetChild(size_t i) { AllocChildren(); return (*children_)[i]; }
		const TChild& GetChild(size_t i) const { AllocChildren(); return (*children_)[i]; }

		bool operator==(const CQuadTreeNode& r)
		{
			return rect_ == r.rect_;
		}
		bool operator!=(const CQuadTreeNode& r)
		{
			return !operator==(r);
		}

	protected:
		void AllocChildren()
		{
			if(children_ == nullptr)
				children_ = new TChildren;
		}

	protected:
		ContainerT container_;
		SRect<SizeType> rect_;

		TChildren* children_ = nullptr;
	};

	// ��� ��ǥ�� ��ǥ��� GL ������ ��ǥ�踦 ����Ѵ�.
	template<typename ContainerT, typename SizeType>
	class CQuadTree
	{
		friend COcTree;

	public:
		using TSize = SizeType;
		using TContainer = ContainerT;
		using TValue = typename ContainerT::value_type;

		using TNode = CQuadTreeNode<ContainerT, SizeType>;

	public:
		CQuadTree(const std::string& treeName, SizeType width, SizeType height, SizeType leafNodeWidth, SizeType leafNodeHeight)
		{
			treeName_ = treeName;

			width_ = width;
			height_ = height;
			leafNodeWidth_ = leafNodeWidth;
			leafNodeHeight_ = leafNodeHeight;

			// GL ��и鿡 �µ��� -1 ~ 0 ~ 1 ��и� ��ǥ��� �ٲ㼭 �־��ش�
			// �� ����Ʈ���� �߽��� GL ������ǥ 0,0�� ��ġ�ϰ� �Ǵ� ���̴�
			BuildQuadTree(rootNode_, width_/-2, height_/-2, width_, height_);

			// ���� ����
			treeDepth_ = CalculateDepth(rootNode_);
			widthGap_ = width_ / std::pow(2, treeDepth_);
			heightGap_ = height_ / std::pow(2, treeDepth_);
			halfWidth_ = width_ / 2;
			halfHeight_ = height_ / 2;

			// ���������� ������ġ�� ������ 2���� ���̺�� �����
			const auto vSize = tempUVNodes_.size();
			uvNodeTable_.reserve(vSize);
			for (auto vIT : tempUVNodes_)	// [v] map
			{
				const auto& tempUNodes = vIT.second;	// [u] map
				const auto uSize = tempUNodes.size();
				
				uvNodeTable_.emplace_back();	// [v]  �Ҵ�
				
				auto& uNodes = *uvNodeTable_.rbegin();
				uNodes.reserve(uSize);
				for (auto uIT : tempUNodes)
				{
					uNodes.emplace_back(uIT.second);	// [u] �Ҵ�
				}
			}
			tempUVNodes_.clear();
		}

		TNode* QueryNodeByPoint(SizeType w, SizeType h)
		{
			//return QueryNodeByPoint(rootNode_, w, h);	// Ʈ�� ������ ���� ����

			// ���̺��� ���� ó��
			// vector �ε����� ���� ���Ͽ� -1 ~ 0 ~ +1 ��ǥ�踦 0 ~ 2 ��ǥ��� �ٲ۴�
			h += halfHeight_;	// -5000 ~ 0 ~ 5000 -> 0 ~ 10000
			w += halfWidth_;

			auto v = h / heightGap_;	// 10000 / 1250	// ���� - ������ �Ⱦ��� ����� ���� �� ����!
			auto u = w / widthGap_;

			// ���� �Ѿ�� ������!

			return uvNodeTable_[v][u];
		}
		
		auto QueryNodesByRect(SizeType uMin, SizeType vMin, SizeType uMax, SizeType vMax)
		{
			assert(uMin <= uMax);
			assert(vMin <= vMax);	// �̷��� ���� �۵���!!

			std::forward_list<TNode*> ret;

			// vector �ε����� ���� ���Ͽ� -1 ~ 0 ~ +1 ��ǥ�踦 0 ~ 2 ��ǥ��� �ٲ۴�
			vMin += halfHeight_; vMax += halfHeight_;
			uMin += halfWidth_; uMax += halfWidth_;

			auto sv = vMin / heightGap_, ev = vMax / heightGap_;
			auto su = uMin / widthGap_, eu = uMax / widthGap_;

			// ������ �ٸ��� �Ѿ �� �����Ƿ� Ŭ���� ������Ѵ�
			sv = std::max(0, sv); ev = std::min(height_ / heightGap_, ev);
			su = std::max(0, su); eu = std::min(width_ / widthGap_, eu);

			if (sv == ev) ev++;
			if (su == eu) eu++;

			for (auto vv = sv; vv < ev; ++vv)
				for (auto uu = su; uu < eu; ++uu)
					ret.emplace_front(uvNodeTable_[vv][uu]);

			return ret;
		}

	protected:
		void BuildQuadTree(TNode& thisNode, SizeType startW, SizeType startH, SizeType width, SizeType height)
		{
			thisNode.TNode::CQuadTreeNode(startW, startH, startW + width, startH + height);

			auto halfW = width / 2, halfH = height / 2;
			if (halfW <= leafNodeWidth_ || halfH <= leafNodeHeight_)
			{	// ����
				tempUVNodes_[startH][startW] = &thisNode;
				return;
			}

			// �ڽĵ鵵 �߽� ������ -1 ~ 0 ~ 1 �� ��ǥ�� ���� ���и��� �����ϰ� �ϱ� ���Ͽ� ��������Ʈ�� �߽����� �Ű��ش�
			startW += halfW;
			startH += halfH;

			// ���� ���ϴ��� ���� ���� ����, ������ �������� ���� �簢���� �ȴ�!
			BuildQuadTree(thisNode.GetChild(0), startW,	startH,	halfW, halfH);	// GL 1��и�
			BuildQuadTree(thisNode.GetChild(1), startW - halfW, startH,	halfW, halfH);	// GL 2��и�
			BuildQuadTree(thisNode.GetChild(2), startW - halfW, startH - halfH, halfW, halfH);	// GL 3��и�
			BuildQuadTree(thisNode.GetChild(3), startW, startH - halfH, halfW, halfH);	// GL 4��и�
		}

		TNode* QueryNodeByPoint(TNode& thisNode, SizeType w, SizeType h)
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
		std::vector<std::vector<TNode*>> uvNodeTable_;	// [v][u]

		// �ӽ� ��
		std::map<SizeType, std::map<SizeType, TNode*>> tempUVNodes_;	// [v][u]
	};

	template<typename ContainerT, typename SizeType>
	class COcTree
	{
	public:
		using TSize = SizeType;
		using TContainer = ContainerT;
		using TValue = typename ContainerT::value_type;

		using TSubTree = CQuadTree<ContainerT, SizeType>;
		using TNode = typename TSubTree::TNode;

	public:
		COcTree(const std::string& treeName, SizeType xWidth, SizeType yWidth, SizeType zWidth, SizeType leafNodeWidth) :
			xyQTree_("xy", xWidth, yWidth, leafNodeWidth, leafNodeWidth),
			zxQTree_("zx", zWidth, xWidth, leafNodeWidth, leafNodeWidth)
		{
			assert(xWidth == zWidth);	// �� �� ���� ���� ������ ���̺��� ���� �Ǻ��� ��������� - QueryValuesByCube( ������ Pred �Լ� ������ ����, x �� ���� ���� ������ ���� ���� üũ�� �ϱ� �����̴�

			treeName_ = treeName;
		}

		TNode* QueryNodeByXY(SizeType x, SizeType y)
		{
			return xyQTree_.QueryNodeByPoint(x, y);
		}
		TNode* QueryNodeByZX(SizeType z, SizeType x)
		{
			return zxQTree_.QueryNodeByPoint(z, x);
		}

		auto QueryValuesByCube(SizeType sx, SizeType sy, SizeType sz, SizeType ex, SizeType ey, SizeType ez)
		{
			assert(sx <= ex);
			assert(sy <= ey);
			assert(sz <= ez);	// �̷��� ����� �۵���

			auto xyNodeList = xyQTree_.QueryNodesByRect(sx, sy, ex, ey);
			auto zxNodeList = zxQTree_.QueryNodesByRect(sz, sx, ez, ex);

			auto intersectNodePred = [](TNode* xyNode, TNode* zxNode)
				{
					return xyNode->rect_.leftBottom_.xy_[0] == zxNode->rect_.leftBottom_.zx_[1];	// x �� ������ ������
				};

			// ���� x���� ���� ��忡�� �������� ������ ã�´�
			std::vector<TValue> ret;
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

			// ::set_intersection( �̰� ������ - stl ���� ���������� Pred �� l, r �� ������� ������ üũ�� �ϴµ� ���⼭ �Ʈ ����! �츮�� ���� �̷��� ������ ���� ���¿����� üũ�� �ؾ��ϱ� �����̴�!!
			//std::set_intersection(xyNodeList.begin(), xyNodeList.end(), zxNodeList.begin(), zxNodeList.end(), std::back_inserter(intersectNodes),
			//	[](TNode* xyNode, TNode* zxNode)
			//	{
			//		return xyNode->rect_.leftBottom_.xy_[0] == zxNode->rect_.leftBottom_.zx_[1];	// x �� ������ ������
			//	});

			return ret;
		}

		auto QueryValuesByPoint(SizeType x, SizeType y, SizeType z)
		{
			std::vector<TValue> intersect;

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

	protected:
		std::string treeName_;
		TSubTree xyQTree_, zxQTree_;
	};
}
