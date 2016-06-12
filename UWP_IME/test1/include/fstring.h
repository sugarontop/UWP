#pragma once
#include <string>
#include <sstream>
namespace V4 {

class FString : public std::wstring
{
	public :
		FString(){}
		FString( LPCWSTR s ):std::wstring(s){}
		FString( LPCWSTR s, LPCWSTR e ):std::wstring(s,e){}
		FString( const std::wstring& s ):std::wstring(s){}
		FString( LPCSTR ansiText )
		{
			int cchWideChar = ::MultiByteToWideChar(CP_ACP, 0, ansiText, -1, NULL, 0);
			LPWSTR lpw = new WCHAR [cchWideChar];						
			int nUnicodeCount = ::MultiByteToWideChar(CP_ACP, 0, ansiText, -1, lpw, cchWideChar);
			this->FString::FString( lpw );			
			delete [] lpw;		
		}
		FString( BSTR s ):std::wstring((LPCWSTR)s){}

		// && 右辺参照値
		FString( std::wstring&& s ){ swap(s); }
		FString& operator=(std::wstring&& s ){ swap(s);	return *this;}

#ifdef _INC_COMUTIL		
		operator _bstr_t() const{ return _bstr_t(c_str()); } // comuti.h
#endif
		operator LPCWSTR() const{ return (LPCWSTR)c_str(); } 
		FString& operator =( const  std::wstring& s ){ this->std::wstring::operator =(s); return *this; }
		// FString& operator =( const LPCSTR ansitext ){ this->FString::FString(ansitext); return *this; }
		FString& operator =( const LPCWSTR cstext ){ this->std::wstring::operator =(cstext); return *this; 	}
		FString& operator =( const BSTR cstext ){ this->std::wstring::operator=((LPCWSTR)cstext); return *this; 	}

		WCHAR operator[]( int idx ) const { return at(idx); }

		static  std::wstring Format( LPCWSTR fm, ... )
		{
			 std::wstring rval;

			if ( fm )
			{
				va_list ap;
				va_start(ap, fm);			
				size_t len = _vscwprintf(fm,ap)+1;

				WCHAR* buffer = new WCHAR[len];
				int nWritten = _vswprintf_l(buffer,len, fm,0,ap);

				if (nWritten > 0)
				{					
					buffer[len-1] = 0;
					rval = buffer;
				}
				
				va_end(ap);
				
				delete [] buffer;
			}
			return rval;
		}
		
		static void Trace( LPCWSTR fm, ... )
		{
			if ( fm )
			{
				va_list ap;
				va_start(ap, fm);			
				size_t len = _vscwprintf(fm,ap)+1;

				
				WCHAR* b = new WCHAR[len];
				WCHAR* buffer = b;


				int nWritten = _vswprintf_l(buffer,len, fm,0,ap);

				if (nWritten > 0)
				{					
					buffer[len-1] = 0;
					
				}
				
				va_end(ap);
				
				::OutputDebugString( buffer );

				delete [] b;
			}
		}

		FString Left( int len ) const
		{
			return this->substr( 0, len );
		}
		FString Right( int len ) const
		{
			int a = length() - len;
			if ( a < 0 )
			{
				return *this;
			}
			return this->substr( a, len );
		}
		void ToLower()
		{			
			_ASSERT( c_str() );
			auto e = _wcslwr_s( (WCHAR*)c_str(), length() + 1);
		}
		void ToUpper()
		{
			_ASSERT( c_str() );
			auto e = _wcsupr_s( (WCHAR*)c_str(), length() + 1);
		}

		// lengthにはnull文字分が含まれる。deleteが必要
		LPSTR ToUtf8( int *length )		
		{
			LPCWSTR vs = (LPCWSTR)c_str();
			int cchMultiByte = ::WideCharToMultiByte(CP_UTF8, 0, vs,this->length(), NULL, 0, NULL, NULL);
			LPSTR lpa = new CHAR [cchMultiByte+1];
			int nMultiCount = ::WideCharToMultiByte(CP_UTF8, 0, vs, this->length(), lpa, cchMultiByte, NULL, NULL);
			lpa[cchMultiByte] = 0;
			
			*length = nMultiCount;
			return lpa;
			
		}

		// encryptの場合、cchがnullで終わらない場合があるためlen指定を追加
		void FromUtf8( LPCSTR cch, int len=-1 )
		{
			if ( len == -1 )
				len = fstrlenA(cch);
			
			int cnt = ::MultiByteToWideChar(CP_UTF8, 0, cch, len, NULL, 0);			
			
			WCHAR* lpa = new WCHAR [cnt+1];
			int nMultiCount = ::MultiByteToWideChar(CP_UTF8, 0, cch, len, lpa, cnt);
			lpa[nMultiCount] = 0;
			
			this->operator=( lpa );
			delete [] lpa;
		}
		

		/*bool Find( LPCWSTR text )
		{
			return ( this->find( text, 0 ) > -1  );
		}*/

		static FString NewGuid()
		{			
			GUID uuid; 			
			CoCreateGuid(&uuid);
			return GuidToString( uuid );
		}
		static FString GuidToString(UUID& uuid)
		{
			return FString::Format( L"%.8x-%.4x-%.4x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x", uuid.Data1, uuid.Data2, uuid.Data3, uuid.Data4[0],uuid.Data4[1],
					uuid.Data4[2],uuid.Data4[3],uuid.Data4[4],uuid.Data4[5],uuid.Data4[6],uuid.Data4[7] );	

		}
		static FString Number( int number )
		{
			std::wstringstream sm;
			sm << number;
			return sm.str();
		}

	#if defined(__cplusplus_winrt)
		operator Platform::String^()
		{
			Platform::String^ rval = ref new Platform::String( c_str() );
			return rval;
		}

	#endif
	
		FString Trim() const
		{
		  // Trim leading non-letters
		  LPCWSTR str = c_str();
		  while(*str == ' ') str++;

		  // Trim trailing non-letters
		  WCHAR* end = (WCHAR*)str + fstrlenW(str) - 1;
		  while(end > str && (*end == ' ')) end--;

		  return FString(str, end+1);
			  
		}	
		
		static int fstrlenA(const char* p )
		{
			int r = 0;
			while( *p ){r++; p++;}
			return r;
		}	
		static int fstrlenW(const WCHAR* p )
		{
			int r = 0;
			while( *p ){r++; p++;}
			return r;
		}	
};


struct FStringPair
{
	FString key;
	FString val;
};


};
