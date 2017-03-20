//////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Andreas Milton Maniotis
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////////////////

// Example: Builder Pattern

#include <iostream>
#include <string>

#include "nargs.hpp"

class product_with_ugly_interface
{
public:
    product_with_ugly_interface(std::string const& name
				, double price
				, double weight				
				, int height_in_cm, int width_in_cm, int depth_in_cm
				, bool online_delivery_possible  = true
				, bool is_on_discount            = false
				, std::string const& other_stats = std::string() 
)
	: name_(name)
	, price_(price)
	,  weight_(weight)
	, height_in_cm_(height_in_cm)
	, width_in_cm_(width_in_cm)
	, depth_in_cm_(depth_in_cm)	  
	,  online_delivery_possible_(online_delivery_possible)
	,  is_on_discount_(is_on_discount)
	,  other_stats_(other_stats)
    { }


    auto& reset(std::string const& name, double price, double weight,
				int height_in_cm, int width_in_cm, int depth_in_cm,
				bool online_delivery_possible  = true,
				bool is_on_discount            = false,
				std::string const& other_stats = std::string() )
    {
	name_           = name;
	price_          = price;
	weight_         = weight;
	height_in_cm_   = height_in_cm;
	width_in_cm_    = width_in_cm;
	depth_in_cm_    = depth_in_cm;	
	online_delivery_possible_ = online_delivery_possible;
	is_on_discount_ = is_on_discount;
	other_stats_    = other_stats;

	return *this;
    }

    friend std::ostream& operator<<(std::ostream& os,
				    product_with_ugly_interface const& p)
    {
	os << std::boolalpha
	   << "name:           " << p.name_           << std::endl
	   << "price:          " << p.price_          << std::endl
	   << "weight:         " << p.weight_         << std::endl
	   << "height (cm):    " << p.height_in_cm_   << std::endl
	   << "width  (cm):    " << p.width_in_cm_    << std::endl
	   << "depth  (cm):    " << p.depth_in_cm_    << std::endl
	   << "online delivery possible: " << p.online_delivery_possible_ << std::endl
	   << "is on discount: " << p.is_on_discount_ << std::endl
	   <<  "other stats:   " << p.other_stats_    << std::endl
	   << std::endl;
	return os;
    }
   
private:
    std::string name_;
    double      price_;
    double      weight_;
    int         height_in_cm_;
    int         width_in_cm_;
    int         depth_in_cm_;    
    bool        online_delivery_possible_;
    bool        is_on_discount_;
    std::string other_stats_;
};

int main()
{
    NARG_PAIR(name,        std::string);
    NARG_PAIR(price,            double);
    NARG_PAIR(weight,           double);
    NARG_PAIR(height,              int);
    NARG_PAIR(width,               int);
    NARG_PAIR(depth,               int);
    NARG_PAIR(online_delivery,    bool);
    NARG_PAIR(on_discount,        bool);
    NARG_PAIR(other_stats, std::string);

    auto product_builder = nargs::signature<name 
					    , price
					    , weight
					    , height, width, depth
					    , online_delivery
					    , on_discount
					    , other_stats >::
	defaults(on_discount(false),
		 online_delivery(true),
		 other_stats("none")).

	builder<product_with_ugly_interface>();

    product_with_ugly_interface x =
	product_builder( price(24.4), height(2.5), name("hello"), weight(243.4), depth(40), width(300), on_discount(true)
			 //, online_delivery(false)
			 //		 , other_stats("none")
	    );

    std::cout << x << std::endl;


    product_with_ugly_interface p =
	product_builder( height(1), name("HelloWorld"),
			 depth(2), price(200.50), width(3),
			 online_delivery(false),
			 weight(23.4),
			 on_discount(false),
			 other_stats("none")
	    );

    std::cout << p << std::endl;
    

}
   
