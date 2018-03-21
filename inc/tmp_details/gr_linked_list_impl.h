//构造函数
// m为链表的最大长度
#ifndef GR_LNK_LST_IMPL_H
#define GR_LNK_LST_IMPL_H

template <typename T>
GRLinkedList<T>::GRLinkedList(u32 max)
{
	_maxsize=max;
	_size=0;
	_head=NULL,_tail=NULL;
}

template <typename T>
GRLinkedList<T>::GRLinkedList(GRLinkedList<T>& l)
{
	typename GRLinkedList<T>::iterator it = l.head();
	while(it++)
	{
		addToTail(*it);
	}
}

//析构函数
//清空整个链表，释放内存
template <typename T>
GRLinkedList<T>::~GRLinkedList()
{
	clear();
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::tail() const
{
	return typename GRLinkedList<T>::iterator(_tail);
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::head() const
{
	return typename GRLinkedList<T>::iterator(_head);
}

/*移除表尾/表头节点，将该节点的值读取到d中，并返回成功与否*/
template <typename T>
bool GRLinkedList<T>::removeFromTail(T& d)
{
	if(_tail && _size)
	{
		d = _tail->_data;//读取表尾的值	
		_node* tmp = _tail;//暂存当前的表尾指针
		_tail = _tail->_last;//将表尾指针改为上一个节点的指针
		if(_size != 1)//如果移除当前节点后表不为空
			_tail->_next = NULL;//把表尾指针的下一个节点指针置为0
		delete tmp;//释放被移除的节点的内存
		--_size;//链表长度减一
		return true;
	}
	else return false;
}

template <typename T>
bool GRLinkedList<T>::removeFromHead(T& d)
{
	if(_head && _size)
	{
		d = _head->_data;
		_node* tmp = _head;
		_head = _head->_next;
		if(_size != 1)//如果移除当前节点后表不为空
			_head->_last = NULL;//将下一节点指向上级节点的指针置0
		delete tmp;
		--_size;
		return true;
	}
	else return false;
}

/*在表尾/表头插入值d，并返回成功与否*/
template <typename T>
bool GRLinkedList<T>::addToTail(const T& d)
{
	if (_size+1 > _maxsize)//如果长度大于设定最大长度，返回false
		return false;
	if(isEmpty())//如果是空链表
		_head = _tail = new _node(d,NULL,NULL);//建立一个新的节点
	else
	{
		_tail->_next = new _node(d,_tail,NULL);//建立一个新节点并且将它的指针赋给上一节点
		_tail = (_tail->_next);//尾节点指针指向新建的节点
	}
	++_size;//链表长度自增
	return true;
}

template <typename T>
bool GRLinkedList<T>::addToHead(const T& d)
{
	if (_size+1 > _maxsize)
		return false;
	if(isEmpty())
		_head = _tail = new _node(d,NULL,NULL);
	else
	{
		_head->_last =  new _node(d,NULL,_head);
		_head = (_head->_last);
	}
	++_size;
	return true;
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::at(u32 index)
{
	if(index+1 > _size)return iterator(NULL);
	
	_node* tmp = _head;
	for(u32 i=0;i<index;++i)
	{
		tmp = tmp->_next;
	}
	return iterator(tmp);
}

template <typename T>
T& GRLinkedList<T>::operator[](int index)
{
	return *at(index);
}

template <typename T>
GRLinkedList<T>& GRLinkedList<T>::operator=(GRLinkedList<T>& l)
{
	clear();
	typename GRLinkedList<T>::iterator it = l.head();
	while(it)
	{
		addToTail(*it++);
	}
	return *this;
}

/*清空整个链表*/
template <typename T>
void GRLinkedList<T>::clear()
{
	T d;
	while(removeFromHead(d));//历遍链表，删除所有数据并释放内存
}

/*链表反序*/
template <typename T>
void GRLinkedList<T>::reverse()
{
	if(isEmpty())return;	

	//cur的步进表达式是cur = cur->_last而不是next，因为链表反序以后原来的下一个现在变成了上一个了。
	_node* mid;
	for(_node* cur = _head;cur!=0;cur = cur->_last)
	{
		mid = cur->_next;
		cur->_next = cur->_last;
		cur->_last = mid;
	}
	mid = _head;//swap()
	_head = _tail;
	_tail = mid;
}

/****迭代器实现****/

//返回该迭代位置有没有上一个节点/下一个节点
template <typename T>
bool GRLinkedList<T>::iterator:: hasNext()
{
	return m_node->_next != NULL;
}

template <typename T>
bool GRLinkedList<T>::iterator:: hasLast()
{
	return m_node->_last != NULL;
}

//返回上一个节点或下一个节点的迭代器
template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::iterator::next()
{
	if(!hasNext())return iterator(NULL);
	
	return iterator(m_node->_next);
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::iterator::last()
{
	if(!hasLast())return iterator(NULL);
	
	return iterator(m_node->_last);
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::iterator::operator++()
{
	m_node = m_node->_next;
	return *this;
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::iterator::operator--()
{
	m_node = m_node->_last;
	return *this;
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::iterator::operator++(int)
{
	typename GRLinkedList<T>::iterator it(m_node);
	m_node = m_node->_next;
	return it;
}

template <typename T>
typename GRLinkedList<T>::iterator GRLinkedList<T>::iterator::operator--(int)
{
	typename GRLinkedList<T>::iterator it(m_node);
	m_node = m_node->_last;
	return it;
}

#endif
