#pragma once
template <typename T>
class ComPTR
{
	public :
		ComPTR():p(nullptr){}
		ComPTR(const ComPTR<T>& lp )
		{
			p = lp.p;
			if (p != NULL)
				p->AddRef();
		}
		ComPTR(T* lp )
		{
			p = lp;
			if (p != NULL)
				p->AddRef();
		}
		~ComPTR()
		{
			if ( p )
				Release();	
		}
		bool CreateInstance(const IID& clsid)
		{					
			return ( S_OK == ::CoCreateInstance( clsid, NULL, CLSCTX_INPROC,  __uuidof(T), (void**)&p ));
		}
		bool CreateInstance( LPCWSTR LPCLSID )
		{			
			CLSID clsid;
			if ( S_OK == CLSIDFromProgID( obj, &LPCLSID ))
			{
				return ( S_OK == ::CoCreateInstance( clsid, NULL, CLSCTX_INPROC,  __uuidof(T), (void**)&p ));
			}
			return false;
		}
		operator T*() const throw(){ return p;}
		T& operator*() const{_ASSERT(p!=NULL);return *p;}
		T* operator->() const throw(){_ASSERT(p!=NULL);	return p;}
		T** operator&() throw()
		{
			_ASSERT(p==NULL);
			return &p;
		}
		T* operator=( const ComPTR<T>& lp )
		{
			if(!(*this==lp))
			{
				ComPTR(lp).Swap(*this);
			}
			return *this;
		}
		T* operator=(T* lp) throw()
		{
			if(*this!=lp)
			{
				ComPTR(lp).Swap(*this);
			}
			return *this;
		}
		bool operator==( const ComPTR& src ) const 
		{			
			return ( p == src.p );
		}
		bool operator==( T* lp ) const 
		{			
			return ( p == lp );
		}

		ULONG Release()
		{
			T* pTemp = p;
			if (pTemp)
			{
				p = NULL;
				return pTemp->Release();
			}
			return 0;
		}
		T* Detach() throw()
		{
			T* pt = p;
			p = NULL;
			return pt;
		}
		void Attach(T* p2) throw()
		{
			if (p)
			{
				ULONG ref = p->Release();
				_ASSERT(ref != 0 || p2 != p);
			}
			p = p2;
		}	
	protected :
		void Swap(ComPTR<T>& other)
		{
			T* pTemp = p;
			p = other.p;
			other.p = pTemp;
		}	
	public :
		T* p;
};