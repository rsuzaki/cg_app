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
  void OnDoubleClick( Touch touch, int x, int y );
};
