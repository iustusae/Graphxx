#include "Graphing.hpp"
#include "Tokenizer.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <unordered_map>

sf::VertexArray getAllPoints(int max_y, const std::string &expression) {

  sf::VertexArray points{};
  std::unordered_map<char, double> var_values{{'x', 0.0}};

  while (var_values.at('x') < max_y) {
    double y = Tokenizer::evaluate<double>(expression, var_values);
    points.append(
        sf::Vertex(sf::Vector2f(var_values.at('x'), y), sf::Color::Blue));
    std::cout << "Point " << var_values.at('x') << ": (" << y << ")\n";
    var_values.at('x') += 1.0;
  }

  return points;
}
void Grapher::draw(const std::string &expression) {
  // Draw the graph here
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
  sf::Vertex y_axis[]{
      sf::Vertex(sf::Vector2f(400, 0)),
      sf::Vertex(sf::Vector2f(400, 600)),
  };
  sf::Vertex x_axis[]{
      sf::Vertex(sf::Vector2f(0, 300)),
      sf::Vertex(sf::Vector2f(800, 300)),
  };

  auto vertices = getAllPoints(600, expression);
  sf::CircleShape center_circle{sf::CircleShape(3, 100)};
  center_circle.setFillColor(sf::Color::Yellow);
  center_circle.setPosition(400, 300);
  center_circle.setOrigin(center_circle.getRadius(), center_circle.getRadius());
  // Start the main loop - the program will continue running until w
  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    sf::Event event;
    while (window.pollEvent(event)) {
      // "close requested" event: we close the window
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::MouseMoved) {
        // std::cout << sf::Mouse::getPosition().x << ", "
        //           << sf::Mouse::getPosition().y << '\n';
      }
    }
    window.clear(sf::Color::Black);
    window.draw(y_axis, 2, sf::Lines);
    window.draw(x_axis, 2, sf::Lines);
    window.draw(center_circle);
    window.draw(vertices);
    window.display();
  }
}