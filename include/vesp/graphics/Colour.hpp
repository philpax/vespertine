#pragma once

#include "vesp/Types.hpp"
#include <cstring>

namespace vesp { namespace graphics {

	struct Colour
	{
		Colour();
		Colour(U8 data[4]);
		Colour(U8 r, U8 g, U8 b, U8 a = 255);
		Colour(Vec4 const& vec);

		union
		{
			U8 data[4];
			struct
			{
				U8 r;
				U8 g;
				U8 b;
				U8 a;
			};
		};

		static const Colour AliceBlue;
		static const Colour AntiqueWhite;
		static const Colour Aqua;
		static const Colour Aquamarine;
		static const Colour Azure;
		static const Colour Beige;
		static const Colour Bisque;
		static const Colour Black;
		static const Colour BlanchedAlmond;
		static const Colour Blue;
		static const Colour BlueViolet;
		static const Colour Brown;
		static const Colour BurlyWood;
		static const Colour CadetBlue;
		static const Colour Chartreuse;
		static const Colour Chocolate;
		static const Colour Coral;
		static const Colour CornflowerBlue;
		static const Colour Cornsilk;
		static const Colour Crimson;
		static const Colour Cyan;
		static const Colour DarkBlue;
		static const Colour DarkCyan;
		static const Colour DarkGoldenRod;
		static const Colour DarkGray;
		static const Colour DarkGreen;
		static const Colour DarkKhaki;
		static const Colour DarkMagenta;
		static const Colour DarkOliveGreen;
		static const Colour DarkOrange;
		static const Colour DarkOrchid;
		static const Colour DarkRed;
		static const Colour DarkSalmon;
		static const Colour DarkSeaGreen;
		static const Colour DarkSlateBlue;
		static const Colour DarkSlateGray;
		static const Colour DarkTurquoise;
		static const Colour DarkViolet;
		static const Colour DeepPink;
		static const Colour DeepSkyBlue;
		static const Colour DimGray;
		static const Colour DodgerBlue;
		static const Colour FireBrick;
		static const Colour FloralWhite;
		static const Colour ForestGreen;
		static const Colour Fuchsia;
		static const Colour Gainsboro;
		static const Colour GhostWhite;
		static const Colour Gold;
		static const Colour GoldenRod;
		static const Colour Gray;
		static const Colour Green;
		static const Colour GreenYellow;
		static const Colour HoneyDew;
		static const Colour HotPink;
		static const Colour IndianRed;
		static const Colour Indigo;
		static const Colour Ivory;
		static const Colour Khaki;
		static const Colour Lavender;
		static const Colour LavenderBlush;
		static const Colour LawnGreen;
		static const Colour LemonChiffon;
		static const Colour LightBlue;
		static const Colour LightCoral;
		static const Colour LightCyan;
		static const Colour LightGoldenRodYellow;
		static const Colour LightGray;
		static const Colour LightGreen;
		static const Colour LightPink;
		static const Colour LightSalmon;
		static const Colour LightSeaGreen;
		static const Colour LightSkyBlue;
		static const Colour LightSlateGray;
		static const Colour LightSteelBlue;
		static const Colour LightYellow;
		static const Colour Lime;
		static const Colour LimeGreen;
		static const Colour Linen;
		static const Colour Magenta;
		static const Colour Maroon;
		static const Colour MediumAquaMarine;
		static const Colour MediumBlue;
		static const Colour MediumOrchid;
		static const Colour MediumPurple;
		static const Colour MediumSeaGreen;
		static const Colour MediumSlateBlue;
		static const Colour MediumSpringGreen;
		static const Colour MediumTurquoise;
		static const Colour MediumVioletRed;
		static const Colour MidnightBlue;
		static const Colour MintCream;
		static const Colour MistyRose;
		static const Colour Moccasin;
		static const Colour NavajoWhite;
		static const Colour Navy;
		static const Colour OldLace;
		static const Colour Olive;
		static const Colour OliveDrab;
		static const Colour Orange;
		static const Colour OrangeRed;
		static const Colour Orchid;
		static const Colour PaleGoldenRod;
		static const Colour PaleGreen;
		static const Colour PaleTurquoise;
		static const Colour PaleVioletRed;
		static const Colour PapayaWhip;
		static const Colour PeachPuff;
		static const Colour Peru;
		static const Colour Pink;
		static const Colour Plum;
		static const Colour PowderBlue;
		static const Colour Purple;
		static const Colour Red;
		static const Colour RosyBrown;
		static const Colour RoyalBlue;
		static const Colour SaddleBrown;
		static const Colour Salmon;
		static const Colour SandyBrown;
		static const Colour SeaGreen;
		static const Colour SeaShell;
		static const Colour Sienna;
		static const Colour Silver;
		static const Colour SkyBlue;
		static const Colour SlateBlue;
		static const Colour SlateGray;
		static const Colour Snow;
		static const Colour SpringGreen;
		static const Colour SteelBlue;
		static const Colour Tan;
		static const Colour Teal;
		static const Colour Thistle;
		static const Colour Tomato;
		static const Colour Turquoise;
		static const Colour Violet;
		static const Colour Wheat;
		static const Colour White;
		static const Colour WhiteSmoke;
		static const Colour Yellow;
		static const Colour YellowGreen;
	};

} }