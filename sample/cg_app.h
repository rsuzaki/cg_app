#pragma once

#include "resource.h"
#include "cg_widget.h"

///////////////////////////////////////////////////////////////////////
// [�ύX�_1] CGWidget ���p�����A�����ŏ������������C�x���g����������
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
