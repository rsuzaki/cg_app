/* -*- coding: utf-8 -*- マルチバイト */

#include "cg_widget.h"

namespace pgn
{

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CGWidget::CGWidget()
{
  _hInst = NULL;
  _hWnd = NULL;
  _width = 0;
  _height = 0;

  _buffer = nullptr;
  _hBitmap = NULL;
  _hDC = NULL;
  _hOldHandle = NULL;
}

////////////////////////////////////////////////////////////////////////////////
CGWidget::~CGWidget()
{
  FreeDS();
}

////////////////////////////////////////////////////////////////////////////////
bool CGWidget::CreateDS( int width, int height )
{
  FreeDS();

  if (width < 1) width = 1;
  if (height < 1) height = 1;

  BITMAPINFO bmi;
  ZeroMemory( &bmi, sizeof(bmi) );
  bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
  bmi.bmiHeader.biWidth  = width;
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  // デスクトップのDCを得る
  const HDC tmpDC = GetDC( GetDesktopWindow() );

  // ビットマップのハンドルと、作成したバッファの先頭アドレスを取得
  _hBitmap = CreateDIBSection( tmpDC, &bmi, DIB_RGB_COLORS, (void**)&_buffer, 0,0 );

  // 作成失敗 (致命的)
  if (_hBitmap == nullptr) return false;

  // HBITMAPにHDCを結び付ける
  _hDC = CreateCompatibleDC( tmpDC );
  _hOldHandle = SelectObject( _hDC, _hBitmap );

  ReleaseDC( GetDesktopWindow(), tmpDC );

  return true;
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::FreeDS()
{
  // 確保済みなら、DIBSection を解放
  if (_buffer != NULL)
  {
    SelectObject( _hDC, _hOldHandle );
    DeleteDC( _hDC );
    DeleteObject( _hBitmap );

    _buffer = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::Display() const
{
  const HDC destDC = GetDC( _hWnd );
  BitBlt( destDC, 0, 0, Width(), Height(), _hDC, 0, 0, SRCCOPY );
  ReleaseDC( _hWnd, destDC );
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::Fill( uint32_t color )
{
  uint32_t* ptr = Buffer();
  if (ptr == nullptr) return;

  const int s = Width() * Height();
  for (int i=0; i<s; i++)
  {
    *ptr = color;
    ptr++;
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
LRESULT CGWidget::WndProc( HINSTANCE hInst, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  // 保存しておく
  _hInst = hInst;
  _hWnd = hWnd;

  // マウスイベントのパラメータを取得
  const int x = LOWORD(lParam);
  const int y = HIWORD(lParam);
  
  Touch touch = TouchNone;
  if (wParam & MK_LBUTTON) touch = TouchLeft;
  if (wParam & MK_MBUTTON) touch = TouchMiddle;
  if (wParam & MK_RBUTTON) touch = TouchRight;

  switch (message)
  {
    /////////////////////
    // ウィンドウ生成時
    /////////////////////
  case WM_CREATE:
    OnCreate();
    break;

    /////////////////////////
    // ウィンドウリサイズ時
    /////////////////////////
  case WM_SIZE:
    _width = LOWORD(lParam);
    _height = HIWORD(lParam);
    CreateDS( _width, _height );
    OnResize( _width, _height );
    break;

    //////////////////////////////
    // 左、中、右ボタンを押した
    //////////////////////////////
  case WM_LBUTTONDOWN:
    OnTouchPress( TouchLeft, x, y );
    break;

  case WM_MBUTTONDOWN:
    OnTouchPress( TouchMiddle, x, y );
    break;

  case WM_RBUTTONDOWN:
    OnTouchPress( TouchRight, x, y );
    break;

    //////////////////////////////
    // 左、中、右ボタンを離した
    //////////////////////////////
  case WM_LBUTTONUP:
    OnTouchRelease( TouchLeft, x, y );
    break;

  case WM_MBUTTONUP:
    OnTouchRelease( TouchMiddle, x, y );
    break;

  case WM_RBUTTONUP:
    OnTouchRelease( TouchRight, x, y );
    break;

    ////////////////////////
    // マウス移動中
    ////////////////////////
  case WM_MOUSEMOVE:
    OnTouchMove( touch, x, y );

    ////////////////////////
    // 描画イベント発生時
    ////////////////////////
  case WM_PAINT:
    {
    PAINTSTRUCT ps;
    const HDC hdc = BeginPaint( hWnd, &ps );
    OnPaint( hdc );
    EndPaint( hWnd, &ps );
    }
    break;

    /////////////////////
    // 終了処理
    /////////////////////
  case WM_DESTROY:
    PostQuitMessage( 0 );
    break;

    /////////////////////
    // デフォルトの挙動
    /////////////////////
  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::SetWindowTitle( std::wstring s ) const
{
  SetWindowTextW( _hWnd, s.c_str() );
}

////////////////////////////////////////////////////////////////////////////////
std::wstring CGWidget::GetOpenFilePath() const
{
  std::wstring res;

  OPENFILENAME ofn;
  TCHAR szFile[260] = { 0 };

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = Hwnd();
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = L"All (*.*)\0*.*\0PNG (*.png)\0*.png\0BMP (*.bmp)\0*.bmp\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = nullptr;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = nullptr;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if (GetOpenFileName( &ofn ) == TRUE)
  {
    res = ofn.lpstrFile;
  }

  return res;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CGWidget::OnCreate()
{
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::OnPaint( HDC hDC )
{
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::OnResize( int width, int height )
{
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::OnTouchPress( CGWidget::Touch touch, int x, int y )
{
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::OnTouchMove( CGWidget::Touch touch, int x, int y )
{
}

////////////////////////////////////////////////////////////////////////////////
void CGWidget::OnTouchRelease( CGWidget::Touch touch, int x, int y )
{
}

} ///< neet

