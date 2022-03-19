//#pragma once
//
//namespace Concurrent
//{
//	using T = int;
//	// template<typename T>
//	class Stack
//	{
//	public:
//		Stack();
//		~Stack()noexcept = default;
//
//		Stack(const Stack&) = delete;
//		Stack& operator=(const Stack&) = delete;
//
//		Stack(Stack&&)noexcept = delete;
//		Stack& operator=(Stack&&)noexcept = delete;
//
//	public:
//		void Push(const T& Value) & noexcept;
//		void Push(T&& Value) & noexcept;
//		[[nodiscard]] std::optional<T> TryPop() & noexcept;
//
//	private:
//		struct Node
//		{
//		private:
//			friend class Stack;
//
//			Node(T&& Data) noexcept :
//				Data{ move(Data) }, Next{ nullptr }
//			{}
//			Node(const T& Data) :
//				Data{ Data }, Next{ nullptr }
//			{}
//
//			T Data;
//			Node* Next;
//		};
//
//	private:
//		void _PushLockFree(Node* const NewNode) & noexcept
//		{
//			/*if value == expected
//				expected = value
//				value = desired
//				return true
//			else
//				expected = value
//				return false*/
//			NewNode->Next = Top.load();
//			while (false == Top.compare_exchange_weak(NewNode->Next, NewNode))
//			{
//				__noop;
//			}
//		}
//
//		void Pending(Node* DeleteNode) & noexcept
//		{
//			//	여기서 스레드 혼자일때만 삭제처리를 하는 이유는 TryPop 메서드 상단부가
//			//	동시에 실행되어서 같은 Head 를 참조하는 경우 여기서 Delete 해버리면
//			//	같은 Head를 참조하던 스레드는 CAS 연산중 next 를 참조하는 순간 댕글링 포인터이기 때문에
//			//	Crash 나는 상황을 방지한다
//			if(1 == PopCount)
//			{
//				Node* PendingHead = PendingList.exchange(nullptr);
//
//				if(0 == PopCount.fetch_add(-1))
//				{
//					DeleteNodes(PendingHead);
//				}
//				else if (PendingHead)
//				{
//					ChainPendingNodeList(PendingHead);
//				}
//
//				delete DeleteNode;
//			}
//			else
//			{
//				ChainPendingNode(DeleteNode);
//				PopCount.fetch_add(-1);
//			}
//		}
//
//		void ChainPendingNodeList(Node* const First, Node* const Last) & noexcept
//		{
//			Last->Next = PendingList;
//
//			while(false == PendingList.compare_exchange_weak(Last->Next, First))
//			{
//				__noop;
//			}
//		}
//
//		void ChainPendingNode(Node* Target) & noexcept
//		{
//			ChainPendingNodeList(Target, Target);
//		}
//
//		void ChainPendingNodeList(Node* Target) & noexcept
//		{
//			Node* Last = Target;
//			while (Last->Next)
//				Last = Last->Next;
//
//			ChainPendingNodeList(Target, Last);
//		}
//		
//		static void DeleteNodes(Node* DeleteTop) noexcept
//		{
//			while(DeleteTop)
//			{
//				auto const NextDelete = DeleteTop->Next;
//				delete DeleteTop;
//				DeleteTop = NextDelete;
//			}
//		}
//
//	private:
//		atomic<Node*> Top;
//		atomic<uint64> PopCount; // 현재 Pop을 하고 있는 쓰레드의 개수
//		atomic<Node*> PendingList;	 // 삭제 되어야 할 노드들
//
//	};
//}
