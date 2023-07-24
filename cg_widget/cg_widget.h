/* -*- coding: utf-8 -*- マルチバイト */

#ifndef _CG_WIDGET_H_
#define _CG_WIDGET_H_

#include <windows.h>
#include <string>

namespace pgn
{

///////////////////////////////////////////////////////////////////////////
// Visual C++ の Win32 プロジェクト用のラッパークラス
///////////////////////////////////////////////////////////////////////////
class CGWidget
{
public:
  // タッチ型
  enum Touch
  {
    TouchNone = 0, // 何も押していない

    TouchLeft = 1,   // マウス左ボタン
    TouchMiddle = 2, // マウス中ボタン
    TouchRight = 4,  // マウス右ボタン
  };

private:
  // 基本プロパティ
  HINSTANCE _hInst;
  HWND _hWnd;
  int _width,_height;

  // CreateDIBSection 関連
  uint32_t* _buffer;
  HBITMAP _hBitmap;
  HDC _hDC;
  HGDIOBJ _hOldHandle;

  // DIBSection の生成と破棄
  bool CreateDS( int width, int height );
  void FreeDS();

public:
  CGWidget();
  ~CGWidget();

  // 基本プロパティ
  HINSTANCE Instance() const { return _hInst; }
  HWND Hwnd() const { return _hWnd; }
  
  int Width() const { return _width; }
  int Height() const { return _height; }

  // メモリアクセスできる表示バッファ
  // 
  // - ウィンドウサイズの表示バッファを常に取得している (Width(), Height() サイズ) 
  // - 表示バッファの先頭アドレスは、Buffer() で取得できる
  // - 表示バッファをウィンドウに表示するには、Dipslay() を呼ぶ
  //
  uint32_t* Buffer(){ return _buffer; } // 先頭バッファ
  const uint32_t* Buffer() const { return _buffer; } // 先頭バッファ
  
  void Display() const; // バッファの内容を表示する
  void Fill( uint32_t color ); // バッファを塗りつぶす

  /*
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    // という感じで、WinProc内で呼び出す
    return ws.WndProc( hInst, hWnd, message, wParam, lParam );
  }
  */
  LRESULT WndProc( HINSTANCE hInst, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

  // タイトルバーの文字列を指定
  void SetWindowTitle( std::wstring s ) const;

  // ファイルを開くダイアログを表示
  std::wstring GetOpenFilePath() const;

  // 基本イベント
  virtual void OnCreate();
  virtual void OnPaint( HDC hDC );
  virtual void OnResize( int width, int height );

  // タッチイベント (マウスイベント)
  virtual void OnTouchPress( Touch touch, int x, int y );
  virtual void OnTouchMove( Touch touch, int x, int y );
  virtual void OnTouchRelease( Touch touch, int x, int y );

  // ダブルクリックイベント
  //
  // - デフォルトでは使用できない
  // - MyRegisterClass の
  // - wcex.style          = CS_HREDRAW | CS_VREDRAW;
  // - を
  // - wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  // - こう書き換え、ダブルクリックを有効にする必要がある
  virtual void OnDoubleClick( Touch touch, int x, int y );
};

} ///< neet

#endif
