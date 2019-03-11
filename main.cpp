#include <SFML/Graphics.hpp>
#include <iostream>
#include <complex>
#include "timer.cpp"
using namespace sf;
using namespace std;

#define WIDTH 1050
#define HEIGHT 700
#define SIZE WIDTH, HEIGHT
#define MAX_DELAY 1000
using ld = double;

template<typename T>
vector<ld> linspace(T start_in, T end_in, int num_in)
  {

    vector<ld> linspaced;

    ld start = static_cast<ld>(start_in);
    ld end = static_cast<ld>(end_in);
    ld num = static_cast<ld>(num_in);

    if (num == 0) { return linspaced; }
    if (num == 1)
      {
        linspaced.push_back(start);
        return linspaced;
      }

    ld delta = (end - start) / (num - 1);

    for(int i=0; i < num-1; ++i)
      {
        linspaced.push_back(start + delta * i);
      }
    linspaced.push_back(end); // I want to ensure that start and end
                              // are exactly the same as the input
    return linspaced;
  }

class Mandelbrot: public VertexArray{
public:
  Mandelbrot(ld x_srt,ld x_stp,ld y_srt,ld y_stp
            ,size_t wd, size_t ht
            ,int iters=10
            )
  :VertexArray{Points,wd*ht},
    width{wd}, height{ht}
    ,max_iterations{iters}
  {
    int index=0;
    for(uint row=0; row < width ;row++)
    for(uint colum=0; colum < height ;colum++)
      {
        (*this)[index].position = Vector2f(row, colum);
        (*this)[index].color = Color::Black;
        index++;
      }
    // generate_set(x_srt, x_stp, y_srt, y_stp ,max_iterations);
  }

  void generate_set(ld x_srt,ld x_stp,ld y_srt,ld y_stp
    ,int iters)
    {
      int index=0;
      vector<ld> x_range = linspace(x_srt,x_stp,width);
      vector<ld> y_range = linspace(y_srt,y_stp,height);
      for(uint row=0; row < width ;row++)
      for(uint colum=0; colum < height ;colum++)
        { auto &color_pixel = (*this)[index++].color;
          if (color_pixel == Color::Black)
            color_pixel = retrieve_color({x_range[row],y_range[colum]},iters);
        }
    }

  int getMaxIters(){
      return max_iterations;
    }

private:
  size_t width;
  size_t height;
  int max_iterations;
  Color retrieve_color(const complex<ld> &c, int max_iters){
    complex<ld> z{0};
    for (int iter = 0; iter <= max_iters; iter++){
      z = pow(z,2) + c;
      if (abs(z) > 3)
        return Color::White;
    }
    return Color::Black;
  }


};

int main(){
    RenderWindow window(VideoMode(SIZE), "Fractal");
    window.setFramerateLimit(40);

    Mandelbrot img(-2,1,-1,1,WIDTH,HEIGHT,20);

    int iter=0;
    int max_iterations = img.getMaxIters();
    Timer tmr;
    while (window.isOpen())
    {
        if (iter<=max_iterations)
          img.generate_set(-2,1,-1,1,iter++);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(img);
        window.display();

        // while(tmr.elapsed() < MAX_DELAY){
        //     //do nothing
        //   }
        // tmr.reset();
    }

    return 0;
}
