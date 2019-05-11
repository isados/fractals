#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <complex>
#include <unordered_map>
#include <math.h>
#include "timer.hpp"
#include "image.hpp"
#include <cmath>
using namespace sf;
using namespace std;

#define WIDTH 1050
#define HEIGHT 700
#define SIZE WIDTH, HEIGHT
#define MAX_DELAY 1000
#define NUM_THREADS 128

//Working with double seems to be the fastest
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
  Mandelbrot(size_t wd, size_t ht ,int iters=20,
            ld x_srt=-2,ld x_stp=1,ld y_srt=-1,ld y_stp=1
            )
  :VertexArray{Points,wd*ht},
    width{wd}, height{ht},
    max_iterations{iters},
    x_srt{-2}, x_stp{1}, y_srt{-1}, y_stp{1}
  {
    int index=0;
    for(uint y=0; y < height ;y++)
    for(uint x=0; x < width ;x++)
      {
        (*this)[index].position = Vector2f(x, y);
        (*this)[index].color = Color::Black;
        index++;
      }

    // generate_palette(max_iterations);

    //Generate ranges for plotting
    x_range = linspace(x_srt,x_stp,width);
    y_range = linspace(y_srt,y_stp,height);

    //Time both functions
    Timer tr;

    generate_set(x_srt, x_stp, y_srt, y_stp,true);
    cout<<"Time for threaded function "
        <<tr.elapsed()/1000<<"s"
        <<" with "<<NUM_THREADS<<" threads"<<endl;

    // tr.reset();
    // generate_set(x_srt, x_stp, y_srt, y_stp);
    // cout<<"Time for normal function "
    //     <<tr.elapsed()/1000<<"s"<<endl;

  }

  void generate_set(ld x_srt=-2, ld x_stp=1, ld y_srt=-1, ld y_stp=1, bool threaded=false)
    {
      if (this->x_srt!=x_srt || this->x_stp!=x_stp)
        x_range = linspace(x_srt,x_stp,width);
      if (this->y_srt!=y_srt || this->y_stp!=y_stp)
        y_range = linspace(y_srt,y_stp,height);

      if (!threaded) {
        int index=0;
        for(uint y=0; y < height ;y++)
        for(uint x=0; x < width ;x++)
            (*this)[index++].color = retrieve_color({x_range[x],y_range[y]},max_iterations);
      }
      else{
          uint batch_size, rem;
          batch_size = height / NUM_THREADS; rem = height % NUM_THREADS;

          vector<thread> threads;
          uint scale = 0;
          for (;scale<NUM_THREADS-1; scale++){
            threads.push_back(thread(&Mandelbrot::threaded_func, this,
              batch_size*scale, batch_size*(scale+1)));
          }
          threads.push_back(thread(&Mandelbrot::threaded_func, this,
            batch_size*scale, batch_size*(scale+1)+rem));

          //Check for completion of threads
          for (thread &x : threads) {x.join();}
      }
    }

  void generate_set(bool threaded){
    generate_set(x_srt, x_stp, y_srt, y_stp, threaded);}

  void generate_sequence(int iters){
        int temp{this->max_iterations};
        this->max_iterations = iters;
        // generate_palette(this->max_iterations);
        generate_set(true);
        this->max_iterations = temp;
      }
  int getMaxIters(){return max_iterations;}
  float getHeight(){return height;}
  float getWidth(){return width;}

private:
  size_t width;
  size_t height;
  int max_iterations;
  ld x_srt, x_stp, y_srt, y_stp;
  vector<ld> x_range;
  vector<ld> y_range;
  // unordered_map <uint, Color> palette;

  inline void threaded_func(uint start, uint end){
      uint index = start * width;
      for(uint y = start; y < end ;y++)
        for(uint x=0; x < width ;x++)
          (*this)[index++].color = retrieve_color({x_range[x],y_range[y]},max_iterations);
    }

  inline  Color retrieve_color(const complex<ld> &c, uint max_iters){
      complex<ld> z{0};
      ld sn = 0,dist;

      constexpr ld ESCAPE_RADIUS = 2;
      uint iter = 0;
      for (; iter < max_iters; iter++){
        z = pow(z,2) + c;
        dist = abs(z);
        if (dist > ESCAPE_RADIUS)
          {break;}
      }

      if (iter ==  max_iters) return Color::Black;

      //Magic function
      // sn = iter - log(log(dist)/log(4))/log(2);
      sn = iter + 1 - log(log(dist)/log(2));

      constexpr Uint32 start = 0, end = 0xFFFFFF;
      constexpr Uint32 range = end - start;
      Uint32 res = 0;
      res = end - round((sn/(max_iters-1))*range);
      return Color( (res<<8) + 0xFF );

      // if (sn > max) {max = sn; cout<<"max : "<<max<<endl;}
      // else cout<<log(abs(z))<<endl;
      return Color::Black;
    }

};

void zoomin(Mandelbrot& plot){
  static uint num_zooms=1000;
  static float scale = 10;
  // static float aspect = 2/3;

  static ld x_srt=-2, x_end=1, y_srt=-1, y_end=1;
  static const ld height=plot.getHeight(), width=plot.getWidth();

  static ld center = 0;
  static ld bullseye = -2;
  if(num_zooms){
  ld xdelta = (x_end - x_srt) / (width - 1);
  ld ydelta = (y_end - y_srt) / (height - 1);
  if (center > bullseye)
    { x_end -= 2*scale * xdelta;
      y_end -= scale * ydelta;
      y_srt += scale * ydelta;
    }
  else if (center < bullseye)
      { x_srt += 2*scale * xdelta;
        y_end -= scale * ydelta;
        y_srt += scale * ydelta;
      }
  else if(center == bullseye){
      x_end -= scale * xdelta;
      x_srt += scale * xdelta;
      y_end -= scale * ydelta;
      y_srt += scale * ydelta;
  }


  //Execution Phase
  plot.generate_set(x_srt, x_end, y_srt, y_end,true);
  // plot.generate_set(x_srt, x_end, y_srt, y_end);
  --num_zooms;
}

}

int main(int argc, char** args)
{
    RenderWindow window(VideoMode(SIZE), "Fractal");
    window.setFramerateLimit(60);

    // Check for input arguments, specifically for number of iterations 'i'
    string i = "25";
    if (argc > 1) {i = args[1];}

    // Mandelbrot img(SIZE,stoul(i),-5,1,-2,2);
    Mandelbrot img(SIZE,stoul(i));

    int iter=5;
    int max_iterations = 5000;

    while (window.isOpen())
    {
        // if (iter<=max_iterations){
        //   img.generate_sequence(iter+=100);
        //   cout<<iter<<endl;
        // }

        // zoomin(img);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        if(Keyboard::isKeyPressed(Keyboard::Key::Escape)) break;
        window.draw(img);
        window.display();
    }
    image::write_image(img, WIDTH, HEIGHT);
    return 0;
}
