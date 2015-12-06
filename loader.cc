/**
 * Functions for loading graphs in the format of the Graph Drawing 2015 challenge.
 *
 * Program for minimizing the number of crossings in a book embedding of a given
 * graph to a given number of pages.
 *
 * Author: Josef Cibulka
 * License: see the file LICENSE
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "loader.h"
#include "tools.h"

using std::string;
using std::vector;
using std::istream;
using std::istringstream;
using std::cout;
using std::cerr;
using std::endl;

class NoMoreLinesException : public std::runtime_error
{
 public:
  NoMoreLinesException(const string &msg)
      : runtime_error(msg)
  {
  }
};

/**
 * Read a single non-empty line from input. Empty (whitespace-only) lines are skipped.
 * Throws NoMoreLinesException when end of file reached and runtime_error in case of
 * a failure while reading from input.
 */
string Loader::mygetline(istream &input)
{
  bool line_read = false;
  string line;
  while (!line_read)
  {
    if (input.eof())
      throw NoMoreLinesException("Failed to read a line from the input.");
    std::getline(input, line);
    if (!input.eof() && input.fail())
      throw std::runtime_error("Failure occurred while trying to read a line.");
    line = line.substr(0, line.find_first_of("\n\r#"));
    if (line.find_first_not_of(" \t") != line.npos)
      line_read = true;
  }
  return line;
}

/**
 * Reads a number from input.
 * The number may be preceded only by whitespace on the line. The rest of the line after the number is ignored.
 * Throws runtime_error in case of an error.
 */
int Loader::mygetnumber(istream &input)
{
  string line = mygetline(input);
  istringstream istr(line);
  int result;
  istr >> result;
  if (istr.fail())
    throw std::runtime_error("Failed to parse " + line + " as a number.");
  return result;
}

/**
 * Reads an edge, that is, a line starting with two whitespace separated number.
 * The rest of the line after the numbers is ignored.
 * Throws runtime_error in case of an error.
 */
Edge Loader::mygetedge(istream &input, vector<int> &whereIsVertex)
{
  string line = mygetline(input);
  istringstream istr(line);
  int v1;
  istr >> v1;
  int v2;
  istr >> v2;
  char c;
  istr >> c;
  if (c != '[')
    throw std::runtime_error(
        "Failed to parse " + line + " as an Edge: missing \'[\'");
  int page;
  istr >> page;
  istr >> c;
  if (c != ']')
    throw std::runtime_error(
        "Failed to parse " + line + " as an Edge: missing \']\'");
  if (istr.fail())
    throw std::runtime_error("Failed to parse " + line + " as an Edge.");
  return Edge(whereIsVertex[v1], whereIsVertex[v2], page);
}

/**
 * Load gr from input. Original contents of gr (if any) are removed.
 */
void Loader::load(istream &input, Graph *gr)
{
  int n = mygetnumber(input);

  gr->v.clear();
  gr->e.clear();
  for (int i = 0; i < n; i++)
    gr->v.push_back(i);

  gr->p = mygetnumber(input);

  std::vector<int> whereIsVertex(n, -1);
  for (int i = 0; i < n; i++)
  {
    int id = mygetnumber(input);
    whereIsVertex[id] = i;
  }

  // Read edges up to the end of the file.
  while (true)
  {
    try
    {
      gr->e.push_back(mygetedge(input, whereIsVertex));
    }
    catch (NoMoreLinesException &exc)
    {
      break;
    }
    catch (std::exception &exc)
    {
      throw;
    }
  }
  cout << "Loaded graph with " << gr->v.size() << " vertices and "
       << gr->e.size() << " edges." << endl;

  // Set the pointers to edges now, when the vector of all edges is filled and will not be moved.
  gr->restoreNeighs();
}
