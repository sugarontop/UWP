#pragma once

namespace V4 {

template <typename T>
class faststack
{
	public : 
		faststack( int maxcnt )
		{
			head_ = new T[maxcnt];
			ZeroMemory( head_, sizeof(T)*maxcnt );
			id_ = -1;
			maxcnt_ = maxcnt;
		}
		~faststack()
		{
			delete [] head_;
		}
		void push( T t )
		{
			_ASSERT( id_+1 < maxcnt_ );
			head_[++id_] = t;
		}
		T top()
		{
			_ASSERT( id_ > -1 );
			return head_[id_];
		}
		bool empty()
		{
			return ( id_ < 0 );
		}
		void pop()
		{
			head_[id_] = NULL;
			--id_;

			_ASSERT( id_ >= -1 );
		}
		bool include( T t )
		{
			if ( empty()) 
				return false;

			for (int i = 0; i < size(); i++)
			{
				if ( t == head_[i] )
					return true;
			}
			return false;
		}

		int size(){ return id_+1; }
		T* head(){ return head_; }

	protected :
		T* head_;
		int id_;
		int maxcnt_;

};

};
