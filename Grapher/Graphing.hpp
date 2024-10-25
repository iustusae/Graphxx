#pragma once
#include "Tokenizer.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <fmt/base.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>

inline static float getNiceStep(float range) {
  float rough = range / 10.0f;
  float scale = std::pow(10.0f, std::floor(std::log10(rough)));
  float normalized = rough / scale;

  if (normalized < 1.5f)
    return scale;
  if (normalized < 3.0f)
    return scale * 2.0f;
  if (normalized < 7.0f)
    return scale * 5.0f;
  return scale * 10.0f;
}

static inline std::string formatNumber(float value) {
  if (std::abs(value) < 1e-10)
    return "0";
  if (std::abs(value) >= 10000 || std::abs(value) < 0.01) {
    return fmt::format("{:.1e}", value);
  }
  return fmt::format("{:.2g}", value);
}

class AxisSystem {
private:
  sf::VertexArray m_gridLines;
  sf::VertexArray m_axisTicks;
  sf::Font &m_font;

public:
  AxisSystem(sf::Font &font)
      : m_font(font), m_gridLines(sf::Lines), m_axisTicks(sf::Lines) {}

  void update(const sf::View &view, const sf::Vector2u &windowSize) {
    m_gridLines.clear();
    m_axisTicks.clear();

    // Calculate view bounds
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    float xMin = viewCenter.x - viewSize.x / 2;
    float xMax = viewCenter.x + viewSize.x / 2;
    float yMin = viewCenter.y - viewSize.y / 2;
    float yMax = viewCenter.y + viewSize.y / 2;

    // Calculate steps for grid and labels
    float xStep = getNiceStep(xMax - xMin);
    float yStep = getNiceStep(yMax - yMin);

    // Calculate starting positions
    float xStart = std::floor(xMin / xStep) * xStep;
    float yStart = std::floor(yMin / yStep) * yStep;

    // Calculate pixel-to-unit ratios
    float unitsPerPixelY = viewSize.y / windowSize.y;
    float baseTextSize = 14.0f;                     // Base size in pixels
    float textSize = baseTextSize / unitsPerPixelY; // Scale text size with zoom

    // Tick size in view units
    float tickSize = 0.2f * unitsPerPixelY;

    // Create grid lines and labels
    for (float x = xStart; x <= xMax; x += xStep) {
      // Vertical grid line
      m_gridLines.append(
          sf::Vertex(sf::Vector2f(x, yMin), sf::Color(200, 200, 200, 100)));
      m_gridLines.append(
          sf::Vertex(sf::Vector2f(x, yMax), sf::Color(200, 200, 200, 100)));

      // Tick mark on x-axis
      m_axisTicks.append(
          sf::Vertex(sf::Vector2f(x, -tickSize), sf::Color::Black));
      m_axisTicks.append(
          sf::Vertex(sf::Vector2f(x, tickSize), sf::Color::Black));

      for (float y = yStart; y <= yMax; y += yStep) {
        // Horizontal grid line
        m_gridLines.append(
            sf::Vertex(sf::Vector2f(xMin, y), sf::Color(200, 200, 200, 100)));
        m_gridLines.append(
            sf::Vertex(sf::Vector2f(xMax, y), sf::Color(200, 200, 200, 100)));

        // Tick mark on y-axis
        m_axisTicks.append(
            sf::Vertex(sf::Vector2f(-tickSize, y), sf::Color::Black));
        m_axisTicks.append(
            sf::Vertex(sf::Vector2f(tickSize, y), sf::Color::Black));
      }

      // Main axes (drawn last to be on top)
      m_gridLines.append(sf::Vertex(sf::Vector2f(xMin, 0), sf::Color::Black));
      m_gridLines.append(sf::Vertex(sf::Vector2f(xMax, 0), sf::Color::Black));
      m_gridLines.append(sf::Vertex(sf::Vector2f(0, yMin), sf::Color::Black));
      m_gridLines.append(sf::Vertex(sf::Vector2f(0, yMax), sf::Color::Black));
    }
  }
  void draw(sf::RenderTarget &target,
            sf::RenderStates states = sf::RenderStates::Default) {
    // Draw grid and axes
    target.draw(m_gridLines, states);
    target.draw(m_axisTicks, states);
  }
};

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

class InputBox {
public:
  InputBox(const sf::Font &font) : m_font(font) {
    m_box.setSize(sf::Vector2f(400, 30));
    m_box.setFillColor(sf::Color::White);
    m_box.setOutlineColor(sf::Color::Black);
    m_box.setOutlineThickness(2);

    m_text.setFont(m_font);
    m_text.setCharacterSize(20);
    m_text.setFillColor(sf::Color::Black);
  }

  void setPosition(float x, float y) {
    m_box.setPosition(x, y);
    m_text.setPosition(x + 5, y + 5);
  }

  void handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::TextEntered) {
      if (event.text.unicode == 8 && m_input.size() > 0) { // Backspace
        m_input.pop_back();
      } else if (event.text.unicode == 13) { // Enter
        m_inputReady = true;
      } else if (event.text.unicode < 128) { // Regular character
        m_input += static_cast<char>(event.text.unicode);
      }
      m_text.setString(m_input);
    }
  }

  void draw(sf::RenderWindow &window) const {
    window.draw(m_box);
    window.draw(m_text);
  }

  bool isInputReady() const { return m_inputReady; }
  std::string getInput() {
    m_inputReady = false;
    return m_input;
  }
  void clear() {
    m_input.clear();
    m_text.setString(m_input);
  }

private:
  sf::RectangleShape m_box;
  sf::Text m_text;
  const sf::Font &m_font;
  std::string m_input;
  bool m_inputReady = false;
};

inline void drawAxes(sf::RenderWindow &window, const sf::View &view) {
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
  sf::RenderWindow window(sf::VideoMode(1200, 900), "Graph Viewer");
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("../fonts/UbuntuMono-RI.ttf")) {
    std::cerr << "Error loading font" << std::endl;
    return 1;
  }

  std::string initialExpression = (argc > 1) ? argv[1] : "x";
  Graph graph(initialExpression);
  CoordinateBox coordBox(font);
  InputBox inputBox(font);
  inputBox.setPosition(10, window.getSize().y - 60);

  AxisSystem axisSystem(font);

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

      inputBox.handleEvent(event);
    }

    if (inputBox.isInputReady()) {
      graph = Graph(inputBox.getInput());
      inputBox.clear();
    }

    graph.calculatePoints(graphView);
    coordBox.update(window, graphView);
    axisSystem.update(graphView, window.getSize());

    window.clear(sf::Color::White);

    // Draw graph and axes
    window.setView(graphView);
    axisSystem.draw(window);
    graph.draw(window);

    // Draw UI elements
    window.setView(uiView);
    coordBox.draw(window);
    inputBox.draw(window);

    window.display();
  }
  return 0;
}
