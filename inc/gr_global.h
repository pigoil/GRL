#ifndef GR_GLOBAL_H
#define GR_GLOBAL_H

#define STM32F10X_HD

#include "stm32f10x.h"

#include "gr_user.h"//用户设定

/****宏定义****/
#define GRCFunc extern "C" //C方式编译函数
#ifndef NULL
#define NULL 0 //定义NULL
#endif

/****伪随机数生成函数****/
uc16 RAND_MAX = 32767;//随机数最大值 最小值为0
void grSRand(u32 seed=SysTick->VAL);//设置随机种子
u16 grRand();//获取随机数

/****位带操作宏函数****/
inline u32 BITBAND(u32 addr,u32 bitnum)
{
	return (addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2);
}
inline volatile u32* BIT_ADDR(u32 addr,u32 bitnum)
{
	return (volatile unsigned long*)BITBAND(addr,bitnum);
}

/****简单数值函数****/
inline s32 grRound(double d){return (s32)(d+0.5);}//四舍五入
inline s32 grAbs(s32 d){return d<0 ? -d : d;}//绝对值
inline double grAbs(double d){return d<0 ? -d : d;}
template <typename T> void grSwap(T& t1,T& t2);//交换两个数据类型相同的数
template <typename T> T& grMax(T& t1,T& t2);//返回两个数据类型相同的数中较大的一个，相等时返回第一个数的引用
template <typename T> T& grMin(T& t1,T& t2);//返回两个数据类型相同的数中较小的一个，相等时返回第一个数的引用
template <typename T> T& grMax(T* t,u32 cnt);//返回一个数组中最大一个数的引用
template <typename T> T& grMin(T* t,u32 cnt);//返回一个数组中最小一个数的引用
#include "tmp_details/gr_tool_impl.h" //包含模板实现文件

/****简单的双向链表****/
template <typename T>
class GRLinkedList;

template <typename T>
class GRLinkedList
{
private:
	class _node
	{
	public:
			//
	private:
		friend class GRLinkedList;//声明为链表类的友元
		T _data;
		_node* _last;
		_node* _next;
		
		_node(T d,_node* l,_node* n):
		_data(d),
		_last(l),
		_next(n){}
	};

public:
	class iterator
	{
	public:
		friend class GRLinkedList<T>;
		iterator();
		iterator next();
		iterator last();
		bool hasNext();
		bool hasLast();
		bool isValid(){return m_node != NULL;}//当前迭代器是否合法
		iterator operator++();
		iterator operator--();
		iterator operator++(int);
		iterator operator--(int);
		T& operator*(){return m_node->_data;}
		operator bool(){return isValid();}
	private:
		iterator(GRLinkedList::_node* n):
		m_node(n){};
		typename GRLinkedList::_node* m_node;
	};
	friend class iterator;
	
	GRLinkedList<T>(u32 max=(u32)-1);
	GRLinkedList<T>(GRLinkedList<T>&);
	~GRLinkedList();	
	iterator tail() const;
	iterator head() const;
	iterator at(u32 index);
	T& operator[](int index);
	GRLinkedList<T>& operator=(GRLinkedList<T>&);

	bool removeFromTail(T&);
	bool removeFromHead(T&);
	bool addToTail(const T& d);
	bool addToHead(const T&);
	
	void 	clear();//清除整个链表
	u32 	size(){return _size;}//元素个数
	bool 	isEmpty(){return _size==0;}//测试链表是否空
	void 	reverse();
	
private:
	_node *_head,*_tail;
	u32 _size,_maxsize;
};

#include "tmp_details/gr_linked_list_impl.h"//包含模板实现文件

#endif
