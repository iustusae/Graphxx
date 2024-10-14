#pragma once
#include "Tokenizer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>

#include "Tokenizer.hpp" // Include your tokenizer header
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

class Graph {
private:
  sf::VertexArray m_vertices;
  std::string m_expression;

public:
  Graph(const std::string &expression) : m_expression(expression) {
    m_vertices.setPrimitiveType(sf::Lines);
  }

  void calculatePoints(const sf::View &view) {
    m_vertices.clear();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f viewCenter = view.getCenter();

    float xStart = viewCenter.x - viewSize.x / 2;
    float xEnd = viewCenter.x + viewSize.x / 2;
    float step = viewSize.x / 800; // Adjust for desired resolution

    for (float x = xStart; x < xEnd; x += step) {
      float y1 = Tokenizer::evaluate<float>(m_expression, {{'x', x}});
      float y2 = Tokenizer::evaluate<float>(m_expression, {{'x', x + step}});

      if (std::isfinite(y1) && std::isfinite(y2)) {
        m_vertices.append(sf::Vertex(sf::Vector2f(x, -y1), sf::Color::Blue));
        m_vertices.append(
            sf::Vertex(sf::Vector2f(x + step, -y2), sf::Color::Blue));
      }
    }
  }

  void draw(sf::RenderWindow &window) const { window.draw(m_vertices); }
};

class CoordinateBox {
private:
  sf::RectangleShape m_box;
  sf::Text m_text;
  sf::Font m_font;

public:
  CoordinateBox(const sf::Font &font) : m_font(font) {
    m_box.setFillColor(sf::Color(0, 0, 0, 128)); // Semi-transparent black
    m_box.setOutlineColor(sf::Color::White);
    m_box.setOutlineThickness(1);

    m_text.setFont(m_font);
    m_text.setCharacterSize(14);
    m_text.setFillColor(sf::Color::White);
  }

  void update(const sf::RenderWindow &window, const sf::View &view) {
    sf::Vector2f boxSize(150, 50);
    m_box.setSize(boxSize);
    m_box.setPosition(10, 10); // Fixed position in the top-left corner

    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);

    std::ostringstream oss;
    oss << "X: " << std::fixed << std::setprecision(2) << worldPos.x << "\n"
        << "Y: " << std::fixed << std::setprecision(2) << -worldPos.y;
    m_text.setString(oss.str());
    m_text.setPosition(m_box.getPosition() + sf::Vector2f(5, 5));
  }

  void draw(sf::RenderWindow &window) const {
    window.draw(m_box);
    window.draw(m_text);
  }
};

void drawAxes(sf::RenderWindow &window, const sf::View &view) {
  sf::VertexArray axes(sf::Lines, 4);
  sf::Vector2f viewSize = view.getSize();
  sf::Vector2f viewCenter = view.getCenter();

  // X-axis
  axes[0].position = sf::Vector2f(viewCenter.x - viewSize.x / 2, 0);
  axes[1].position = sf::Vector2f(viewCenter.x + viewSize.x / 2, 0);

  // Y-axis
  axes[2].position = sf::Vector2f(0, viewCenter.y - viewSize.y / 2);
  axes[3].position = sf::Vector2f(0, viewCenter.y + viewSize.y / 2);

  for (int i = 0; i < 4; ++i) {
    axes[i].color = sf::Color::Black;
  }

  window.draw(axes);
}

inline int draw(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <expression>" << std::endl;
    return 1;
  }

  sf::RenderWindow window(sf::VideoMode(1200, 900),
                          "Graph Viewer with Coordinates");
  window.setFramerateLimit(60);

  Graph graph(argv[1]);
  sf::Font font;
  if (!font.loadFromFile("../fonts/UbuntuMono-RI.ttf")) {
    std::cerr << "Error loading font" << std::endl;
    return 1;
  }
  CoordinateBox coordBox(font);

  // Increased initial view size for a bigger graph
  sf::View graphView(sf::FloatRect(-15.f, -11.25f, 30.f, 22.5f));
  sf::View uiView(sf::FloatRect(0, 0, 1200, 900));

  sf::Vector2f lastPos;
  bool isDragging = false;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          isDragging = true;
          lastPos = window.mapPixelToCoords(
              sf::Vector2i(event.mouseButton.x, event.mouseButton.y),
              graphView);
        }
      } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          isDragging = false;
        }
      } else if (event.type == sf::Event::MouseMoved) {
        if (isDragging) {
          sf::Vector2f newPos = window.mapPixelToCoords(
              sf::Vector2i(event.mouseMove.x, event.mouseMove.y), graphView);
          sf::Vector2f deltaPos = lastPos - newPos;
          graphView.move(deltaPos);
          lastPos = newPos;
        }
      } else if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
          float zoomFactor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
          graphView.zoom(zoomFactor);
        }
      }
    }

    graph.calculatePoints(graphView);
    coordBox.update(window, graphView);

    window.clear(sf::Color::White);

    // Draw graph and axes
    window.setView(graphView);
    drawAxes(window, graphView);
    graph.draw(window);

    // Draw UI elements (coordinate box)
    window.setView(uiView);
    coordBox.draw(window);

    window.display();
  }
  return 0;
}