#pragma once

#include <cmath>
#include <array>
#include <cstdint>
#include <iostream>

//To save from weird ambiguity errors there are two make functions
//rather than an overloaded constructor, the Color is stored as four
//floats internally, but can be retrieved as both bytes as floats.
//It is stored this way to allow the values to be below 0.0 and above
//1.0, for certain operations.
//Using bytes is therefore a little inaccurate.
class Color
{
public:
	typedef uint8_t byte_t;

	Color()
	{
		op = multiply_op;
	}

	Color static white()
	{
		return Color::make_from_floats(1.f, 1.f, 1.f);
	}

	Color static black()
	{
		return Color::make_from_floats(0.f, 0.f, 0.f);
	}

	Color static red()
	{
		return Color::make_from_floats(1.f, 0.f, 0.f);
	}

	Color static green()
	{
		return Color::make_from_floats(0.f, 1.f, 0.f);
	}

	Color static blue()
	{
		return Color::make_from_floats(0.f, 0.f, 1.f);
	}

	Color static make_from_bytes(byte_t red_, byte_t green_, byte_t blue_, byte_t alpha_ = 255)
	{
		Color ret;
		ret.Colors_[red_i] = to_float(red_);
		ret.Colors_[green_i] = to_float(green_);
		ret.Colors_[blue_i] = to_float(blue_);
		ret.Colors_[alpha_i] = to_float(alpha_);
		return ret;
	}

	//Range for a Color is [0-1]
	Color static make_from_floats(float red_, float green_, float blue_, float alpha_ = 1.0f)
	{
		Color ret;
		ret.Colors_[red_i] = red_;
		ret.Colors_[green_i] = green_;
		ret.Colors_[blue_i] = blue_;
		ret.Colors_[alpha_i] = alpha_;
		return ret;
	}

	Color static make_from_bytes(byte_t greyscale, byte_t alpha_ = 255)
	{
		Color ret;
		ret.Colors_[red_i] = to_float(greyscale);
		ret.Colors_[green_i] = to_float(greyscale);
		ret.Colors_[blue_i] = to_float(greyscale);
		ret.Colors_[alpha_i] = to_float(alpha_);
		return ret;
	}

	//Range for a Color is [0-1]
	Color static make_from_floats(float greyscale, float alpha_ = 1.0f)
	{
		Color ret;
		ret.Colors_[red_i] = greyscale;
		ret.Colors_[green_i] = greyscale;
		ret.Colors_[blue_i] = greyscale;
		ret.Colors_[alpha_i] = alpha_;
		return ret;
	}

	//Clamps ALL the values so that they are within the range [0-1], this is
	//a hard operation, and is not reversable. Returns itself for chaining.
	Color& clamp()
	{
		for (auto& c : Colors_)
		{
			c = clamp(c);
		}
		return *this;
	}

	float get_red() const
	{
		return Colors_[red_i];
	}

	float get_green() const
	{
		return Colors_[green_i];
	}

	float get_blue() const
	{
		return Colors_[blue_i];
	}

	float get_alpha() const
	{
		return Colors_[alpha_i];
	}

	void set_red(float red_)
	{
		Colors_[red_i] = red_;
	}

	void set_green(float green_)
	{
		Colors_[green_i] = green_;
	}

	void set_blue(float blue_)
	{
		Colors_[blue_i] = blue_;
	}

	void set_alpha(float alpha_)
	{
		Colors_[alpha_i] = alpha_;
	}

	byte_t get_red_byte() const
	{
		return to_byte(Colors_[red_i]);
	}

	byte_t get_green_byte() const
	{
		return to_byte(Colors_[green_i]);
	}

	byte_t get_blue_byte() const
	{
		return to_byte(Colors_[blue_i]);
	}

	byte_t get_alpha_byte() const
	{
		return to_byte(Colors_[alpha_i]);
	}

	void set_red_byte(byte_t red_)
	{
		Colors_[red_i] = to_float(red_);
	}

	void set_green_byte(byte_t green_)
	{
		Colors_[green_i] = to_float(green_);
	}

	void set_blue_byte(byte_t blue_)
	{
		Colors_[blue_i] = to_float(blue_);
	}

	void set_alpha_byte(byte_t alpha_)
	{
		Colors_[alpha_i] = to_float(alpha_);
	}

	/*	set the operation to be performed when apply is 
	 *	called with this Color as an argument.
	 *	this sets the operation to add.
	 */
	void add()
	{
		op = add_op;
	}

	/*	set the operation to be performed when apply is
	*	called with this Color as an argument.
	*	this sets the operation to subtract.
	*/
	void subtract()
	{
		op = subtract_op;
	}

	/*	set the operation to be performed when apply is
	*	called with this Color as an argument.
	*	this sets the operation to multiply.
	*/
	void multiply()
	{
		op = multiply_op;
	}

	/*	Usage:
	 *		Color c = Color::white();
	 *		c = apply(Color::black().subtract());
	 *		
	 *	Will give the same result as:
	 *		Color c = Color::white();
	 *		Color result = c.add(Color().subtract());
	 *		c = result;
	 *	Note that the second line doesn't change object c,
	 *	c is modified first when result is assigned to it.
	 */
	Color apply(Color& other) const
	{
		return do_operation(*this, other, other.op);
	}

	Color add(const Color& c0, const Color& c1)
	{
		return do_operation(c0, c1, add_op);
	}

	Color subtract(const Color& c0, const Color& c1)
	{
		return do_operation(c0, c1, subtract_op);
	}

	Color multiply(const Color& c0, const Color& c1)
	{
		return do_operation(c0, c1, multiply_op);
	}
private:
	enum Color_operator {add_op, subtract_op, multiply_op};

	static Color do_operation(const Color& a, const Color& b, Color_operator op)
	{
		Color ret;

		switch (op)
		{
		case add_op:
			for (int i = 0; i < 4; ++i)
			{
				ret.Colors_[i] = a.Colors_[i] + b.Colors_[i];
			}
			break;
		case subtract_op:
			for (int i = 0; i < 4; ++i)
			{
				ret.Colors_[i] = a.Colors_[i] - b.Colors_[i];
			}
			break;
		case multiply_op:
			for (int i = 0; i < 4; ++i)
			{
				ret.Colors_[i] = a.Colors_[i] * b.Colors_[i];
			}
			break;
		default:
			break;
		}

		return ret;
	}

	enum Color_index { red_i, green_i, blue_i, alpha_i };
	std::array<float, 4U> Colors_;
	Color_operator op;

	static inline float to_float(byte_t val)
	{
		return static_cast<float>(val) / 255.0;
	}

	//We must be careful that this doesn't just return 255 when the value 
	//of the float is a very small negative number, or the reverse.
	//This is the problem with this approach. There are other problems, of course
	static inline byte_t to_byte(float val)
	{
		return static_cast<byte_t>(clamp(val * 255.0, 0.0, 255.0));;
	}

	static inline float clamp(float val, float min = 0.0f, float max = 1.0f)
	{
		return std::fmax(min, std::fmin(max, val));
	}
};
