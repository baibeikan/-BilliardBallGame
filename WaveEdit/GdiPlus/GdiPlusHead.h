#ifndef __GdiPlusHead_H__
#define __GdiPlusHead_H__
#include <comdef.h>
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")

typedef Gdiplus::BitmapData GpBitmapData;
typedef Gdiplus::Bitmap GpBitmap;
typedef Gdiplus::Graphics GpGraphics;
typedef Gdiplus::SolidBrush GpSolidBrush;
typedef Gdiplus::CachedBitmap GpCachedBitmap;
typedef Gdiplus::Color GpColor;
typedef Gdiplus::PointF GpPointF;
typedef Gdiplus::Font GpFont;
typedef Gdiplus::FontFamily GpFontFamily;
typedef Gdiplus::Pen GpPen;
typedef Gdiplus::Point GpPoint;
typedef Gdiplus::PointF GpPointF;
typedef Gdiplus::Rect GpRect;
typedef Gdiplus::RectF GpRectF;
typedef Gdiplus::GdiplusStartupInput GpGdiplusStartupInput;




#define GpSmoothingModeAntiAlias (Gdiplus::SmoothingModeAntiAlias)
#define GpFontStyleRegular (Gdiplus::FontStyleRegular)
#define GpFontStyleBold (Gdiplus::FontStyleBold)

#define GpUnitPixel (Gdiplus::UnitPixel)
#define GpTextRenderingHintAntiAliasGridFit (Gdiplus::TextRenderingHintAntiAliasGridFit)

#define GpPixelFormat24bppRGB (PixelFormat24bppRGB)
#define GpImageLockModeWrite (Gdiplus::ImageLockModeWrite)

#endif

