# README #

Nargs provides a non-intrusive generative framework to incept the Python-feature
of named arguments to callable objects in C++.

It provides a generalisation of std::invoke for suitable signatures f(T1, ... , Tn)  with the following uniqueness condition:  no type Ti of an argument can be converted into the type Tj of an argument at another position . In this case the arguments for an invocation can be given in any order. (This behaviour cannot be implemented by hand, but needs a generative framework because of the factorial growth of the possible combinations).

Nargs can be used to generate new interfaces, but it can also serve as a tool
for non-intrusive improvement of the safety and readability of existing interfaces.

==================================================================================

(0) Motivation
----------------------------------------------------------------------------------
0.a: A Simple Example with Named Arguments




0.b.: A Builder Example:


struct date
{
public:
    date(int d, int m, int y) {  assert_validity_of_this_date(); }

    int get_day()   const { return day_;   }  // range: 1..31
    int get_month() const { return month_; }  // range: 1..12
    int get_year()  const { return year_;  } 

private:
    int day_, month_, year_;
    void assert_validity_of_this_date() { //throw exception if date is invalid... }
};




Regardless how carefully we implement the assertion method check_validity_of_this_date() to 
ensure the validity of an input made by a user of the class, there is a certain risk inherent to 
the interface of date class, which cannot be eliminated by the assertion method. For instance a user might 
be accustomed to the order mm-dd-yyyy  (mm month, dd day, yyyy year) for dates. If the user calls the date constructor
   
  date(2, 3, 2016)

then the valid date "March, 2nd, 2016" will be constructed. assert_validity() has no chance to detect that the user 
wanted "February, 3rd, 2016" as both dates are valid inputs. If we are able or willing to change the source code of date, 
this ambiguity problem can be fixed easily. But in many situations it is impossible or undesirable to change given source 
code with ambiguous interfaces. Then nargs gives a fast and easy non-intrusive solution. This can be done in four lines: 

NARG_PAIR(day,   int); 
NARG_PAIR(month, int);
NARG_PAIR(year,  int);

auto create_date = signature<day, month, year>::builder<date>();

create_date is a safe builder for dates. We need not bother about the order of the parameters in the construction process of  dates. All of the six calls of create_date with permuted arguments  will create "February, 3rd, 2016": 


create_date(year(2016), month(2), day(3)); 

create_date(year(2016), day(3), month(2)); 

create_date(month(2), year(2016), day(3)); 

create_date(day(3), year(2016), month(2));

create_date(day(3), month(2), year(2016)); 

create_date(month(2), day(3), year(2016)); 

========================================================

(1) Design-Objectives: 

1. Perfect Forwarding 
2. Name Overloading
3. Validity of signatures and signatures collections for permutation-invariant 
invokations is checked at compile time 
4. Apart from accepting permuted input orders, permutation-invariant invokations behave exactly like std::invoke, that is: 
permutation invariant invokations transfer their arguments to std::invoke in the right order. Left-value references must be wrapped by std::ref. 
(There is an additional policy to allow for the direct use of lvalue reference, but it is not default behaviour).



