#include <SFML/Graphics.hpp>
#include <iostream>
#include "timer.cpp"
using namespace sf;
using namespace std;

#define WIDTH 800
#define HEIGHT 600
#define SIZE WIDTH, HEIGHT
#define MAX_DELAY 1000

class Mandelbrot: public VertexArray{
public:
  Mandelbrot(size_t wd, size_t ht, Color cl):
    VertexArray{Points,wd*ht},
    width{wd}, height{ht}, color{cl}
  {
    int index=0;
    for(int i=0; i<width ;i++)
    for(int c=0; c<height ;c++)
    {
      (*this)[index].position = Vector2f(i, c);
      (*this)[index++].color = color;
    }
  }
private:
  size_t width; size_t height;
  Color color;
};

void switch_colors(VertexArray& plot){
  static Color color = Color::Red;
  if (color == Color::Red)
    color = Color::Yellow;
  else
    color = Color::Red;
    int index=0;
    for(int i=0; i<WIDTH ;i++)
      for(int c=0; c<HEIGHT ;c++)
      {
        plot[index].position = Vector2f(i, c);
        plot[index++].color = color;
      }
}

int main(){
    RenderWindow window(VideoMode(SIZE), "MANDELBROT");
    window.setFramerateLimit(40);

    Mandelbrot img(WIDTH,HEIGHT,Color::Red);

    Timer tmr;
    while (window.isOpen())
    {
      double t = tmr.elapsed();
      if(t > MAX_DELAY){
          tmr.reset();
          switch_colors(img);
        }

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(img);
        window.display();
    }

    return 0;
}
