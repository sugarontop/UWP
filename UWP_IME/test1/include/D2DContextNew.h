#pragma once

#include "D2DMisc.h"

namespace V4
{
	class D2DMat : public D2D1_MATRIX_3X2_F
	{
		public :
			D2DMat()
			{
				Reset();
			}
			D2DMat( const D2D1_MATRIX_3X2_F& m )
			{
				*this = m;
			}

			FPointF LPtoDP( const FPointF& ptlog ) const 
			{
				return LPtoDP( *this, ptlog );
			}
			FRectF LPtoDP( const FRectF& rc ) const 
			{
				FPointF p1 =  LPtoDP( *this, FPointF( rc.left, rc.top) );
				FPointF p2 =  LPtoDP( *this, FPointF( rc.right, rc.bottom) );
			
				return FRectF( p1.x, p1.y, p2.x, p2.y );
			}
			FSizeF LPtoDP( const FSizeF& sz ) const 
			{
				FPointF p1 =  LPtoDP( *this, FPointF( sz.width, sz.height) );				
				return FSizeF( p1.x - _31, p1.y - _32 );
			}

			FPointF DPtoLP( LPARAM lp ) const 
			{
				return DPtoLP( *this, FPointF(lp) );
			}
			FPointF DPtoLP( const FPointF& ptdev ) const 
			{
				return DPtoLP( *this, ptdev );
			}
			FRectF DPtoLP( const FRectF& rc ) const 
			{
				FPointF p1 =  DPtoLP( *this, FPointF( rc.left, rc.top) );
				FPointF p2 =  DPtoLP( *this, FPointF( rc.right, rc.bottom) );
			
				return FRectF( p1.x, p1.y, p2.x, p2.y );
			}
			FSizeF DPtoLP( const FSizeF& sz ) const 
			{
				FPointF p1 =  DPtoLP( *this, FPointF( sz.width, sz.height) );
				FPointF p2 =  DPtoLP( *this, FPointF( 0,0) );			
				return FSizeF( p1.x-p2.x, p1.y-p2.y );
			}

			static FPointF LPtoDP( const D2D1_MATRIX_3X2_F& mat, const FPointF& ptlog )
			{	
				Matrix3x2F m( mat._11, mat._12, mat._21, mat._22, mat._31, mat._32 );				
				return m.TransformPoint( ptlog ); // this is ptdev.
			}

			static FPointF DPtoLP(  const D2D1_MATRIX_3X2_F& mat, const FPointF& ptdev )
			{	
				Matrix3x2F m( mat._11, mat._12, mat._21, mat._22, mat._31, mat._32 );
				m.Invert();
				
				return m.TransformPoint( ptdev ); // this is ptlog.
			}
			static FRectF LPtoDP(  const D2D1_MATRIX_3X2_F& mat, const FRectF& rc )
			{
				FPointF p1 =  LPtoDP( mat, FPointF( rc.left, rc.top) );
				FPointF p2 =  LPtoDP( mat, FPointF( rc.right, rc.bottom) );
			
				return FRectF( p1.x, p1.y, p2.x, p2.y );
			}
			static FRectF DPtoLP( D2D1_MATRIX_3X2_F& mat, const FRectF& rc )
			{
				FPointF p1 =  DPtoLP( mat, FPointF( rc.left, rc.top) );
				FPointF p2 =  DPtoLP( mat, FPointF( rc.right, rc.bottom) );
			
				return FRectF( p1.x, p1.y, p2.x, p2.y );
			}
		
			void Offset( float cx, float cy )
			{
				FSizeF sz = LPtoDP( FSizeF(cx,cy));

				OffsetDev( sz.width, sz.height );
			}
			void OffsetDev( float cx, float cy )
			{
				_31 += cx;
				_32 += cy;			
			}
			D2D1_MATRIX_3X2_F CalcOffset( float cx, float cy )
			{
				D2DMat d(*this);
				d.Offset(cx,cy);
				return d;
			}
			D2DMat& operator = ( const D2D1_MATRIX_3X2_F& mat )
			{
				_11 = mat._11; _21 = mat._21;
				_12 = mat._12; _22 = mat._22;
				_31 = mat._31; _32 = mat._32;
				return *this;
			}
			FPointF Multi( const FPointF& pt )
			{
				FPointF rval;
				rval.x = _11 * pt.x + _21 * pt.y + _31;
				rval.y = _21 * pt.x + _22 * pt.y + _32;
				return rval;
			}
			
			void Rotate( float rad )
			{
				_11 = cos( rad );
				_12 = sin( rad );
				_21 = -_12;		//sin(rad);
				_22 = _11;		//cos(rad);
			}
			
			void Reset( int typ = 0 )
			{				
				_11 = _22 = 1;
				_21 = _12 = 0;
				_31 = _32 = 0;		

				if ( typ == 1 )
				{
					_11 = _22 = 0;					
				}
				else if ( typ == 2 )
				{
					// matが表示前にPtInRectでtrueをかえさないように、存在しないであろう座標を_31,_32へ設定する。
					// 表示時にmatは正確に設定される
					_11 = _22 = 0;	
					_31 = _32 = 100000;		
				}
			}

			static void Multi(D2D1_MATRIX_3X2_F &ou, const D2D1_MATRIX_3X2_F &a, const D2D1_MATRIX_3X2_F &b )
			{
				ou._11 = a._11 * b._11 + a._12 * b._21;
				ou._12 = a._11 * b._12 + a._12 * b._22;
				ou._21 = a._21 * b._11 + a._22 * b._21;
				ou._22 = a._21 * b._12 + a._22 * b._22;
				ou._31 = a._31 * b._11 + a._32 * b._21 + b._31;
				ou._32 = a._31 * b._12 + a._32 * b._22 + b._32;
			}
			
			
	};
	///////////////////////////////////////////////////////////////
	// D2DMat with stack

	class D2DMatrix : public D2DMat
	{
		public :
			
			D2DMatrix( const D2D1_MATRIX_3X2_F& m ):D2DMat(m),st_i_(0){}
			//D2DMatrix( D2DContext& cxt ):g_(cxt.cxt),st_i_(0){}
			D2DMatrix( ID2D1RenderTarget* g ):g_(g),st_i_(0){}

			void Push()
			{
				_ASSERT( st_i_ < 4 );
				stack_[st_i_++] = *this;
			}

			void Pop()
			{
				_ASSERT( st_i_ > 0 );
				*this = stack_[--st_i_];
			}
			D2DMatrix& operator = ( const D2D1_MATRIX_3X2_F& mat )
			{
				_11 = mat._11; _21 = mat._21;
				_12 = mat._12; _22 = mat._22;
				_31 = mat._31; _32 = mat._32;
				return *this;
			}

			void PushTransformInCaputre(const D2DMat& mat )
			{				
				g_->SetTransform( &mat );
				PushTransform();				
			}
			D2DMatrix& GetTransform()
			{			
				g_->GetTransform( this );					
				return *this;
			}

			D2DMatrix& PushTransform()
			{			
				g_->GetTransform( this );
				Push();			
				return *this;
			}
			void PopTransform()
			{
				Pop();			
				g_->SetTransform( *this );			
			}
			D2DMatrix& Offset( const FRectF& rc )
			{
				return Offset( rc.left, rc.top );
			}
			D2DMatrix& Offset( float cx, float cy )
			{
				// _31,_32に小数点を付けない。96DPI時に線が太くなる場合があるため。
				int cx1 = static_cast<int>(cx);
				int cy1 = static_cast<int>(cy);
								
				D2DMat::Offset((float)cx1,(float)cy1);
				g_->SetTransform( *this );
				return *this;
			}
			D2DMatrix& Scale( float scx, float scy )
			{
				_11 = _11 * scx;
				_22 = _22 * scy;
				g_->SetTransform( *this );
				return *this;

			}
			void SetTransform()
			{
				g_->SetTransform( *this );
			}
			void ReplaceTransform(const D2D1_MATRIX_3X2_F& mat)
			{
				this->operator=( mat );
				g_->SetTransform( *this );
			}
			void NewTransform(const D2D1_MATRIX_3X2_F& mat)
			{
				this->operator=( mat );
				g_->SetTransform( *this );
			}
			void Multi( const D2D1_MATRIX_3X2_F &a )
			{
				D2DMat::Multi( *this, *this, a );
				g_->SetTransform( *this );
			}

		protected :
			int st_i_;
			D2D1_MATRIX_3X2_F stack_[4]; // 4個あれば十分だと思う。
			ID2D1RenderTarget* g_;
	};
};

namespace V4
{
	// 均等配置をサポートする
	class NBunkatu
	{
		public :
			NBunkatu( const FRectF& rc, int Ny, int Nx=1 ):rc_(rc),Ny_(Ny),Nx_(Nx)
			{
				_ASSERT( Ny_ >= 1 );
				_ASSERT( Nx_ >= 1 );
			}
			// outerの中にセンター配置のinnerを計算
			void CalcPosition( int posY, int posX, const FSizeF& innerSize, FRectF& outer, FRectF& inner )
			{
				CalcPosition(posY,posX,innerSize,1,outer,inner ); // 横：センター配置
			}
			void CalcPosition( int posY, int posX, const FSizeF& innerSize,int align, FRectF& outer, FRectF& inner )
			{
				_ASSERT( 0 <=posY && posY < Ny_ );
				_ASSERT( 0 <=posX && posX < Nx_ );
								
						
				float height = rc_.Height();
				float width = rc_.Width();
				
				outer.top = rc_.top + height / Ny_ * posY;
				outer.bottom = outer.top + height/Ny_;

				outer.left = rc_.left + width / Nx_ * posX;
				outer.right = outer.left + width / Nx_;

				float offx=0;

				float offy = (height / Ny_ - innerSize.height) / 2.0f;

				if ( align == 1 )
				{					
					offx = (width / Nx_ - innerSize.width) / 2.0f;
				}
				else if ( align == 2 )
				{
					offx = (width / Nx_ - innerSize.width);
				}

				inner.top = outer.top + offy;
				inner.bottom = inner.top + innerSize.height;
				inner.left = outer.left + offx;
				inner.right = inner.left + innerSize.width;
			}
			static FRectF Inner( const FSizeF& innerSize, const FRectF& outer )
			{
				float offy = (outer.Height() - innerSize.height) / 2.0f;
				float offx = (outer.Width() - innerSize.width) / 2.0f;
				return FRectF( outer.left + offx, outer.top + offy, innerSize );
			}
		private :
			FRectF rc_;
			int Ny_;	// 縦にN分割
			int Nx_;	// 横にN分割

	};
	
	//
	// 境界線を多用する場合に1:1の条件下でaliasを自動で切る
	//
	//class D2DAntialias
	//{
	//	public :
	//	D2DAntialias( D2DContext& c ):cxt(c),bl(false)
	//	{
	//		D2DMat mat;
	//		cxt.cxt->GetTransform(&mat);
	//		
	//		if ( mat._11 == 1 && mat._22 == 1 )
	//		{
	//			bl = true;
	//			cxt.SetAntiAlias(false);
	//		}
	//	}
	//	~D2DAntialias()
	//	{
	//		if ( bl )
	//			cxt.SetAntiAlias(true);

	//	}
	//	void AliasOff()
	//	{
	//		if ( !bl )
	//		{
	//			bl = true;
	//			cxt.SetAntiAlias(false);
	//		}
	//	}

	//	private :
	//		bool bl;
	//		D2DContext& cxt;
	//};
};



	
