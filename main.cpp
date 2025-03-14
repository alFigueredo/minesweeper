#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>

typedef struct Square {
  sf::RectangleShape back;
  size_t number;
  bool selected;
  bool flagged;
  size_t minesAround;
} Square;

const unsigned int size = 576;
const unsigned int rectangleSize = size / 9;
const unsigned int offset = 4;
const int pause = 10;

bool gameOver;
const size_t mine_proportion = 9;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> randNum(0, mine_proportion);

void reset(std::vector<std::vector<Square>> &list) {
  gameOver = false;
  for (auto &aux : list) {
    for (auto &sqr : aux) {
      sqr.number = randNum(gen);
      sqr.selected = false;
      sqr.flagged = false;
    }
  }
}

void get_text(sf::Text &text, sf::Font &font, std::string str) {
  text.setFont(font);
  text.setFillColor(sf::Color::White);
  text.setCharacterSize(32);
  text.setPosition(sf::Vector2f(size / 3.f, size * 4.f / 9.f));
  text.setString(str);
}

void display(sf::RenderWindow &window, std::vector<std::vector<Square>> &list,
             sf::Font &font) {
  sf::Text text;
  window.clear();
  for (size_t i = 0; i < 9; ++i) {
    for (size_t j = 0; j < 9; ++j) {
      Square aux = list.at(i).at(j);
      if (!aux.selected) {
        aux.back.setFillColor(aux.flagged ? sf::Color::Red : sf::Color::Green);
        window.draw(aux.back);
      } else if (aux.number == 0)
        gameOver = true;
    }
  }
  if (gameOver)
    get_text(text, font, "Game over!");
  window.draw(text);
  window.display();
}

void mouse_event(sf::RenderWindow &window,
                 std::vector<std::vector<Square>> &list, sf::Font &font,
                 sf::Event &event) {
  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  for (auto &aux : list) {
    for (auto &square : aux) {
      sf::FloatRect bounds = square.back.getGlobalBounds();
      if (bounds.contains(sf::Vector2f(mousePos))) {
        if (event.mouseButton.button == sf::Mouse::Left)
          square.selected = true;
        else if (event.mouseButton.button == sf::Mouse::Right)
          square.flagged = !square.flagged;
      }
    }
  }
}

int main() {
  sf::RenderWindow window(sf::VideoMode({size, size}), "Minesweeper");

  std::vector<std::vector<Square>> list;
  for (size_t i = 0; i < 9; ++i) {
    std::vector<Square> aux;
    for (size_t j = 0; j < 9; ++j) {
      sf::RectangleShape shape(
          sf::Vector2f(rectangleSize - offset * 2, rectangleSize - offset * 2));
      shape.setFillColor(sf::Color::Green);
      shape.setPosition(
          sf::Vector2f(rectangleSize * i + offset, rectangleSize * j + offset));
      Square square;
      square.back = shape;
      aux.push_back(square);
    }
    list.push_back(aux);
  }

  reset(list);

  sf::Font font;
  if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
    std::cout << "Error: cannot load font!" << '\n';
    return 1;
  }

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::MouseButtonPressed)
        mouse_event(window, list, font, event);
      if (event.type == sf::Event::KeyPressed &&
          event.key.scancode == sf::Keyboard::Scancode::R)
        reset(list);
    }

    if (gameOver) {
      sf::sleep(sf::milliseconds(pause));
      continue;
    }

    display(window, list, font);

    sf::sleep(sf::milliseconds(pause));
  }

  return 0;
}
