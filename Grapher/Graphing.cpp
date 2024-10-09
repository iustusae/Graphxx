#include "Graphing.hpp"
#include "Tokenizer.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <sstream>
#include <unordered_map>

sf::VertexArray Grapher::getAllPoints(int max_y,
                                      const std::string &expression) {
  sf::VertexArray lines(sf::PrimitiveType::LinesStrip);
  std::unordered_map<char, double> var_values{{'x', -max_y}};
  double previous_x = 0.0;
  double previous_y = 0.0;

  while (var_values.at('x') < max_y) {
    double y = Tokenizer::evaluate<double>(expression, var_values);
    float screen_x = (var_values.at('x')) + 400;
    float screen_y = 300 - (y);

    lines.append(sf::Vertex(sf::Vector2f(screen_x, screen_y), sf::Color::Blue));

    if (var_values.at('x') > -max_y) {
      lines.append(
          sf::Vertex(sf::Vector2f((previous_x) + 400, 300 - (previous_y)),
                     sf::Color::Blue));
    }

    previous_x = var_values.at('x');
    previous_y = y;
    var_values.at('x') += 0.1;
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
  sf::String n_expr{};
  sf::Text p_text{};

  std::string font_path;
#ifdef __APPLE__
  font_path = "/Users/iustus/Library/Fonts/UbuntuMonoNerdFont-Bold.ttf";
#elif defined(__linux__)
  font_path =
      "/home/ayman/.local/share/fonts/NerdFonts/UbuntuMonoNerdFont-Regular.ttf";
#endif
  if (!font.loadFromFile(font_path)) {
    std::cerr << "Failed to load font" << std::endl;
    return;
  }

  p_text.setCharacterSize(30);
  p_text.setFont(font);
  p_text.setFillColor(sf::Color::White);
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

  sf::VertexArray vertices = getAllPoints(600, expression);

  sf::RectangleShape hoverBox(sf::Vector2f(100, 50));
  hoverBox.setFillColor(sf::Color(0, 0, 0, 200));
  sf::Text hoverText = createText(font, "", 0, 0);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::TextEntered) {
        n_expr += event.text.unicode;
        p_text.setString(n_expr);
      } else if (event.type == sf::Event::KeyPressed) {
        // if (event.key.code == sf::Keyboard::BackSpace && !n_expr.isEmpty()) {
        //   n_expr.erase(n_expr.getSize() - 1);
        //   p_text.setString(n_expr);
        //   window.draw(p_text);
        // } else if (event.key.code == sf::Keyboard::Enter) {
        //   return this->draw(n_expr);
        // }
      }
    }

    window.clear(sf::Color::Black);
    window.draw(p_text);
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
      float x = (closestPoint.x - 400);
      float y = (300 - closestPoint.y);
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