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

// sf::VertexArray getAllPoints(int max_y, const std::string &expression) {

//   sf::VertexArray points{};
//   std::unordered_map<char, double> var_values{{'x', -max_y}};

//   while (var_values.at('x') < max_y) {
//     double y = Tokenizer::evaluate<double>(expression, var_values);
//     points.append(sf::Vertex(sf::Vector2f(var_values.at('x') + 400, 300 - y),
//                              sf::Color::Blue));
//     std::cout << "Point " << var_values.at('x') << ": (" << y << ")\n";
//     var_values.at('x') += 0.1;
//   }

//   return points;
// }
// sf::VertexArray getAllPoints(int max_y, const std::string &expression) {
//   sf::VertexArray points{sf::PrimitiveType::Points}; // For individual points
//   sf::VertexArray lines{
//       sf::PrimitiveType::LinesStrip}; // For lines between points
//   std::unordered_map<char, double> var_values{{'x', -max_y}};

//   double previous_x = 0.0;
//   double previous_y = 0.0;

//   while (var_values.at('x') < max_y) {
//     double y = Tokenizer::evaluate<double>(expression, var_values);

//     // Centering the coordinates
//     float screen_x = var_values.at('x') + 400;
//     float screen_y = 300 - y;

//     points.append(
//         sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Blue));
//     lines.append(sf::Vertex(sf::Vector2f(screen_x, screen_y),
//     sf::Color::Blue));

//     // Add a line between the current point and the previous point
//     if (var_values.at('x') > 0) { // Skip first point for line
//       lines.append(sf::Vertex(sf::Vector2f(previous_x + 400, 300 -
//       previous_y),
//                               sf::Color::Blue));
//     }

//     std::cout << "Point " << var_values.at('x') << ": (" << y << ")\n";
//     previous_x = var_values.at('x');
//     previous_y = y;

//     var_values.at('x') += 0.1;
//   }

//   return lines; // Return only the lines for drawing
// }
// void Grapher::draw(const std::string &expression) {
//   // Draw the graph here
//   sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
//   sf::Vertex y_axis[]{
//       sf::Vertex(sf::Vector2f(400, 0)),
//       sf::Vertex(sf::Vector2f(400, 600)),
//   };
//   sf::Vertex x_axis[]{
//       sf::Vertex(sf::Vector2f(0, 300)),
//       sf::Vertex(sf::Vector2f(800, 300)),
//   };

//   auto vertices = getAllPoints(600, expression);
//   sf::CircleShape center_circle{sf::CircleShape(3, 100)};
//   center_circle.setFillColor(sf::Color::Yellow);
//   center_circle.setPosition(400, 300);
//   center_circle.setOrigin(center_circle.getRadius(),
//   center_circle.getRadius());
//   // Start the main loop - the program will continue running until w
//   // run the program as long as the window is open
//   while (window.isOpen()) {
//     // check all the window's events that were triggered since the last
//     // iteration of the loop
//     sf::Event event;
//     while (window.pollEvent(event)) {
//       // "close requested" event: we close the window
//       if (event.type == sf::Event::Closed)
//         window.close();
//       else if (event.type == sf::Event::MouseMoved) {
//         // std::cout << sf::Mouse::getPosition().x << ", "
//         //           << sf::Mouse::getPosition().y << '\n';
//       }
//     }
//     window.clear(sf::Color::Black);
//     window.draw(y_axis, 2, sf::Lines);
//     window.draw(x_axis, 2, sf::Lines);
//     window.draw(center_circle);
//     window.draw(vertices);
//     window.display();
//   }
// }

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <sstream>

sf::VertexArray Grapher::getAllPoints(int max_y, const std::string &expression,
                                      float zoom) {
  sf::VertexArray lines(sf::PrimitiveType::LinesStrip);
  std::unordered_map<char, double> var_values{{'x', -max_y / zoom}};
  double previous_x = 0.0;
  double previous_y = 0.0;

  while (var_values.at('x') < max_y / zoom) {
    double y = Tokenizer::evaluate<double>(expression, var_values);
    float screen_x = (var_values.at('x') * zoom) + 400;
    float screen_y = 300 - (y * zoom);

    lines.append(sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Blue));

    if (var_values.at('x') > -max_y / zoom) {
      lines.append(sf::Vertex(
          sf::Vector2f((previous_x * zoom) + 400, 300 - (previous_y * zoom)),
          sf::Color::Blue));
    }

    previous_x = var_values.at('x');
    previous_y = y;
    var_values.at('x') += 0.1 / zoom;
  }

  return lines;
}

sf::Text Grapher::createText(const sf::Font &font, const std::string &content,
                             float x, float y) {
  sf::Text text;
  text.setFont(font);
  text.setString(content);
  text.setCharacterSize(12);
  text.setFillColor(sf::Color::White);
  text.setPosition(x, y);
  return text;
}

void Grapher::draw(const std::string &expression) {
  sf::RenderWindow window(sf::VideoMode(800, 600), "Graph Viewer");
  sf::View view(sf::FloatRect(0, 0, 800, 600));
  window.setView(view);

  sf::Font font;
  if (!font.loadFromFile("/home/ayman/.local/share/fonts/NerdFonts/"
                         "UbuntuMonoNerdFont-Regular.ttf")) {
    std::cerr << "Failed to load font" << std::endl;
    return;
  }

  sf::Vertex y_axis[] = {
      sf::Vertex(sf::Vector2f(400, 0)),
      sf::Vertex(sf::Vector2f(400, 600)),
  };
  sf::Vertex x_axis[] = {
      sf::Vertex(sf::Vector2f(0, 300)),
      sf::Vertex(sf::Vector2f(800, 300)),
  };

  sf::CircleShape center_circle(3, 100);
  center_circle.setFillColor(sf::Color::Yellow);
  center_circle.setPosition(400, 300);
  center_circle.setOrigin(center_circle.getRadius(), center_circle.getRadius());

  float zoom = 1.0f;
  sf::VertexArray vertices = getAllPoints(600, expression, zoom);

  sf::RectangleShape hoverBox(sf::Vector2f(100, 50));
  hoverBox.setFillColor(sf::Color(0, 0, 0, 200));
  sf::Text hoverText = createText(font, "", 0, 0);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0)
          zoom *= 1.1f;
        else if (event.mouseWheelScroll.delta < 0)
          zoom /= 1.1f;

        vertices = getAllPoints(600, expression, zoom);
        view.zoom(1.f / (event.mouseWheelScroll.delta > 0 ? 1.1f : 1.f / 1.1f));
        window.setView(view);
      }
    }

    window.clear(sf::Color::Black);

    window.draw(y_axis, 2, sf::Lines);
    window.draw(x_axis, 2, sf::Lines);
    window.draw(center_circle);
    window.draw(vertices);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    float closestDistance = std::numeric_limits<float>::max();
    sf::Vector2f closestPoint;

    for (size_t i = 0; i < vertices.getVertexCount(); ++i) {
      sf::Vector2f vertexPos = vertices[i].position;
      float distance = std::sqrt(std::pow(vertexPos.x - worldPos.x, 2) +
                                 std::pow(vertexPos.y - worldPos.y, 2));

      if (distance < closestDistance) {
        closestDistance = distance;
        closestPoint = vertexPos;
      }
    }

    if (closestDistance < 5.0f) {
      float x = (closestPoint.x - 400) / zoom;
      float y = (300 - closestPoint.y) / zoom;

      std::ostringstream oss;
      oss << "X: " << x << "\nY: " << y;
      hoverText.setString(oss.str());

      hoverBox.setPosition(closestPoint.x + 10, closestPoint.y - 25);
      hoverText.setPosition(closestPoint.x + 15, closestPoint.y - 20);

      window.draw(hoverBox);
      window.draw(hoverText);
    }

    window.display();
  }
}