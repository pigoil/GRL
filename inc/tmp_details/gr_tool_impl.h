//交换两个数据类型相同的数
//对象交换需注意深度复制问题
template <typename T>
void grSwap(T& t1,T& t2)
{
	T tmp=t1;
	t1=t2;
	t2=tmp;
}

//返回两个数据类型相同的数中较大的一个，相等时返回第一个数的引用
template <typename T>
T& grMax(T& t1,T& t2)
{
	return t2 > t1 ? t2 : t1;
}

//返回两个数据类型相同的数中较小的一个，相等时返回第一个数的引用
template <typename T>
T& grMin(T& t1,T& t2)
{
	return t2 < t1 ? t2 : t1;
}

//返回数组中最大的
//后期应该考虑容器问题
template <typename T>
T& grMax(T* t,u32 cnt)
{
	u32 max_index = 0;
	while(cnt--)
	{
		if(*(t+cnt) > *(t+max_index))
			max_index = cnt;
	}
	return *(t+max_index);
}

template <typename T>
T& grMin(T* t,u32 cnt)
{
	u32 min_index = 0;
	while(cnt--)
	{
		if(*(t+cnt) < *(t+min_index))
			min_index = cnt;
	}
	return *(t+min_index);
}

