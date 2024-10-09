#pragma once
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <string>
class Grapher {
public:
  void draw(const std::string &expression);

private:
  sf::VertexArray getAllPoints(int max_y, const std::string &expression);
  sf::Text createText(const sf::Font &font, const std::string &content, float x,
                      float y);
};
