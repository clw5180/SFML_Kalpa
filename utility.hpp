#ifndef BOOK_UTILITY_HPP
#define BOOK_UTILITY_HPP

#include <sstream>


namespace sf
{
	class Sprite;
	class Text;
}

// Since std::to_string doesn't work on MinGW we have to implement
// our own to support all platforms.
template <typename T>
std::string toString(const T& value);

void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);
void topLeftOrigin(sf::Text& text);
void topRightOrigin(sf::Text& text);
void resizeBackgroundToWindow(sf::Sprite& backgroundSprite);   

#include "utility.inl"
#endif // BOOK_UTILITY_HPP
