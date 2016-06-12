UWP IME sample 

2016/06 

Single line only.

UWP (DirectX11 application)

using namespace Windows::UI::Text::Core;

Directg2D


NO XAML

Windows10 japanese

VisualStudio2015


void template1Main::OnInitail()
{
	//
	// Create Window controls. Button.
	// 

	FRectF rc(10,10,200,40);
	D2DButton* b1 = new D2DButton();
	b1->CreateButton( this, this, rc, 0, L"noname" );

	rc.Offset( 220, 0 );
	b1 = new D2DButton();
	b1->CreateButton(this, this, rc, 0, L"noname");

	rc.Offset(220, 0);
	b1 = new D2DButton();
	b1->CreateButton(this, this, rc, 0, L"noname");


	
	_ASSERT(imebridge_ != nullptr );

	//
	// Create Window controls. Textbox.
	// 
	
	FRectF rcTextbox( 10, 400, FSizeF(800,60));
	auto* t1 = new D2DTextbox(*imebridge_);
	t1->Create(this, this, rcTextbox, 0, L"noname");

	rcTextbox.Offset(0, 120);
	auto* t2 = new D2DTextbox(*imebridge_);
	t2->Create(this, this, rcTextbox, 0, L"noname");
}