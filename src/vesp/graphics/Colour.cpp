#include "vesp/graphics/Colour.hpp"
#include "vesp/math/Vector.hpp"

namespace vesp { namespace graphics {

	Colour::Colour()
	{
		memset(this->data, 0, 4);
	}

	Colour::Colour(U8 data[4])
	{
		memcpy(this->data, data, 4);
	}

	Colour::Colour(U8 r, U8 g, U8 b, U8 a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Colour::Colour(Vec4 const& vec)
	{
		this->r = static_cast<U8>(vec.x * 255.0f);
		this->g = static_cast<U8>(vec.y * 255.0f);
		this->b = static_cast<U8>(vec.z * 255.0f);
		this->a = static_cast<U8>(vec.w * 255.0f);
	}

	const Colour Colour::AliceBlue = Colour(240, 248, 255);
	const Colour Colour::AntiqueWhite = Colour(250, 235, 215);
	const Colour Colour::Aqua = Colour(0, 255, 255);
	const Colour Colour::Aquamarine = Colour(127, 255, 212);
	const Colour Colour::Azure = Colour(240, 255, 255);
	const Colour Colour::Beige = Colour(245, 245, 220);
	const Colour Colour::Bisque = Colour(255, 228, 196);
	const Colour Colour::Black = Colour(0, 0, 0);
	const Colour Colour::BlanchedAlmond = Colour(255, 235, 205);
	const Colour Colour::Blue = Colour(0, 0, 255);
	const Colour Colour::BlueViolet = Colour(138, 43, 226);
	const Colour Colour::Brown = Colour(165, 42, 42);
	const Colour Colour::BurlyWood = Colour(222, 184, 135);
	const Colour Colour::CadetBlue = Colour(95, 158, 160);
	const Colour Colour::Chartreuse = Colour(127, 255, 0);
	const Colour Colour::Chocolate = Colour(210, 105, 30);
	const Colour Colour::Coral = Colour(255, 127, 80);
	const Colour Colour::CornflowerBlue = Colour(100, 149, 237);
	const Colour Colour::Cornsilk = Colour(255, 248, 220);
	const Colour Colour::Crimson = Colour(220, 20, 60);
	const Colour Colour::Cyan = Colour(0, 255, 255);
	const Colour Colour::DarkBlue = Colour(0, 0, 139);
	const Colour Colour::DarkCyan = Colour(0, 139, 139);
	const Colour Colour::DarkGoldenRod = Colour(184, 134, 11);
	const Colour Colour::DarkGray = Colour(169, 169, 169);
	const Colour Colour::DarkGreen = Colour(0, 100, 0);
	const Colour Colour::DarkKhaki = Colour(189, 183, 107);
	const Colour Colour::DarkMagenta = Colour(139, 0, 139);
	const Colour Colour::DarkOliveGreen = Colour(85, 107, 47);
	const Colour Colour::DarkOrange = Colour(255, 140, 0);
	const Colour Colour::DarkOrchid = Colour(153, 50, 204);
	const Colour Colour::DarkRed = Colour(139, 0, 0);
	const Colour Colour::DarkSalmon = Colour(233, 150, 122);
	const Colour Colour::DarkSeaGreen = Colour(143, 188, 143);
	const Colour Colour::DarkSlateBlue = Colour(72, 61, 139);
	const Colour Colour::DarkSlateGray = Colour(47, 79, 79);
	const Colour Colour::DarkTurquoise = Colour(0, 206, 209);
	const Colour Colour::DarkViolet = Colour(148, 0, 211);
	const Colour Colour::DeepPink = Colour(255, 20, 147);
	const Colour Colour::DeepSkyBlue = Colour(0, 191, 255);
	const Colour Colour::DimGray = Colour(105, 105, 105);
	const Colour Colour::DodgerBlue = Colour(30, 144, 255);
	const Colour Colour::FireBrick = Colour(178, 34, 34);
	const Colour Colour::FloralWhite = Colour(255, 250, 240);
	const Colour Colour::ForestGreen = Colour(34, 139, 34);
	const Colour Colour::Fuchsia = Colour(255, 0, 255);
	const Colour Colour::Gainsboro = Colour(220, 220, 220);
	const Colour Colour::GhostWhite = Colour(248, 248, 255);
	const Colour Colour::Gold = Colour(255, 215, 0);
	const Colour Colour::GoldenRod = Colour(218, 165, 32);
	const Colour Colour::Gray = Colour(128, 128, 128);
	const Colour Colour::Green = Colour(0, 128, 0);
	const Colour Colour::GreenYellow = Colour(173, 255, 47);
	const Colour Colour::HoneyDew = Colour(240, 255, 240);
	const Colour Colour::HotPink = Colour(255, 105, 180);
	const Colour Colour::IndianRed = Colour(205, 92, 92);
	const Colour Colour::Indigo = Colour(75, 0, 130);
	const Colour Colour::Ivory = Colour(255, 255, 240);
	const Colour Colour::Khaki = Colour(240, 230, 140);
	const Colour Colour::Lavender = Colour(230, 230, 250);
	const Colour Colour::LavenderBlush = Colour(255, 240, 245);
	const Colour Colour::LawnGreen = Colour(124, 252, 0);
	const Colour Colour::LemonChiffon = Colour(255, 250, 205);
	const Colour Colour::LightBlue = Colour(173, 216, 230);
	const Colour Colour::LightCoral = Colour(240, 128, 128);
	const Colour Colour::LightCyan = Colour(224, 255, 255);
	const Colour Colour::LightGoldenRodYellow = Colour(250, 250, 210);
	const Colour Colour::LightGray = Colour(211, 211, 211);
	const Colour Colour::LightGreen = Colour(144, 238, 144);
	const Colour Colour::LightPink = Colour(255, 182, 193);
	const Colour Colour::LightSalmon = Colour(255, 160, 122);
	const Colour Colour::LightSeaGreen = Colour(32, 178, 170);
	const Colour Colour::LightSkyBlue = Colour(135, 206, 250);
	const Colour Colour::LightSlateGray = Colour(119, 136, 153);
	const Colour Colour::LightSteelBlue = Colour(176, 196, 222);
	const Colour Colour::LightYellow = Colour(255, 255, 224);
	const Colour Colour::Lime = Colour(0, 255, 0);
	const Colour Colour::LimeGreen = Colour(50, 205, 50);
	const Colour Colour::Linen = Colour(250, 240, 230);
	const Colour Colour::Magenta = Colour(255, 0, 255);
	const Colour Colour::Maroon = Colour(128, 0, 0);
	const Colour Colour::MediumAquaMarine = Colour(102, 205, 170);
	const Colour Colour::MediumBlue = Colour(0, 0, 205);
	const Colour Colour::MediumOrchid = Colour(186, 85, 211);
	const Colour Colour::MediumPurple = Colour(147, 112, 219);
	const Colour Colour::MediumSeaGreen = Colour(60, 179, 113);
	const Colour Colour::MediumSlateBlue = Colour(123, 104, 238);
	const Colour Colour::MediumSpringGreen = Colour(0, 250, 154);
	const Colour Colour::MediumTurquoise = Colour(72, 209, 204);
	const Colour Colour::MediumVioletRed = Colour(199, 21, 133);
	const Colour Colour::MidnightBlue = Colour(25, 25, 112);
	const Colour Colour::MintCream = Colour(245, 255, 250);
	const Colour Colour::MistyRose = Colour(255, 228, 225);
	const Colour Colour::Moccasin = Colour(255, 228, 181);
	const Colour Colour::NavajoWhite = Colour(255, 222, 173);
	const Colour Colour::Navy = Colour(0, 0, 128);
	const Colour Colour::OldLace = Colour(253, 245, 230);
	const Colour Colour::Olive = Colour(128, 128, 0);
	const Colour Colour::OliveDrab = Colour(107, 142, 35);
	const Colour Colour::Orange = Colour(255, 165, 0);
	const Colour Colour::OrangeRed = Colour(255, 69, 0);
	const Colour Colour::Orchid = Colour(218, 112, 214);
	const Colour Colour::PaleGoldenRod = Colour(238, 232, 170);
	const Colour Colour::PaleGreen = Colour(152, 251, 152);
	const Colour Colour::PaleTurquoise = Colour(175, 238, 238);
	const Colour Colour::PaleVioletRed = Colour(219, 112, 147);
	const Colour Colour::PapayaWhip = Colour(255, 239, 213);
	const Colour Colour::PeachPuff = Colour(255, 218, 185);
	const Colour Colour::Peru = Colour(205, 133, 63);
	const Colour Colour::Pink = Colour(255, 192, 203);
	const Colour Colour::Plum = Colour(221, 160, 221);
	const Colour Colour::PowderBlue = Colour(176, 224, 230);
	const Colour Colour::Purple = Colour(128, 0, 128);
	const Colour Colour::Red = Colour(255, 0, 0);
	const Colour Colour::RosyBrown = Colour(188, 143, 143);
	const Colour Colour::RoyalBlue = Colour(65, 105, 225);
	const Colour Colour::SaddleBrown = Colour(139, 69, 19);
	const Colour Colour::Salmon = Colour(250, 128, 114);
	const Colour Colour::SandyBrown = Colour(244, 164, 96);
	const Colour Colour::SeaGreen = Colour(46, 139, 87);
	const Colour Colour::SeaShell = Colour(255, 245, 238);
	const Colour Colour::Sienna = Colour(160, 82, 45);
	const Colour Colour::Silver = Colour(192, 192, 192);
	const Colour Colour::SkyBlue = Colour(135, 206, 235);
	const Colour Colour::SlateBlue = Colour(106, 90, 205);
	const Colour Colour::SlateGray = Colour(112, 128, 144);
	const Colour Colour::Snow = Colour(255, 250, 250);
	const Colour Colour::SpringGreen = Colour(0, 255, 127);
	const Colour Colour::SteelBlue = Colour(70, 130, 180);
	const Colour Colour::Tan = Colour(210, 180, 140);
	const Colour Colour::Teal = Colour(0, 128, 128);
	const Colour Colour::Thistle = Colour(216, 191, 216);
	const Colour Colour::Tomato = Colour(255, 99, 71);
	const Colour Colour::Turquoise = Colour(64, 224, 208);
	const Colour Colour::Violet = Colour(238, 130, 238);
	const Colour Colour::Wheat = Colour(245, 222, 179);
	const Colour Colour::White = Colour(255, 255, 255);
	const Colour Colour::WhiteSmoke = Colour(245, 245, 245);
	const Colour Colour::Yellow = Colour(255, 255, 0);
	const Colour Colour::YellowGreen = Colour(154, 205, 50);

} }