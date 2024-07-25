#ifndef SPLIT_HPP
#define SPLIT_HPP

#include <Arduino.h>

void splitStringRecursive(const String& input, const char* delimiter, int& startPos) {
  // Base case for recursion - no more arguments to process
}

template <typename First, typename... Rest>
void splitStringRecursive(const String& input, const char* delimiter, int& startPos, First& first, Rest&... rest) {
  int delimiterPos = input.indexOf(delimiter, startPos);
  if (delimiterPos == -1) {
    // If no more delimiters, assign the rest of the string to the last variable
    first = input.substring(startPos);
    splitStringRecursive(input, delimiter, startPos); // Call with no more arguments to end recursion
  } else {
    first = input.substring(startPos, delimiterPos);
    startPos = delimiterPos + strlen(delimiter);
    splitStringRecursive(input, delimiter, startPos, rest...); // Process the rest of the arguments
  }
}

template <typename... Args>
void splitString(const String& input, const char* delimiter, Args&... args) {
  int startPos = 0;
  splitStringRecursive(input, delimiter, startPos, args...);
}

#endif //SPLIT_HPP