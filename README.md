# cg_app

VisualStudio (Visual C++) の 「Windows デスクトップアプリケーション」を新規作成した際に、Win32 プログラミングの知識がなくても開発を進めやすくするための、CGWidget クラス (cg_widget.h/cpp) とサンプルプロジェクトです。

# CGWidget クラスの使い方

Visual Studio の新規作成から「Windows デスクトップアプリケーション」を選び、Win32アプリケーションを作成します。

「ソリューションエクスプローラー」を右クリックして、「追加 > 既存の項目」を選び、cg_widget.h と cg_widget.cpp をプロジェクトに追加します。

「ソリューションエクスプローラー」に "プロジェクト名".h というヘッダファイルが出来ているので、ダブルクリックで開きます。
```
#pragma once

#include "resource.h"
```
という内容になっているので、
```
#pragma once

#include "resource.h"
#include "cg_widget.h"

///////////////////////////////////////////////////////////////////////
// [変更点1] CGWidget を継承し、自分で処理をしたいイベントを実装する
///////////////////////////////////////////////////////////////////////
class Widget : public pgn::CGWidget
{
public:
  void OnResize( int width, int height );
  void OnPaint( HDC hdc );
  void OnTouchPress( Touch touch, int x, int y );
  void OnTouchMove( Touch touch, int x, int y );
};
```
といった感じで、cg_widget.h を include し、CGWidget を継承したクラスを作ります。

"プロジェクト名".cpp というソースファイルを開きます。
```
// cg_app.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "cg_app.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
```
というコードが書かれているので、
```
// cg_app.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "cg_app.h"

#define MAX_LOADSTRING 100

//////////////////////////////////////////////////////////////
// [変更点2] CGWidget を継承する
//////////////////////////////////////////////////////////////
void Widget::OnResize( int width, int height )
{
  // ウィンドウがリサイズした時のイベントを書く
  std::wstring s = L"w " + std::to_wstring( width ) + L" h " + std::to_wstring( height );

  // ウィンドウの幅と高さをタイトルバーに表示する
  SetWindowTitle( s );
}

//////////////////////////////////////////////////////////////
void Widget::OnPaint( HDC hdc )
{
  // ウィンドウの更新処理が起きた時のイベントを書く
  Fill( 0xFF00FFFF );
  Display();
}

//////////////////////////////////////////////////////////////
void Widget::OnTouchMove( Touch touch, int x, int y )
{
  // タッチ移動（マウス移動）イベントが起きた時の処理を書く
  std::wstring s = L"(" + std::to_wstring( x ) + L"," + std::to_wstring( y ) + L")";

  // タッチ位置 (マウス位置) をウィンドウのタイトルバーに表示する
  SetWindowTitle( s );
}

//////////////////////////////////////////////////////////////
void Widget::OnTouchPress( Touch touch, int x, int y )
{
  // マウスの左ボタンを押した時
  if (touch & TouchLeft)
  {
    Fill( 0xFF00FF00 );
    Display();
  }

  // マウスの右ボタンを押した時
  if (touch & TouchRight)
  {
    std::wstring filePath = GetOpenFilePath();
    SetWindowTitle( filePath );
  }
}

Widget widget;
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
```
といった具合に、CGWidget クラスを継承したメソッドを実装します。

OnResize は、ウィンドウがリサイズした時に呼ばれるイベントです。

OnTouchPress は、マウスボタンを押した時などに発生するイベントです。

このように、何かしらのイベントが発生した際の処理を実装し、処理を行うためのクラスになっています。

最後に、WndProc を書き換えます。
```
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
```
というコードがあるので、ここを、
```
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  //////////////////////////////////////////////
  // [変更点3] widget 側で WndProc を処理する
  //////////////////////////////////////////////
  return widget.WndProc( hInst, hWnd, message, wParam, lParam );
}
```
と書き換えます。Win32 プログラミングで面倒臭い、WndProc の処理を CGWidget クラスに任せ、必要なイベントだけを実装するというのがコンセプトです。

自分でプロジェクトを作って差し替えるのが難しいのなら、この cg_app プロジェクトをそのまま使っても良いでしょう。

# CGWidget の主なメソッド

CGWidget は、グラフィックスプログラミング学習のために用意したクラスです。グラフィックスプログラミングに必要な「メモリアクセス可能な画像バッファ」が、ウィンドウサイズで常に確保されています。

画像バッファの先頭は、Buffer() メソッドでアクセスできます。ここに対して、Width() と Height() でバッファの幅と高さを取得、利用して、画像バッファに（自力で）書き込んでいく事ができます。

画像バッファと幅・高さがあれば、何でもできるのがグラフィックスプログラミングです。

書き込んだバッファは、Display() メソッドでウィンドウ上に表示できます。

Fill( uint32_t color )　メソッドを使えば、バッファを単色で塗りつぶす事ができます。

