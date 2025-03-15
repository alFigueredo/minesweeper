#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>

const float PI = 3.14159265359f;

typedef struct Square {
  sf::RectangleShape back;
  size_t number;
  bool selected;
  bool flagged;
  sf::Text minesAround;
  size_t nMines;
} Square;

const unsigned int size = 576;
const unsigned int rectangleSize = size / 9;
const unsigned int offset = 4;
const int pause = 10;

bool gameOver;
const size_t mine_proportion = 9;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> randNum(0, mine_proportion - 1);

void set_number(Square &sqr, size_t &i, size_t &j, int &num, sf::Font &font) {
  sqr.nMines = num;
  sqr.minesAround.setFont(font);
  sqr.minesAround.setFillColor(sf::Color::Yellow);
  sqr.minesAround.setCharacterSize(32);
  sqr.minesAround.setPosition(
      sf::Vector2f(rectangleSize * i + rectangleSize * 2.f / 5.f,
                   rectangleSize * j + rectangleSize / 5.f));
  sqr.minesAround.setString(std::to_string(num));
}

void reset(std::vector<std::vector<Square>> &list, sf::Font &font) {
  gameOver = false;
  for (auto &aux : list) {
    for (auto &sqr : aux) {
      sqr.number = randNum(gen);
      sqr.selected = false;
      sqr.flagged = false;
      sqr.nMines = 0;
      sqr.minesAround.setString("");
    }
  }
  for (size_t i = 0; i < 9; ++i) {
    for (size_t j = 0; j < 9; ++j) {
      int num = 0;
      for (size_t k = 0; k < 8; ++k) {
        int x = rint(cos(k * PI / 4)) + i;
        int y = rint(sin(k * PI / 4)) + j;
        if (x >= 0 && x < 9 && y >= 0 && y < 9 && list.at(x).at(y).number == 0)
          ++num;
      }
      list.at(i).at(j).nMines = num;
      if (num > 0)
        set_number(list.at(i).at(j), i, j, num, font);
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
  bool win = true;
  window.clear();
  for (size_t i = 0; i < 9; ++i) {
    for (size_t j = 0; j < 9; ++j) {
      Square aux = list.at(i).at(j);
      if (!aux.selected) {
        aux.back.setFillColor(aux.flagged ? sf::Color::Blue : sf::Color::Green);
        window.draw(aux.back);
        if (aux.number != 0)
          win = false;
      } else if (aux.number == 0) {
        gameOver = true;
        win = false;
        aux.back.setFillColor(sf::Color::Red);
        window.draw(aux.back);
      } else
        window.draw(aux.minesAround);
    }
  }
  sf::Text text;
  if (gameOver)
    get_text(text, font, "Game over!");
  if (win) {
    gameOver = true;
    get_text(text, font, "You win!");
  }
  window.draw(text);
  window.display();
}

void select_square(std::vector<std::vector<Square>> &list, int &i, int &j) {
  Square *sqr = &list.at(i).at(j);
  sqr->selected = true;
  if (sqr->number != 0 && sqr->nMines == 0) {
    for (size_t k = 0; k < 8; ++k) {
      int x = rint(cos(k * PI / 4)) + i;
      int y = rint(sin(k * PI / 4)) + j;
      std::cout << "[DEBUG] x: " << x << ", y: " << y << "\n";
      if (x >= 0 && x < 9 && y >= 0 && y < 9 && !list.at(x).at(y).selected)
        select_square(list, x, y);
    }
  }
}

void mouse_event(sf::RenderWindow &window,
                 std::vector<std::vector<Square>> &list, sf::Font &font,
                 sf::Event &event) {
  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      Square *square = &list.at(i).at(j);
      sf::FloatRect bounds = square->back.getGlobalBounds();
      if (bounds.contains(sf::Vector2f(mousePos))) {
        if (event.mouseButton.button == sf::Mouse::Left)
          select_square(list, i, j);
        else if (event.mouseButton.button == sf::Mouse::Right)
          square->flagged = !square->flagged;
        return;
      }
    }
  }
}

int main() {
  sf::RenderWindow window(sf::VideoMode({size, size}), "Minesweeper");

  sf::Font font;
  if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
    std::cout << "Error: cannot load font!" << '\n';
    return 1;
  }

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

  reset(list, font);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::MouseButtonPressed)
        mouse_event(window, list, font, event);
      if (event.type == sf::Event::KeyPressed &&
          event.key.scancode == sf::Keyboard::Scancode::R)
        reset(list, font);
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
