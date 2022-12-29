# Cello

Classes for working with juce ValueTree objects. 

**Fall 2022 * Brett g Porter**

brett@bgporter.net

## Motivation and Overview

### Confessions of a `ValueTree` Skeptic

I've been using the JUCE framework for over a decade now, but there's a major component of JUCE that never clicked for me as a developer -- ValueTrees. This wasn't a problem for me until I changed jobs and started needing to work on a mature codebase that made significant use of them. This code makes efforts to hide some of the more cumbersome or repetitive aspects of integrating ValueTrees into an application, but that `ValueTreeWrapper` class still seemed like it required too much effort to work with; where I'm used to thinking in terms of objects that contain values, any time I needed to get near data that's stored in a ValueTree, it was impossible to avoid the awareness that I was always working through an API to perform operations on data that should just be directly manipulable, and while the wrapper class approach mitigated this to some extent, there was still more boilerplate code to write than seems good to me, as well as other places where the gaps around the abstraction were more obvious than I like. 

I've always found that the only way for me to work through these issues when I encounter them is to sit down with a blank document in an editor and start enumerating the problems that I see with a system and use that as a guide to start thinking about ways that I can engineer around the parts that aren' tmy favorite, and sometimes how I can reframe my thinking to start seeing superpowers where I thought there were deficiencies. 

One of my current teammates has expressed confusion that I wasn't immediately on board with ValueTrees, and his defense of them was key to my eventually starting this re-analysis. They give you: 

- A really easy way to capture and pass around the entire state of your application's data at run time
- A rich mechanism to watch that data at a fine degree of granularity
- Trivially easy persistence of application state

...but at the cost (in comparison to using native POD or class/struct variables) of being:
- slower
- less convenient to use
- less type-safe, since all values are stored in the JUCE `var` variant type. 

As I started listing the tradeoffs, I considered ways to work around the convenience and type-safety issues. I also reflected on the years in my career when I wrote far more Python code than I did C++, and many of these same charges can be filed against that language, which I love. 

At one level, you can look at Python as being nothing but a bunch of associative arrays (or in python, `dict`s) with the ability to be manipulated dynamically by code. Once I started thinking in those terms, the project became much more interesting. 

As frequently happens with me, these thoughts sat collecting dust in a document until I hit upon a name for the project -- `cello`, short for 'cellophane' (since the code is wrapping a ValueTree)

### `cello`

In short, my goal was: create a set of C++ classes that I can derive my own classes from where member variables are stored transparently in JUCE ValueTrees instead of directly in those object instances, combining the comfort and simplicity of working with normal-looking C++ code with the benefits and tradeoffs of ValueTrees. 

Something like:

```cpp

// define a struct with two members and then create an instance
struct CelloDemo : public cello::Object 
{
    // we'll figure this type out shortly...
    cello::Value<int> x;
    cello::Value<float> y;
};

CelloDemo demoObject;

// give that object a lambda to call whenever the value of `x` changes. 
demoObject.onPropertyChange(demoObject.x, [&demoObject]() 
{ 
    std::cout << "x changed to " << demoObject.x << "\n"; 
});

// after executing this line, stdout should print: "x changed to 100"
demoObject.x = 100;

```
## Values


- 
## Objects

t/k
